/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import {
    global,
    passNode,
    passNodeArray,
    unpackNonNullableNode,
    unpackNode,
    unpackNodeArray,
    assertValidPeer,
    AstNode,
    KNativePointer,
    nodeByType,
    ArktsObject,
    isSameNativeObject,
    unpackString,
    updateNodeByNode
} from "../reexport-for-generated"

import { AnnotationDeclaration } from "./peers/AnnotationDeclaration"
import { AnnotationUsage } from "./peers/AnnotationUsage"
import { ArrowFunctionExpression } from "./peers/ArrowFunctionExpression"
import { AssertStatement } from "./peers/AssertStatement"
import { AwaitExpression } from "./peers/AwaitExpression"
import { BigIntLiteral } from "./peers/BigIntLiteral"
import { BinaryExpression } from "./peers/BinaryExpression"
import { BlockExpression } from "./peers/BlockExpression"
import { BlockStatement } from "./peers/BlockStatement"
import { BooleanLiteral } from "./peers/BooleanLiteral"
import { BreakStatement } from "./peers/BreakStatement"
import { CatchClause } from "./peers/CatchClause"
import { ChainExpression } from "./peers/ChainExpression"
import { CharLiteral } from "./peers/CharLiteral"
import { ClassDeclaration } from "./peers/ClassDeclaration"
import { ClassDefinition } from "./peers/ClassDefinition"
import { ClassExpression } from "./peers/ClassExpression"
import { ConditionalExpression } from "./peers/ConditionalExpression"
import { ContinueStatement } from "./peers/ContinueStatement"
import { DebuggerStatement } from "./peers/DebuggerStatement"
import { Decorator } from "./peers/Decorator"
import { DoWhileStatement } from "./peers/DoWhileStatement"
import { ETSClassLiteral } from "./peers/ETSClassLiteral"
import { ETSKeyofType } from "./peers/ETSKeyofType"
import { ETSNewArrayInstanceExpression } from "./peers/ETSNewArrayInstanceExpression"
import { ETSNewClassInstanceExpression } from "./peers/ETSNewClassInstanceExpression"
import { ETSNewMultiDimArrayInstanceExpression } from "./peers/ETSNewMultiDimArrayInstanceExpression"
import { ETSNullType } from "./peers/ETSNullType"
import { ETSPrimitiveType } from "./peers/ETSPrimitiveType"
import { ETSTypeReference } from "./peers/ETSTypeReference"
import { ETSTypeReferencePart } from "./peers/ETSTypeReferencePart"
import { ETSUndefinedType } from "./peers/ETSUndefinedType"
import { ETSUnionType } from "./peers/ETSUnionType"
import { EmptyStatement } from "./peers/EmptyStatement"
import { Es2pandaMetaPropertyKind } from "./Es2pandaEnums"
import { Es2pandaPrimitiveType } from "./Es2pandaEnums"
import { Es2pandaPropertyKind } from "./Es2pandaEnums"
import { Es2pandaTokenType } from "./Es2pandaEnums"
import { Es2pandaVariableDeclarationKind } from "./Es2pandaEnums"
import { ExportAllDeclaration } from "./peers/ExportAllDeclaration"
import { ExportDefaultDeclaration } from "./peers/ExportDefaultDeclaration"
import { ExportSpecifier } from "./peers/ExportSpecifier"
import { Expression } from "./peers/Expression"
import { ExpressionStatement } from "./peers/ExpressionStatement"
import { ForInStatement } from "./peers/ForInStatement"
import { ForOfStatement } from "./peers/ForOfStatement"
import { ForUpdateStatement } from "./peers/ForUpdateStatement"
import { FunctionDeclaration } from "./peers/FunctionDeclaration"
import { FunctionExpression } from "./peers/FunctionExpression"
import { FunctionSignature } from "./peers/FunctionSignature"
import { Identifier } from "./peers/Identifier"
import { IfStatement } from "./peers/IfStatement"
import { ImportDefaultSpecifier } from "./peers/ImportDefaultSpecifier"
import { ImportExpression } from "./peers/ImportExpression"
import { ImportNamespaceSpecifier } from "./peers/ImportNamespaceSpecifier"
import { ImportSpecifier } from "./peers/ImportSpecifier"
import { LabelledStatement } from "./peers/LabelledStatement"
import { MetaProperty } from "./peers/MetaProperty"
import { NamedType } from "./peers/NamedType"
import { NewExpression } from "./peers/NewExpression"
import { NullLiteral } from "./peers/NullLiteral"
import { OmittedExpression } from "./peers/OmittedExpression"
import { OpaqueTypeNode } from "./peers/OpaqueTypeNode"
import { PrefixAssertionExpression } from "./peers/PrefixAssertionExpression"
import { Property } from "./peers/Property"
import { ReturnStatement } from "./peers/ReturnStatement"
import { ScriptFunction } from "./peers/ScriptFunction"
import { SequenceExpression } from "./peers/SequenceExpression"
import { Statement } from "./peers/Statement"
import { StringLiteral } from "./peers/StringLiteral"
import { SuperExpression } from "./peers/SuperExpression"
import { SwitchCaseStatement } from "./peers/SwitchCaseStatement"
import { SwitchStatement } from "./peers/SwitchStatement"
import { TSAnyKeyword } from "./peers/TSAnyKeyword"
import { TSArrayType } from "./peers/TSArrayType"
import { TSAsExpression } from "./peers/TSAsExpression"
import { TSBigintKeyword } from "./peers/TSBigintKeyword"
import { TSBooleanKeyword } from "./peers/TSBooleanKeyword"
import { TSClassImplements } from "./peers/TSClassImplements"
import { TSConditionalType } from "./peers/TSConditionalType"
import { TSEnumMember } from "./peers/TSEnumMember"
import { TSExternalModuleReference } from "./peers/TSExternalModuleReference"
import { TSImportEqualsDeclaration } from "./peers/TSImportEqualsDeclaration"
import { TSImportType } from "./peers/TSImportType"
import { TSIndexSignature } from "./peers/TSIndexSignature"
import { TSIndexedAccessType } from "./peers/TSIndexedAccessType"
import { TSInferType } from "./peers/TSInferType"
import { TSInterfaceBody } from "./peers/TSInterfaceBody"
import { TSInterfaceHeritage } from "./peers/TSInterfaceHeritage"
import { TSIntersectionType } from "./peers/TSIntersectionType"
import { TSLiteralType } from "./peers/TSLiteralType"
import { TSModuleBlock } from "./peers/TSModuleBlock"
import { TSNamedTupleMember } from "./peers/TSNamedTupleMember"
import { TSNeverKeyword } from "./peers/TSNeverKeyword"
import { TSNonNullExpression } from "./peers/TSNonNullExpression"
import { TSNullKeyword } from "./peers/TSNullKeyword"
import { TSNumberKeyword } from "./peers/TSNumberKeyword"
import { TSObjectKeyword } from "./peers/TSObjectKeyword"
import { TSQualifiedName } from "./peers/TSQualifiedName"
import { TSStringKeyword } from "./peers/TSStringKeyword"
import { TSThisType } from "./peers/TSThisType"
import { TSTupleType } from "./peers/TSTupleType"
import { TSTypeAliasDeclaration } from "./peers/TSTypeAliasDeclaration"
import { TSTypeAssertion } from "./peers/TSTypeAssertion"
import { TSTypeLiteral } from "./peers/TSTypeLiteral"
import { TSTypeParameter } from "./peers/TSTypeParameter"
import { TSTypeParameterDeclaration } from "./peers/TSTypeParameterDeclaration"
import { TSTypeParameterInstantiation } from "./peers/TSTypeParameterInstantiation"
import { TSTypePredicate } from "./peers/TSTypePredicate"
import { TSTypeQuery } from "./peers/TSTypeQuery"
import { TSTypeReference } from "./peers/TSTypeReference"
import { TSUndefinedKeyword } from "./peers/TSUndefinedKeyword"
import { TSUnionType } from "./peers/TSUnionType"
import { TSUnknownKeyword } from "./peers/TSUnknownKeyword"
import { TSVoidKeyword } from "./peers/TSVoidKeyword"
import { TaggedTemplateExpression } from "./peers/TaggedTemplateExpression"
import { TemplateElement } from "./peers/TemplateElement"
import { TemplateLiteral } from "./peers/TemplateLiteral"
import { ThisExpression } from "./peers/ThisExpression"
import { ThrowStatement } from "./peers/ThrowStatement"
import { TypeNode } from "./peers/TypeNode"
import { TypeofExpression } from "./peers/TypeofExpression"
import { UnaryExpression } from "./peers/UnaryExpression"
import { UndefinedLiteral } from "./peers/UndefinedLiteral"
import { UpdateExpression } from "./peers/UpdateExpression"
import { VariableDeclaration } from "./peers/VariableDeclaration"
import { VariableDeclarator } from "./peers/VariableDeclarator"
import { WhileStatement } from "./peers/WhileStatement"
import { YieldExpression } from "./peers/YieldExpression"
export const factory = {
    createLabelledStatement(ident?: Identifier, body?: Statement): LabelledStatement {
        return LabelledStatement.createLabelledStatement(ident, body)
    }
    ,
    updateLabelledStatement(original: LabelledStatement, ident?: Identifier, body?: Statement): LabelledStatement {
        if (isSameNativeObject(ident, original.ident) && isSameNativeObject(body, original.body))
            return original
        return updateNodeByNode(LabelledStatement.createLabelledStatement(ident, body), original)
    }
    ,
    createThrowStatement(argument?: Expression): ThrowStatement {
        return ThrowStatement.createThrowStatement(argument)
    }
    ,
    updateThrowStatement(original: ThrowStatement, argument?: Expression): ThrowStatement {
        if (isSameNativeObject(argument, original.argument))
            return original
        return updateNodeByNode(ThrowStatement.createThrowStatement(argument), original)
    }
    ,
    createTSVoidKeyword(): TSVoidKeyword {
        return TSVoidKeyword.createTSVoidKeyword()
    }
    ,
    updateTSVoidKeyword(original: TSVoidKeyword): TSVoidKeyword {
        return updateNodeByNode(TSVoidKeyword.createTSVoidKeyword(), original)
    }
    ,
    createIfStatement(test?: Expression, consequent?: Statement, alternate?: Statement): IfStatement {
        return IfStatement.createIfStatement(test, consequent, alternate)
    }
    ,
    updateIfStatement(original: IfStatement, test?: Expression, consequent?: Statement, alternate?: Statement): IfStatement {
        if (isSameNativeObject(test, original.test) && isSameNativeObject(consequent, original.consequent) && isSameNativeObject(alternate, original.alternate))
            return original
        return updateNodeByNode(IfStatement.createIfStatement(test, consequent, alternate), original)
    }
    ,
    createDecorator(expr?: Expression): Decorator {
        return Decorator.createDecorator(expr)
    }
    ,
    updateDecorator(original: Decorator, expr?: Expression): Decorator {
        if (isSameNativeObject(expr, original.expr))
            return original
        return updateNodeByNode(Decorator.createDecorator(expr), original)
    }
    ,
    createTSNeverKeyword(): TSNeverKeyword {
        return TSNeverKeyword.createTSNeverKeyword()
    }
    ,
    updateTSNeverKeyword(original: TSNeverKeyword): TSNeverKeyword {
        return updateNodeByNode(TSNeverKeyword.createTSNeverKeyword(), original)
    }
    ,
    createImportDefaultSpecifier(local?: Identifier): ImportDefaultSpecifier {
        return ImportDefaultSpecifier.createImportDefaultSpecifier(local)
    }
    ,
    updateImportDefaultSpecifier(original: ImportDefaultSpecifier, local?: Identifier): ImportDefaultSpecifier {
        if (isSameNativeObject(local, original.local))
            return original
        return updateNodeByNode(ImportDefaultSpecifier.createImportDefaultSpecifier(local), original)
    }
    ,
    createImportSpecifier(imported?: Identifier, local?: Identifier): ImportSpecifier {
        return ImportSpecifier.createImportSpecifier(imported, local)
    }
    ,
    updateImportSpecifier(original: ImportSpecifier, imported?: Identifier, local?: Identifier): ImportSpecifier {
        if (isSameNativeObject(imported, original.imported) && isSameNativeObject(local, original.local))
            return original
        return updateNodeByNode(ImportSpecifier.createImportSpecifier(imported, local), original)
    }
    ,
    createConditionalExpression(test?: Expression, consequent?: Expression, alternate?: Expression): ConditionalExpression {
        return ConditionalExpression.createConditionalExpression(test, consequent, alternate)
    }
    ,
    updateConditionalExpression(original: ConditionalExpression, test?: Expression, consequent?: Expression, alternate?: Expression): ConditionalExpression {
        if (isSameNativeObject(test, original.test) && isSameNativeObject(consequent, original.consequent) && isSameNativeObject(alternate, original.alternate))
            return original
        return updateNodeByNode(ConditionalExpression.createConditionalExpression(test, consequent, alternate), original)
    }
    ,
    createBigIntLiteral(str: string): BigIntLiteral {
        return BigIntLiteral.createBigIntLiteral(str)
    }
    ,
    updateBigIntLiteral(original: BigIntLiteral, str: string): BigIntLiteral {
        if (isSameNativeObject(str, original.str))
            return original
        return updateNodeByNode(BigIntLiteral.createBigIntLiteral(str), original)
    }
    ,
    createTSImportType(param: Expression | undefined, typeParams: TSTypeParameterInstantiation | undefined, qualifier: Expression | undefined, isTypeof: boolean): TSImportType {
        return TSImportType.createTSImportType(param, typeParams, qualifier, isTypeof)
    }
    ,
    updateTSImportType(original: TSImportType, param: Expression | undefined, typeParams: TSTypeParameterInstantiation | undefined, qualifier: Expression | undefined, isTypeof: boolean): TSImportType {
        if (isSameNativeObject(param, original.param) && isSameNativeObject(typeParams, original.typeParams) && isSameNativeObject(qualifier, original.qualifier) && isSameNativeObject(isTypeof, original.isTypeof))
            return original
        return updateNodeByNode(TSImportType.createTSImportType(param, typeParams, qualifier, isTypeof), original)
    }
    ,
    createTaggedTemplateExpression(tag?: Expression, quasi?: TemplateLiteral, typeParams?: TSTypeParameterInstantiation): TaggedTemplateExpression {
        return TaggedTemplateExpression.createTaggedTemplateExpression(tag, quasi, typeParams)
    }
    ,
    updateTaggedTemplateExpression(original: TaggedTemplateExpression, tag?: Expression, quasi?: TemplateLiteral, typeParams?: TSTypeParameterInstantiation): TaggedTemplateExpression {
        if (isSameNativeObject(tag, original.tag) && isSameNativeObject(quasi, original.quasi) && isSameNativeObject(typeParams, original.typeParams))
            return original
        return updateNodeByNode(TaggedTemplateExpression.createTaggedTemplateExpression(tag, quasi, typeParams), original)
    }
    ,
    createFunctionDeclaration(_function: ScriptFunction | undefined, annotations: readonly AnnotationUsage[], isAnonymous: boolean): FunctionDeclaration {
        return FunctionDeclaration.createFunctionDeclaration(_function, annotations, isAnonymous)
    }
    ,
    updateFunctionDeclaration(original: FunctionDeclaration, _function: ScriptFunction | undefined, annotations: readonly AnnotationUsage[], isAnonymous: boolean): FunctionDeclaration {
        if (isSameNativeObject(_function, original.function) && isSameNativeObject(annotations, original.annotations) && isSameNativeObject(isAnonymous, original.isAnonymous))
            return original
        return updateNodeByNode(FunctionDeclaration.createFunctionDeclaration(_function, annotations, isAnonymous), original)
    }
    ,
    createETSTypeReference(part?: ETSTypeReferencePart): ETSTypeReference {
        return ETSTypeReference.createETSTypeReference(part)
    }
    ,
    updateETSTypeReference(original: ETSTypeReference, part?: ETSTypeReferencePart): ETSTypeReference {
        if (isSameNativeObject(part, original.part))
            return original
        return updateNodeByNode(ETSTypeReference.createETSTypeReference(part), original)
    }
    ,
    createTSTypeReference(typeName?: Expression, typeParams?: TSTypeParameterInstantiation): TSTypeReference {
        return TSTypeReference.createTSTypeReference(typeName, typeParams)
    }
    ,
    updateTSTypeReference(original: TSTypeReference, typeName?: Expression, typeParams?: TSTypeParameterInstantiation): TSTypeReference {
        if (isSameNativeObject(typeName, original.typeName) && isSameNativeObject(typeParams, original.typeParams))
            return original
        return updateNodeByNode(TSTypeReference.createTSTypeReference(typeName, typeParams), original)
    }
    ,
    createNamedType(name?: Identifier): NamedType {
        return NamedType.createNamedType(name)
    }
    ,
    updateNamedType(original: NamedType, name?: Identifier): NamedType {
        if (isSameNativeObject(name, original.name))
            return original
        return updateNodeByNode(NamedType.createNamedType(name), original)
    }
    ,
    createTemplateElement(raw: string, cooked: string): TemplateElement {
        return TemplateElement.create1TemplateElement(raw, cooked)
    }
    ,
    updateTemplateElement(original: TemplateElement, raw: string, cooked: string): TemplateElement {
        if (isSameNativeObject(raw, original.raw) && isSameNativeObject(cooked, original.cooked))
            return original
        return updateNodeByNode(TemplateElement.create1TemplateElement(raw, cooked), original)
    }
    ,
    createVariableDeclaration(kind: Es2pandaVariableDeclarationKind, declarators: readonly VariableDeclarator[]): VariableDeclaration {
        return VariableDeclaration.createVariableDeclaration(kind, declarators)
    }
    ,
    updateVariableDeclaration(original: VariableDeclaration, kind: Es2pandaVariableDeclarationKind, declarators: readonly VariableDeclarator[]): VariableDeclaration {
        if (isSameNativeObject(kind, original.kind) && isSameNativeObject(declarators, original.declarators))
            return original
        return updateNodeByNode(VariableDeclaration.createVariableDeclaration(kind, declarators), original)
    }
    ,
    createUndefinedLiteral(): UndefinedLiteral {
        return UndefinedLiteral.createUndefinedLiteral()
    }
    ,
    updateUndefinedLiteral(original: UndefinedLiteral): UndefinedLiteral {
        return updateNodeByNode(UndefinedLiteral.createUndefinedLiteral(), original)
    }
    ,
    createTSClassImplements(expr?: Expression, typeParameters?: TSTypeParameterInstantiation): TSClassImplements {
        return TSClassImplements.createTSClassImplements(expr, typeParameters)
    }
    ,
    updateTSClassImplements(original: TSClassImplements, expr?: Expression, typeParameters?: TSTypeParameterInstantiation): TSClassImplements {
        if (isSameNativeObject(expr, original.expr) && isSameNativeObject(typeParameters, original.typeParameters))
            return original
        return updateNodeByNode(TSClassImplements.createTSClassImplements(expr, typeParameters), original)
    }
    ,
    createTSObjectKeyword(): TSObjectKeyword {
        return TSObjectKeyword.createTSObjectKeyword()
    }
    ,
    updateTSObjectKeyword(original: TSObjectKeyword): TSObjectKeyword {
        return updateNodeByNode(TSObjectKeyword.createTSObjectKeyword(), original)
    }
    ,
    createETSUnionType(types: readonly TypeNode[]): ETSUnionType {
        return ETSUnionType.createETSUnionType(types)
    }
    ,
    updateETSUnionType(original: ETSUnionType, types: readonly TypeNode[]): ETSUnionType {
        if (isSameNativeObject(types, original.types))
            return original
        return updateNodeByNode(ETSUnionType.createETSUnionType(types), original)
    }
    ,
    createETSKeyofType(typeRef?: TypeNode): ETSKeyofType {
        return ETSKeyofType.createETSKeyofType(typeRef)
    }
    ,
    updateETSKeyofType(original: ETSKeyofType, typeRef?: TypeNode): ETSKeyofType {
        if (isSameNativeObject(typeRef, original.typeRef))
            return original
        return updateNodeByNode(ETSKeyofType.createETSKeyofType(typeRef), original)
    }
    ,
    createTSConditionalType(checkType?: Expression, extendsType?: Expression, trueType?: Expression, falseType?: Expression): TSConditionalType {
        return TSConditionalType.createTSConditionalType(checkType, extendsType, trueType, falseType)
    }
    ,
    updateTSConditionalType(original: TSConditionalType, checkType?: Expression, extendsType?: Expression, trueType?: Expression, falseType?: Expression): TSConditionalType {
        if (isSameNativeObject(checkType, original.checkType) && isSameNativeObject(extendsType, original.extendsType) && isSameNativeObject(trueType, original.trueType) && isSameNativeObject(falseType, original.falseType))
            return original
        return updateNodeByNode(TSConditionalType.createTSConditionalType(checkType, extendsType, trueType, falseType), original)
    }
    ,
    createTSLiteralType(literal?: Expression): TSLiteralType {
        return TSLiteralType.createTSLiteralType(literal)
    }
    ,
    updateTSLiteralType(original: TSLiteralType, literal?: Expression): TSLiteralType {
        if (isSameNativeObject(literal, original.literal))
            return original
        return updateNodeByNode(TSLiteralType.createTSLiteralType(literal), original)
    }
    ,
    createTSTypeAliasDeclaration(id: Identifier | undefined, typeParams: TSTypeParameterDeclaration | undefined, typeAnnotation: TypeNode): TSTypeAliasDeclaration {
        return TSTypeAliasDeclaration.createTSTypeAliasDeclaration(id, typeParams, typeAnnotation)
    }
    ,
    updateTSTypeAliasDeclaration(original: TSTypeAliasDeclaration, id: Identifier | undefined, typeParams: TSTypeParameterDeclaration | undefined, typeAnnotation: TypeNode): TSTypeAliasDeclaration {
        if (isSameNativeObject(id, original.id) && isSameNativeObject(typeParams, original.typeParams) && isSameNativeObject(typeAnnotation, original.typeAnnotation))
            return original
        return updateNodeByNode(TSTypeAliasDeclaration.createTSTypeAliasDeclaration(id, typeParams, typeAnnotation), original)
    }
    ,
    createDebuggerStatement(): DebuggerStatement {
        return DebuggerStatement.createDebuggerStatement()
    }
    ,
    updateDebuggerStatement(original: DebuggerStatement): DebuggerStatement {
        return updateNodeByNode(DebuggerStatement.createDebuggerStatement(), original)
    }
    ,
    createReturnStatement(argument?: Expression): ReturnStatement {
        return ReturnStatement.create1ReturnStatement(argument)
    }
    ,
    updateReturnStatement(original: ReturnStatement, argument?: Expression): ReturnStatement {
        if (isSameNativeObject(argument, original.argument))
            return original
        return updateNodeByNode(ReturnStatement.create1ReturnStatement(argument), original)
    }
    ,
    createExportDefaultDeclaration(decl: AstNode | undefined, isExportEquals: boolean): ExportDefaultDeclaration {
        return ExportDefaultDeclaration.createExportDefaultDeclaration(decl, isExportEquals)
    }
    ,
    updateExportDefaultDeclaration(original: ExportDefaultDeclaration, decl: AstNode | undefined, isExportEquals: boolean): ExportDefaultDeclaration {
        if (isSameNativeObject(decl, original.decl) && isSameNativeObject(isExportEquals, original.isExportEquals))
            return original
        return updateNodeByNode(ExportDefaultDeclaration.createExportDefaultDeclaration(decl, isExportEquals), original)
    }
    ,
    createTSInterfaceBody(body: readonly AstNode[]): TSInterfaceBody {
        return TSInterfaceBody.createTSInterfaceBody(body)
    }
    ,
    updateTSInterfaceBody(original: TSInterfaceBody, body: readonly AstNode[]): TSInterfaceBody {
        if (isSameNativeObject(body, original.body))
            return original
        return updateNodeByNode(TSInterfaceBody.createTSInterfaceBody(body), original)
    }
    ,
    createTSTypeQuery(exprName?: Expression): TSTypeQuery {
        return TSTypeQuery.createTSTypeQuery(exprName)
    }
    ,
    updateTSTypeQuery(original: TSTypeQuery, exprName?: Expression): TSTypeQuery {
        if (isSameNativeObject(exprName, original.exprName))
            return original
        return updateNodeByNode(TSTypeQuery.createTSTypeQuery(exprName), original)
    }
    ,
    createTSBigintKeyword(): TSBigintKeyword {
        return TSBigintKeyword.createTSBigintKeyword()
    }
    ,
    updateTSBigintKeyword(original: TSBigintKeyword): TSBigintKeyword {
        return updateNodeByNode(TSBigintKeyword.createTSBigintKeyword(), original)
    }
    ,
    createProperty(kind: Es2pandaPropertyKind, key: Expression | undefined, value: Expression | undefined, isMethod: boolean, isComputed: boolean): Property {
        return Property.create1Property(kind, key, value, isMethod, isComputed)
    }
    ,
    updateProperty(original: Property, kind: Es2pandaPropertyKind, key: Expression | undefined, value: Expression | undefined, isMethod: boolean, isComputed: boolean): Property {
        if (isSameNativeObject(kind, original.kind) && isSameNativeObject(key, original.key) && isSameNativeObject(value, original.value) && isSameNativeObject(isMethod, original.isMethod) && isSameNativeObject(isComputed, original.isComputed))
            return original
        return updateNodeByNode(Property.create1Property(kind, key, value, isMethod, isComputed), original)
    }
    ,
    createStringLiteral(str: string): StringLiteral {
        return StringLiteral.create1StringLiteral(str)
    }
    ,
    updateStringLiteral(original: StringLiteral, str: string): StringLiteral {
        if (isSameNativeObject(str, original.str))
            return original
        return updateNodeByNode(StringLiteral.create1StringLiteral(str), original)
    }
    ,
    createTSTypeAssertion(typeAnnotation?: TypeNode, expression?: Expression): TSTypeAssertion {
        return TSTypeAssertion.createTSTypeAssertion(typeAnnotation, expression)
    }
    ,
    updateTSTypeAssertion(original: TSTypeAssertion, typeAnnotation?: TypeNode, expression?: Expression): TSTypeAssertion {
        if (isSameNativeObject(typeAnnotation, original.typeAnnotation) && isSameNativeObject(expression, original.expression))
            return original
        return updateNodeByNode(TSTypeAssertion.createTSTypeAssertion(typeAnnotation, expression), original)
    }
    ,
    createTSExternalModuleReference(expr?: Expression): TSExternalModuleReference {
        return TSExternalModuleReference.createTSExternalModuleReference(expr)
    }
    ,
    updateTSExternalModuleReference(original: TSExternalModuleReference, expr?: Expression): TSExternalModuleReference {
        if (isSameNativeObject(expr, original.expr))
            return original
        return updateNodeByNode(TSExternalModuleReference.createTSExternalModuleReference(expr), original)
    }
    ,
    createTSUndefinedKeyword(): TSUndefinedKeyword {
        return TSUndefinedKeyword.createTSUndefinedKeyword()
    }
    ,
    updateTSUndefinedKeyword(original: TSUndefinedKeyword): TSUndefinedKeyword {
        return updateNodeByNode(TSUndefinedKeyword.createTSUndefinedKeyword(), original)
    }
    ,
    createUnaryExpression(argument: Expression | undefined, operatorType: Es2pandaTokenType): UnaryExpression {
        return UnaryExpression.createUnaryExpression(argument, operatorType)
    }
    ,
    updateUnaryExpression(original: UnaryExpression, argument: Expression | undefined, operatorType: Es2pandaTokenType): UnaryExpression {
        if (isSameNativeObject(argument, original.argument) && isSameNativeObject(operatorType, original.operatorType))
            return original
        return updateNodeByNode(UnaryExpression.createUnaryExpression(argument, operatorType), original)
    }
    ,
    createForInStatement(left?: AstNode, right?: Expression, body?: Statement): ForInStatement {
        return ForInStatement.createForInStatement(left, right, body)
    }
    ,
    updateForInStatement(original: ForInStatement, left?: AstNode, right?: Expression, body?: Statement): ForInStatement {
        if (isSameNativeObject(left, original.left) && isSameNativeObject(right, original.right) && isSameNativeObject(body, original.body))
            return original
        return updateNodeByNode(ForInStatement.createForInStatement(left, right, body), original)
    }
    ,
    createThisExpression(): ThisExpression {
        return ThisExpression.createThisExpression()
    }
    ,
    updateThisExpression(original: ThisExpression): ThisExpression {
        return updateNodeByNode(ThisExpression.createThisExpression(), original)
    }
    ,
    createBinaryExpression(left: Expression | undefined, right: Expression | undefined, operatorType: Es2pandaTokenType): BinaryExpression {
        return BinaryExpression.createBinaryExpression(left, right, operatorType)
    }
    ,
    updateBinaryExpression(original: BinaryExpression, left: Expression | undefined, right: Expression | undefined, operatorType: Es2pandaTokenType): BinaryExpression {
        if (isSameNativeObject(left, original.left) && isSameNativeObject(right, original.right) && isSameNativeObject(operatorType, original.operatorType))
            return original
        return updateNodeByNode(BinaryExpression.createBinaryExpression(left, right, operatorType), original)
    }
    ,
    createSuperExpression(): SuperExpression {
        return SuperExpression.createSuperExpression()
    }
    ,
    updateSuperExpression(original: SuperExpression): SuperExpression {
        return updateNodeByNode(SuperExpression.createSuperExpression(), original)
    }
    ,
    createAssertStatement(test?: Expression, second?: Expression): AssertStatement {
        return AssertStatement.createAssertStatement(test, second)
    }
    ,
    updateAssertStatement(original: AssertStatement, test?: Expression, second?: Expression): AssertStatement {
        if (isSameNativeObject(test, original.test) && isSameNativeObject(second, original.second))
            return original
        return updateNodeByNode(AssertStatement.createAssertStatement(test, second), original)
    }
    ,
    createTSStringKeyword(): TSStringKeyword {
        return TSStringKeyword.createTSStringKeyword()
    }
    ,
    updateTSStringKeyword(original: TSStringKeyword): TSStringKeyword {
        return updateNodeByNode(TSStringKeyword.createTSStringKeyword(), original)
    }
    ,
    createExpressionStatement(expression?: Expression): ExpressionStatement {
        return ExpressionStatement.createExpressionStatement(expression)
    }
    ,
    updateExpressionStatement(original: ExpressionStatement, expression?: Expression): ExpressionStatement {
        if (isSameNativeObject(expression, original.expression))
            return original
        return updateNodeByNode(ExpressionStatement.createExpressionStatement(expression), original)
    }
    ,
    createMetaProperty(kind: Es2pandaMetaPropertyKind): MetaProperty {
        return MetaProperty.createMetaProperty(kind)
    }
    ,
    updateMetaProperty(original: MetaProperty, kind: Es2pandaMetaPropertyKind): MetaProperty {
        if (isSameNativeObject(kind, original.kind))
            return original
        return updateNodeByNode(MetaProperty.createMetaProperty(kind), original)
    }
    ,
    createTSArrayType(elementType?: TypeNode): TSArrayType {
        return TSArrayType.createTSArrayType(elementType)
    }
    ,
    updateTSArrayType(original: TSArrayType, elementType?: TypeNode): TSArrayType {
        if (isSameNativeObject(elementType, original.elementType))
            return original
        return updateNodeByNode(TSArrayType.createTSArrayType(elementType), original)
    }
    ,
    createExportAllDeclaration(source?: StringLiteral, exported?: Identifier): ExportAllDeclaration {
        return ExportAllDeclaration.createExportAllDeclaration(source, exported)
    }
    ,
    updateExportAllDeclaration(original: ExportAllDeclaration, source?: StringLiteral, exported?: Identifier): ExportAllDeclaration {
        if (isSameNativeObject(source, original.source) && isSameNativeObject(exported, original.exported))
            return original
        return updateNodeByNode(ExportAllDeclaration.createExportAllDeclaration(source, exported), original)
    }
    ,
    createExportSpecifier(local?: Identifier, exported?: Identifier): ExportSpecifier {
        return ExportSpecifier.createExportSpecifier(local, exported)
    }
    ,
    updateExportSpecifier(original: ExportSpecifier, local?: Identifier, exported?: Identifier): ExportSpecifier {
        if (isSameNativeObject(local, original.local) && isSameNativeObject(exported, original.exported))
            return original
        return updateNodeByNode(ExportSpecifier.createExportSpecifier(local, exported), original)
    }
    ,
    createTSTupleType(elementType: readonly TypeNode[]): TSTupleType {
        return TSTupleType.createTSTupleType(elementType)
    }
    ,
    updateTSTupleType(original: TSTupleType, elementType: readonly TypeNode[]): TSTupleType {
        if (isSameNativeObject(elementType, original.elementType))
            return original
        return updateNodeByNode(TSTupleType.createTSTupleType(elementType), original)
    }
    ,
    createFunctionExpression(id?: Identifier, _function?: ScriptFunction): FunctionExpression {
        return FunctionExpression.create1FunctionExpression(id, _function)
    }
    ,
    updateFunctionExpression(original: FunctionExpression, id?: Identifier, _function?: ScriptFunction): FunctionExpression {
        if (isSameNativeObject(id, original.id) && isSameNativeObject(_function, original.function))
            return original
        return updateNodeByNode(FunctionExpression.create1FunctionExpression(id, _function), original)
    }
    ,
    createTSIndexSignature(param: Expression | undefined, typeAnnotation: TypeNode | undefined, readonly: boolean): TSIndexSignature {
        return TSIndexSignature.createTSIndexSignature(param, typeAnnotation, readonly)
    }
    ,
    updateTSIndexSignature(original: TSIndexSignature, param: Expression | undefined, typeAnnotation: TypeNode | undefined, readonly: boolean): TSIndexSignature {
        if (isSameNativeObject(param, original.param) && isSameNativeObject(typeAnnotation, original.typeAnnotation) && isSameNativeObject(readonly, original.readonly))
            return original
        return updateNodeByNode(TSIndexSignature.createTSIndexSignature(param, typeAnnotation, readonly), original)
    }
    ,
    createCharLiteral(): CharLiteral {
        return CharLiteral.createCharLiteral()
    }
    ,
    updateCharLiteral(original: CharLiteral): CharLiteral {
        return updateNodeByNode(CharLiteral.createCharLiteral(), original)
    }
    ,
    createTSModuleBlock(statements: readonly Statement[]): TSModuleBlock {
        return TSModuleBlock.createTSModuleBlock(statements)
    }
    ,
    updateTSModuleBlock(original: TSModuleBlock, statements: readonly Statement[]): TSModuleBlock {
        if (isSameNativeObject(statements, original.statements))
            return original
        return updateNodeByNode(TSModuleBlock.createTSModuleBlock(statements), original)
    }
    ,
    createETSNewArrayInstanceExpression(typeReference?: TypeNode, dimension?: Expression): ETSNewArrayInstanceExpression {
        return ETSNewArrayInstanceExpression.createETSNewArrayInstanceExpression(typeReference, dimension)
    }
    ,
    updateETSNewArrayInstanceExpression(original: ETSNewArrayInstanceExpression, typeReference?: TypeNode, dimension?: Expression): ETSNewArrayInstanceExpression {
        if (isSameNativeObject(typeReference, original.typeReference) && isSameNativeObject(dimension, original.dimension))
            return original
        return updateNodeByNode(ETSNewArrayInstanceExpression.createETSNewArrayInstanceExpression(typeReference, dimension), original)
    }
    ,
    createAnnotationDeclaration(expr: Expression | undefined, properties: readonly AstNode[]): AnnotationDeclaration {
        return AnnotationDeclaration.create1AnnotationDeclaration(expr, properties)
    }
    ,
    updateAnnotationDeclaration(original: AnnotationDeclaration, expr: Expression | undefined, properties: readonly AstNode[]): AnnotationDeclaration {
        if (isSameNativeObject(expr, original.expr) && isSameNativeObject(properties, original.properties))
            return original
        return updateNodeByNode(AnnotationDeclaration.create1AnnotationDeclaration(expr, properties), original)
    }
    ,
    createAnnotationUsage(expr: Expression | undefined, properties: readonly AstNode[]): AnnotationUsage {
        return AnnotationUsage.create1AnnotationUsage(expr, properties)
    }
    ,
    updateAnnotationUsage(original: AnnotationUsage, expr: Expression | undefined, properties: readonly AstNode[]): AnnotationUsage {
        if (isSameNativeObject(expr, original.expr) && isSameNativeObject(properties, original.properties))
            return original
        return updateNodeByNode(AnnotationUsage.create1AnnotationUsage(expr, properties), original)
    }
    ,
    createEmptyStatement(isBrokenStatement: boolean): EmptyStatement {
        return EmptyStatement.create1EmptyStatement(isBrokenStatement)
    }
    ,
    updateEmptyStatement(original: EmptyStatement, isBrokenStatement: boolean): EmptyStatement {
        if (isSameNativeObject(isBrokenStatement, original.isBrokenStatement))
            return original
        return updateNodeByNode(EmptyStatement.create1EmptyStatement(isBrokenStatement), original)
    }
    ,
    createWhileStatement(test?: Expression, body?: Statement): WhileStatement {
        return WhileStatement.createWhileStatement(test, body)
    }
    ,
    updateWhileStatement(original: WhileStatement, test?: Expression, body?: Statement): WhileStatement {
        if (isSameNativeObject(test, original.test) && isSameNativeObject(body, original.body))
            return original
        return updateNodeByNode(WhileStatement.createWhileStatement(test, body), original)
    }
    ,
    createFunctionSignature(typeParams: TSTypeParameterDeclaration | undefined, params: readonly Expression[], returnType: TypeNode | undefined, hasReceiver: boolean): FunctionSignature {
        return FunctionSignature.createFunctionSignature(typeParams, params, returnType, hasReceiver)
    }
    ,
    updateFunctionSignature(original: FunctionSignature, typeParams: TSTypeParameterDeclaration | undefined, params: readonly Expression[], returnType: TypeNode | undefined, hasReceiver: boolean): FunctionSignature {
        if (isSameNativeObject(typeParams, original.typeParams) && isSameNativeObject(params, original.params) && isSameNativeObject(returnType, original.returnType) && isSameNativeObject(hasReceiver, original.hasReceiver))
            return original
        return updateNodeByNode(FunctionSignature.createFunctionSignature(typeParams, params, returnType, hasReceiver), original)
    }
    ,
    createChainExpression(expression?: Expression): ChainExpression {
        return ChainExpression.createChainExpression(expression)
    }
    ,
    updateChainExpression(original: ChainExpression, expression?: Expression): ChainExpression {
        if (isSameNativeObject(expression, original.expression))
            return original
        return updateNodeByNode(ChainExpression.createChainExpression(expression), original)
    }
    ,
    createTSIntersectionType(types: readonly Expression[]): TSIntersectionType {
        return TSIntersectionType.createTSIntersectionType(types)
    }
    ,
    updateTSIntersectionType(original: TSIntersectionType, types: readonly Expression[]): TSIntersectionType {
        if (isSameNativeObject(types, original.types))
            return original
        return updateNodeByNode(TSIntersectionType.createTSIntersectionType(types), original)
    }
    ,
    createUpdateExpression(argument: Expression | undefined, operatorType: Es2pandaTokenType, isPrefix: boolean): UpdateExpression {
        return UpdateExpression.createUpdateExpression(argument, operatorType, isPrefix)
    }
    ,
    updateUpdateExpression(original: UpdateExpression, argument: Expression | undefined, operatorType: Es2pandaTokenType, isPrefix: boolean): UpdateExpression {
        if (isSameNativeObject(argument, original.argument) && isSameNativeObject(operatorType, original.operatorType) && isSameNativeObject(isPrefix, original.isPrefix))
            return original
        return updateNodeByNode(UpdateExpression.createUpdateExpression(argument, operatorType, isPrefix), original)
    }
    ,
    createBlockExpression(statements: readonly Statement[]): BlockExpression {
        return BlockExpression.createBlockExpression(statements)
    }
    ,
    updateBlockExpression(original: BlockExpression, statements: readonly Statement[]): BlockExpression {
        if (isSameNativeObject(statements, original.statements))
            return original
        return updateNodeByNode(BlockExpression.createBlockExpression(statements), original)
    }
    ,
    createTSTypeLiteral(members: readonly AstNode[]): TSTypeLiteral {
        return TSTypeLiteral.createTSTypeLiteral(members)
    }
    ,
    updateTSTypeLiteral(original: TSTypeLiteral, members: readonly AstNode[]): TSTypeLiteral {
        if (isSameNativeObject(members, original.members))
            return original
        return updateNodeByNode(TSTypeLiteral.createTSTypeLiteral(members), original)
    }
    ,
    createTSBooleanKeyword(): TSBooleanKeyword {
        return TSBooleanKeyword.createTSBooleanKeyword()
    }
    ,
    updateTSBooleanKeyword(original: TSBooleanKeyword): TSBooleanKeyword {
        return updateNodeByNode(TSBooleanKeyword.createTSBooleanKeyword(), original)
    }
    ,
    createTSTypePredicate(parameterName: Expression | undefined, typeAnnotation: TypeNode | undefined, asserts: boolean): TSTypePredicate {
        return TSTypePredicate.createTSTypePredicate(parameterName, typeAnnotation, asserts)
    }
    ,
    updateTSTypePredicate(original: TSTypePredicate, parameterName: Expression | undefined, typeAnnotation: TypeNode | undefined, asserts: boolean): TSTypePredicate {
        if (isSameNativeObject(parameterName, original.parameterName) && isSameNativeObject(typeAnnotation, original.typeAnnotation) && isSameNativeObject(asserts, original.asserts))
            return original
        return updateNodeByNode(TSTypePredicate.createTSTypePredicate(parameterName, typeAnnotation, asserts), original)
    }
    ,
    createImportNamespaceSpecifier(local?: Identifier): ImportNamespaceSpecifier {
        return ImportNamespaceSpecifier.createImportNamespaceSpecifier(local)
    }
    ,
    updateImportNamespaceSpecifier(original: ImportNamespaceSpecifier, local?: Identifier): ImportNamespaceSpecifier {
        if (isSameNativeObject(local, original.local))
            return original
        return updateNodeByNode(ImportNamespaceSpecifier.createImportNamespaceSpecifier(local), original)
    }
    ,
    createTSTypeParameterInstantiation(params: readonly TypeNode[]): TSTypeParameterInstantiation {
        return TSTypeParameterInstantiation.createTSTypeParameterInstantiation(params)
    }
    ,
    updateTSTypeParameterInstantiation(original: TSTypeParameterInstantiation, params: readonly TypeNode[]): TSTypeParameterInstantiation {
        if (isSameNativeObject(params, original.params))
            return original
        return updateNodeByNode(TSTypeParameterInstantiation.createTSTypeParameterInstantiation(params), original)
    }
    ,
    createNullLiteral(): NullLiteral {
        return NullLiteral.createNullLiteral()
    }
    ,
    updateNullLiteral(original: NullLiteral): NullLiteral {
        return updateNodeByNode(NullLiteral.createNullLiteral(), original)
    }
    ,
    createTSInferType(typeParam?: TSTypeParameter): TSInferType {
        return TSInferType.createTSInferType(typeParam)
    }
    ,
    updateTSInferType(original: TSInferType, typeParam?: TSTypeParameter): TSInferType {
        if (isSameNativeObject(typeParam, original.typeParam))
            return original
        return updateNodeByNode(TSInferType.createTSInferType(typeParam), original)
    }
    ,
    createSwitchCaseStatement(test: Expression | undefined, consequent: readonly Statement[]): SwitchCaseStatement {
        return SwitchCaseStatement.createSwitchCaseStatement(test, consequent)
    }
    ,
    updateSwitchCaseStatement(original: SwitchCaseStatement, test: Expression | undefined, consequent: readonly Statement[]): SwitchCaseStatement {
        if (isSameNativeObject(test, original.test) && isSameNativeObject(consequent, original.consequent))
            return original
        return updateNodeByNode(SwitchCaseStatement.createSwitchCaseStatement(test, consequent), original)
    }
    ,
    createYieldExpression(argument: Expression | undefined, hasDelegate: boolean): YieldExpression {
        return YieldExpression.createYieldExpression(argument, hasDelegate)
    }
    ,
    updateYieldExpression(original: YieldExpression, argument: Expression | undefined, hasDelegate: boolean): YieldExpression {
        if (isSameNativeObject(argument, original.argument) && isSameNativeObject(hasDelegate, original.hasDelegate))
            return original
        return updateNodeByNode(YieldExpression.createYieldExpression(argument, hasDelegate), original)
    }
    ,
    createTSImportEqualsDeclaration(id: Identifier | undefined, moduleReference: Expression | undefined, isExport: boolean): TSImportEqualsDeclaration {
        return TSImportEqualsDeclaration.createTSImportEqualsDeclaration(id, moduleReference, isExport)
    }
    ,
    updateTSImportEqualsDeclaration(original: TSImportEqualsDeclaration, id: Identifier | undefined, moduleReference: Expression | undefined, isExport: boolean): TSImportEqualsDeclaration {
        if (isSameNativeObject(id, original.id) && isSameNativeObject(moduleReference, original.moduleReference) && isSameNativeObject(isExport, original.isExport))
            return original
        return updateNodeByNode(TSImportEqualsDeclaration.createTSImportEqualsDeclaration(id, moduleReference, isExport), original)
    }
    ,
    createBooleanLiteral(value: boolean): BooleanLiteral {
        return BooleanLiteral.createBooleanLiteral(value)
    }
    ,
    updateBooleanLiteral(original: BooleanLiteral, value: boolean): BooleanLiteral {
        if (isSameNativeObject(value, original.value))
            return original
        return updateNodeByNode(BooleanLiteral.createBooleanLiteral(value), original)
    }
    ,
    createTSNumberKeyword(): TSNumberKeyword {
        return TSNumberKeyword.createTSNumberKeyword()
    }
    ,
    updateTSNumberKeyword(original: TSNumberKeyword): TSNumberKeyword {
        return updateNodeByNode(TSNumberKeyword.createTSNumberKeyword(), original)
    }
    ,
    createTSNonNullExpression(expr?: Expression): TSNonNullExpression {
        return TSNonNullExpression.createTSNonNullExpression(expr)
    }
    ,
    updateTSNonNullExpression(original: TSNonNullExpression, expr?: Expression): TSNonNullExpression {
        if (isSameNativeObject(expr, original.expr))
            return original
        return updateNodeByNode(TSNonNullExpression.createTSNonNullExpression(expr), original)
    }
    ,
    createPrefixAssertionExpression(expr?: Expression, type?: TypeNode): PrefixAssertionExpression {
        return PrefixAssertionExpression.createPrefixAssertionExpression(expr, type)
    }
    ,
    updatePrefixAssertionExpression(original: PrefixAssertionExpression, expr?: Expression, type?: TypeNode): PrefixAssertionExpression {
        if (isSameNativeObject(expr, original.expr) && isSameNativeObject(type, original.type))
            return original
        return updateNodeByNode(PrefixAssertionExpression.createPrefixAssertionExpression(expr, type), original)
    }
    ,
    createClassExpression(definition?: ClassDefinition): ClassExpression {
        return ClassExpression.createClassExpression(definition)
    }
    ,
    updateClassExpression(original: ClassExpression, definition?: ClassDefinition): ClassExpression {
        if (isSameNativeObject(definition, original.definition))
            return original
        return updateNodeByNode(ClassExpression.createClassExpression(definition), original)
    }
    ,
    createForOfStatement(left: AstNode | undefined, right: Expression | undefined, body: Statement | undefined, isAwait: boolean): ForOfStatement {
        return ForOfStatement.createForOfStatement(left, right, body, isAwait)
    }
    ,
    updateForOfStatement(original: ForOfStatement, left: AstNode | undefined, right: Expression | undefined, body: Statement | undefined, isAwait: boolean): ForOfStatement {
        if (isSameNativeObject(left, original.left) && isSameNativeObject(right, original.right) && isSameNativeObject(body, original.body) && isSameNativeObject(isAwait, original.isAwait))
            return original
        return updateNodeByNode(ForOfStatement.createForOfStatement(left, right, body, isAwait), original)
    }
    ,
    createTemplateLiteral(quasis: readonly TemplateElement[], expressions: readonly Expression[], multilineString: string): TemplateLiteral {
        return TemplateLiteral.createTemplateLiteral(quasis, expressions, multilineString)
    }
    ,
    updateTemplateLiteral(original: TemplateLiteral, quasis: readonly TemplateElement[], expressions: readonly Expression[], multilineString: string): TemplateLiteral {
        if (isSameNativeObject(quasis, original.quasis) && isSameNativeObject(expressions, original.expressions) && isSameNativeObject(multilineString, original.multilineString))
            return original
        return updateNodeByNode(TemplateLiteral.createTemplateLiteral(quasis, expressions, multilineString), original)
    }
    ,
    createTSUnionType(types: readonly TypeNode[]): TSUnionType {
        return TSUnionType.createTSUnionType(types)
    }
    ,
    updateTSUnionType(original: TSUnionType, types: readonly TypeNode[]): TSUnionType {
        if (isSameNativeObject(types, original.types))
            return original
        return updateNodeByNode(TSUnionType.createTSUnionType(types), original)
    }
    ,
    createTSUnknownKeyword(): TSUnknownKeyword {
        return TSUnknownKeyword.createTSUnknownKeyword()
    }
    ,
    updateTSUnknownKeyword(original: TSUnknownKeyword): TSUnknownKeyword {
        return updateNodeByNode(TSUnknownKeyword.createTSUnknownKeyword(), original)
    }
    ,
    createIdentifier(name: string, typeAnnotation?: TypeNode): Identifier {
        return Identifier.create2Identifier(name, typeAnnotation)
    }
    ,
    updateIdentifier(original: Identifier, name: string, typeAnnotation?: TypeNode): Identifier {
        if (isSameNativeObject(name, original.name) && isSameNativeObject(typeAnnotation, original.typeAnnotation))
            return original
        return updateNodeByNode(Identifier.create2Identifier(name, typeAnnotation), original)
    }
    ,
    createOpaqueTypeNode(): OpaqueTypeNode {
        return OpaqueTypeNode.create1OpaqueTypeNode()
    }
    ,
    updateOpaqueTypeNode(original: OpaqueTypeNode): OpaqueTypeNode {
        return updateNodeByNode(OpaqueTypeNode.create1OpaqueTypeNode(), original)
    }
    ,
    createBlockStatement(statements: readonly Statement[]): BlockStatement {
        return BlockStatement.createBlockStatement(statements)
    }
    ,
    updateBlockStatement(original: BlockStatement, statements: readonly Statement[]): BlockStatement {
        if (isSameNativeObject(statements, original.statements))
            return original
        return updateNodeByNode(BlockStatement.createBlockStatement(statements), original)
    }
    ,
    createTSTypeParameterDeclaration(params: readonly TSTypeParameter[], requiredParams: number): TSTypeParameterDeclaration {
        return TSTypeParameterDeclaration.createTSTypeParameterDeclaration(params, requiredParams)
    }
    ,
    updateTSTypeParameterDeclaration(original: TSTypeParameterDeclaration, params: readonly TSTypeParameter[], requiredParams: number): TSTypeParameterDeclaration {
        if (isSameNativeObject(params, original.params) && isSameNativeObject(requiredParams, original.requiredParams))
            return original
        return updateNodeByNode(TSTypeParameterDeclaration.createTSTypeParameterDeclaration(params, requiredParams), original)
    }
    ,
    createTSNullKeyword(): TSNullKeyword {
        return TSNullKeyword.createTSNullKeyword()
    }
    ,
    updateTSNullKeyword(original: TSNullKeyword): TSNullKeyword {
        return updateNodeByNode(TSNullKeyword.createTSNullKeyword(), original)
    }
    ,
    createTSInterfaceHeritage(expr?: TypeNode): TSInterfaceHeritage {
        return TSInterfaceHeritage.createTSInterfaceHeritage(expr)
    }
    ,
    updateTSInterfaceHeritage(original: TSInterfaceHeritage, expr?: TypeNode): TSInterfaceHeritage {
        if (isSameNativeObject(expr, original.expr))
            return original
        return updateNodeByNode(TSInterfaceHeritage.createTSInterfaceHeritage(expr), original)
    }
    ,
    createETSClassLiteral(expr?: TypeNode): ETSClassLiteral {
        return ETSClassLiteral.createETSClassLiteral(expr)
    }
    ,
    updateETSClassLiteral(original: ETSClassLiteral, expr?: TypeNode): ETSClassLiteral {
        if (isSameNativeObject(expr, original.expr))
            return original
        return updateNodeByNode(ETSClassLiteral.createETSClassLiteral(expr), original)
    }
    ,
    createBreakStatement(ident?: Identifier): BreakStatement {
        return BreakStatement.create1BreakStatement(ident)
    }
    ,
    updateBreakStatement(original: BreakStatement, ident?: Identifier): BreakStatement {
        if (isSameNativeObject(ident, original.ident))
            return original
        return updateNodeByNode(BreakStatement.create1BreakStatement(ident), original)
    }
    ,
    createTSAnyKeyword(): TSAnyKeyword {
        return TSAnyKeyword.createTSAnyKeyword()
    }
    ,
    updateTSAnyKeyword(original: TSAnyKeyword): TSAnyKeyword {
        return updateNodeByNode(TSAnyKeyword.createTSAnyKeyword(), original)
    }
    ,
    createClassDeclaration(definition?: ClassDefinition): ClassDeclaration {
        return ClassDeclaration.createClassDeclaration(definition)
    }
    ,
    updateClassDeclaration(original: ClassDeclaration, definition?: ClassDefinition): ClassDeclaration {
        if (isSameNativeObject(definition, original.definition))
            return original
        return updateNodeByNode(ClassDeclaration.createClassDeclaration(definition), original)
    }
    ,
    createTSIndexedAccessType(objectType?: TypeNode, indexType?: TypeNode): TSIndexedAccessType {
        return TSIndexedAccessType.createTSIndexedAccessType(objectType, indexType)
    }
    ,
    updateTSIndexedAccessType(original: TSIndexedAccessType, objectType?: TypeNode, indexType?: TypeNode): TSIndexedAccessType {
        if (isSameNativeObject(objectType, original.objectType) && isSameNativeObject(indexType, original.indexType))
            return original
        return updateNodeByNode(TSIndexedAccessType.createTSIndexedAccessType(objectType, indexType), original)
    }
    ,
    createTSQualifiedName(left?: Expression, right?: Identifier): TSQualifiedName {
        return TSQualifiedName.createTSQualifiedName(left, right)
    }
    ,
    updateTSQualifiedName(original: TSQualifiedName, left?: Expression, right?: Identifier): TSQualifiedName {
        if (isSameNativeObject(left, original.left) && isSameNativeObject(right, original.right))
            return original
        return updateNodeByNode(TSQualifiedName.createTSQualifiedName(left, right), original)
    }
    ,
    createAwaitExpression(argument?: Expression): AwaitExpression {
        return AwaitExpression.createAwaitExpression(argument)
    }
    ,
    updateAwaitExpression(original: AwaitExpression, argument?: Expression): AwaitExpression {
        if (isSameNativeObject(argument, original.argument))
            return original
        return updateNodeByNode(AwaitExpression.createAwaitExpression(argument), original)
    }
    ,
    createContinueStatement(ident?: Identifier): ContinueStatement {
        return ContinueStatement.create1ContinueStatement(ident)
    }
    ,
    updateContinueStatement(original: ContinueStatement, ident?: Identifier): ContinueStatement {
        if (isSameNativeObject(ident, original.ident))
            return original
        return updateNodeByNode(ContinueStatement.create1ContinueStatement(ident), original)
    }
    ,
    createETSNewMultiDimArrayInstanceExpression(typeReference: TypeNode | undefined, dimensions: readonly Expression[]): ETSNewMultiDimArrayInstanceExpression {
        return ETSNewMultiDimArrayInstanceExpression.createETSNewMultiDimArrayInstanceExpression(typeReference, dimensions)
    }
    ,
    updateETSNewMultiDimArrayInstanceExpression(original: ETSNewMultiDimArrayInstanceExpression, typeReference: TypeNode | undefined, dimensions: readonly Expression[]): ETSNewMultiDimArrayInstanceExpression {
        if (isSameNativeObject(typeReference, original.typeReference) && isSameNativeObject(dimensions, original.dimensions))
            return original
        return updateNodeByNode(ETSNewMultiDimArrayInstanceExpression.createETSNewMultiDimArrayInstanceExpression(typeReference, dimensions), original)
    }
    ,
    createTSNamedTupleMember(label: Expression | undefined, elementType: TypeNode | undefined, isOptional: boolean): TSNamedTupleMember {
        return TSNamedTupleMember.createTSNamedTupleMember(label, elementType, isOptional)
    }
    ,
    updateTSNamedTupleMember(original: TSNamedTupleMember, label: Expression | undefined, elementType: TypeNode | undefined, isOptional: boolean): TSNamedTupleMember {
        if (isSameNativeObject(label, original.label) && isSameNativeObject(elementType, original.elementType) && isSameNativeObject(isOptional, original.isOptional))
            return original
        return updateNodeByNode(TSNamedTupleMember.createTSNamedTupleMember(label, elementType, isOptional), original)
    }
    ,
    createImportExpression(source?: Expression): ImportExpression {
        return ImportExpression.createImportExpression(source)
    }
    ,
    updateImportExpression(original: ImportExpression, source?: Expression): ImportExpression {
        if (isSameNativeObject(source, original.source))
            return original
        return updateNodeByNode(ImportExpression.createImportExpression(source), original)
    }
    ,
    createETSNullType(): ETSNullType {
        return ETSNullType.createETSNullType()
    }
    ,
    updateETSNullType(original: ETSNullType): ETSNullType {
        return updateNodeByNode(ETSNullType.createETSNullType(), original)
    }
    ,
    createETSUndefinedType(): ETSUndefinedType {
        return ETSUndefinedType.createETSUndefinedType()
    }
    ,
    updateETSUndefinedType(original: ETSUndefinedType): ETSUndefinedType {
        return updateNodeByNode(ETSUndefinedType.createETSUndefinedType(), original)
    }
    ,
    createTypeofExpression(argument?: Expression): TypeofExpression {
        return TypeofExpression.createTypeofExpression(argument)
    }
    ,
    updateTypeofExpression(original: TypeofExpression, argument?: Expression): TypeofExpression {
        if (isSameNativeObject(argument, original.argument))
            return original
        return updateNodeByNode(TypeofExpression.createTypeofExpression(argument), original)
    }
    ,
    createTSEnumMember(key: Expression | undefined, init: Expression | undefined, isGenerated: boolean): TSEnumMember {
        return TSEnumMember.create1TSEnumMember(key, init, isGenerated)
    }
    ,
    updateTSEnumMember(original: TSEnumMember, key: Expression | undefined, init: Expression | undefined, isGenerated: boolean): TSEnumMember {
        if (isSameNativeObject(key, original.key) && isSameNativeObject(init, original.init) && isSameNativeObject(isGenerated, original.isGenerated))
            return original
        return updateNodeByNode(TSEnumMember.create1TSEnumMember(key, init, isGenerated), original)
    }
    ,
    createSwitchStatement(discriminant: Expression | undefined, cases: readonly SwitchCaseStatement[]): SwitchStatement {
        return SwitchStatement.createSwitchStatement(discriminant, cases)
    }
    ,
    updateSwitchStatement(original: SwitchStatement, discriminant: Expression | undefined, cases: readonly SwitchCaseStatement[]): SwitchStatement {
        if (isSameNativeObject(discriminant, original.discriminant) && isSameNativeObject(cases, original.cases))
            return original
        return updateNodeByNode(SwitchStatement.createSwitchStatement(discriminant, cases), original)
    }
    ,
    createDoWhileStatement(body?: Statement, test?: Expression): DoWhileStatement {
        return DoWhileStatement.createDoWhileStatement(body, test)
    }
    ,
    updateDoWhileStatement(original: DoWhileStatement, body?: Statement, test?: Expression): DoWhileStatement {
        if (isSameNativeObject(body, original.body) && isSameNativeObject(test, original.test))
            return original
        return updateNodeByNode(DoWhileStatement.createDoWhileStatement(body, test), original)
    }
    ,
    createCatchClause(param?: Expression, body?: BlockStatement): CatchClause {
        return CatchClause.createCatchClause(param, body)
    }
    ,
    updateCatchClause(original: CatchClause, param?: Expression, body?: BlockStatement): CatchClause {
        if (isSameNativeObject(param, original.param) && isSameNativeObject(body, original.body))
            return original
        return updateNodeByNode(CatchClause.createCatchClause(param, body), original)
    }
    ,
    createSequenceExpression(sequence: readonly Expression[]): SequenceExpression {
        return SequenceExpression.createSequenceExpression(sequence)
    }
    ,
    updateSequenceExpression(original: SequenceExpression, sequence: readonly Expression[]): SequenceExpression {
        if (isSameNativeObject(sequence, original.sequence))
            return original
        return updateNodeByNode(SequenceExpression.createSequenceExpression(sequence), original)
    }
    ,
    createArrowFunctionExpression(_function?: ScriptFunction): ArrowFunctionExpression {
        return ArrowFunctionExpression.createArrowFunctionExpression(_function)
    }
    ,
    updateArrowFunctionExpression(original: ArrowFunctionExpression, _function?: ScriptFunction): ArrowFunctionExpression {
        if (isSameNativeObject(_function, original.function))
            return original
        return updateNodeByNode(ArrowFunctionExpression.createArrowFunctionExpression(_function), original)
    }
    ,
    createOmittedExpression(): OmittedExpression {
        return OmittedExpression.createOmittedExpression()
    }
    ,
    updateOmittedExpression(original: OmittedExpression): OmittedExpression {
        return updateNodeByNode(OmittedExpression.createOmittedExpression(), original)
    }
    ,
    createETSNewClassInstanceExpression(typeRef: Expression | undefined, _arguments: readonly Expression[]): ETSNewClassInstanceExpression {
        return ETSNewClassInstanceExpression.createETSNewClassInstanceExpression(typeRef, _arguments)
    }
    ,
    updateETSNewClassInstanceExpression(original: ETSNewClassInstanceExpression, typeRef: Expression | undefined, _arguments: readonly Expression[]): ETSNewClassInstanceExpression {
        if (isSameNativeObject(typeRef, original.typeRef) && isSameNativeObject(_arguments, original.arguments))
            return original
        return updateNodeByNode(ETSNewClassInstanceExpression.createETSNewClassInstanceExpression(typeRef, _arguments), original)
    }
    ,
    createTSAsExpression(expr: Expression | undefined, typeAnnotation: TypeNode | undefined, isConst: boolean): TSAsExpression {
        return TSAsExpression.createTSAsExpression(expr, typeAnnotation, isConst)
    }
    ,
    updateTSAsExpression(original: TSAsExpression, expr: Expression | undefined, typeAnnotation: TypeNode | undefined, isConst: boolean): TSAsExpression {
        if (isSameNativeObject(expr, original.expr) && isSameNativeObject(typeAnnotation, original.typeAnnotation) && isSameNativeObject(isConst, original.isConst))
            return original
        return updateNodeByNode(TSAsExpression.createTSAsExpression(expr, typeAnnotation, isConst), original)
    }
    ,
    createForUpdateStatement(init?: AstNode, test?: Expression, update?: Expression, body?: Statement): ForUpdateStatement {
        return ForUpdateStatement.createForUpdateStatement(init, test, update, body)
    }
    ,
    updateForUpdateStatement(original: ForUpdateStatement, init?: AstNode, test?: Expression, update?: Expression, body?: Statement): ForUpdateStatement {
        if (isSameNativeObject(init, original.init) && isSameNativeObject(test, original.test) && isSameNativeObject(update, original.update) && isSameNativeObject(body, original.body))
            return original
        return updateNodeByNode(ForUpdateStatement.createForUpdateStatement(init, test, update, body), original)
    }
    ,
    createETSPrimitiveType(primitiveType: Es2pandaPrimitiveType): ETSPrimitiveType {
        return ETSPrimitiveType.createETSPrimitiveType(primitiveType)
    }
    ,
    updateETSPrimitiveType(original: ETSPrimitiveType, primitiveType: Es2pandaPrimitiveType): ETSPrimitiveType {
        if (isSameNativeObject(primitiveType, original.primitiveType))
            return original
        return updateNodeByNode(ETSPrimitiveType.createETSPrimitiveType(primitiveType), original)
    }
    ,
    createNewExpression(callee: Expression | undefined, _arguments: readonly Expression[]): NewExpression {
        return NewExpression.createNewExpression(callee, _arguments)
    }
    ,
    updateNewExpression(original: NewExpression, callee: Expression | undefined, _arguments: readonly Expression[]): NewExpression {
        if (isSameNativeObject(callee, original.callee) && isSameNativeObject(_arguments, original.arguments))
            return original
        return updateNodeByNode(NewExpression.createNewExpression(callee, _arguments), original)
    }
    ,
    createTSThisType(): TSThisType {
        return TSThisType.createTSThisType()
    }
    ,
    updateTSThisType(original: TSThisType): TSThisType {
        return updateNodeByNode(TSThisType.createTSThisType(), original)
    }
    ,
}