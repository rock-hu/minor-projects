/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

import { FaultID } from './Problems';
import { ProblemSeverity } from './ProblemSeverity';

export class FaultAttributes {
  constructor(
    public cookBookRef: number,
    public severity: ProblemSeverity = ProblemSeverity.ERROR
  ) {}
}

export const faultsAttrs: FaultAttributes[] = [];

faultsAttrs[FaultID.LiteralAsPropertyName] = new FaultAttributes(1);
faultsAttrs[FaultID.ComputedPropertyName] = new FaultAttributes(1);
faultsAttrs[FaultID.SymbolType] = new FaultAttributes(2);
faultsAttrs[FaultID.PrivateIdentifier] = new FaultAttributes(3);
faultsAttrs[FaultID.DeclWithDuplicateName] = new FaultAttributes(4);
faultsAttrs[FaultID.VarDeclaration] = new FaultAttributes(5);
faultsAttrs[FaultID.AnyType] = new FaultAttributes(8);
faultsAttrs[FaultID.UnknownType] = new FaultAttributes(8);
faultsAttrs[FaultID.CallSignature] = new FaultAttributes(14);
faultsAttrs[FaultID.ConstructorType] = new FaultAttributes(15);
faultsAttrs[FaultID.MultipleStaticBlocks] = new FaultAttributes(16);
faultsAttrs[FaultID.IndexMember] = new FaultAttributes(17);
faultsAttrs[FaultID.IntersectionType] = new FaultAttributes(19);
faultsAttrs[FaultID.ThisType] = new FaultAttributes(21);
faultsAttrs[FaultID.ConditionalType] = new FaultAttributes(22);
faultsAttrs[FaultID.ParameterProperties] = new FaultAttributes(25);
faultsAttrs[FaultID.ConstructorIface] = new FaultAttributes(27);
faultsAttrs[FaultID.IndexedAccessType] = new FaultAttributes(28);
faultsAttrs[FaultID.PropertyAccessByIndex] = new FaultAttributes(29);
faultsAttrs[FaultID.StructuralIdentity] = new FaultAttributes(30);
faultsAttrs[FaultID.GenericCallNoTypeArgs] = new FaultAttributes(34);
faultsAttrs[FaultID.ObjectLiteralNoContextType] = new FaultAttributes(38);
faultsAttrs[FaultID.ObjectTypeLiteral] = new FaultAttributes(40);
faultsAttrs[FaultID.ArrayLiteralNoContextType] = new FaultAttributes(43);
faultsAttrs[FaultID.FunctionExpression] = new FaultAttributes(46);
faultsAttrs[FaultID.ClassExpression] = new FaultAttributes(50);
faultsAttrs[FaultID.ImplementsClass] = new FaultAttributes(51);
faultsAttrs[FaultID.MethodReassignment] = new FaultAttributes(52);
faultsAttrs[FaultID.TypeAssertion] = new FaultAttributes(53);
faultsAttrs[FaultID.JsxElement] = new FaultAttributes(54);
faultsAttrs[FaultID.UnaryArithmNotNumber] = new FaultAttributes(55);
faultsAttrs[FaultID.DeleteOperator] = new FaultAttributes(59);
faultsAttrs[FaultID.TypeQuery] = new FaultAttributes(60);
faultsAttrs[FaultID.InstanceofUnsupported] = new FaultAttributes(65);
faultsAttrs[FaultID.InOperator] = new FaultAttributes(66);
faultsAttrs[FaultID.DestructuringAssignment] = new FaultAttributes(69);
faultsAttrs[FaultID.CommaOperator] = new FaultAttributes(71);
faultsAttrs[FaultID.DestructuringDeclaration] = new FaultAttributes(74);
faultsAttrs[FaultID.CatchWithUnsupportedType] = new FaultAttributes(79);
faultsAttrs[FaultID.ForInStatement] = new FaultAttributes(80);
faultsAttrs[FaultID.MappedType] = new FaultAttributes(83);
faultsAttrs[FaultID.WithStatement] = new FaultAttributes(84);
faultsAttrs[FaultID.ThrowStatement] = new FaultAttributes(87);
faultsAttrs[FaultID.LimitedReturnTypeInference] = new FaultAttributes(90);
faultsAttrs[FaultID.DestructuringParameter] = new FaultAttributes(91);
faultsAttrs[FaultID.LocalFunction] = new FaultAttributes(92);
faultsAttrs[FaultID.FunctionContainsThis] = new FaultAttributes(93);
faultsAttrs[FaultID.GeneratorFunction] = new FaultAttributes(94);
faultsAttrs[FaultID.YieldExpression] = new FaultAttributes(94);
faultsAttrs[FaultID.IsOperator] = new FaultAttributes(96);
faultsAttrs[FaultID.SpreadOperator] = new FaultAttributes(99);
faultsAttrs[FaultID.IntefaceExtendDifProps] = new FaultAttributes(102);
faultsAttrs[FaultID.InterfaceMerging] = new FaultAttributes(103);
faultsAttrs[FaultID.InterfaceExtendsClass] = new FaultAttributes(104);
faultsAttrs[FaultID.ConstructorFuncs] = new FaultAttributes(106);
faultsAttrs[FaultID.EnumMemberNonConstInit] = new FaultAttributes(111);
faultsAttrs[FaultID.EnumMerging] = new FaultAttributes(113);
faultsAttrs[FaultID.NamespaceAsObject] = new FaultAttributes(114);
faultsAttrs[FaultID.NonDeclarationInNamespace] = new FaultAttributes(116);
faultsAttrs[FaultID.ImportAssignment] = new FaultAttributes(121);
faultsAttrs[FaultID.ExportAssignment] = new FaultAttributes(126);
faultsAttrs[FaultID.ShorthandAmbientModuleDecl] = new FaultAttributes(128);
faultsAttrs[FaultID.WildcardsInModuleName] = new FaultAttributes(129);
faultsAttrs[FaultID.UMDModuleDefinition] = new FaultAttributes(130);
faultsAttrs[FaultID.NewTarget] = new FaultAttributes(132);
faultsAttrs[FaultID.DefiniteAssignment] = new FaultAttributes(134, ProblemSeverity.WARNING);
faultsAttrs[FaultID.DefiniteAssignmentError] = new FaultAttributes(134);
faultsAttrs[FaultID.Prototype] = new FaultAttributes(136);
faultsAttrs[FaultID.GlobalThis] = new FaultAttributes(137, ProblemSeverity.WARNING);
faultsAttrs[FaultID.GlobalThisError] = new FaultAttributes(137);
faultsAttrs[FaultID.UtilityType] = new FaultAttributes(138);
faultsAttrs[FaultID.PropertyDeclOnFunction] = new FaultAttributes(139);
faultsAttrs[FaultID.FunctionBind] = new FaultAttributes(140, ProblemSeverity.WARNING);
faultsAttrs[FaultID.FunctionBindError] = new FaultAttributes(140);
faultsAttrs[FaultID.ConstAssertion] = new FaultAttributes(142);
faultsAttrs[FaultID.ImportAssertion] = new FaultAttributes(143);
faultsAttrs[FaultID.LimitedStdLibApi] = new FaultAttributes(144);
faultsAttrs[FaultID.StrictDiagnostic] = new FaultAttributes(145);
faultsAttrs[FaultID.ErrorSuppression] = new FaultAttributes(146);
faultsAttrs[FaultID.ClassAsObject] = new FaultAttributes(149, ProblemSeverity.WARNING);
faultsAttrs[FaultID.ClassAsObjectError] = new FaultAttributes(149);
faultsAttrs[FaultID.ImportAfterStatement] = new FaultAttributes(150);
faultsAttrs[FaultID.EsObjectType] = new FaultAttributes(151, ProblemSeverity.WARNING);
faultsAttrs[FaultID.EsObjectTypeError] = new FaultAttributes(151);
faultsAttrs[FaultID.FunctionApplyCall] = new FaultAttributes(152);
faultsAttrs[FaultID.SendableClassInheritance] = new FaultAttributes(153);
faultsAttrs[FaultID.SendablePropType] = new FaultAttributes(154);
faultsAttrs[FaultID.SendableDefiniteAssignment] = new FaultAttributes(155);
faultsAttrs[FaultID.SendableGenericTypes] = new FaultAttributes(156);
faultsAttrs[FaultID.SendableCapturedVars] = new FaultAttributes(157);
faultsAttrs[FaultID.SendableClassDecorator] = new FaultAttributes(158);
faultsAttrs[FaultID.SendableObjectInitialization] = new FaultAttributes(159);
faultsAttrs[FaultID.SendableComputedPropName] = new FaultAttributes(160);
faultsAttrs[FaultID.SendableAsExpr] = new FaultAttributes(161);
faultsAttrs[FaultID.SharedNoSideEffectImport] = new FaultAttributes(162);
faultsAttrs[FaultID.SharedModuleExports] = new FaultAttributes(163);
faultsAttrs[FaultID.SharedModuleNoWildcardExport] = new FaultAttributes(164);
faultsAttrs[FaultID.NoTsImportEts] = new FaultAttributes(165);
faultsAttrs[FaultID.SendableTypeInheritance] = new FaultAttributes(166);
faultsAttrs[FaultID.SendableTypeExported] = new FaultAttributes(167);
faultsAttrs[FaultID.NoTsReExportEts] = new FaultAttributes(168);
faultsAttrs[FaultID.NoNameSpaceImportEtsToTs] = new FaultAttributes(169);
faultsAttrs[FaultID.NoSideEffectImportEtsToTs] = new FaultAttributes(170);
faultsAttrs[FaultID.SendableExplicitFieldType] = new FaultAttributes(171);
faultsAttrs[FaultID.SendableFunctionImportedVariables] = new FaultAttributes(172);
faultsAttrs[FaultID.SendableFunctionDecorator] = new FaultAttributes(173);
faultsAttrs[FaultID.SendableTypeAliasDecorator] = new FaultAttributes(174);
faultsAttrs[FaultID.SendableTypeAliasDeclaration] = new FaultAttributes(175);
faultsAttrs[FaultID.SendableFunctionAssignment] = new FaultAttributes(176);
faultsAttrs[FaultID.SendableFunctionOverloadDecorator] = new FaultAttributes(177);
faultsAttrs[FaultID.SendableFunctionProperty] = new FaultAttributes(178);
faultsAttrs[FaultID.SendableFunctionAsExpr] = new FaultAttributes(179);
faultsAttrs[FaultID.SendableDecoratorLimited] = new FaultAttributes(180);
faultsAttrs[FaultID.SendableClosureExport] = new FaultAttributes(181, ProblemSeverity.WARNING);
faultsAttrs[FaultID.SharedModuleExportsWarning] = new FaultAttributes(163, ProblemSeverity.WARNING);
faultsAttrs[FaultID.SendableBetaCompatible] = new FaultAttributes(182);
faultsAttrs[FaultID.ObjectLiteralProperty] = new FaultAttributes(183);
faultsAttrs[FaultID.OptionalMethod] = new FaultAttributes(184);
faultsAttrs[FaultID.ImportType] = new FaultAttributes(185);
faultsAttrs[FaultID.DynamicCtorCall] = new FaultAttributes(186);
