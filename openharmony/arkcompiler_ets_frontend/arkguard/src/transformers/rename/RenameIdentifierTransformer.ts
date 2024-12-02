/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import {
  SyntaxKind,
  factory,
  forEachChild,
  isBreakOrContinueStatement,
  isConstructorDeclaration,
  isExportSpecifier,
  isIdentifier,
  isImportSpecifier,
  isLabeledStatement,
  isMetaProperty,
  isSourceFile,
  isStructDeclaration,
  setParentRecursive,
  visitEachChild,
  isPropertyDeclaration,
  isMethodDeclaration,
  isGetAccessor,
  isSetAccessor,
  isClassDeclaration,
  isFunctionExpression,
  isArrowFunction,
  isVariableDeclaration,
  isPropertyAssignment,
  isPrivateIdentifier,
  isParameter,
  isPropertyAccessExpression
} from 'typescript';

import type {
  ClassElement,
  Declaration,
  Identifier,
  Node,
  SourceFile,
  StructDeclaration,
  Symbol,
  TransformationContext,
  Transformer,
  TransformerFactory,
  TypeChecker
} from 'typescript';

import {
  createScopeManager,
  isClassScope,
  isGlobalScope,
  isEnumScope,
  isInterfaceScope,
  isObjectLiteralScope,
  exportElementsWithoutSymbol,
  getNameWithScopeLoc
} from '../../utils/ScopeAnalyzer';

import type {
  Label,
  Scope,
  ScopeManager
} from '../../utils/ScopeAnalyzer';

import {
  IDENTIFIER_CACHE,
  MEM_METHOD_CACHE
} from '../../utils/NameCacheUtil';

import type {INameGenerator, NameGeneratorOptions} from '../../generator/INameGenerator';
import type {IOptions} from '../../configs/IOptions';
import type { INameObfuscationOption, IUnobfuscationOption } from '../../configs/INameObfuscationOption';
import type {TransformPlugin} from '../TransformPlugin';
import type { MangledSymbolInfo } from '../../common/type';
import {TransformerOrder} from '../TransformPlugin';
import {getNameGenerator, NameGeneratorType} from '../../generator/NameFactory';
import {TypeUtils} from '../../utils/TypeUtils';
import {
  isInTopLevelWhitelist,
  isInLocalWhitelist,
  isReservedLocalVariable,
  isReservedTopLevel,
  recordHistoryUnobfuscatedNames,
  isInPropertyWhitelist,
  isReservedProperty
} from '../../utils/TransformUtil';
import {NodeUtils} from '../../utils/NodeUtils';
import {ApiExtractor} from '../../common/ApiExtractor';
import {performancePrinter, ArkObfuscator, cleanFileMangledNames} from '../../ArkObfuscator';
import { EventList, endSingleFileEvent, startSingleFileEvent } from '../../utils/PrinterUtils';
import { isViewPUBasedClass } from '../../utils/OhsUtil';
import {
  PropCollections,
  UnobfuscationCollections,
  LocalVariableCollections
} from '../../utils/CommonCollections';

namespace secharmony {
  /**
   * Rename Identifiers, including:
   * 1. variable name
   * 2. function name
   * 3. label name
   * 4. class name/interface name/ label name
   * we need implement some features:
   * 1. rename identifiers
   * 2. store/restore name to/from nameCache file.
   * 3. do scope analysis for identifier obfuscations
   *
   * @param option
   */
  const createRenameIdentifierFactory = function (option: IOptions): TransformerFactory<Node> {
    const profile: INameObfuscationOption | undefined = option?.mNameObfuscation;
    const unobfuscationOption: IUnobfuscationOption | undefined = option?.mUnobfuscationOption;
    if (!profile || !profile.mEnable) {
      return null;
    }

    let options: NameGeneratorOptions = {};
    let generator: INameGenerator = getNameGenerator(profile.mNameGeneratorType, options);

    const enableToplevel: boolean = option?.mNameObfuscation?.mTopLevel;
    const exportObfuscation: boolean = option?.mExportObfuscation;
    let isInitializedReservedList = false;
    return renameIdentifierFactory;

    function renameIdentifierFactory(context: TransformationContext): Transformer<Node> {
      initWhitelist();
      let mangledSymbolNames: Map<Symbol, MangledSymbolInfo> = new Map<Symbol, MangledSymbolInfo>();
      let mangledPropertyParameterSymbolNames: Map<Declaration, MangledSymbolInfo> = new Map<Declaration, MangledSymbolInfo>();
      let mangledLabelNames: Map<Label, string> = new Map<Label, string>();
      let fileExportNames: Set<string> = undefined;
      let fileImportNames: Set<string> = undefined;
      exportElementsWithoutSymbol.clear();

      let historyMangledNames: Set<string> = undefined;
      if (historyNameCache && historyNameCache.size > 0) {
        historyMangledNames = new Set<string>(Array.from(historyNameCache.values()));
      }

      let checker: TypeChecker = undefined;
      let manager: ScopeManager = createScopeManager();

      return renameTransformer;

      /**
       * Transformer to rename identifiers
       *
       * @param node ast node of a file.
       */
      function renameTransformer(node: Node): Node {
        if (nameCache.size === 0) {
          nameCache.set(IDENTIFIER_CACHE, new Map<string, string>());
          nameCache.set(MEM_METHOD_CACHE, new Map<string, string>());
        }

        if (!isSourceFile(node) || ArkObfuscator.isKeptCurrentFile) {
          return node;
        }

        startSingleFileEvent(EventList.CREATE_CHECKER, performancePrinter.timeSumPrinter);
        checker = TypeUtils.createChecker(node);
        endSingleFileEvent(EventList.CREATE_CHECKER, performancePrinter.timeSumPrinter);

        startSingleFileEvent(EventList.SCOPE_ANALYZE, performancePrinter.timeSumPrinter);
        manager.analyze(node, checker, exportObfuscation);
        endSingleFileEvent(EventList.SCOPE_ANALYZE, performancePrinter.timeSumPrinter);

        let rootScope: Scope = manager.getRootScope();
        fileExportNames = rootScope.fileExportNames;
        fileImportNames = rootScope.fileImportNames;
        let renameProcessors: ((scope: Scope) => void)[] = [renameLabelsInScope, renameNamesInScope];
        if (profile.mRenameProperties) {
          renameProcessors.push(renamePropertyParametersInScope);
        }

        startSingleFileEvent(EventList.CREATE_OBFUSCATED_NAMES, performancePrinter.timeSumPrinter);
        getMangledNamesInScope(rootScope, renameProcessors);
        endSingleFileEvent(EventList.CREATE_OBFUSCATED_NAMES, performancePrinter.timeSumPrinter);

        rootScope = undefined;

        startSingleFileEvent(EventList.OBFUSCATE_NODES, performancePrinter.timeSumPrinter);
        let updatedNode: Node = renameIdentifiers(node);

        // obfuscate property parameter declaration
        if (profile.mRenameProperties) {
          updatedNode = visitPropertyParameter(updatedNode);
        }

        let parentNodes = setParentRecursive(updatedNode, true);
        endSingleFileEvent(EventList.OBFUSCATE_NODES, performancePrinter.timeSumPrinter);
        return parentNodes;
      }

      /**
       * get mangled names of symbols stored in scopes.
       *
       * @param scope scope, such as global, module, function, block
       * @param processors processors to get mangled names
       */
      function getMangledNamesInScope(scope: Scope, processors: ((scope: Scope) => void)[]): void {
        for (const process of processors) {
          process(scope);
        }

        let subScope = undefined;
        while (scope.children.length > 0) {
          subScope = scope.children.pop();
          getMangledNamesInScope(subScope, processors);
          subScope = undefined;
        }
      }

      // process symbols in scope, exclude property name.
      function renameNamesInScope(scope: Scope): void {
        if (isExcludeScope(scope)) {
          return;
        }

        if (!exportObfuscation) {
          scope.defs.forEach((def) => {
            let parentScope = scope;
            while (parentScope) {
              if (parentScope.importNames && parentScope.importNames.has(def.name)) {
                scope.defs.delete(def);
                scope.mangledNames.add(def.name);
              }
              parentScope = parentScope.parent;
            }
          });
        }

        renames(scope, scope.defs, generator);
      }

      // process property parameters symbols in class scope
      function renamePropertyParametersInScope(scope: Scope): void {
        if (!isClassScope(scope)) {
          return;
        }

        renamePropertyParameters(scope, scope.defs, generator);
      }

      function renames(scope: Scope, defs: Set<Symbol>, generator: INameGenerator): void {
        defs.forEach((def) => {
          const original: string = def.name;
          let mangled: string = original;
          const path: string = getNameWithScopeLoc(scope, original);
          // No allow to rename reserved names.
          if (!Reflect.has(def, 'obfuscateAsProperty') &&
            isInLocalWhitelist(original, UnobfuscationCollections.unobfuscatedNamesMap, path) ||
            (!exportObfuscation && scope.exportNames.has(def.name)) ||
            isSkippedGlobal(enableToplevel, scope)) {
            scope.mangledNames.add(mangled);
            mangledSymbolNames.set(def, {mangledName: mangled, originalNameWithScope: path});
            return;
          }

          if (mangledSymbolNames.has(def)) {
            return;
          }

          const historyName: string = historyNameCache?.get(path);
          if (historyName) {
            recordHistoryUnobfuscatedNames(path); // For incremental build
            mangled = historyName;
          } else if (Reflect.has(def, 'obfuscateAsProperty')) {
            // obfuscate toplevel, export
            mangled = getPropertyMangledName(original, path);
          } else {
            // obfuscate local variable
            mangled = getMangled(scope, generator);
          }
          // add new names to name cache
          let identifierCache = nameCache?.get(IDENTIFIER_CACHE);
          (identifierCache as Map<string, string>).set(path, mangled);
          let symbolInfo: MangledSymbolInfo = {
            mangledName: mangled,
            originalNameWithScope: path
          };
          scope.mangledNames.add(mangled);
          mangledSymbolNames.set(def, symbolInfo);
        });
      }

      function renamePropertyParameters(scope: Scope, defs: Set<Symbol>, generator: INameGenerator): void {
        defs.forEach((def) => {
          //only rename property parameters
          if (!def.valueDeclaration || !isParameter(def.valueDeclaration)) {
            return;
          }
          const originalName: string = def.name;
          const path: string = getNameWithScopeLoc(scope, originalName);
          let mangledName: string;
          if (isInPropertyWhitelist(originalName, UnobfuscationCollections.unobfuscatedPropMap)) {
            mangledName = originalName;
          } else {
            mangledName = getMangledPropertyParameters(scope, generator, originalName);
          }
          scope.mangledNames.add(mangledName);
          let symbolInfo: MangledSymbolInfo = {
            mangledName: mangledName,
            originalNameWithScope: path
          };
          mangledPropertyParameterSymbolNames.set(def.valueDeclaration, symbolInfo);
        });
      }

      function getMangledPropertyParameters(scope: Scope, localGenerator: INameGenerator, originalName: string): string {
        const historyName: string = PropCollections.historyMangledTable?.get(originalName);
        let mangledName: string = historyName ? historyName : PropCollections.globalMangledTable.get(originalName);
        while (!mangledName) {
          let tmpName = localGenerator.getName();
          if (isReservedLocalVariable(tmpName)) {
            continue;
          }
          if (isReservedProperty(tmpName) || tmpName === originalName) {
            continue;
          }
          if (historyMangledNames && historyMangledNames.has(tmpName)) {
            continue;
          }
          if (PropCollections.newlyOccupiedMangledProps.has(tmpName) || PropCollections.mangledPropsInNameCache.has(tmpName)) {
            continue;
          }
          if (searchMangledInParent(scope, tmpName)) {
            continue;
          }
          mangledName = tmpName;
        }
        PropCollections.globalMangledTable.set(originalName, mangledName);
        PropCollections.newlyOccupiedMangledProps.add(mangledName);
        return mangledName;
      }

      function getPropertyMangledName(original: string, nameWithScope: string): string {
        if (isInTopLevelWhitelist(original, UnobfuscationCollections.unobfuscatedNamesMap, nameWithScope)) {
          return original;
        }

        const historyName: string = PropCollections.historyMangledTable?.get(original);
        let mangledName: string = historyName ? historyName : PropCollections.globalMangledTable.get(original);
        while (!mangledName) {
          let tmpName = generator.getName();
          if (isReservedTopLevel(tmpName) ||
            tmpName === original) {
            continue;
          }

          /**
           * In case b is obfuscated as a when only enable toplevel obfuscation:
           * let b = 1;
           * export let a = 1;
           */
          if (cleanFileMangledNames && fileExportNames && fileExportNames.has(tmpName)) {
            continue;
          }

          /**
           * In case b is obfuscated as a when only enable toplevel obfuscation:
           * import {a} from 'filePath';
           * let b = 1;
           */
          if (cleanFileMangledNames && fileImportNames.has(tmpName)) {
            continue;
          }

          /**
           * In case a newly added variable get an obfuscated name that is already in history namecache
           */
          if (historyMangledNames && historyMangledNames.has(tmpName)) {
            continue;
          }

          if (PropCollections.newlyOccupiedMangledProps.has(tmpName) || PropCollections.mangledPropsInNameCache.has(tmpName)) {
            continue;
          }

          if (ApiExtractor.mConstructorPropertySet?.has(tmpName)) {
            continue;
          }

          if (ApiExtractor.mEnumMemberSet?.has(tmpName)) {
            continue;
          }

          mangledName = tmpName;
        }

        PropCollections.globalMangledTable.set(original, mangledName);
        PropCollections.newlyOccupiedMangledProps.add(mangledName);
        return mangledName;
      }

      function isExcludeScope(scope: Scope): boolean {
        if (isClassScope(scope)) {
          return true;
        }

        if (isInterfaceScope(scope)) {
          return true;
        }

        if (isEnumScope(scope)) {
          return true;
        }

        return isObjectLiteralScope(scope);
      }

      function searchMangledInParent(scope: Scope, name: string): boolean {
        let found: boolean = false;
        let parentScope = scope;
        while (parentScope) {
          if (parentScope.mangledNames.has(name)) {
            found = true;
            break;
          }

          parentScope = parentScope.parent;
        }

        return found;
      }

      function getMangled(scope: Scope, localGenerator: INameGenerator): string {
        let mangled: string = '';
        do {
          mangled = localGenerator.getName()!;
          // if it is a globally reserved name, it needs to be regenerated
          if (isReservedLocalVariable(mangled)) {
            mangled = '';
            continue;
          }

          if (fileExportNames && fileExportNames.has(mangled)) {
            mangled = '';
            continue;
          }

          if (historyMangledNames && historyMangledNames.has(mangled)) {
            mangled = '';
            continue;
          }

          if (searchMangledInParent(scope, mangled)) {
            mangled = '';
            continue;
          }

          if (ApiExtractor.mConstructorPropertySet?.has(mangled)) {
            mangled = '';
          }

          if (ApiExtractor.mEnumMemberSet?.has(mangled)) {
            mangled = '';
          }
        } while (mangled === '');

        return mangled;
      }

      // process labels in scope, the label can't rename as the name of top labels.
      function renameLabelsInScope(scope: Scope): void {
        const labels: Label[] = scope.labels;
        if (labels.length > 0) {
          let upperMangledLabels = getUpperMangledLabelNames(labels[0]);
          for (const label of labels) {
            let mangledLabel = getMangledLabel(label, upperMangledLabels);
            mangledLabelNames.set(label, mangledLabel);
          }
        }
      }

      function getMangledLabel(label: Label, mangledLabels: string[]): string {
        let mangledLabel: string = '';
        do {
          mangledLabel = generator.getName();
          if (mangledLabel === label.name) {
            mangledLabel = '';
          }

          if (mangledLabels.includes(mangledLabel)) {
            mangledLabel = '';
          }
        } while (mangledLabel === '');

        return mangledLabel;
      }

      function getUpperMangledLabelNames(label: Label): string[] {
        const results: string[] = [];
        let parent: Label = label.parent;
        while (parent) {
          let mangledLabelName: string = mangledLabelNames.get(parent);
          if (mangledLabelName) {
            results.push(mangledLabelName);
          }
          parent = parent.parent;
        }

        return results;
      }

      function isFunctionLike(node: Node): boolean {
        switch (node.kind) {
          case SyntaxKind.FunctionDeclaration:
          case SyntaxKind.MethodDeclaration:
          case SyntaxKind.GetAccessor:
          case SyntaxKind.SetAccessor:
          case SyntaxKind.Constructor:
          case SyntaxKind.FunctionExpression:
          case SyntaxKind.ArrowFunction:
            return true;
        }
        return false;
      }

      function nodeHasFunctionLikeChild(node: Node): boolean {
        let hasFunctionLikeChild: boolean = false;
        let childVisitor: (child: Node) => Node = (child: Node): Node => {
          if (!hasFunctionLikeChild && child && isFunctionLike(child)) {
            hasFunctionLikeChild = true;
          }
          return child;
        };
        visitEachChild(node, childVisitor, context);
        return hasFunctionLikeChild;
      }

      /**
       * visit each node to change identifier name to mangled name
       *  - calculate shadow name index to find shadow node
       * @param node
       */
      function renameIdentifiers(node: Node): Node {
        let needHandlePositionInfo: boolean = isFunctionLike(node) || nodeHasFunctionLikeChild(node);
        if (needHandlePositionInfo) {
          // Obtain line info for nameCache.
          handlePositionInfo(node);
        }

        if (!isIdentifier(node) || !node.parent) {
          return visitEachChild(node, renameIdentifiers, context);
        }

        if (isLabeledStatement(node.parent) || isBreakOrContinueStatement(node.parent)) {
          return updateLabelNode(node);
        }

        return updateNameNode(node);
      }

      /**
       * visit each property parameter to change identifier name to mangled name
       *  - calculate shadow name index to find shadow node
       * @param node
       */
      function visitPropertyParameter(node: Node): Node {
        if (isConstructorDeclaration(node)) {
          return visitPropertyParameterInConstructor(node);
        }

        return visitEachChild(node, visitPropertyParameter, context);

        function visitPropertyParameterInConstructor(node: Node): Node {
          if (!isIdentifier(node) || !node.parent) {
            return visitEachChild(node, visitPropertyParameterInConstructor, context);
          }

          // we do not obfuscate the identifier of property access expression, like "a" in "this.a",
          // since it will be obfuscated in renamePropertiesTransformer
          if (NodeUtils.isPropertyNode(node)) {
            return node;
          }

          return updatePropertyParameterNameNode(node);
        }
      }

      function handlePositionInfo(node: Node): void {
        const sourceFile = NodeUtils.getSourceFileOfNode(node);
        if (node && node.pos < 0 && node.end < 0) {
          // Node must have a real position for following operations.
          // Adapting to the situation that the node does not have a real postion.
          return;
        }
        const startPosition = sourceFile.getLineAndCharacterOfPosition(node.getStart());
        const endPosition = sourceFile.getLineAndCharacterOfPosition(node.getEnd());
        // 1: The line number in sourceFile starts from 0 while in IDE starts from 1.
        const startLine = startPosition.line + 1;
        const startCharacter = startPosition.character + 1; // 1: Same as above.
        const endLine = endPosition.line + 1; // 1: Same as above.
        const endCharacter = endPosition.character + 1; // 1: Same as above.
        const lineAndColum: string = ':' + startLine + ':' + startCharacter + ':' + endLine + ':' + endCharacter;

        let isProperty: boolean = isMethodDeclaration(node) || isGetAccessor(node) ||
                                  isSetAccessor(node) || (isConstructorDeclaration(node) &&
                                  !(isClassDeclaration(node.parent) && isViewPUBasedClass(node.parent)));
        // Arrow functions are anoymous, only function expressions are considered.
        let isPropertyParent: boolean = isFunctionExpression(node) &&
                                        (isPropertyDeclaration(node.parent) || isPropertyAssignment(node.parent));
        let isMemberMethod: boolean = isProperty || isPropertyParent;
        if (isMemberMethod) {
          writeMemberMethodCache(node, lineAndColum);
          return;
        }

        let name = Reflect.get(node, 'name') as Identifier;
        if (name?.kind === SyntaxKind.Identifier) {
          identifierLineMap.set(name, lineAndColum);
        } else if ((isFunctionExpression(node) || isArrowFunction(node)) && isVariableDeclaration(node.parent) &&
          node.parent.name?.kind === SyntaxKind.Identifier) {
          // The node is anonymous, and we need to find its parent node.
          // e.g.: let foo = function() {};
          identifierLineMap.set(node.parent.name, lineAndColum);
        }
      }

      function writeMemberMethodCache(node: Node, lineAndColum: string): void {
        let gotNode;
        if (node.kind === SyntaxKind.Constructor) {
          gotNode = node.parent;
        } else if ((node.kind === SyntaxKind.FunctionExpression &&
          (isPropertyDeclaration(node.parent) || isPropertyAssignment(node.parent)))) {
          gotNode = node.parent.initializer ?? node.parent;
        } else {
          gotNode = node;
        }

        let isIdentifierNode: boolean = gotNode.name && (isIdentifier(gotNode.name) || isPrivateIdentifier(gotNode.name));
        let valueName: string = '';

        if (isIdentifierNode) {
          // The original method for retrieving method names used gotNode.name.escapedText. This approach limited the collection
          // of method records in MemberMethodCache to cases where gotNode.name was an Identifier or PrivateIdentifier.
          // To address the issue where method names starting with double underscores were transformed to start with triple underscores,
          // we changed the retrieval method to use gotNode.name.text instead of escapedText. However, this change introduced the possibility
          // of collecting method records when gotNode.name is a NumericLiteral or StringLiteral, which is not desired.
          // To avoid altering the collection specifications of MemberMethodCache, we restricted the collection scenarios
          // to match the original cases where only identifiers and private identifiers are collected.
          valueName = gotNode.name.text;
        }

        if (valueName === '') {
          return;
        }

        let originalName: string = valueName;
        let keyName = originalName + lineAndColum;
        if (node.kind === SyntaxKind.Constructor && classMangledName.has(gotNode.name)) {
          valueName = classMangledName.get(gotNode.name);
          classInfoInMemberMethodCache.add(keyName);
        }
        let memberMethodCache = nameCache?.get(MEM_METHOD_CACHE);
        if (memberMethodCache) {
          (memberMethodCache as Map<string, string>).set(keyName, valueName);
        }
      }

      function updateNameNode(node: Identifier): Node {
        // skip property in property access expression
        if (NodeUtils.isPropertyAccessNode(node)) {
          return node;
        }

        if (NodeUtils.isNewTargetNode(node)) {
          return node;
        }

        let sym: Symbol | undefined = NodeUtils.findSymbolOfIdentifier(checker, node);
        let mangledPropertyNameOfNoSymbolImportExport = '';
        if (!sym) {
          if (shouldObfuscateNodeWithoutSymbol(node)) {
            mangledPropertyNameOfNoSymbolImportExport = mangleNoSymbolImportExportPropertyName(node.text);
          } else {
            return node;
          }
        }

        // Add new names to name cache
        const symbolInfo: MangledSymbolInfo = mangledSymbolNames.get(sym);
        const identifierCache = nameCache?.get(IDENTIFIER_CACHE);
        const lineAndColumn = identifierLineMap?.get(node);
        // We only save the line info of FunctionLike.
        const isFunction: boolean = sym ? Reflect.has(sym, 'isFunction') : false;
        if (isFunction && symbolInfo && lineAndColumn) {
          const originalName = symbolInfo.originalNameWithScope;
          const pathWithLine: string = originalName + lineAndColumn;
          (identifierCache as Map<string, string>).set(pathWithLine, symbolInfo.mangledName);
          (identifierCache as Map<string, string>).delete(originalName);
        }

        let mangledName: string = mangledSymbolNames.get(sym)?.mangledName;
        if (node?.parent.kind === SyntaxKind.ClassDeclaration) {
          classMangledName.set(node, mangledName);
        }
        if (!mangledName && mangledPropertyNameOfNoSymbolImportExport !== '') {
          mangledName = mangledPropertyNameOfNoSymbolImportExport;
        }

        if (!mangledName || mangledName === sym?.name) {
          return node;
        }

        return factory.createIdentifier(mangledName);
      }

      function updatePropertyParameterNameNode(node: Identifier): Node {
        let sym: Symbol | undefined = NodeUtils.findSymbolOfIdentifier(checker, node);
        if (!sym || sym.valueDeclaration?.kind !== SyntaxKind.Parameter) {
          return node;
        }

        let mangledName: string | undefined = mangledPropertyParameterSymbolNames.get(sym.valueDeclaration)?.mangledName;
        if (!mangledName || mangledName === sym?.name) {
          return node;
        }

        return factory.createIdentifier(mangledName);
      }

      function updateLabelNode(node: Identifier): Node {
        let label: Label | undefined;
        let labelName: string = '';

        mangledLabelNames.forEach((value, key) => {
          if (key.refs.includes(node)) {
            label = key;
            labelName = value;
          }
        });

        return label ? factory.createIdentifier(labelName) : node;
      }

      /**
       * import {A as B} from 'modulename';
       * import {C as D} from 'modulename';
       * above A、C have no symbol, so deal with them specially.
       */
      function mangleNoSymbolImportExportPropertyName(original: string): string {
        const path: string = '#' + original;
        const historyName: string = historyNameCache?.get(path);
        let mangled = historyName ?? getPropertyMangledName(original, path);
        if (nameCache && nameCache.get(IDENTIFIER_CACHE)) {
          (nameCache.get(IDENTIFIER_CACHE) as Map<string, string>).set(path, mangled);
        }
        return mangled;
      }

      function trySearchImportExportSpecifier(node: Node): boolean {
        while (node.parent) {
          node = node.parent;
          if ((isImportSpecifier(node) || isExportSpecifier(node)) && node.propertyName && isIdentifier(node.propertyName)) {
            return true;
          }
        }
        return false;
      }

      function shouldObfuscateNodeWithoutSymbol(node: Identifier): boolean {
        if (exportObfuscation && exportElementsWithoutSymbol.has(node) && trySearchImportExportSpecifier(node)) {
          let isGlobalNode: boolean = exportElementsWithoutSymbol.get(node);
          if ((isGlobalNode && enableToplevel) || !isGlobalNode) {
            return true;
          }
        }
        return false;
      }
    }

    function initWhitelist(): void {
      if (isInitializedReservedList) {
        return;
      }
      if (profile?.mRenameProperties) {
        PropCollections.enablePropertyObfuscation = true;
        const tmpReservedProps: string[] = profile?.mReservedProperties ?? [];
        tmpReservedProps.forEach(item => {
          PropCollections.reservedProperties.add(item);
        });
        PropCollections.mangledPropsInNameCache = new Set(PropCollections.historyMangledTable?.values());
        if (profile?.mUniversalReservedProperties) {
          PropCollections.universalReservedProperties = [...profile.mUniversalReservedProperties];
        }
        UnobfuscationCollections.reservedLangForTopLevel.forEach(element => {
          UnobfuscationCollections.reservedLangForProperty.add(element);
        });
        UnobfuscationCollections.reservedExportName.forEach(element => {
          UnobfuscationCollections.reservedExportNameAndProp.add(element);
        });
        UnobfuscationCollections.reservedSdkApiForGlobal.forEach(element => {
          UnobfuscationCollections.reservedSdkApiForProp.add(element);
        });
      }
      LocalVariableCollections.reservedConfig = new Set(profile?.mReservedNames ?? []);
      LocalVariableCollections.reservedStruct = new Set();
      profile?.mReservedToplevelNames?.forEach(item => PropCollections.reservedProperties.add(item));
      profile?.mUniversalReservedToplevelNames?.forEach(item => PropCollections.universalReservedProperties.push(item));
      isInitializedReservedList = true;
    }
  };

  function isSkippedGlobal(enableTopLevel: boolean, scope: Scope): boolean {
    return !enableTopLevel && isGlobalScope(scope);
  }

  export let transformerPlugin: TransformPlugin = {
    'name': 'renameIdentifierPlugin',
    'order': TransformerOrder.RENAME_IDENTIFIER_TRANSFORMER,
    'createTransformerFactory': createRenameIdentifierFactory
  };

  export let nameCache: Map<string, string | Map<string, string>> = new Map();
  export let historyNameCache: Map<string, string> = undefined;
  export let historyUnobfuscatedNamesMap: Map<string, string[]> = undefined;
  export let identifierLineMap: Map<Identifier, string> = new Map();
  export let classMangledName: Map<Node, string> = new Map();
  // Record the original class name and line number range to distinguish between class names and member method names.
  export let classInfoInMemberMethodCache: Set<string> = new Set();

  export function clearCaches(): void {
    nameCache.clear();
    historyNameCache = undefined;
    historyUnobfuscatedNamesMap = undefined;
    identifierLineMap.clear();
    classMangledName.clear();
    classInfoInMemberMethodCache.clear();
    UnobfuscationCollections.unobfuscatedNamesMap.clear();
  }
}

export = secharmony;