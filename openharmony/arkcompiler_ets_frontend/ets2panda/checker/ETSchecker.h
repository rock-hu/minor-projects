/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef ES2PANDA_CHECKER_ETS_CHECKER_H
#define ES2PANDA_CHECKER_ETS_CHECKER_H

#include <cstddef>
#include <mutex>

#include "checker/checker.h"

#include "checker/types/ets/types.h"
#include "checker/resolveResult.h"
#include "ir/ts/tsInterfaceDeclaration.h"
#include "ir/visitor/AstVisitor.h"
#include "util/helpers.h"

namespace ark::es2panda::varbinder {
class VarBinder;
class Decl;
class EnumVariable;
class FunctionDecl;
class LocalVariable;
class Scope;
class Variable;
class ETSBinder;
class RecordTable;
class FunctionParamScope;
}  // namespace ark::es2panda::varbinder

namespace ark::es2panda::evaluate {
class ScopedDebugInfoPlugin;
}  // namespace ark::es2panda::evaluate

namespace ark::es2panda::checker {

struct Accessor {
    bool isGetter {false};
    bool isSetter {false};
    bool isExternal {false};
};

struct PairHash {
    size_t operator()(const std::pair<Type *, bool> &p) const
    {
        size_t hash1 = std::hash<Type *> {}(p.first);
        size_t hash2 = std::hash<bool> {}(p.second);
        return hash1 ^ (hash2 << 1ULL);
    }
};
using ComputedAbstracts =
    ArenaUnorderedMap<ETSObjectType *, std::pair<ArenaVector<ETSFunctionType *>, ArenaUnorderedSet<ETSObjectType *>>>;
using ArrayMap = ArenaUnorderedMap<std::pair<Type *, bool>, ETSArrayType *, PairHash>;
using GlobalArraySignatureMap = ArenaUnorderedMap<const ETSArrayType *, Signature *>;
using DynamicCallIntrinsicsMap = ArenaUnorderedMap<Language, ArenaUnorderedMap<util::StringView, ir::ScriptFunction *>>;
using DynamicClassIntrinsicsMap = ArenaUnorderedMap<Language, ir::ClassDeclaration *>;
using DynamicLambdaObjectSignatureMap = ArenaUnorderedMap<std::string, Signature *>;
using FunctionalInterfaceMap = ArenaUnorderedMap<util::StringView, ETSObjectType *>;
using TypeMapping = ArenaUnorderedMap<Type const *, Type *>;
using DynamicCallNamesMap = ArenaMap<const ArenaVector<util::StringView>, uint32_t>;
using ConstraintCheckRecord = std::tuple<const ArenaVector<Type *> *, const Substitution *, lexer::SourcePosition>;
// can't use util::DiagnosticWithParams because std::optional can't contain references
using MaybeDiagnosticInfo =
    std::optional<std::pair<const diagnostic::DiagnosticKind, const util::DiagnosticMessageParams>>;

class ETSChecker final : public Checker {
public:
    explicit ETSChecker(util::DiagnosticEngine &diagnosticEngine)
        // NOLINTNEXTLINE(readability-redundant-member-init)
        : Checker(diagnosticEngine),
          arrayTypes_(Allocator()->Adapter()),
          pendingConstraintCheckRecords_(Allocator()->Adapter()),
          globalArraySignatures_(Allocator()->Adapter()),
          cachedComputedAbstracts_(Allocator()->Adapter()),
          dynamicIntrinsics_ {DynamicCallIntrinsicsMap {Allocator()->Adapter()},
                              DynamicCallIntrinsicsMap {Allocator()->Adapter()}},
          dynamicClasses_ {DynamicClassIntrinsicsMap(Allocator()->Adapter()),
                           DynamicClassIntrinsicsMap(Allocator()->Adapter())},
          dynamicLambdaSignatureCache_(Allocator()->Adapter()),
          functionalInterfaceCache_(Allocator()->Adapter()),
          apparentTypes_(Allocator()->Adapter()),
          dynamicCallNames_ {
              {DynamicCallNamesMap(Allocator()->Adapter()), DynamicCallNamesMap(Allocator()->Adapter())}},
          overloadSigContainer_(Allocator()->Adapter())
    {
    }

    ~ETSChecker() override = default;

    NO_COPY_SEMANTIC(ETSChecker);
    NO_MOVE_SEMANTIC(ETSChecker);

    [[nodiscard]] static inline TypeFlag ETSType(const Type *const type) noexcept
    {
        return static_cast<TypeFlag>(type->TypeFlags() & TypeFlag::ETS_TYPE);
    }

    [[nodiscard]] static inline TypeFlag TypeKind(const Type *const type) noexcept
    {
        return static_cast<checker::TypeFlag>(type->TypeFlags() & checker::TypeFlag::ETS_TYPE);
    }

    Type *GlobalByteType() const;
    Type *GlobalShortType() const;
    Type *GlobalIntType() const;
    Type *GlobalLongType() const;
    Type *GlobalFloatType() const;
    Type *GlobalDoubleType() const;
    Type *GlobalCharType() const;
    Type *GlobalETSBooleanType() const;
    Type *GlobalVoidType() const;
    Type *GlobalETSNullType() const;
    Type *GlobalETSUndefinedType() const;
    Type *GlobalETSNeverType() const;
    Type *GlobalETSStringLiteralType() const;
    Type *GlobalETSBigIntType() const;
    Type *GlobalWildcardType() const;

    ETSObjectType *GlobalETSObjectType() const;
    ETSUnionType *GlobalETSNullishType() const;
    ETSUnionType *GlobalETSNullishObjectType() const;
    ETSObjectType *GlobalBuiltinETSStringType() const;
    ETSObjectType *GlobalBuiltinETSBigIntType() const;
    ETSObjectType *GlobalBuiltinTypeType() const;
    ETSObjectType *GlobalBuiltinExceptionType() const;
    ETSObjectType *GlobalBuiltinErrorType() const;
    ETSObjectType *GlobalStringBuilderBuiltinType() const;
    ETSObjectType *GlobalBuiltinPromiseType() const;
    ETSObjectType *GlobalBuiltinFunctionType() const;
    ETSObjectType *GlobalBuiltinJSRuntimeType() const;
    ETSObjectType *GlobalBuiltinJSValueType() const;
    ETSObjectType *GlobalBuiltinBoxType(Type *contents);

    ETSObjectType *GlobalBuiltinFunctionType(size_t nargs, bool hasRest) const;
    ETSObjectType *GlobalBuiltinLambdaType(size_t nargs, bool hasRest) const;
    size_t GlobalBuiltinFunctionTypeVariadicThreshold() const;

    ETSObjectType *GlobalBuiltinTupleType(size_t nargs) const;

    ETSObjectType *GlobalBuiltinDynamicType(Language lang) const;

    GlobalArraySignatureMap &GlobalArrayTypes();
    const GlobalArraySignatureMap &GlobalArrayTypes() const;

    Type *GlobalTypeError() const;
    [[nodiscard]] Type *InvalidateType(ir::Typed<ir::AstNode> *node);
    [[nodiscard]] Type *TypeError(ir::Typed<ir::AstNode> *node, const diagnostic::DiagnosticKind &diagKind,
                                  const lexer::SourcePosition &at);
    [[nodiscard]] Type *TypeError(ir::Typed<ir::AstNode> *node, const diagnostic::DiagnosticKind &diagKind,
                                  const util::DiagnosticMessageParams &list, const lexer::SourcePosition &at);
    [[nodiscard]] Type *TypeError(varbinder::Variable *var, const diagnostic::DiagnosticKind &diagKind,
                                  const lexer::SourcePosition &at);
    [[nodiscard]] Type *TypeError(varbinder::Variable *var, const diagnostic::DiagnosticKind &diagKind,
                                  const util::DiagnosticMessageParams &list, const lexer::SourcePosition &at);

    void InitializeBuiltins(varbinder::ETSBinder *varbinder);
    void InitializeBuiltin(varbinder::Variable *var, const util::StringView &name);
    bool StartChecker([[maybe_unused]] varbinder::VarBinder *varbinder, const util::Options &options) override;
    Type *CheckTypeCached(ir::Expression *expr) override;
    void ResolveStructuredTypeMembers([[maybe_unused]] Type *type) override {};
    Type *GetTypeFromVariableDeclaration(varbinder::Variable *const var);
    Type *GetTypeOfVariable([[maybe_unused]] varbinder::Variable *var) override;
    Type *GuaranteedTypeForUncheckedCast(Type *base, Type *substituted);
    Type *GuaranteedTypeForUncheckedCallReturn(Signature *sig);
    Type *GuaranteedTypeForUncheckedPropertyAccess(varbinder::Variable *prop);
    Type *GuaranteedTypeForUnionFieldAccess(ir::MemberExpression *memberExpression, ETSUnionType *etsUnionType);

    [[nodiscard]] bool IsETSChecker() const noexcept override
    {
        return true;
    }

    // Object
    void CheckObjectLiteralKeys(const ArenaVector<ir::Expression *> &properties);
    Type *BuildBasicClassProperties(ir::ClassDefinition *classDef);
    ETSObjectType *BuildAnonymousClassProperties(ir::ClassDefinition *classDef, ETSObjectType *superType);
    Type *BuildBasicInterfaceProperties(ir::TSInterfaceDeclaration *interfaceDecl);
    ETSObjectType *GetSuperType(ETSObjectType *type);
    ArenaVector<ETSObjectType *> GetInterfaces(ETSObjectType *type);
    void GetInterfacesOfClass(ETSObjectType *type);
    void GetInterfacesOfInterface(ETSObjectType *type);
    void ValidateImplementedInterface(ETSObjectType *type, Type *interface, std::unordered_set<Type *> *extendsSet,
                                      const lexer::SourcePosition &pos);
    void ResolveDeclaredMembersOfObject(const Type *type);
    std::optional<std::size_t> GetTupleElementAccessValue(const Type *type);
    bool ValidateArrayIndex(ir::Expression *expr, bool relaxed = false);
    bool ValidateTupleIndex(const ETSTupleType *tuple, ir::MemberExpression *expr, bool reportError = true);
    bool ValidateTupleIndexFromEtsObject(const ETSTupleType *const tuple, ir::MemberExpression *expr);
    ETSObjectType *CheckThisOrSuperAccess(ir::Expression *node, ETSObjectType *classType, std::string_view msg);
    void CreateTypeForClassOrInterfaceTypeParameters(ETSObjectType *type);
    ETSTypeParameter *SetUpParameterType(ir::TSTypeParameter *param);
    void GetInterfacesOfClass(ETSObjectType *type, ArenaVector<ETSObjectType *> &interfaces);
    void CheckIfOverrideIsValidInInterface(ETSObjectType *classType, Signature *sig, Signature *sigFunc);
    void CheckFunctionRedeclarationInInterface(ETSObjectType *classType, ArenaVector<Signature *> &similarSignatures,
                                               Signature *sigFunc);
    void ValidateAbstractMethodsToBeImplemented(ArenaVector<ETSFunctionType *> &abstractsToBeImplemented,
                                                ETSObjectType *classType,
                                                const std::vector<Signature *> &implementedSignatures);
    void ApplyModifiersAndRemoveImplementedAbstracts(ArenaVector<ETSFunctionType *>::iterator &it,
                                                     ArenaVector<ETSFunctionType *> &abstractsToBeImplemented,
                                                     ETSObjectType *classType, bool &functionOverridden,
                                                     const Accessor &isGetSetExternal);
    void ValidateAbstractSignature(ArenaVector<ETSFunctionType *>::iterator &it,
                                   ArenaVector<ETSFunctionType *> &abstractsToBeImplemented,
                                   const std::vector<Signature *> &implementedSignatures, bool &functionOverridden,
                                   Accessor &isGetSetExternal);
    void ValidateNonOverriddenFunction(ETSObjectType *classType, ArenaVector<ETSFunctionType *>::iterator &it,
                                       ArenaVector<ETSFunctionType *> &abstractsToBeImplemented,
                                       bool &functionOverridden, const Accessor &isGetSet);
    void MaybeReportErrorsForOverridingValidation(ArenaVector<ETSFunctionType *> &abstractsToBeImplemented,
                                                  ETSObjectType *classType, const lexer::SourcePosition &pos,
                                                  bool reportError);
    void ValidateOverriding(ETSObjectType *classType, const lexer::SourcePosition &pos);
    void CheckInterfaceFunctions(ETSObjectType *classType);
    void CollectImplementedMethodsFromInterfaces(ETSObjectType *classType,
                                                 std::vector<Signature *> *implementedSignatures,
                                                 const ArenaVector<ETSFunctionType *> &abstractsToBeImplemented);
    void AddImplementedSignature(std::vector<Signature *> *implementedSignatures, varbinder::LocalVariable *function,
                                 ETSFunctionType *it);
    void CheckInnerClassMembers(const ETSObjectType *classType);
    void CheckLocalClass(ir::ClassDefinition *classDef, CheckerStatus &checkerStatus);
    void CheckClassDefinition(ir::ClassDefinition *classDef);
    void CheckClassElement(ir::ClassDefinition *classDef);
    void CheckClassAnnotations(ir::ClassDefinition *classDef);
    void CheckInterfaceAnnotations(ir::TSInterfaceDeclaration *interfaceDecl);
    void CheckConstructors(ir::ClassDefinition *classDef, ETSObjectType *classType);
    void FindAssignment(const ir::AstNode *node, const varbinder::LocalVariable *classVar, bool &initialized);
    void FindAssignments(const ir::AstNode *node, const varbinder::LocalVariable *classVar, bool &initialized);
    void CheckConstFields(const ETSObjectType *classType);
    void CheckConstFieldInitialized(const ETSObjectType *classType, varbinder::LocalVariable *classVar);
    void CheckConstFieldInitialized(const Signature *signature, varbinder::LocalVariable *classVar);
    void ComputeAbstractsFromInterface(ETSObjectType *interfaceType);
    ArenaVector<ETSFunctionType *> &GetAbstractsForClass(ETSObjectType *classType);
    std::vector<Signature *> CollectAbstractSignaturesFromObject(const ETSObjectType *objType);
    void CreateFunctionTypesFromAbstracts(const std::vector<Signature *> &abstracts,
                                          ArenaVector<ETSFunctionType *> *target);
    void CheckCyclicConstructorCall(Signature *signature);
    std::vector<ResolveResult *> ResolveMemberReference(const ir::MemberExpression *memberExpr,
                                                        const ETSObjectType *target);
    void WarnForEndlessLoopInGetterSetter(const ir::MemberExpression *const memberExpr);
    varbinder::Variable *GetExtensionFuncVarInGlobalFunction(const ir::MemberExpression *const memberExpr);
    varbinder::Variable *GetExtensionFuncVarInGlobalField(const ir::MemberExpression *const memberExpr);
    varbinder::Variable *GetExtensionFuncVarInFunctionScope(const ir::MemberExpression *const memberExpr);
    varbinder::Variable *ResolveInstanceExtension(const ir::MemberExpression *memberExpr);
    void CheckImplicitSuper(ETSObjectType *classType, Signature *ctorSig);
    void CheckThisOrSuperCallInConstructor(ETSObjectType *classType, Signature *ctorSig);
    void CheckExpressionsInConstructor(const ArenaVector<const ir::Expression *> &arguments);
    ArenaVector<const ir::Expression *> CheckMemberOrCallOrObjectExpressionInConstructor(const ir::Expression *arg);
    void CheckValidInheritance(ETSObjectType *classType, ir::ClassDefinition *classDef);
    void CheckProperties(ETSObjectType *classType, ir::ClassDefinition *classDef, varbinder::LocalVariable *it,
                         varbinder::LocalVariable *found, ETSObjectType *interfaceFound);
    void CheckReadonlyClassPropertyInImplementedInterface(ETSObjectType *classType, varbinder::LocalVariable *field);
    void TransformProperties(ETSObjectType *classType);
    void CheckGetterSetterProperties(ETSObjectType *classType);
    void AddElementsToModuleObject(ETSObjectType *moduleObj, const util::StringView &str);
    void ComputeApparentType(Type *type)
    {
        // SUPPRESS_CSA_NEXTLINE(alpha.core.AllocatorETSCheckerHint)
        [[maybe_unused]] auto x = GetApparentType(type);
    }
    [[nodiscard]] Type *GetApparentType(Type *type);
    [[nodiscard]] Type const *GetApparentType(Type const *type) const;
    ETSObjectType *GetClosestCommonAncestor(ETSObjectType *source, ETSObjectType *target);
    bool HasETSFunctionType(ir::TypeNode *typeAnnotation);

    void VariableTypeFromInitializer(varbinder::Variable *variable, Type *annotationType, Type *initType);

    // Type creation
    ByteType *CreateByteType(int8_t value);
    ETSBooleanType *CreateETSBooleanType(bool value);
    DoubleType *CreateDoubleType(double value);
    FloatType *CreateFloatType(float value);
    IntType *CreateIntType(int32_t value);
    LongType *CreateLongType(int64_t value);
    ShortType *CreateShortType(int16_t value);
    CharType *CreateCharType(char16_t value);
    ETSBigIntType *CreateETSBigIntLiteralType(util::StringView value);
    ETSStringType *CreateETSStringLiteralType(util::StringView value);
    ETSArrayType *CreateETSArrayType(Type *elementType, bool isCachePolluting = false);
    Type *CreateETSUnionType(Span<Type *const> constituentTypes);
    template <size_t N>
    Type *CreateETSUnionType(Type *const (&arr)[N])  // NOLINT(modernize-avoid-c-arrays)
    {
        return CreateETSUnionType(Span(arr));
    }
    Type *CreateETSUnionType(ArenaVector<Type *> &&constituentTypes)
    {
        return CreateETSUnionType(Span<Type *const>(constituentTypes));
    }
    Type *CreateUnionFromKeyofType(ETSObjectType *const type);
    void ProcessTypeMembers(ETSObjectType *type, ArenaVector<Type *> &literals);
    ETSAsyncFuncReturnType *CreateETSAsyncFuncReturnTypeFromPromiseType(ETSObjectType *promiseType);
    ETSAsyncFuncReturnType *CreateETSAsyncFuncReturnTypeFromBaseType(Type *baseType);
    ETSTypeAliasType *CreateETSTypeAliasType(util::StringView name, const ir::AstNode *declNode,
                                             bool isRecursive = false);
    ETSFunctionType *CreateETSArrowType(Signature *signature);
    ETSFunctionType *CreateETSMethodType(util::StringView name, ArenaVector<Signature *> &&signatures);
    ETSFunctionType *CreateETSDynamicArrowType(Signature *signature, Language lang);
    ETSFunctionType *CreateETSDynamicMethodType(util::StringView name, ArenaVector<Signature *> &&signatures,
                                                Language lang);
    ETSExtensionFuncHelperType *CreateETSExtensionFuncHelperType(ETSFunctionType *classMethodType,
                                                                 ETSFunctionType *extensionFunctionType);
    void AddThisReturnTypeFlagForInterfaceInvoke(ETSObjectType *interface);
    ETSTypeParameter *CreateTypeParameter();
    ETSObjectType *CreateETSObjectType(ir::AstNode *declNode, ETSObjectFlags flags);
    ETSObjectType *CreateETSObjectTypeOrBuiltin(ir::AstNode *declNode, ETSObjectFlags flags);
    std::tuple<util::StringView, SignatureInfo *> CreateBuiltinArraySignatureInfo(const ETSArrayType *arrayType,
                                                                                  size_t dim);
    Signature *CreateBuiltinArraySignature(const ETSArrayType *arrayType, size_t dim);
    IntType *CreateIntTypeFromType(Type *type);
    std::tuple<Language, bool> CheckForDynamicLang(ir::AstNode *declNode, util::StringView assemblerName);
    ETSObjectType *CreatePromiseOf(Type *type);

    Signature *CreateSignature(SignatureInfo *info, Type *returnType, ir::ScriptFunction *func);
    Signature *CreateSignature(SignatureInfo *info, Type *returnType, ir::ScriptFunctionFlags sff, bool hasReceiver);
    SignatureInfo *CreateSignatureInfo();

    // Arithmetic
    Type *NegateNumericType(Type *type, ir::Expression *node);
    bool CheckBinaryOperatorForBigInt(Type *left, Type *right, lexer::TokenType op);
    [[nodiscard]] bool CheckBinaryPlusMultDivOperandsForUnionType(const Type *leftType, const Type *rightType,
                                                                  const ir::Expression *left,
                                                                  const ir::Expression *right);
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    std::tuple<Type *, Type *> CheckBinaryOperator(ir::Expression *left, ir::Expression *right, ir::Expression *expr,
                                                   lexer::TokenType operationType, lexer::SourcePosition pos,
                                                   bool forcePromotion = false);
    std::tuple<Type *, Type *> CheckArithmeticOperations(
        ir::Expression *expr,
        std::tuple<ir::Expression *, ir::Expression *, lexer::TokenType, lexer::SourcePosition> op, bool isEqualOp,
        std::tuple<checker::Type *, checker::Type *, Type *, Type *> types);
    checker::Type *CheckBinaryOperatorMulDivMod(
        std::tuple<ir::Expression *, ir::Expression *, lexer::TokenType, lexer::SourcePosition> op, bool isEqualOp,
        std::tuple<checker::Type *, checker::Type *, Type *, Type *> types);
    checker::Type *CheckBinaryOperatorForIntEnums(const checker::Type *const leftType,
                                                  const checker::Type *const rightType);
    checker::Type *CheckBinaryBitwiseOperatorForIntEnums(const checker::Type *const leftType,
                                                         const checker::Type *const rightType);
    checker::Type *CheckBinaryOperatorPlusForEnums(const checker::Type *const leftType,
                                                   const checker::Type *const rightType);
    checker::Type *CheckBinaryOperatorPlus(
        std::tuple<ir::Expression *, ir::Expression *, lexer::TokenType, lexer::SourcePosition> op, bool isEqualOp,
        std::tuple<checker::Type *, checker::Type *, Type *, Type *> types);
    checker::Type *CheckBinaryOperatorShift(
        std::tuple<ir::Expression *, ir::Expression *, lexer::TokenType, lexer::SourcePosition> op, bool isEqualOp,
        std::tuple<checker::Type *, checker::Type *, Type *, Type *> types);
    checker::Type *CheckBinaryOperatorBitwise(
        std::tuple<ir::Expression *, ir::Expression *, lexer::TokenType, lexer::SourcePosition> op, bool isEqualOp,
        std::tuple<checker::Type *, checker::Type *, Type *, Type *> types);
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    checker::Type *CheckBinaryOperatorLogical(ir::Expression *left, ir::Expression *right, ir::BinaryExpression *expr,
                                              checker::Type *leftType, checker::Type *rightType, Type *unboxedL,
                                              Type *unboxedR);
    std::tuple<Type *, Type *> CheckBinaryOperatorStrictEqual(ir::Expression *left, lexer::TokenType operationType,
                                                              lexer::SourcePosition pos, checker::Type *leftType,
                                                              checker::Type *rightType);
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    std::tuple<Type *, Type *> CheckBinaryOperatorLessGreater(ir::Expression *left, ir::Expression *right,
                                                              lexer::TokenType operationType, lexer::SourcePosition pos,
                                                              bool isEqualOp, checker::Type *leftType,
                                                              checker::Type *rightType, Type *unboxedL, Type *unboxedR);
    std::tuple<Type *, Type *> CheckBinaryOperatorInstanceOf(lexer::SourcePosition pos, checker::Type *leftType,
                                                             checker::Type *rightType);
    checker::Type *CheckBinaryOperatorNullishCoalescing(ir::Expression *left, ir::Expression *right,
                                                        lexer::SourcePosition pos);
    bool AdjustNumberLiteralType(ir::NumberLiteral *literal, Type *literalType, Type *otherType);

    Type *HandleArithmeticOperationOnTypes(Type *left, Type *right, lexer::TokenType operationType);
    Type *HandleBitwiseOperationOnTypes(Type *left, Type *right, lexer::TokenType operationType);
    void FlagExpressionWithUnboxing(Type *type, Type *unboxedType, ir::Expression *typeExpression);
    template <typename ValueType>
    Type *PerformArithmeticOperationOnTypes(Type *left, Type *right, lexer::TokenType operationType);

    Type *HandleRelationOperationOnTypes(Type *left, Type *right, lexer::TokenType operationType);
    template <typename TargetType>
    Type *PerformRelationOperationOnTypes(Type *left, Type *right, lexer::TokenType operationType);

    // Function
    bool NeedTypeInference(const ir::ScriptFunction *lambda);
    std::vector<bool> FindTypeInferenceArguments(const ArenaVector<ir::Expression *> &arguments);
    void InferTypesForLambda(ir::ScriptFunction *lambda, ir::ETSFunctionType *calleeType,
                             Signature *maybeSubstitutedFunctionSig = nullptr);
    void InferTypesForLambda(ir::ScriptFunction *lambda, Signature *signature);
    void TryInferTypeForLambdaTypeAlias(ir::ArrowFunctionExpression *expr, ETSFunctionType *calleeType);
    bool TypeInference(Signature *signature, const ArenaVector<ir::Expression *> &arguments,
                       TypeRelationFlag flags = TypeRelationFlag::NONE);
    bool CheckLambdaTypeAnnotation(ir::AstNode *typeAnnotation, ir::ArrowFunctionExpression *arrowFuncExpr,
                                   Type *parameterType, TypeRelationFlag flags);
    bool CheckLambdaInfer(ir::AstNode *typeAnnotation, ir::ArrowFunctionExpression *arrowFuncExpr,
                          Type *const subParameterType);
    bool CheckLambdaAssignable(ir::Expression *param, ir::ScriptFunction *lambda);
    bool CheckLambdaAssignableUnion(ir::AstNode *typeAnn, ir::ScriptFunction *lambda);
    bool IsCompatibleTypeArgument(ETSTypeParameter *typeParam, Type *typeArgument, const Substitution *substitution);
    Substitution *NewSubstitution()
    {
        return Allocator()->New<Substitution>(Allocator()->Adapter());
    }
    Substitution *CopySubstitution(const Substitution *src)
    {
        return Allocator()->New<Substitution>(*src);
    }
    bool ValidateTypeSubstitution(const ArenaVector<Type *> &typeParams, Type *ctype, Type *argumentType,
                                  Substitution *substitution);
    bool ProcessUntypedParameter(ir::AstNode *declNode, size_t paramIndex, Signature *paramSig, Signature *argSig,
                                 Substitution *substitution);
    void EmplaceSubstituted(Substitution *substitution, ETSTypeParameter *tparam, Type *typeArg);
    [[nodiscard]] bool EnhanceSubstitutionForType(const ArenaVector<Type *> &typeParams, Type *paramType,
                                                  Type *argumentType, Substitution *substitution);
    [[nodiscard]] bool EnhanceSubstitutionForReadonly(const ArenaVector<Type *> &typeParams, ETSReadonlyType *paramType,
                                                      Type *argumentType, Substitution *substitution);
    [[nodiscard]] bool EnhanceSubstitutionForObject(const ArenaVector<Type *> &typeParams, ETSObjectType *paramType,
                                                    Type *argumentType, Substitution *substitution);
    [[nodiscard]] bool EnhanceSubstitutionForFunction(const ArenaVector<Type *> &typeParams, ETSFunctionType *paramType,
                                                      Type *argumentType, Substitution *substitution);
    [[nodiscard]] bool EnhanceSubstitutionForUnion(const ArenaVector<Type *> &typeParams, ETSUnionType *paramUn,
                                                   Type *argumentType, Substitution *substitution);
    [[nodiscard]] bool EnhanceSubstitutionForArray(const ArenaVector<Type *> &typeParams, ETSArrayType *paramType,
                                                   Type *argumentType, Substitution *substitution);
    std::pair<ArenaVector<Type *>, bool> CreateUnconstrainedTypeParameters(
        ir::TSTypeParameterDeclaration const *typeParams);
    void AssignTypeParameterConstraints(ir::TSTypeParameterDeclaration const *typeParams);
    Signature *ValidateParameterlessConstructor(Signature *signature, const lexer::SourcePosition &pos,
                                                bool throwError);
    Signature *CollectParameterlessConstructor(ArenaVector<Signature *> &signatures, const lexer::SourcePosition &pos);
    Signature *ValidateSignature(
        std::tuple<Signature *, const ir::TSTypeParameterInstantiation *, TypeRelationFlag> info,
        const ArenaVector<ir::Expression *> &arguments, const lexer::SourcePosition &pos,
        const std::vector<bool> &argTypeInferenceRequired, const bool unique);
    bool ValidateSignatureRequiredParams(Signature *substitutedSig, const ArenaVector<ir::Expression *> &arguments,
                                         TypeRelationFlag flags, const std::vector<bool> &argTypeInferenceRequired,
                                         bool reportError);
    bool ValidateSignatureInvocationContext(Signature *substitutedSig, ir::Expression *argument, std::size_t index,
                                            TypeRelationFlag flags);
    bool CheckOptionalLambdaFunction(ir::Expression *argument, Signature *substitutedSig, std::size_t index);
    bool ValidateArgumentAsIdentifier(const ir::Identifier *identifier);
    bool IsValidRestArgument(ir::Expression *argument, Signature *substitutedSig, TypeRelationFlag flags,
                             std::size_t index);
    bool ValidateSignatureRestParams(Signature *substitutedSig, const ArenaVector<ir::Expression *> &arguments,
                                     TypeRelationFlag flags, bool reportError, bool unique);
    void ThrowSignatureMismatch(ArenaVector<Signature *> &signatures, const ArenaVector<ir::Expression *> &arguments,
                                const lexer::SourcePosition &pos, std::string_view signatureKind);
    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    Signature *ValidateSignatures(ArenaVector<Signature *> &signatures,
                                  const ir::TSTypeParameterInstantiation *typeArguments,
                                  const ArenaVector<ir::Expression *> &arguments, const lexer::SourcePosition &pos,
                                  std::string_view signatureKind,
                                  TypeRelationFlag resolveFlags = TypeRelationFlag::NONE);
    Signature *FindMostSpecificSignature(const ArenaVector<Signature *> &signatures,
                                         const ArenaMultiMap<size_t, Signature *> &bestSignaturesForParameter,
                                         size_t paramCount);
    void SearchAmongMostSpecificTypes(Type *&mostSpecificType, Signature *&prevSig,
                                      std::tuple<const lexer::SourcePosition &, size_t, Signature *> info,
                                      bool lookForClassType);
    void CollectSuitableSignaturesForTypeInference(size_t paramIdx, ArenaVector<Signature *> &signatures,
                                                   ArenaMultiMap<size_t, Signature *> &bestSignaturesForParameter,
                                                   const ArenaVector<ir::Expression *> &arguments);
    ArenaMultiMap<size_t, Signature *> GetSuitableSignaturesForParameter(
        const std::vector<bool> &argTypeInferenceRequired, size_t paramCount, ArenaVector<Signature *> &signatures,
        const ArenaVector<ir::Expression *> &arguments, const lexer::SourcePosition &pos);
    Signature *ChooseMostSpecificSignature(ArenaVector<Signature *> &signatures,
                                           const std::vector<bool> &argTypeInferenceRequired,
                                           const ArenaVector<ir::Expression *> &arguments,
                                           const lexer::SourcePosition &pos, size_t argumentsSize = ULONG_MAX);
    Signature *ResolvePotentialTrailingLambdaWithReceiver(ir::CallExpression *callExpr,
                                                          ArenaVector<Signature *> const &signatures,
                                                          ArenaVector<ir::Expression *> &arguments);
    Signature *ResolveCallExpressionAndTrailingLambda(ArenaVector<Signature *> &signatures,
                                                      ir::CallExpression *callExpr, const lexer::SourcePosition &pos,
                                                      TypeRelationFlag reportFlag = TypeRelationFlag::NONE);
    void UpdateDeclarationFromSignature(ir::CallExpression *expr, checker::Signature *signature);
    Signature *ResolveConstructExpression(ETSObjectType *type, const ArenaVector<ir::Expression *> &arguments,
                                          const lexer::SourcePosition &pos);
    void CheckObjectLiteralArguments(Signature *sig, ArenaVector<ir::Expression *> const &arguments);

    Signature *ComposeSignature(ir::ScriptFunction *func, SignatureInfo *signatureInfo, Type *returnType,
                                varbinder::Variable *nameVar);
    Type *ComposeReturnType(ir::TypeNode *typeAnnotation, bool isAsync);
    SignatureInfo *ComposeSignatureInfo(ir::TSTypeParameterDeclaration *typeParams,
                                        ArenaVector<ir::Expression *> const &params);
    void ValidateMainSignature(ir::ScriptFunction *func);
    void BuildFunctionSignature(ir::ScriptFunction *func, bool isConstructSig = false);
    ETSFunctionType *BuildMethodType(ir::ScriptFunction *func);
    Type *BuildMethodSignature(ir::MethodDefinition *method);
    Signature *CheckEveryAbstractSignatureIsOverridden(ETSFunctionType *target, ETSFunctionType *source);
    static Signature *GetSignatureFromMethodDefinition(const ir::MethodDefinition *methodDef);
    bool CheckIdenticalOverloads(ETSFunctionType *func, ETSFunctionType *overload,
                                 const ir::MethodDefinition *currentFunc, bool omitSameAsm = false);
    static bool CmpAssemblerTypesWithRank(Signature const *const sig1, Signature const *const sig2) noexcept;
    static bool HasSameAssemblySignature(Signature const *const sig1, Signature const *const sig2) noexcept;
    static bool HasSameAssemblySignatures(ETSFunctionType const *const func1,
                                          ETSFunctionType const *const func2) noexcept;

    Signature *AdjustForTypeParameters(Signature *source, Signature *target);
    void ReportOverrideError(Signature *signature, Signature *overriddenSignature, const OverrideErrorCode &errorCode);
    void CheckOverride(Signature *signature);
    bool CheckOverride(Signature *signature, ETSObjectType *site);
    OverrideErrorCode CheckOverride(Signature *signature, Signature *other);
    bool IsMethodOverridesOther(Signature *base, Signature *derived);
    bool IsOverridableIn(Signature *signature);
    [[nodiscard]] bool AreOverrideCompatible(Signature *s1, Signature *s2);
    [[nodiscard]] bool IsReturnTypeSubstitutable(Signature *s1, Signature *s2);
    bool NeedToVerifySignatureVisibility(Signature *signature, const lexer::SourcePosition &pos);
    void ValidateSignatureAccessibility(ETSObjectType *callee, Signature *signature, const lexer::SourcePosition &pos,
                                        const MaybeDiagnosticInfo &maybeErrorInfo = std::nullopt);
    void CheckCapturedVariables();
    void CheckCapturedVariableInSubnodes(ir::AstNode *node, varbinder::Variable *var);
    void CheckCapturedVariable(ir::AstNode *node, varbinder::Variable *var);

    // CC-OFFNXT(G.FUN.01-CPP) solid logic
    ir::MethodDefinition *CreateMethod(const util::StringView &name, ir::ModifierFlags modifiers,
                                       ir::ScriptFunctionFlags flags, ArenaVector<ir::Expression *> &&params,
                                       varbinder::FunctionParamScope *paramScope, ir::TypeNode *returnType,
                                       ir::AstNode *body);
    varbinder::FunctionParamScope *CopyParams(
        const ArenaVector<ir::Expression *> &params, ArenaVector<ir::Expression *> &outParams,
        ArenaUnorderedMap<varbinder::Variable *, varbinder::Variable *> *paramVarMap);
    void ReplaceScope(ir::AstNode *root, ir::AstNode *oldNode, varbinder::Scope *newScope);

    // Helpers
    static std::string GetAsyncImplName(const util::StringView &name);
    static std::string GetAsyncImplName(ir::MethodDefinition *asyncMethod);
    static bool IsAsyncImplMethod(ir::MethodDefinition const *method);
    std::vector<util::StringView> GetNameForSynteticObjectType(const util::StringView &source);
    template <checker::PropertyType TYPE>
    void BindingsModuleObjectAddProperty(checker::ETSObjectType *moduleObjType, ir::ETSImportDeclaration *importDecl,
                                         const varbinder::Scope::VariableMap &bindings,
                                         const util::StringView &importPath);
    util::StringView FindPropNameForNamespaceImport(const util::StringView &originalName,
                                                    const util::StringView &importPath);
    void SetPropertiesForModuleObject(checker::ETSObjectType *moduleObjType, const util::StringView &importPath,
                                      ir::ETSImportDeclaration *importDecl = nullptr);
    void SetrModuleObjectTsType(ir::Identifier *local, checker::ETSObjectType *moduleObjType);
    Type *GetReferencedTypeFromBase(Type *baseType, ir::Expression *name);
    Type *GetReferencedTypeBase(ir::Expression *name);
    Type *ResolveReferencedType(varbinder::LocalVariable *refVar, const ir::Expression *name);
    Type *GetTypeFromInterfaceReference(varbinder::Variable *var);
    Type *GetTypeFromTypeAliasReference(varbinder::Variable *var);
    Type *GetTypeFromClassReference(varbinder::Variable *var);
    void ValidateGenericTypeAliasForClonedNode(ir::TSTypeAliasDeclaration *typeAliasNode,
                                               const ir::TSTypeParameterInstantiation *exactTypeParams);
    Type *HandleTypeAlias(ir::Expression *name, const ir::TSTypeParameterInstantiation *typeParams,
                          ir::TSTypeAliasDeclaration *const typeAliasNode);
    bool CheckMinimumTypeArgsPresent(const ir::TSTypeAliasDeclaration *typeAliasNode,
                                     const ir::TSTypeParameterInstantiation *typeParams);
    static ir::TypeNode *ResolveTypeNodeForTypeArg(const ir::TSTypeAliasDeclaration *typeAliasNode,
                                                   const ir::TSTypeParameterInstantiation *typeParams, size_t idx);
    Type *GetTypeFromTypeParameterReference(varbinder::LocalVariable *var, const lexer::SourcePosition &pos);
    Type *GetNonConstantType(Type *type);
    bool IsNullLikeOrVoidExpression(const ir::Expression *expr) const;
    bool IsConstantExpression(ir::Expression *expr, Type *type);
    void ValidateUnaryOperatorOperand(varbinder::Variable *variable);
    bool ValidateAnnotationPropertyType(checker::Type *tsType);
    void ProcessRequiredFields(ArenaUnorderedMap<util::StringView, ir::ClassProperty *> &fieldMap,
                               ir::AnnotationUsage *st, ETSChecker *checker) const;
    void CheckFunctionSignatureAnnotations(const ArenaVector<ir::Expression *> &params,
                                           ir::TSTypeParameterDeclaration *typeParams,
                                           ir::TypeNode *returnTypeAnnotation);
    bool CheckAndLogInvalidThisUsage(const ir::TypeNode *type, const diagnostic::DiagnosticKind &diagnostic);
    void ValidateThisUsage(const ir::TypeNode *returnTypeAnnotation);
    void CheckAnnotations(const ArenaVector<ir::AnnotationUsage *> &annotations);
    void CheckAmbientAnnotation(ir::AnnotationDeclaration *annoImpl, ir::AnnotationDeclaration *annoDecl);
    bool CheckAmbientAnnotationFieldInitializerValue(ir::Expression *init, ir::Expression *expected);
    bool CheckAmbientAnnotationFieldInitializer(ir::Expression *init, ir::Expression *expected);
    void CheckAnnotationRetention(ir::AnnotationUsage *anno);
    void HandleAnnotationRetention(ir::AnnotationUsage *anno, ir::AnnotationDeclaration *annoDecl);
    void CheckStandardAnnotation(ir::AnnotationUsage *anno);
    void CheckAnnotationPropertyType(ir::ClassProperty *property);
    void CheckSinglePropertyAnnotation(ir::AnnotationUsage *st, ir::AnnotationDeclaration *annoDecl);
    void CheckMultiplePropertiesAnnotation(ir::AnnotationUsage *st, util::StringView const &baseName,
                                           ArenaUnorderedMap<util::StringView, ir::ClassProperty *> &fieldMap);
    void InferAliasLambdaType(ir::TypeNode *localTypeAnnotation, ir::ArrowFunctionExpression *init);
    checker::Type *ApplyConditionalOperatorPromotion(checker::ETSChecker *checker, checker::Type *unboxedL,
                                                     checker::Type *unboxedR);
    Type *ApplyUnaryOperatorPromotion(Type *type, bool createConst = true, bool doPromotion = true,
                                      bool isCondExpr = false);
    Type *HandleBooleanLogicalOperators(Type *leftType, Type *rightType, lexer::TokenType tokenType);

    bool HandleLogicalPotentialResult(ir::Expression *left, ir::Expression *right, ir::BinaryExpression *expr,
                                      checker::Type *leftType);

    checker::Type *FixOptionalVariableType(varbinder::Variable *const bindingVar, ir::ModifierFlags flags,
                                           ir::Expression *init);
    void CheckEnumType(ir::Expression *init, checker::Type *initType, const util::StringView &varName);
    checker::Type *CheckVariableDeclaration(ir::Identifier *ident, ir::TypeNode *typeAnnotation, ir::Expression *init,
                                            ir::ModifierFlags flags);
    void CheckTruthinessOfType(ir::Expression *expr);

    bool CheckNonNullish(ir::Expression const *expr);
    Type *GetNonNullishType(Type *type);
    Type *RemoveNullType(Type *type);
    Type *RemoveUndefinedType(Type *type);
    std::pair<Type *, Type *> RemoveNullishTypes(Type *type);

    void ConcatConstantString(util::UString &target, Type *type);
    Type *HandleStringConcatenation(Type *leftType, Type *rightType);
    Type *ResolveIdentifier(ir::Identifier *ident);
    ETSFunctionType *FindFunctionInVectorGivenByName(util::StringView name, ArenaVector<ETSFunctionType *> &list);
    void MergeComputedAbstracts(ArenaVector<ETSFunctionType *> &merged, ArenaVector<ETSFunctionType *> &current);
    void MergeSignatures(ETSFunctionType *target, ETSFunctionType *source);
    ir::AstNode *FindAncestorGivenByType(ir::AstNode *node, ir::AstNodeType type, const ir::AstNode *endNode = nullptr);
    util::StringView GetContainingObjectNameFromSignature(Signature *signature);
    bool IsFunctionContainsSignature(ETSFunctionType *funcType, Signature *signature);
    bool CheckFunctionContainsClashingSignature(const ETSFunctionType *funcType, Signature *signature);
    static bool IsReferenceType(const Type *type)
    {
        return type->IsETSReferenceType();
    }
    void ValidatePropertyAccess(varbinder::Variable *var, ETSObjectType *obj, const lexer::SourcePosition &pos);
    varbinder::VariableFlags GetAccessFlagFromNode(const ir::AstNode *node);
    Type *CheckSwitchDiscriminant(ir::Expression *discriminant);
    Type *MaybeUnboxInRelation(Type *objectType);
    Type *MaybeUnboxConditionalInRelation(Type *objectType);
    Type *MaybeBoxInRelation(Type *objectType);
    void AddBoxingUnboxingFlagsToNode(ir::AstNode *node, Type *boxingUnboxingType);
    ir::BoxingUnboxingFlags GetBoxingFlag(Type *boxingType);
    ir::BoxingUnboxingFlags GetUnboxingFlag(Type const *unboxingType) const;
    Type *MaybeBoxExpression(ir::Expression *expr);
    Type *MaybeUnboxExpression(ir::Expression *expr);
    Type *MaybeBoxType(Type *type) const;
    Type *MaybeUnboxType(Type *type) const;
    Type const *MaybeBoxType(Type const *type) const;
    Type const *MaybeUnboxType(Type const *type) const;
    void CheckForSameSwitchCases(ArenaVector<ir::SwitchCaseStatement *> const &cases);
    std::string GetStringFromIdentifierValue(checker::Type *caseType) const;
    bool CompareIdentifiersValuesAreDifferent(ir::Expression *compareValue, const std::string &caseValue);
    void CheckIdentifierSwitchCase(ir::Expression *currentCase, ir::Expression *compareCase,
                                   const lexer::SourcePosition &pos);
    std::string GetStringFromLiteral(ir::Expression *caseTest) const;
    varbinder::Variable *FindVariableInFunctionScope(util::StringView name);
    std::pair<varbinder::Variable *, const ETSObjectType *> FindVariableInClassOrEnclosing(
        util::StringView name, const ETSObjectType *classType);
    varbinder::Variable *FindVariableInGlobal(const ir::Identifier *identifier);
    varbinder::Variable *ExtraCheckForResolvedError(ir::Identifier *ident);
    void ValidateResolvedIdentifier(ir::Identifier *ident);
    static bool IsVariableStatic(const varbinder::Variable *var);
    static bool IsVariableGetterSetter(const varbinder::Variable *var);
    static bool IsVariableExtensionAccessor(const varbinder::Variable *var);
    bool IsSameDeclarationType(varbinder::LocalVariable *target, varbinder::LocalVariable *compare);
    void SaveCapturedVariable(varbinder::Variable *var, ir::Identifier *ident);
    bool SaveCapturedVariableInLocalClass(varbinder::Variable *var, ir::Identifier *ident);
    void MaybeAddBoxingFlagInRelation(TypeRelation *relation, Type *target);
    void MaybeAddUnboxingFlagInRelation(TypeRelation *relation, Type *source, Type *self);
    void CheckUnboxedTypeWidenable(TypeRelation *relation, Type *target, Type *self);
    void CheckUnboxedTypesAssignable(TypeRelation *relation, Type *source, Type *target);
    void CheckBoxedSourceTypeAssignable(TypeRelation *relation, Type *source, Type *target);
    void CheckUnboxedSourceTypeWithWideningAssignable(TypeRelation *relation, Type *source, Type *target);
    void CheckValidGenericTypeParameter(Type *argType, const lexer::SourcePosition &pos);
    void ValidateNamespaceProperty(varbinder::Variable *property, const ETSObjectType *target,
                                   const ir::Identifier *ident);
    void ValidateResolvedProperty(varbinder::LocalVariable **property, const ETSObjectType *target,
                                  const ir::Identifier *ident, PropertySearchFlags flags);
    bool CheckNumberOfTypeArguments(ETSObjectType *type, ir::TSTypeParameterInstantiation *typeArgs,
                                    const lexer::SourcePosition &pos);
    ir::BlockStatement *FindFinalizerOfTryStatement(ir::AstNode *startFrom, const ir::AstNode *p);
    void CheckExceptionClauseType(const std::vector<checker::ETSObjectType *> &exceptions, ir::CatchClause *catchClause,
                                  checker::Type *clauseType);
    ETSObjectType *GetRelevantArgumentedTypeFromChild(ETSObjectType *child, ETSObjectType *target);
    util::StringView GetHashFromTypeArguments(const ArenaVector<Type *> &typeArgTypes);
    util::StringView GetHashFromSubstitution(const Substitution *substitution, const bool isExtensionFuncFlag);
    util::StringView GetHashFromFunctionType(ir::ETSFunctionType *type);
    static ETSObjectType *GetOriginalBaseType(Type *object);
    void SetArrayPreferredTypeForNestedMemberExpressions(ir::MemberExpression *expr, Type *annotationType);
    bool IsExtensionETSFunctionType(const checker::Type *type);
    bool IsExtensionAccessorFunctionType(const checker::Type *type);
    bool IsArrayExprSizeValidForTuple(const ir::ArrayExpression *arrayExpr, const ETSTupleType *tuple);
    void ModifyPreferredType(ir::ArrayExpression *arrayExpr, Type *newPreferredType);
    Type *SelectGlobalIntegerTypeForNumeric(Type *type);

    ir::ClassProperty *ClassPropToImplementationProp(ir::ClassProperty *classProp, varbinder::ClassScope *scope);
    ir::Expression *GenerateImplicitInstantiateArg(const std::string &className);
    void GenerateGetterSetterBody(ArenaVector<ir::Statement *> &stmts, ArenaVector<ir::Expression *> &params,
                                  ir::ClassProperty *field, varbinder::FunctionParamScope *paramScope, bool isSetter);
    static ir::MethodDefinition *GenerateDefaultGetterSetter(ir::ClassProperty *property, ir::ClassProperty *field,
                                                             varbinder::ClassScope *scope, bool isSetter,
                                                             ETSChecker *checker);
    void GenerateGetterSetterPropertyAndMethod(ir::ClassProperty *originalProp, ETSObjectType *classType);
    void SetupGetterSetterFlags(ir::ClassProperty *originalProp, ETSObjectType *classType, ir::MethodDefinition *getter,
                                ir::MethodDefinition *setter, const bool inExternal);
    Type *GetImportSpecifierObjectType(ir::ETSImportDeclaration *importDecl, ir::Identifier *ident);
    void ImportNamespaceObjectTypeAddReExportType(ir::ETSImportDeclaration *importDecl,
                                                  checker::ETSObjectType *lastObjectType, ir::Identifier *ident);
    bool CheckValidEqualReferenceType(checker::Type *const leftType, checker::Type *const rightType);
    bool CheckVoidAnnotation(const ir::ETSPrimitiveType *typeAnnotation);
    void ETSObjectTypeDeclNode(ETSChecker *checker, ETSObjectType *const objectType);
    ir::CallExpression *CreateExtensionAccessorCall(ETSChecker *checker, ir::MemberExpression *expr,
                                                    ArenaVector<ir::Expression *> &&args);
    static void SetPreferredTypeIfPossible(ir::Expression *expr, Type *targetType);
    Signature *FindRelativeExtensionGetter(ir::MemberExpression *const expr, ETSFunctionType *funcType);
    Signature *FindRelativeExtensionSetter(ir::MemberExpression *const expr, ETSFunctionType *funcType);
    Type *GetExtensionAccessorReturnType(ir::MemberExpression *expr);
    // Utility type handler functions
    std::optional<ir::TypeNode *> GetUtilityTypeTypeParamNode(const ir::TSTypeParameterInstantiation *typeParams,
                                                              const std::string_view &utilityTypeName);
    Type *HandleUtilityTypeParameterNode(const ir::TSTypeParameterInstantiation *typeParams,
                                         const ir::Identifier *const ident);
    // Partial
    Type *CreatePartialType(Type *typeToBePartial);
    Type *HandlePartialInterface(ir::TSInterfaceDeclaration *interfaceDecl, ETSObjectType *typeToBePartial);

    ir::ClassProperty *CreateNullishProperty(ir::ClassProperty *prop, ir::ClassDefinition *newClassDefinition);
    ir::ClassProperty *CreateNullishProperty(ir::ClassProperty *const prop,
                                             ir::TSInterfaceDeclaration *const newTSInterfaceDefinition);
    ir::MethodDefinition *CreateNullishAccessor(ir::MethodDefinition *const accessor,
                                                ir::TSInterfaceDeclaration *interface);
    ir::ClassProperty *CreateNullishPropertyFromAccessor(ir::MethodDefinition *const accessor,
                                                         ir::ClassDefinition *newClassDefinition);
    void CreatePartialClassDeclaration(ir::ClassDefinition *newClassDefinition, ir::ClassDefinition *classDef);
    ir::ETSTypeReference *BuildSuperPartialTypeReference(Type *superPartialType,
                                                         ir::TSTypeParameterInstantiation *superPartialRefTypeParams);
    ir::TSInterfaceDeclaration *CreateInterfaceProto(util::StringView name, parser::Program *const interfaceDeclProgram,
                                                     const bool isStatic, const ir::ModifierFlags flags);
    ir::TSTypeParameterInstantiation *CreateNewSuperPartialRefTypeParamsDecl(
        ArenaMap<ir::TSTypeParameter *, ir::TSTypeParameter *> *likeSubstitution, const Type *const superPartialType,
        ir::Expression *superRef);
    ir::TSTypeParameterDeclaration *ProcessTypeParamAndGenSubstitution(
        ir::TSTypeParameterDeclaration const *const thisTypeParams,
        ArenaMap<ir::TSTypeParameter *, ir::TSTypeParameter *> *likeSubstitution,
        ir::TSTypeParameterDeclaration *newTypeParams);
    Type *CreatePartialTypeInterfaceDecl(ir::TSInterfaceDeclaration *const interfaceDecl,
                                         ETSObjectType *const typeToBePartial,
                                         ir::TSInterfaceDeclaration *partialInterface);
    void CreatePartialTypeInterfaceMethods(ir::TSInterfaceDeclaration *const interfaceDecl,
                                           ir::TSInterfaceDeclaration *partialInterface);
    ir::ClassDefinition *CreateClassPrototype(util::StringView name, parser::Program *classDeclProgram);
    varbinder::Variable *SearchNamesInMultiplePrograms(const std::set<const parser::Program *> &programs,
                                                       const std::set<util::StringView> &classNamesToFind);
    std::pair<ir::ScriptFunction *, ir::Identifier *> CreateScriptFunctionForConstructor(
        varbinder::FunctionScope *scope);
    ir::MethodDefinition *CreateNonStaticClassInitializer(varbinder::ClassScope *classScope,
                                                          varbinder::RecordTable *recordTable);
    // Readonly
    Type *GetReadonlyType(Type *type);
    void MakePropertiesReadonly(ETSObjectType *classType);
    // Required
    Type *HandleRequiredType(Type *typeToBeRequired);
    void MakePropertiesNonNullish(ETSObjectType *classType);
    template <PropertyType PROP_TYPE>
    void MakePropertyNonNullish(ETSObjectType *classType, varbinder::LocalVariable *prop);
    void ValidateObjectLiteralForRequiredType(const ETSObjectType *requiredType,
                                              const ir::ObjectExpression *initObjExpr);

    using NamedAccessMeta = std::tuple<ETSObjectType const *, checker::Type const *, const util::StringView>;
    static NamedAccessMeta FormNamedAccessMetadata(varbinder::Variable const *prop);

    // Smart cast support
    [[nodiscard]] checker::Type *ResolveSmartType(checker::Type *sourceType, checker::Type *targetType);
    [[nodiscard]] std::pair<Type *, Type *> CheckTestNullishCondition(Type *testedType, Type *actualType, bool strict);
    [[nodiscard]] std::pair<Type *, Type *> CheckTestObjectCondition(ETSObjectType *testedType, Type *actualType,
                                                                     bool strict);
    [[nodiscard]] std::pair<Type *, Type *> CheckTestObjectCondition(ETSArrayType *testedType, Type *actualType);

    void ApplySmartCast(varbinder::Variable const *variable, checker::Type *smartType) noexcept;

    bool IsInLocalClass(const ir::AstNode *node) const;
    // Exception
    ETSObjectType *CheckExceptionOrErrorType(checker::Type *type, lexer::SourcePosition pos);

    static Type *TryToInstantiate(Type *type, ArenaAllocator *allocator, TypeRelation *relation,
                                  GlobalTypesHolder *globalTypes);

    // Dynamic interop
    template <typename T>
    Signature *ResolveDynamicCallExpression(ir::Expression *callee, const ArenaVector<T *> &arguments, Language lang,
                                            bool isConstruct);
    ir::ClassProperty *CreateStaticReadonlyField(const char *name);
    void BuildClassBodyFromDynamicImports(const ArenaVector<ir::ETSImportDeclaration *> &dynamicImports,
                                          ArenaVector<ir::AstNode *> *classBody);
    void BuildDynamicImportClass();
    void BuildLambdaObjectClass(ETSObjectType *functionalInterface, ir::TypeNode *retTypeAnnotation);
    // Trailing lambda
    void EnsureValidCurlyBrace(ir::CallExpression *callExpr);

    // Extension function
    void HandleUpdatedCallExpressionNode(ir::CallExpression *callExpr);
    Signature *FindExtensionSetterInMap(util::StringView name, ETSObjectType *type);
    Signature *FindExtensionGetterInMap(util::StringView name, ETSObjectType *type);
    void InsertExtensionSetterToMap(util::StringView name, ETSObjectType *type, Signature *sig);
    void InsertExtensionGetterToMap(util::StringView name, ETSObjectType *type, Signature *sig);

    // Static invoke
    void CheckInvokeMethodsLegitimacy(ETSObjectType *classType);
    bool IsClassStaticMethod(checker::ETSObjectType *objType, checker::Signature *signature);

    // Covariant and contravariant
    void CheckTypeParameterVariance(ir::ClassDefinition *classDef);

    checker::Type *CheckArrayElements(ir::ArrayExpression *init);
    void ResolveReturnStatement(checker::Type *funcReturnType, checker::Type *argumentType,
                                ir::ScriptFunction *containingFunc, ir::ReturnStatement *st);

    auto *DynamicCallNames(bool isConstruct)
    {
        return &dynamicCallNames_[static_cast<uint32_t>(isConstruct)];
    }

    const auto *DynamicCallNames(bool isConstruct) const
    {
        return &dynamicCallNames_[static_cast<uint32_t>(isConstruct)];
    }

    std::recursive_mutex *Mutex()
    {
        return &mtx_;
    }

    template <typename T, typename... Args>
    T *AllocNode(Args &&...args)
    {
        // SUPPRESS_CSA_NEXTLINE(alpha.core.AllocatorETSCheckerHint)
        return util::NodeAllocator::ForceSetParent<T>(Allocator(), std::forward<Args>(args)...);
    }

    ArenaVector<ConstraintCheckRecord> &PendingConstraintCheckRecords();
    size_t &ConstraintCheckScopesCount();

    ETSObjectType *GetCachedFunctionalInterface(ir::ETSFunctionType *type);
    void CacheFunctionalInterface(ir::ETSFunctionType *type, ETSObjectType *ifaceType);
    void CollectReturnStatements(ir::AstNode *parent);
    ir::ETSParameterExpression *AddParam(util::StringView name, ir::TypeNode *type);

    evaluate::ScopedDebugInfoPlugin *GetDebugInfoPlugin();
    const evaluate::ScopedDebugInfoPlugin *GetDebugInfoPlugin() const;

    void SetDebugInfoPlugin(evaluate::ScopedDebugInfoPlugin *debugInfo);

    using ClassBuilder = std::function<void(ArenaVector<ir::AstNode *> *)>;
    using ClassInitializerBuilder =
        std::function<void(ArenaVector<ir::Statement *> *, ArenaVector<ir::Expression *> *)>;
    using MethodBuilder = std::function<void(ArenaVector<ir::Statement *> *, ArenaVector<ir::Expression *> *, Type **)>;

    ir::ClassStaticBlock *CreateClassStaticInitializer(const ClassInitializerBuilder &builder,
                                                       ETSObjectType *type = nullptr);
    ir::MethodDefinition *CreateClassInstanceInitializer(const ClassInitializerBuilder &builder,
                                                         ETSObjectType *type = nullptr);
    ir::MethodDefinition *CreateClassMethod(std::string_view name, ir::ScriptFunctionFlags funcFlags,
                                            ir::ModifierFlags modifierFlags, const MethodBuilder &builder);
    ir::ClassDeclaration *BuildClass(util::StringView name, const ClassBuilder &builder);

    void LogUnresolvedReferenceError(ir::Identifier *ident);
    void WrongContextErrorClassifyByType(ir::Identifier *ident);

    void CreateOverloadSigContainer(Signature *overloadHelperSig)
    {
        if (!overloadSigContainer_.empty()) {
            overloadSigContainer_.pop_back();
        }
        ES2PANDA_ASSERT(overloadSigContainer_.empty());
        overloadSigContainer_.insert(overloadSigContainer_.end(), overloadHelperSig);
    }

    ArenaVector<Signature *> &GetOverloadSigContainer()
    {
        ES2PANDA_ASSERT(overloadSigContainer_.size() == 1);
        return overloadSigContainer_;
    }

    void CleanUp() override
    {
        Checker::CleanUp();
        arrayTypes_.clear();
        pendingConstraintCheckRecords_.clear();
        constraintCheckScopesCount_ = 0;
        globalArraySignatures_.clear();
        cachedComputedAbstracts_.clear();
        for (auto &dynamicCallIntrinsicsMap : dynamicIntrinsics_) {
            dynamicCallIntrinsicsMap.clear();
        }

        for (auto &dynamicClassIntrinsicsMap : dynamicClasses_) {
            dynamicClassIntrinsicsMap.clear();
        }
        dynamicLambdaSignatureCache_.clear();
        functionalInterfaceCache_.clear();
        apparentTypes_.clear();
        for (auto &dynamicCallNamesMap : dynamicCallNames_) {
            dynamicCallNamesMap.clear();
        }
        elementStack_.clear();
        overloadSigContainer_.clear();
    }

    // This helper finds the intersection of two callSignatures sets
    // The result is stored in callSignatures of newly created ETSFunctionType
    checker::ETSFunctionType *IntersectSignatureSets(const checker::ETSFunctionType *left,
                                                     const checker::ETSFunctionType *right);

private:
    std::pair<const ir::Identifier *, ir::TypeNode *> GetTargetIdentifierAndType(ir::Identifier *ident);
    void NotResolvedError(ir::Identifier *const ident, const varbinder::Variable *classVar,
                          const ETSObjectType *classType);
    void ValidateCallExpressionIdentifier(ir::Identifier *const ident, Type *const type);
    void ValidateNewClassInstanceIdentifier(ir::Identifier *const ident);
    void ValidateMemberIdentifier(ir::Identifier *const ident);
    void ValidateAssignmentIdentifier(ir::Identifier *const ident, Type *const type);
    bool ValidateBinaryExpressionIdentifier(ir::Identifier *const ident, Type *const type);
    ETSFunctionType *ResolveAccessorTypeByFlag(ir::MemberExpression *const memberExpr, ETSFunctionType *propType,
                                               ETSFunctionType *funcType, PropertySearchFlags searchFlag);
    std::vector<ResolveResult *> ValidateAccessor(ir::MemberExpression *const memberExpr,
                                                  varbinder::LocalVariable *const oAcc, varbinder::Variable *const eAcc,
                                                  PropertySearchFlags searchFlag);
    ir::ClassProperty *FindClassProperty(const ETSObjectType *objectType, const ETSFunctionType *propType);
    bool IsInitializedProperty(const ir::ClassDefinition *classDefinition, const ir::ClassProperty *prop);
    bool FindPropertyInAssignment(const ir::AstNode *it, const std::string &targetName);
    void ValidateReadonlyProperty(const ir::MemberExpression *memberExpr, const ETSFunctionType *propType,
                                  lexer::SourcePosition sourcePos);
    std::tuple<bool, bool> IsResolvedAndValue(const ir::Expression *expr, Type *type) const;
    PropertySearchFlags GetSearchFlags(const ir::MemberExpression *memberExpr, const varbinder::Variable *targetRef);
    PropertySearchFlags GetInitialSearchFlags(const ir::MemberExpression *memberExpr);
    const varbinder::Variable *GetTargetRef(const ir::MemberExpression *memberExpr);
    Type *GetTypeOfSetterGetter([[maybe_unused]] varbinder::Variable *var);
    void IterateInVariableContext([[maybe_unused]] varbinder::Variable *const var);
    bool CheckInit(ir::Identifier *ident, ir::TypeNode *typeAnnotation, ir::Expression *init,
                   checker::Type *annotationType, varbinder::Variable *const bindingVar);
    void CheckItemCasesConstant(ArenaVector<ir::SwitchCaseStatement *> const &cases);
    void CheckItemCasesDuplicate(ArenaVector<ir::SwitchCaseStatement *> const &cases);

    template <typename EnumType>
    EnumType *CreateEnumTypeFromEnumDeclaration(ir::TSEnumDeclaration const *const enumDecl);

    std::pair<ir::ScriptFunction *, ir::Identifier *> CreateStaticScriptFunction(
        ClassInitializerBuilder const &builder);
    std::pair<ir::ScriptFunction *, ir::Identifier *> CreateScriptFunction(ClassInitializerBuilder const &builder);

    template <typename T>
    ir::MethodDefinition *CreateDynamicCallIntrinsic(ir::Expression *callee, const ArenaVector<T *> &arguments,
                                                     Language lang);
    ir::ClassStaticBlock *CreateDynamicCallClassInitializer(Language lang, bool isConstruct);
    ir::ClassStaticBlock *CreateDynamicModuleClassInitializer(const std::vector<ir::ETSImportDeclaration *> &imports);
    ir::MethodDefinition *CreateDynamicModuleClassInitMethod();

    ir::MethodDefinition *CreateLambdaObjectClassInitializer(ETSObjectType *functionalInterface);

    ir::MethodDefinition *CreateLambdaObjectClassInvokeMethod(Signature *invokeSignature,
                                                              ir::TypeNode *retTypeAnnotation);

    void ClassInitializerFromImport(ir::ETSImportDeclaration *import, ArenaVector<ir::Statement *> *statements);
    void EmitDynamicModuleClassInitCall();
    DynamicCallIntrinsicsMap *DynamicCallIntrinsics(bool isConstruct)
    {
        return &dynamicIntrinsics_[static_cast<size_t>(isConstruct)];
    }

    ir::ClassDeclaration *GetDynamicClass(Language lang, bool isConstruct);

    using Type2TypeMap = std::unordered_map<varbinder::Variable *, varbinder::Variable *>;
    using TypeSet = std::unordered_set<varbinder::Variable *>;
    bool CheckTypeParameterConstraint(ir::TSTypeParameter *param, Type2TypeMap &extends);
    bool CheckDefaultTypeParameter(const ir::TSTypeParameter *param, TypeSet &typeParameterDecls);

    void SetUpTypeParameterConstraint(ir::TSTypeParameter *param);
    ETSObjectType *UpdateGlobalType(ETSObjectType *objType, util::StringView name);
    void CheckProgram(parser::Program *program, bool runAnalysis = false);
    void CheckWarnings(parser::Program *program, const util::Options &options);

    Type *ResolveUnionUncheckedType(ArenaVector<checker::Type *> &&apparentTypes);

    bool ComputeSuperType(ETSObjectType *type);

    template <typename UType>
    UType HandleModulo(UType leftValue, UType rightValue);

    template <typename FloatOrIntegerType, typename IntegerType = FloatOrIntegerType>
    Type *HandleBitWiseArithmetic(Type *leftValue, Type *rightValue, lexer::TokenType operationType);

    template <typename TargetType>
    typename TargetType::UType GetOperand(Type *type);

    template <typename... Args>
    ETSObjectType *AsETSObjectType(Type *(GlobalTypesHolder::*typeFunctor)(Args...), Args... args) const;
    Signature *GetMostSpecificSignature(ArenaVector<Signature *> &compatibleSignatures,
                                        const ArenaVector<ir::Expression *> &arguments,
                                        const lexer::SourcePosition &pos, TypeRelationFlag resolveFlags);
    ArenaVector<Signature *> CollectSignatures(ArenaVector<Signature *> &signatures,
                                               const ir::TSTypeParameterInstantiation *typeArguments,
                                               const ArenaVector<ir::Expression *> &arguments,
                                               const lexer::SourcePosition &pos, TypeRelationFlag resolveFlags);
    // Trailing lambda
    void MoveTrailingBlockToEnclosingBlockStatement(ir::CallExpression *callExpr);
    void TransformTraillingLambda(ir::CallExpression *callExpr, Signature *sig);
    ArenaVector<ir::Expression *> ExtendArgumentsWithFakeLamda(ir::CallExpression *callExpr);

    // Static invoke
    bool TryTransformingToStaticInvoke(ir::Identifier *ident, const Type *resolvedType);

    // Partial
    Type *HandleUnionForPartialType(ETSUnionType *typeToBePartial);
    Type *CreatePartialTypeParameter(ETSTypeParameter *typeToBePartial);
    Type *CreatePartialTypeClass(ETSObjectType *typeToBePartial, ir::AstNode *typeDeclNode);
    Type *CreatePartialTypeClassDef(ir::ClassDefinition *partialClassDef, ir::ClassDefinition *classDef,
                                    ETSObjectType *typeToBePartial, varbinder::RecordTable *recordTableToUse);
    void CreateConstructorForPartialType(ir::ClassDefinition *partialClassDef, checker::ETSObjectType *partialType,
                                         varbinder::RecordTable *recordTable);

    // Check type alias for recursive cases
    bool IsAllowedTypeAliasRecursion(const ir::TSTypeAliasDeclaration *typeAliasNode,
                                     std::unordered_set<const ir::TSTypeAliasDeclaration *> &typeAliases);

    ArrayMap arrayTypes_;
    ArenaVector<ConstraintCheckRecord> pendingConstraintCheckRecords_;
    size_t constraintCheckScopesCount_ {0};
    GlobalArraySignatureMap globalArraySignatures_;
    ComputedAbstracts cachedComputedAbstracts_;
    // NOTE(aleksisch): Extract dynamic from checker to separate class
    std::array<DynamicCallIntrinsicsMap, 2U> dynamicIntrinsics_;
    std::array<DynamicClassIntrinsicsMap, 2U> dynamicClasses_;
    DynamicLambdaObjectSignatureMap dynamicLambdaSignatureCache_;
    FunctionalInterfaceMap functionalInterfaceCache_;
    TypeMapping apparentTypes_;
    std::array<DynamicCallNamesMap, 2U> dynamicCallNames_;
    std::recursive_mutex mtx_;
    evaluate::ScopedDebugInfoPlugin *debugInfoPlugin_ {nullptr};
    std::unordered_set<ir::TSTypeAliasDeclaration *> elementStack_;
    ArenaVector<Signature *> overloadSigContainer_;
};

}  // namespace ark::es2panda::checker

#endif /* CHECKER_H */
