/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

import {ListUtil} from '../../../src/utils/ListUtil';
import {describe, it} from 'mocha';
import {assert} from 'chai';
import * as fs from 'fs';
import sinon from 'sinon';
import {TypeUtils} from '../../../src/utils/TypeUtils';
import {
  Label,
  Scope,
  ScopeKind, 
  ScopeManager, 
  createLabel, 
  createScopeManager, 
  isClassScope, 
  isEnumScope, 
  isFunctionScope, 
  isGlobalScope,
  isInterfaceScope,
  isObjectLiteralScope
} from '../../../src/utils/ScopeAnalyzer';
import { ScriptTarget, SourceFile, createSourceFile, isSourceFile, Symbol, __String, Declaration, JSDocTagInfo,
   SymbolDisplayPart, SymbolFlags, TypeChecker, LabeledStatement, Identifier, SyntaxKind, 
   isIdentifier,
   factory,
   VariableStatement} from 'typescript';

describe('ScopeAnalyzer ut', function () {
  let sourceFile: SourceFile;
  
  before('init parameters', function () {
    const sourceFileContent = `
    //This is a comment
    class Demo{
      constructor(public  title: string, public  content: string, public  mark: number) {
        this.title = title
        this.content = content
        this.mark = mark
      }
    }
    `;

    sourceFile = createSourceFile('sourceFile.ts', sourceFileContent, ScriptTarget.ES2015, true);
  });

  describe('unit test for isGlobalScope', function () {
    it('is Global Scope', function () {
      let curScope = new Scope('curScope', sourceFile, ScopeKind.GLOBAL);
      assert.isTrue(isGlobalScope(curScope));
    });
  
    it('is not Global Scope', function () {
      let curScope = new Scope('curScope', sourceFile, ScopeKind.MODULE);
      assert.isFalse(isGlobalScope(curScope));
    });
  });

  describe('unit test for isFunctionScope', function () {
    it('is Function Scope', function () {
      let curScope = new Scope('curScope', sourceFile, ScopeKind.FUNCTION);
      assert.isTrue(isFunctionScope(curScope));
    });
  
    it('is not Function Scope', function () {
      let curScope = new Scope('curScope', sourceFile, ScopeKind.MODULE);
      assert.isFalse(isFunctionScope(curScope));
    });
  });

  describe('unit test for isClassScope', function () {
    it('is Class Scope', function () {
      let curScope = new Scope('curScope', sourceFile, ScopeKind.CLASS);
      assert.isTrue(isClassScope(curScope));
    });
  
    it('is not Class Scope', function () {
    let curScope = new Scope('curScope', sourceFile, ScopeKind.MODULE);
      assert.isFalse(isClassScope(curScope));
    });
  });

  describe('unit test for isInterfaceScope', function () {
    it('is Interface Scope', function () {
      let curScope = new Scope('curScope', sourceFile, ScopeKind.INTERFACE);
      assert.isTrue(isInterfaceScope(curScope));
    });
  
    it('is not Interface Scope', function () {
    let curScope = new Scope('curScope', sourceFile, ScopeKind.MODULE);
      assert.isFalse(isInterfaceScope(curScope));
    });
  });

  describe('unit test for isEnumScope', function () {
    it('is Enum Scope', function () {
      let curScope = new Scope('curScope', sourceFile, ScopeKind.ENUM);
      assert.isTrue(isEnumScope(curScope));
    });
  
    it('is not Enum Scope', function () {
    let curScope = new Scope('curScope', sourceFile, ScopeKind.MODULE);
      assert.isFalse(isEnumScope(curScope));
    });
  });

  describe('unit test for isObjectLiteralScope', function () {
    it('is ObjectLiteral Scope', function () {
      let curScope = new Scope('curScope', sourceFile, ScopeKind.OBJECT_LITERAL);
      assert.isTrue(isObjectLiteralScope(curScope));
    });
  
    it('is not ObjectLiteral Scope', function () {
    let curScope = new Scope('curScope', sourceFile , ScopeKind.MODULE);
      assert.isFalse(isObjectLiteralScope(curScope));
    });
  });

  describe('unit test for Scope', function () {
    describe('constructor', function () {
      it('init', function () {
        let curScope = new Scope('curScope', sourceFile, ScopeKind.GLOBAL);
        assert.equal(curScope.name, 'curScope');
        assert.equal(curScope.kind, ScopeKind.GLOBAL);
        assert.equal(curScope.block, sourceFile);
        assert.equal(curScope.parent, undefined);
        assert.deepEqual(curScope.children, []);
        assert.deepEqual(curScope.defs, new Set());
        assert.deepEqual(curScope.labels, []);
        assert.deepEqual(curScope.importNames, new Set());
        assert.deepEqual(curScope.exportNames, new Set());
        assert.deepEqual(curScope.mangledNames, new Set());
        assert.equal(curScope.loc, 'curScope');
      });
    });

    describe('addChild', function () {
      it('push', function () {
        let parentScope = new Scope('parentScope', sourceFile, ScopeKind.FUNCTION);
        let childScope = new Scope('childScope', sourceFile, ScopeKind.BLOCK, false, parentScope);
        assert.include(parentScope.children, childScope);
      });
    });

    describe('addDefinition', function () {
      it('should add definition symbol to the current scope', function () {
        let curScope = new Scope('curScope', sourceFile, ScopeKind.GLOBAL);
        let symbol: Symbol = Object(Symbol('testSymbol'));

        curScope.addDefinition(symbol, false);
        assert.isTrue(curScope.defs.has(symbol));
      });
  
      it('should mark symbol as obfuscateAsProperty if required', function () {
        let curScope = new Scope('curScope', sourceFile, ScopeKind.GLOBAL);
        let symbol: Symbol = Object(Symbol('testSymbol'));
  
        curScope.addDefinition(symbol, true);
        assert.isTrue(Reflect.get(symbol, 'obfuscateAsProperty'));
      });
    });

    describe('addLabel', function () {
      it('should add a label to the current scope', function () {
        let curScope = new Scope('curScope', sourceFile, ScopeKind.GLOBAL);
        let label: Label = {name: 'testLabel', locInfo: 'locInfo', refs: [], parent: undefined, children: [], scope: curScope};
  
        curScope.addLabel(label);
        assert.include(curScope.labels, label);
      });
    });

    describe('getSymbolLocation', function () {
      it('should return the correct location if symbol exists', function () {
        let curScope = new Scope('curScope', sourceFile, ScopeKind.GLOBAL);
        let symbol: Symbol = {
          flags: 0,
          escapedName: 'testSymbol' as __String,
          declarations: [],
          valueDeclaration: undefined,
          members: undefined,
          exports: undefined,
          globalExports: undefined,
          exportSymbol: undefined,
          name: 'testSymbol',
          getFlags: function (): SymbolFlags {
            throw new Error('Function not implemented.');
          },
          getEscapedName: function (): __String {
            throw new Error('Function not implemented.');
          },
          getName: function (): string {
            throw new Error('Function not implemented.');
          },
          getDeclarations: function (): Declaration[] | undefined {
            throw new Error('Function not implemented.');
          },
          getDocumentationComment: function (typeChecker: TypeChecker | undefined): SymbolDisplayPart[] {
            throw new Error('Function not implemented.');
          },
          getJsDocTags: function (checker?: TypeChecker): JSDocTagInfo[] {
            throw new Error('Function not implemented.');
          }
        };
        curScope.addDefinition(symbol);
        assert.equal(curScope.getSymbolLocation(symbol), 'testSymbol');
      });
  
      it('should return an empty string if symbol does not exist', function () {
        let curScope = new Scope('curScope', sourceFile, ScopeKind.GLOBAL);
        let symbol: Symbol = Object(Symbol('nonExistentSymbol'));

        assert.equal(curScope.getSymbolLocation(symbol), '');
      });
    });

    describe('getLabelLocation', function () {
      it('should return the correct location if label exists', function () {
        let curScope = new Scope('curScope', sourceFile, ScopeKind.GLOBAL);
        let label: Label = {name: 'testLabel', locInfo: 'locInfo', refs: [], parent: undefined, children: [], scope: curScope};
  
        curScope.addLabel(label);
        assert.equal(curScope.getLabelLocation(label), 'testLabel');
      });
  
      it('should return an empty string if label does not exist', function () {
        let curScope = new Scope('curScope', sourceFile, ScopeKind.GLOBAL);
        let label: Label = {name: 'nonExistentLabel', locInfo: 'locInfo', refs: [], parent: undefined, children: [], scope: curScope};
  
        assert.equal(curScope.getLabelLocation(label), '');
      });
    });
  });

  describe('unit test for createLabel', function () {
    const sourceFileContent = `
    label: console.log('This is a labeled statement');
    `;
    sourceFile = createSourceFile('test.ts', sourceFileContent, ScriptTarget.ES2015, true);
    let labeledStatement: LabeledStatement = sourceFile.statements[0] as LabeledStatement;
    let curScope = new Scope('curScope', sourceFile, ScopeKind.GLOBAL);
  
    it('should create a label and add it to the scope', function () {
      const label = createLabel(labeledStatement, curScope);
  
      assert.equal(label.name, labeledStatement.label.text);
      assert.equal(label.locInfo, `$0_${labeledStatement.label.text}`);
      assert.deepEqual(label.refs, [labeledStatement.label]);
      assert.equal(label.parent, undefined);
      assert.deepEqual(label.children, []);
      assert.equal(label.scope, curScope);
      assert.include(curScope.labels, label);
    });
  
    it('should create a label with a parent and add it to the parent\'s children', function () {
      const parentLabel: Label = {
        name: 'parentLabel',
        locInfo: 'parentLocInfo',
        refs: [],
        parent: undefined,
        children: [],
        scope: curScope
      };
      const label = createLabel(labeledStatement, curScope, parentLabel);
  
      assert.equal(label.name, labeledStatement.label.text);
      assert.equal(label.locInfo, `$1_${labeledStatement.label.text}`);
      assert.deepEqual(label.refs, [labeledStatement.label]);
      assert.equal(label.parent, parentLabel);
      assert.deepEqual(label.children, []);
      assert.equal(label.scope, curScope);
      assert.include(curScope.labels, label);
      assert.include(parentLabel.children, label);
    });
  });

  describe('unit test for createScopeManager', function () {
    let scopeManager: ScopeManager;
    let sourceFile: SourceFile;
    let checker: TypeChecker;

    function InitScopeManager(newFilePath: string): void {
      const fileContent = fs.readFileSync(newFilePath, 'utf8');
      sourceFile = createSourceFile(newFilePath, fileContent, ScriptTarget.ES2015, true);
      checker = TypeUtils.createChecker(sourceFile);
      scopeManager = createScopeManager();
      scopeManager.analyze(sourceFile, checker, false);
    }
  
    describe('analyze', function () {
      describe('analyzeModule', function () {
        let filePath = 'test/ut/utils/ScopeAnalyzer/analyzeModule.ts';
        InitScopeManager(filePath);
        it('getReservedNames', function () {
          const reservedNames = scopeManager.getReservedNames();
          assert.strictEqual(reservedNames.size === 0, true);
        });
        it('getRootScope', function () {
          const rootScope = scopeManager.getRootScope();
          assert.strictEqual(rootScope.fileExportNames?.size !== 0, true);
        });
        describe('getScopeOfNode', function () {
          it('node is not identifier', function () {
            const node: SourceFile = sourceFile;
            const scope = scopeManager.getScopeOfNode(node);
            assert.strictEqual(scope, undefined);
          });
    
          it('node with no symbol', function () {
            const node: Identifier = factory.createIdentifier('noSymbolIdentifier');
            const scope = scopeManager.getScopeOfNode(node);
            assert.strictEqual(scope, undefined);
          });
    
          it('get the scope of node', function () {
            const classDeclaration = sourceFile.statements.find(node => node && node.kind === SyntaxKind.ClassDeclaration);
            if (!classDeclaration) throw new Error('ClassDeclaration not found');
            const node: Identifier = (classDeclaration as any)?.name;
            const scope = scopeManager.getScopeOfNode(node);
            assert.strictEqual(scope?.name, '');
          });
    
          it('get no scope of node', function () {
            const anotherSourceFile = createSourceFile('another.ts', 'let y = 2;', ScriptTarget.ES2015, true);
            const node = (anotherSourceFile.statements[0] as VariableStatement).declarationList.declarations[0].name;
            const scope = scopeManager.getScopeOfNode(node);
            assert.strictEqual(scope, undefined);
          });
        });
      });

      describe('analyzeFunctionLike', function () {
        let filePath = 'test/ut/utils/ScopeAnalyzer/analyzeFunctionLike.ts';
        InitScopeManager(filePath);
        it('getReservedNames', function () {
          const reservedNames = scopeManager.getReservedNames();
          assert.strictEqual(reservedNames.size === 0, true);
        });
        it('getRootScope', function () {
          const rootScope = scopeManager.getRootScope();
          assert.strictEqual(rootScope.fileExportNames?.size !== 0, true);
        });
        describe('getScopeOfNode', function () {
          it('node is not identifier', function () {
            const node: SourceFile = sourceFile;
            const scope = scopeManager.getScopeOfNode(node);
            assert.strictEqual(scope, undefined);
          });
    
          it('node with no symbol', function () {
            const node: Identifier = factory.createIdentifier('noSymbolIdentifier');
            const scope = scopeManager.getScopeOfNode(node);
            assert.strictEqual(scope, undefined);
          });
    
          it('get the scope of node', function () {
            const functionDeclaration = sourceFile.statements.find(node => node && node.kind === SyntaxKind.FunctionDeclaration);
            if (!functionDeclaration) throw new Error('FunctionDeclaration not found');
            const node: Identifier = (functionDeclaration as any)?.name;
            const scope = scopeManager.getScopeOfNode(node);
            assert.strictEqual(scope?.name, '');
          });
    
          it('get no scope of node', function () {
            const anotherSourceFile = createSourceFile('another.ts', 'let y = 2;', ScriptTarget.ES2015, true);
            const node = (anotherSourceFile.statements[0] as VariableStatement).declarationList.declarations[0].name;
            const scope = scopeManager.getScopeOfNode(node);
            assert.strictEqual(scope, undefined);
          });
        });
      });

      describe('analyzeExportNames', function () {
        let filePath = 'test/ut/utils/ScopeAnalyzer/analyzeExportNames.ts';
        InitScopeManager(filePath);
        it('getReservedNames', function () {
          const reservedNames = scopeManager.getReservedNames();
          assert.strictEqual(reservedNames.size === 0, true);
        });
        it('getRootScope', function () {
          const rootScope = scopeManager.getRootScope();
          assert.strictEqual(rootScope.fileExportNames?.size !== 0, true);
        });
        describe('getScopeOfNode', function () {
          it('node is not identifier', function () {
            const node: SourceFile = sourceFile;
            const scope = scopeManager.getScopeOfNode(node);
            assert.strictEqual(scope, undefined);
          });
    
          it('node with no symbol', function () {
            const node: Identifier = factory.createIdentifier('noSymbolIdentifier');
            const scope = scopeManager.getScopeOfNode(node);
            assert.strictEqual(scope, undefined);
          });

          it('get the scope of node', function () {
            const exportDeclaration = sourceFile.statements.find(node => node && node.kind === SyntaxKind.ExportDeclaration);
            if (!exportDeclaration) throw new Error('ExportDeclaration not found');
        
            const exportClause = (exportDeclaration as any).exportClause;
            if (!exportClause || !exportClause.elements) throw new Error('ExportSpecifier not found');
        
            const exportSpecifier = exportClause.elements[0];
            const node: Identifier = exportSpecifier.name;
        
            const scope = scopeManager.getScopeOfNode(node);
            assert.strictEqual(scope?.name, '');
          });
          it('get no scope of node', function () {
            const anotherSourceFile = createSourceFile('another.ts', 'let y = 2;', ScriptTarget.ES2015, true);
            const node = (anotherSourceFile.statements[0] as VariableStatement).declarationList.declarations[0].name;
            const scope = scopeManager.getScopeOfNode(node);
            assert.strictEqual(scope, undefined);
          });
        });
      });
    });
  });
});


