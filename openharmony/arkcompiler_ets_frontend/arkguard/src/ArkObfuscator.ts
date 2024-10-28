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
  createPrinter,
  createTextWriter,
  transform,
  createObfTextSingleLineWriter,
} from 'typescript';

import type {
  CompilerOptions,
  EmitTextWriter,
  Node,
  Printer,
  PrinterOptions,
  RawSourceMap,
  SourceFile,
  SourceMapGenerator,
  TransformationResult,
  TransformerFactory,
} from 'typescript';

import path from 'path';

import { LocalVariableCollections, PropCollections } from './utils/CommonCollections';
import type { IOptions } from './configs/IOptions';
import { FileUtils } from './utils/FileUtils';
import { TransformerManager } from './transformers/TransformerManager';
import { getSourceMapGenerator } from './utils/SourceMapUtil';
import {
  decodeSourcemap,
  ExistingDecodedSourceMap,
  Source,
  SourceMapLink,
  SourceMapSegmentObj,
  mergeSourceMap
} from './utils/SourceMapMergingUtil';
import {
  deleteLineInfoForNameString,
  getMapFromJson,
  IDENTIFIER_CACHE,
  MEM_METHOD_CACHE
} from './utils/NameCacheUtil';
import { ListUtil } from './utils/ListUtil';
import { needReadApiInfo, readProjectPropertiesByCollectedPaths } from './common/ApiReader';
import type { ReseverdSetForArkguard } from './common/ApiReader';
import { ApiExtractor } from './common/ApiExtractor';
import esInfo from './configs/preset/es_reserved_properties.json';
import { EventList, TimeSumPrinter, TimeTracker } from './utils/PrinterUtils';
import { Extension, type ProjectInfo, type FilePathObj } from './common/type';
export { FileUtils } from './utils/FileUtils';
export { MemoryUtils } from './utils/MemoryUtils';
import { TypeUtils } from './utils/TypeUtils';
import { handleReservedConfig } from './utils/TransformUtil';
import { UnobfuscationCollections } from './utils/CommonCollections';
import { historyAllUnobfuscatedNamesMap } from './initialization/Initializer';
export { UnobfuscationCollections } from './utils/CommonCollections';
export { separateUniversalReservedItem, containWildcards, wildcardTransformer } from './utils/TransformUtil';
export type { ReservedNameInfo } from './utils/TransformUtil';
export type { ReseverdSetForArkguard } from './common/ApiReader';

export { initObfuscationConfig } from './initialization/Initializer';
export { nameCacheMap, unobfuscationNamesObj } from './initialization/CommonObject';
export {
  collectResevedFileNameInIDEConfig, // For running unit test.
  enableObfuscatedFilePathConfig,
  enableObfuscateFileName,
  generateConsumerObConfigFile,
  getRelativeSourcePath,
  handleObfuscatedFilePath,
  handleUniversalPathInObf,
  mangleFilePath,
  MergedConfig,
  ObConfigResolver,
  readNameCache,
  writeObfuscationNameCache,
  writeUnobfuscationContent
} from './initialization/ConfigResolver';
export {
  collectReservedNameForObf
} from './utils/NodeUtils';

export const renameIdentifierModule = require('./transformers/rename/RenameIdentifierTransformer');
export const renameFileNameModule = require('./transformers/rename/RenameFileNameTransformer');

export { getMapFromJson, readProjectPropertiesByCollectedPaths, deleteLineInfoForNameString, ApiExtractor, PropCollections };
export let orignalFilePathForSearching: string | undefined;
export let cleanFileMangledNames: boolean = false;
export interface PerformancePrinter {
  filesPrinter?: TimeTracker;
  singleFilePrinter?: TimeTracker;
  timeSumPrinter?: TimeSumPrinter;
  iniPrinter: TimeTracker;
}
export let performancePrinter: PerformancePrinter = {
  iniPrinter: new TimeTracker(),
};

// When the module is compiled, call this function to clear global collections.
export function clearGlobalCaches(): void {
  PropCollections.clearPropsCollections();
  UnobfuscationCollections.clear();
  LocalVariableCollections.clear();
  renameFileNameModule.clearCaches();
}

export type ObfuscationResultType = {
  content: string;
  sourceMap?: RawSourceMap;
  nameCache?: { [k: string]: string | {} };
  filePath?: string;
  unobfuscationNameMap?: Map<string, Set<string>>;
};

const JSON_TEXT_INDENT_LENGTH: number = 2;
export class ArkObfuscator {
  // Used only for testing
  protected mWriteOriginalFile: boolean = false;

  // A text writer of Printer
  private mTextWriter: EmitTextWriter;

  // Compiler Options for typescript,use to parse ast
  private readonly mCompilerOptions: CompilerOptions;

  // User custom obfuscation profiles.
  protected mCustomProfiles: IOptions;

  private mTransformers: TransformerFactory<Node>[];

  static mProjectInfo: ProjectInfo | undefined;

  // If isKeptCurrentFile is true, both identifier and property obfuscation are skipped.
  static mIsKeptCurrentFile: boolean = false;

  public constructor() {
    this.mCompilerOptions = {};
    this.mTransformers = [];
  }

  public setWriteOriginalFile(flag: boolean): void {
    this.mWriteOriginalFile = flag;
  }

  // Pass the collected whitelists related to property obfuscation to Arkguard.
  public addReservedSetForPropertyObf(properties: ReseverdSetForArkguard): void {
    if (properties.structPropertySet && properties.structPropertySet.size > 0) {
      for (let reservedProperty of properties.structPropertySet) {
        UnobfuscationCollections.reservedStruct.add(reservedProperty);
      }
    }

    if (properties.stringPropertySet && properties.stringPropertySet.size > 0) {
      UnobfuscationCollections.reservedStrProp = properties.stringPropertySet;
    }

    if (properties.exportNameAndPropSet && properties.exportNameAndPropSet.size > 0) {
      UnobfuscationCollections.reservedExportNameAndProp = properties.exportNameAndPropSet;
    }

    if (properties.enumPropertySet && properties.enumPropertySet.size > 0) {
      for (let reservedEnum of properties.enumPropertySet) {
        UnobfuscationCollections.reservedEnum.add(reservedEnum);
      }
    }
  }

  public addReservedSetForDefaultObf(properties: ReseverdSetForArkguard): void {
    if (properties.exportNameSet && properties.exportNameSet.size > 0) {
      UnobfuscationCollections.reservedExportName = properties.exportNameSet;
    }
  }

  public setKeepSourceOfPaths(mKeepSourceOfPaths: Set<string>): void {
    this.mCustomProfiles.mKeepFileSourceCode.mKeepSourceOfPaths = mKeepSourceOfPaths;
  }

  public handleTsHarComments(sourceFile: SourceFile, originalPath: string | undefined): void {
    if (ArkObfuscator.projectInfo?.useTsHar && (originalPath?.endsWith(Extension.ETS) && !originalPath?.endsWith(Extension.DETS))) {
      // @ts-ignore
      sourceFile.writeTsHarComments = true;
    }
  }

  public get customProfiles(): IOptions {
    return this.mCustomProfiles;
  }

  public static get isKeptCurrentFile(): boolean {
    return ArkObfuscator.mIsKeptCurrentFile;
  }

  public static set isKeptCurrentFile(isKeptFile: boolean) {
    ArkObfuscator.mIsKeptCurrentFile = isKeptFile;
  }

  public static get projectInfo(): ProjectInfo {
    return ArkObfuscator.mProjectInfo;
  }

  public static set projectInfo(projectInfo: ProjectInfo) {
    ArkObfuscator.mProjectInfo = projectInfo;
  }

  private isCurrentFileInKeepPaths(customProfiles: IOptions, originalFilePath: string): boolean {
    const keepFileSourceCode = customProfiles.mKeepFileSourceCode;
    if (keepFileSourceCode === undefined || keepFileSourceCode.mKeepSourceOfPaths.size === 0) {
      return false;
    }
    const keepPaths: Set<string> = keepFileSourceCode.mKeepSourceOfPaths;
    const originalPath = FileUtils.toUnixPath(originalFilePath);
    return keepPaths.has(originalPath);
  }

  /**
   * init ArkObfuscator according to user config
   * should be called after constructor
   */
  public init(config: IOptions | undefined): boolean {
    if (!config) {
      console.error('obfuscation config file is not found and no given config.');
      return false;
    }

    handleReservedConfig(config, 'mRenameFileName', 'mReservedFileNames', 'mUniversalReservedFileNames');
    handleReservedConfig(config, 'mRemoveDeclarationComments', 'mReservedComments', 'mUniversalReservedComments', 'mEnable');
    this.mCustomProfiles = config;

    if (this.mCustomProfiles.mCompact) {
      this.mTextWriter = createObfTextSingleLineWriter();
    } else {
      this.mTextWriter = createTextWriter('\n');
    }

    if (this.mCustomProfiles.mEnableSourceMap) {
      this.mCompilerOptions.sourceMap = true;
    }

    const enableTopLevel: boolean = this.mCustomProfiles.mNameObfuscation?.mTopLevel;
    const exportObfuscation: boolean = this.mCustomProfiles.mExportObfuscation;
    const propertyObfuscation: boolean = this.mCustomProfiles.mNameObfuscation?.mRenameProperties;
    /**
     * clean mangledNames in case skip name check when generating names
     */
    cleanFileMangledNames = enableTopLevel && !exportObfuscation && !propertyObfuscation;

    this.initPerformancePrinter();
    // load transformers
    this.mTransformers = new TransformerManager(this.mCustomProfiles).getTransformers();

    if (needReadApiInfo(this.mCustomProfiles)) {
      // if -enable-property-obfuscation or -enable-export-obfuscation, collect language reserved keywords.
      let languageSet: Set<string> = new Set();
      for (const key of Object.keys(esInfo)) {
        const valueArray = esInfo[key];
        valueArray.forEach((element: string) => {
          languageSet.add(element);
        });
      }
      UnobfuscationCollections.reservedLangForProperty = languageSet;
    }

    return true;
  }

  private initPerformancePrinter(): void {
    if (this.mCustomProfiles.mPerformancePrinter) {
      const printConfig = this.mCustomProfiles.mPerformancePrinter;
      const printPath = printConfig.mOutputPath;

      if (printConfig.mFilesPrinter) {
        performancePrinter.filesPrinter = performancePrinter.iniPrinter;
        performancePrinter.filesPrinter.setOutputPath(printPath);
      } else {
        performancePrinter.iniPrinter = undefined;
      }

      if (printConfig.mSingleFilePrinter) {
        performancePrinter.singleFilePrinter = new TimeTracker(printPath);
      }

      if (printConfig.mSumPrinter) {
        performancePrinter.timeSumPrinter = new TimeSumPrinter(printPath);
      }
    } else {
      performancePrinter = undefined;
    }
  }

  /**
   * A Printer to output obfuscated codes.
   */
  public createObfsPrinter(isDeclarationFile: boolean): Printer {
    // set print options
    let printerOptions: PrinterOptions = {};
    let removeOption = this.mCustomProfiles.mRemoveDeclarationComments;
    let hasReservedList = removeOption?.mReservedComments?.length || removeOption?.mUniversalReservedComments?.length;
    let keepDeclarationComments = hasReservedList || !removeOption?.mEnable;

    if (isDeclarationFile && keepDeclarationComments) {
      printerOptions.removeComments = false;
    }
    if ((!isDeclarationFile && this.mCustomProfiles.mRemoveComments) || (isDeclarationFile && !keepDeclarationComments)) {
      printerOptions.removeComments = true;
    }

    return createPrinter(printerOptions);
  }

  protected isObfsIgnoreFile(fileName: string): boolean {
    let suffix: string = FileUtils.getFileExtension(fileName);

    return suffix !== 'js' && suffix !== 'ts' && suffix !== 'ets';
  }

  private convertLineBasedOnSourceMap(targetCache: string, sourceMapLink?: SourceMapLink): Map<string, string> {
    let originalCache: Map<string, string> = renameIdentifierModule.nameCache.get(targetCache);
    let updatedCache: Map<string, string> = new Map<string, string>();
    for (const [key, value] of originalCache) {
      if (!key.includes(':')) {
        // No need to save line info for identifier which is not function-like, i.e. key without ':' here.
        updatedCache[key] = value;
        continue;
      }
      const [scopeName, oldStartLine, oldStartColumn, oldEndLine, oldEndColumn] = key.split(':');
      let newKey: string = key;
      if (!sourceMapLink) {
        // In Arkguard, we save line info of source code, so do not need to use sourcemap mapping.
        newKey = `${scopeName}:${oldStartLine}:${oldEndLine}`;
        updatedCache[newKey] = value;
        continue;
      }
      const startPosition: SourceMapSegmentObj | null = sourceMapLink.traceSegment(
        // 1: The line number in originalCache starts from 1 while in source map starts from 0.
        Number(oldStartLine) - 1, Number(oldStartColumn) - 1, ''); // Minus 1 to get the correct original position.
      if (!startPosition) {
        // Do not save methods that do not exist in the source code, e.g. 'build' in ArkUI.
        continue;
      }
      const endPosition: SourceMapSegmentObj | null = sourceMapLink.traceSegment(
        Number(oldEndLine) - 1, Number(oldEndColumn) - 1, ''); // 1: Same as above.
      if (!endPosition) {
        // Do not save methods that do not exist in the source code, e.g. 'build' in ArkUI.
        continue;
      }
      const startLine = startPosition.line + 1; // 1: The final line number in updatedCache should starts from 1.
      const endLine = endPosition.line + 1; // 1: Same as above.
      newKey = `${scopeName}:${startLine}:${endLine}`;
      updatedCache[newKey] = value;
    }
    return updatedCache;
  }

  /**
   * Obfuscate ast of a file.
   * @param content ast or source code of a source file
   * @param sourceFilePathObj
   * @param previousStageSourceMap
   * @param historyNameCache
   * @param originalFilePath When filename obfuscation is enabled, it is used as the source code path.
   */
  public async obfuscate(
    content: SourceFile | string,
    sourceFilePathObj: FilePathObj,
    previousStageSourceMap?: RawSourceMap,
    historyNameCache?: Map<string, string>,
    originalFilePath?: string,
    projectInfo?: ProjectInfo,
  ): Promise<ObfuscationResultType> {
    ArkObfuscator.projectInfo = projectInfo;
    let result: ObfuscationResultType = { content: undefined };
    if (this.isObfsIgnoreFile(sourceFilePathObj.buildFilePath)) {
      // need add return value
      return result;
    }

    let ast: SourceFile = this.createAst(content, sourceFilePathObj.buildFilePath);
    if (ast.statements.length === 0) {
      return result;
    }

    if (historyNameCache && historyNameCache.size > 0 && this.mCustomProfiles.mNameObfuscation) {
      renameIdentifierModule.historyNameCache = historyNameCache;
    }

    if (this.mCustomProfiles.mUnobfuscationOption?.mPrintKeptNames) {
      let historyUnobfuscatedNames = historyAllUnobfuscatedNamesMap.get(sourceFilePathObj.relativeFilePath);
      if (historyUnobfuscatedNames) {
        renameIdentifierModule.historyUnobfuscatedNamesMap = new Map(Object.entries(historyUnobfuscatedNames));
      }
    }

    originalFilePath = originalFilePath ?? ast.fileName;
    if (this.mCustomProfiles.mRenameFileName?.mEnable) {
      orignalFilePathForSearching = originalFilePath;
    }
    ArkObfuscator.isKeptCurrentFile = this.isCurrentFileInKeepPaths(this.mCustomProfiles, originalFilePath);

    this.handleDeclarationFile(ast);

    ast = this.obfuscateAst(ast);

    this.writeObfuscationResult(ast, sourceFilePathObj.buildFilePath, result, previousStageSourceMap, originalFilePath);

    this.clearCaches();
    return result;
  }

  private createAst(content: SourceFile | string, sourceFilePath: string): SourceFile {
    performancePrinter?.singleFilePrinter?.startEvent(EventList.CREATE_AST, performancePrinter.timeSumPrinter, sourceFilePath);
    let ast: SourceFile;
    if (typeof content === 'string') {
      ast = TypeUtils.createObfSourceFile(sourceFilePath, content);
    } else {
      ast = content;
    }
    performancePrinter?.singleFilePrinter?.endEvent(EventList.CREATE_AST, performancePrinter.timeSumPrinter);

    return ast;
  }

  private obfuscateAst(ast: SourceFile): SourceFile {
    performancePrinter?.singleFilePrinter?.startEvent(EventList.OBFUSCATE_AST, performancePrinter.timeSumPrinter);
    let transformedResult: TransformationResult<Node> = transform(ast, this.mTransformers, this.mCompilerOptions);
    performancePrinter?.singleFilePrinter?.endEvent(EventList.OBFUSCATE_AST, performancePrinter.timeSumPrinter);
    ast = transformedResult.transformed[0] as SourceFile;
    return ast;
  }

  private handleDeclarationFile(ast: SourceFile): void {
    if (ast.isDeclarationFile) {
      if (!this.mCustomProfiles.mRemoveDeclarationComments || !this.mCustomProfiles.mRemoveDeclarationComments.mEnable) {
        //@ts-ignore
        ast.reservedComments = undefined;
        //@ts-ignore
        ast.universalReservedComments = undefined;
      } else {
        //@ts-ignore
        ast.reservedComments ??= this.mCustomProfiles.mRemoveDeclarationComments.mReservedComments ?
          this.mCustomProfiles.mRemoveDeclarationComments.mReservedComments : [];
        //@ts-ignore
        ast.universalReservedComments = this.mCustomProfiles.mRemoveDeclarationComments.mUniversalReservedComments ?? [];
      }
    } else {
      //@ts-ignore
      ast.reservedComments = this.mCustomProfiles.mRemoveComments ? [] : undefined;
      //@ts-ignore
      ast.universalReservedComments = this.mCustomProfiles.mRemoveComments ? [] : undefined;
    }
  }

  /**
   * write obfuscated code, sourcemap and namecache
   */
  private writeObfuscationResult(ast: SourceFile, sourceFilePath: string, result: ObfuscationResultType,
    previousStageSourceMap?: RawSourceMap, originalFilePath?: string): void {
    // convert ast to output source file and generate sourcemap if needed.
    let sourceMapGenerator: SourceMapGenerator = undefined;
    if (this.mCustomProfiles.mEnableSourceMap) {
      sourceMapGenerator = getSourceMapGenerator(sourceFilePath);
    }

    if (sourceFilePath.endsWith('.js')) {
      TypeUtils.tsToJs(ast);
    }
    this.handleTsHarComments(ast, originalFilePath);
    performancePrinter?.singleFilePrinter?.startEvent(EventList.CREATE_PRINTER, performancePrinter.timeSumPrinter);
    this.createObfsPrinter(ast.isDeclarationFile).writeFile(ast, this.mTextWriter, sourceMapGenerator);
    performancePrinter?.singleFilePrinter?.endEvent(EventList.CREATE_PRINTER, performancePrinter.timeSumPrinter);

    result.filePath = ast.fileName;
    result.content = this.mTextWriter.getText();

    if (this.mCustomProfiles.mUnobfuscationOption?.mPrintKeptNames) {
      this.handleUnobfuscationNames(result);
    }

    if (this.mCustomProfiles.mEnableSourceMap && sourceMapGenerator) {
      this.handleSourceMapAndNameCache(sourceMapGenerator, sourceFilePath, result, previousStageSourceMap);
    }
  }

  private handleUnobfuscationNames(result: ObfuscationResultType): void {
    result.unobfuscationNameMap = new Map(UnobfuscationCollections.unobfuscatedNamesMap);
  }

  private handleSourceMapAndNameCache(sourceMapGenerator: SourceMapGenerator, sourceFilePath: string,
    result: ObfuscationResultType, previousStageSourceMap?: RawSourceMap): void {
    let sourceMapJson: RawSourceMap = sourceMapGenerator.toJSON();
    sourceMapJson.sourceRoot = '';
    sourceMapJson.file = path.basename(sourceFilePath);
    if (previousStageSourceMap) {
      sourceMapJson = mergeSourceMap(previousStageSourceMap as RawSourceMap, sourceMapJson);
    }
    result.sourceMap = sourceMapJson;
    let nameCache = renameIdentifierModule.nameCache;
    if (this.mCustomProfiles.mEnableNameCache) {
      let newIdentifierCache!: Object;
      let newMemberMethodCache!: Object;
      if (previousStageSourceMap) {
        // The process in sdk, need to use sourcemap mapping.
        // 1: Only one file in the source map; 0: The first and the only one.
        const sourceFileName = previousStageSourceMap.sources?.length === 1 ? previousStageSourceMap.sources[0] : '';
        const source: Source = new Source(sourceFileName, null);
        const decodedSourceMap: ExistingDecodedSourceMap = decodeSourcemap(previousStageSourceMap);
        let sourceMapLink: SourceMapLink = new SourceMapLink(decodedSourceMap, [source]);
        newIdentifierCache = this.convertLineBasedOnSourceMap(IDENTIFIER_CACHE, sourceMapLink);
        newMemberMethodCache = this.convertLineBasedOnSourceMap(MEM_METHOD_CACHE, sourceMapLink);
      } else {
        // The process in Arkguard.
        newIdentifierCache = this.convertLineBasedOnSourceMap(IDENTIFIER_CACHE);
        newMemberMethodCache = this.convertLineBasedOnSourceMap(MEM_METHOD_CACHE);
      }
      nameCache.set(IDENTIFIER_CACHE, newIdentifierCache);
      nameCache.set(MEM_METHOD_CACHE, newMemberMethodCache);
      result.nameCache = { [IDENTIFIER_CACHE]: newIdentifierCache, [MEM_METHOD_CACHE]: newMemberMethodCache };
    }
  }

  private clearCaches(): void {
    // clear cache of text writer
    this.mTextWriter.clear();
    renameIdentifierModule.clearCaches();
    if (cleanFileMangledNames) {
      PropCollections.globalMangledTable.clear();
      PropCollections.newlyOccupiedMangledProps.clear();
    }
    UnobfuscationCollections.unobfuscatedNamesMap.clear();
  }
}
