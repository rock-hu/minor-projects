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

#include "ETSAnalyzerHelpers.h"
#include "checker/types/ets/etsAsyncFuncReturnType.h"

namespace ark::es2panda::checker {

static bool IsValidReceiverParameter(Type *const thisType)
{
    if (thisType == nullptr) {
        return false;
    }

    if (thisType->IsETSArrayType() || thisType->IsETSTypeParameter()) {
        return true;
    }

    if (!thisType->IsETSObjectType() || thisType->IsETSEnumType()) {
        return false;
    }

    auto *const thisObjectType = thisType->AsETSObjectType();
    return thisObjectType->HasObjectFlag(ETSObjectFlags::CLASS | ETSObjectFlags::INTERFACE);
}

void CheckExtensionIsShadowedInCurrentClassOrInterface(checker::ETSChecker *checker, checker::ETSObjectType *objType,
                                                       ir::ScriptFunction *extensionFunc, checker::Signature *signature)
{
    const auto methodName = extensionFunc->Id()->Name();

    // check if there are class and interfaces' instance fields with the same name as extensions.
    auto *const fieldVariable = objType->GetOwnProperty<checker::PropertyType::INSTANCE_FIELD>(methodName);
    if (fieldVariable != nullptr) {
        checker->LogError(diagnostic::EXTENSION_NAME_CONFLICT_WITH_FIELD, {methodName, objType->Name()},
                          extensionFunc->Body()->Start());
        return;
    }

    // check if there are class and interfaces' instance methods with the same name as extensions.
    auto *const methodVariable = objType->GetOwnProperty<checker::PropertyType::INSTANCE_METHOD>(methodName);
    if (methodVariable == nullptr) {
        return;
    }

    const auto *const funcType = methodVariable->TsType()->AsETSFunctionType();
    for (auto *funcSignature : funcType->CallSignatures()) {
        signature->SetReturnType(funcSignature->ReturnType());
        if (!checker->Relation()->SignatureIsSupertypeOf(signature, funcSignature) &&
            !checker->HasSameAssemblySignature(signature, funcSignature)) {
            continue;
        }

        if (signature->Function()->IsGetter() || signature->Function()->IsSetter()) {
            checker->LogError(diagnostic::EXTENSION_NAME_CONFLICT_WITH_METHOD, {funcType->Name(), objType->Name()},
                              extensionFunc->Body()->Start());
        } else if (funcSignature->HasSignatureFlag(SignatureFlags::PUBLIC)) {
            checker->LogError(diagnostic::EXTENSION_FUNC_NAME_CONFLICT_WITH_METH, {funcType->Name(), objType->Name()},
                              extensionFunc->Body()->Start());
        } else {
            checker->ReportWarning({"The extension function '", funcType->Name(),
                                    "' has the same name with non-public method in class ", objType->Name()},
                                   extensionFunc->Body()->Start());
        }
        return;
    }
}

void CheckExtensionIsShadowedByMethod(checker::ETSChecker *checker, checker::ETSObjectType *objType,
                                      ir::ScriptFunction *extensionFunc, checker::Signature *signature)
{
    if (objType == nullptr) {
        return;
    }

    CheckExtensionIsShadowedInCurrentClassOrInterface(checker, objType, extensionFunc, signature);

    for (auto *interface : objType->Interfaces()) {
        CheckExtensionIsShadowedByMethod(checker, interface, extensionFunc, signature);
    }

    CheckExtensionIsShadowedByMethod(checker, objType->SuperType(), extensionFunc, signature);
}

static void ReplaceThisInExtensionMethod(checker::ETSChecker *checker, ir::ScriptFunction *extensionFunc)
{
    //  Skip processing of possibly invalid extension method
    if (extensionFunc->Params().empty() ||
        !extensionFunc->Params()[0]->AsETSParameterExpression()->Ident()->IsReceiver()) {
        ES2PANDA_ASSERT(checker->IsAnyError());
        return;
    }

    if (extensionFunc->ReturnTypeAnnotation() != nullptr && extensionFunc->ReturnTypeAnnotation()->IsTSThisType()) {
        // when return `this` in extensionFunction, the type of `this` actually should be type of the receiver,
        // so some substitution should be done, temporary solution(xingshunxiang).
        auto *const thisType = extensionFunc->Signature()->Params()[0]->TsType();
        auto *const thisTypeAnnotation =
            extensionFunc->Params()[0]->AsETSParameterExpression()->Ident()->TypeAnnotation();
        extensionFunc->Signature()->SetReturnType(thisType);
        extensionFunc->SetReturnTypeAnnotation(thisTypeAnnotation->Clone(checker->Allocator(), extensionFunc));
    }

    auto thisVariable = extensionFunc->Params()[0]->Variable();
    extensionFunc->Body()->TransformChildrenRecursively(
        [=](ir::AstNode *ast) {
            if (ast->IsThisExpression()) {
                auto *thisParam = checker->Allocator()->New<ir::Identifier>(
                    varbinder::TypedBinder::MANDATORY_PARAM_THIS, checker->Allocator());
                thisParam->SetRange(ast->Range());
                thisParam->SetParent(ast->Parent());
                thisParam->SetVariable(thisVariable);
                return static_cast<ir::AstNode *>(thisParam);
            }
            return ast;
        },
        "replace-this-in-extension-method");
}

void CheckExtensionMethod(checker::ETSChecker *checker, ir::ScriptFunction *extensionFunc, ir::AstNode *node)
{
    auto *const thisType =
        !extensionFunc->Signature()->Params().empty() ? extensionFunc->Signature()->Params()[0]->TsType() : nullptr;

    // "Extension Functions" are only allowed for classes, interfaces, arrays and type parameters extends from object.
    if (IsValidReceiverParameter(thisType)) {
        // Skip for arrays (array does not contain a class definition) and checked class definition.
        if (!thisType->IsETSArrayType() && !thisType->IsETSTypeParameter() &&
            thisType->Variable()->Declaration()->Node()->IsClassDefinition() &&
            !thisType->Variable()->Declaration()->Node()->AsClassDefinition()->IsClassDefinitionChecked()) {
            thisType->Variable()->Declaration()->Node()->Check(checker);
        }
        // NOTE(gogabr): should be done in a lowering
        ReplaceThisInExtensionMethod(checker, extensionFunc);
        if (extensionFunc->IsArrow()) {
            return;
        }

        checker::SignatureInfo *originalExtensionSigInfo = checker->Allocator()->New<checker::SignatureInfo>(
            extensionFunc->Signature()->GetSignatureInfo(), checker->Allocator());
        originalExtensionSigInfo->minArgCount -= 1U;
        originalExtensionSigInfo->params.erase(originalExtensionSigInfo->params.begin());
        checker::Signature *originalExtensionSignature =
            checker->CreateSignature(originalExtensionSigInfo, extensionFunc->Signature()->ReturnType(), extensionFunc);

        // The shadowing check is only relevant for classes and interfaces,
        // since for arrays there are no other ways to declare a method other than "Extension Functions".
        if (thisType->IsETSObjectType()) {
            CheckExtensionIsShadowedByMethod(checker, thisType->AsETSObjectType(), extensionFunc,
                                             originalExtensionSignature);
        }

        // etsArrayType may extends method from GlobalETSObjectType
        if (thisType->IsETSArrayType()) {
            CheckExtensionIsShadowedByMethod(checker, checker->GlobalETSObjectType(), extensionFunc,
                                             originalExtensionSignature);
        }
    } else {
        checker->LogError(diagnostic::EXTENSION_FUNC_ON_INEXETENSIBLE, {}, node->Start());
    }
}

static void CheckMethodBodyForNativeAbstractDeclare(ETSChecker *checker, ir::MethodDefinition *node,
                                                    ir::ScriptFunction *scriptFunc)
{
    if ((node->IsNative() && !node->IsConstructor()) || node->IsAbstract() || node->IsDeclare()) {
        checker->LogError(diagnostic::UNEXPECTED_FUNC_BODY, {}, scriptFunc->Body()->Start());
    }
}

static void CheckNativeConstructorBody(ETSChecker *checker, ir::MethodDefinition *node, ir::ScriptFunction *scriptFunc)
{
    if (node->IsNative() && node->IsConstructor()) {
        checker->LogError(diagnostic::NATIVE_WITH_BODY, {}, scriptFunc->Body()->Start());
    }
}

void DoBodyTypeChecking(ETSChecker *checker, ir::MethodDefinition *node, ir::ScriptFunction *scriptFunc)
{
    if (scriptFunc->HasBody()) {
        CheckMethodBodyForNativeAbstractDeclare(checker, node, scriptFunc);
        CheckNativeConstructorBody(checker, node, scriptFunc);
    }

    if (!scriptFunc->HasBody() || (scriptFunc->IsExternal() && !scriptFunc->IsExternalOverload())) {
        return;
    }

    checker::ScopeContext scopeCtx(checker, scriptFunc->Scope());
    checker::SavedCheckerContext savedContext(checker, checker->Context().Status(),
                                              checker->Context().ContainingClass());
    checker->Context().SetContainingSignature(checker->GetSignatureFromMethodDefinition(node));

    if (node->IsStatic() && !node->IsConstructor() &&
        !checker->Context().ContainingClass()->HasObjectFlag(checker::ETSObjectFlags::GLOBAL)) {
        checker->AddStatus(checker::CheckerStatus::IN_STATIC_CONTEXT);
    }

    if (node->IsConstructor()) {
        checker->AddStatus(checker::CheckerStatus::IN_CONSTRUCTOR);
    }

    if (node->IsExtensionMethod()) {
        CheckExtensionMethod(checker, scriptFunc, node);
    }

    scriptFunc->Body()->Check(checker);

    if (scriptFunc->ReturnTypeAnnotation() == nullptr) {
        if (scriptFunc->IsAsyncFunc()) {
            auto returnType = checker->CreateETSAsyncFuncReturnTypeFromBaseType(scriptFunc->Signature()->ReturnType());
            scriptFunc->Signature()->SetReturnType(returnType->PromiseType());
            for (auto &returnStatement : scriptFunc->ReturnStatements()) {
                returnStatement->SetReturnType(checker, returnType);
            }
        } else {
            if (scriptFunc->IsAsyncImplFunc()) {
                ComposeAsyncImplFuncReturnType(checker, scriptFunc);
            }

            for (auto &returnStatement : scriptFunc->ReturnStatements()) {
                returnStatement->SetReturnType(checker, scriptFunc->Signature()->ReturnType());
            }
        }
    }

    checker->Context().SetContainingSignature(nullptr);
}

void ComposeAsyncImplFuncReturnType(ETSChecker *checker, ir::ScriptFunction *scriptFunc)
{
    auto const promiseType = checker->CreatePromiseOf(checker->MaybeBoxType(scriptFunc->Signature()->ReturnType()));

    auto *objectId =
        checker->AllocNode<ir::Identifier>(compiler::Signatures::BUILTIN_OBJECT_CLASS, checker->Allocator());
    checker->VarBinder()->AsETSBinder()->LookupTypeReference(objectId, false);
    auto *returnType = checker->AllocNode<ir::ETSTypeReference>(
        checker->AllocNode<ir::ETSTypeReferencePart>(objectId, nullptr, nullptr, checker->Allocator()),
        checker->Allocator());
    objectId->SetParent(returnType->Part());
    returnType->Part()->SetParent(returnType);
    returnType->SetTsType(
        checker->Allocator()->New<ETSAsyncFuncReturnType>(checker->Allocator(), checker->Relation(), promiseType));
    returnType->Check(checker);
    scriptFunc->Signature()->SetReturnType(returnType->TsType());
}

void CheckPredefinedMethodReturnType(ETSChecker *checker, ir::ScriptFunction *scriptFunc)
{
    if (scriptFunc->Signature() == nullptr) {
        ES2PANDA_ASSERT(checker->IsAnyError());
        return;
    }

    auto const &position = scriptFunc->Start();

    if (scriptFunc->IsGetter() && (scriptFunc->Signature()->ReturnType() == checker->GlobalVoidType())) {
        checker->LogError(diagnostic::GETTER_VOID, {}, position);
    }

    auto const name = scriptFunc->Id()->Name();
    auto const methodName = std::string {ir::PREDEFINED_METHOD} + std::string {name.Utf8()};

    if (name.Is(compiler::Signatures::GET_INDEX_METHOD)) {
        if (scriptFunc->Signature()->ReturnType() == checker->GlobalVoidType()) {
            checker->LogError(diagnostic::UNEXPECTED_VOID, {util::StringView(methodName)}, position);
        }
    } else if (name.Is(compiler::Signatures::SET_INDEX_METHOD)) {
        if (scriptFunc->Signature()->ReturnType() != checker->GlobalVoidType()) {
            checker->LogError(diagnostic::UNEXPECTED_NONVOID, {util::StringView(methodName)}, position);
        }
    } else if (name.Is(compiler::Signatures::ITERATOR_METHOD)) {
        CheckIteratorMethodReturnType(checker, scriptFunc, position, methodName);
    }
}

static bool HasIteratorInterface(ETSObjectType const *const objectType)
{
    auto const hasIteratorInterfaceImpl = [](ETSObjectType const *const checkType,
                                             auto &&iteratorInterfaceImpl) -> bool {
        if (checkType->Name().Is(ir::ITERATOR_INTERFACE_NAME)) {
            return true;
        }
        for (const auto *const interface : checkType->Interfaces()) {
            if (iteratorInterfaceImpl(interface, iteratorInterfaceImpl)) {
                return true;
            }
        }
        return false;
    };

    return hasIteratorInterfaceImpl(objectType, hasIteratorInterfaceImpl);
}

void CheckIteratorMethodReturnType(ETSChecker *checker, ir::ScriptFunction *scriptFunc,
                                   const lexer::SourcePosition &position, const std::string &methodName)
{
    const auto *returnType = scriptFunc->Signature()->ReturnType();

    if (returnType == nullptr) {
        checker->LogError(diagnostic::MISSING_RETURN_TYPE_2, {util::StringView(methodName)}, position);
        return;
    }

    if (returnType->IsETSTypeParameter()) {
        returnType = checker->GetApparentType(returnType->AsETSTypeParameter()->GetConstraintType());
    }

    if (returnType->IsETSObjectType() && HasIteratorInterface(returnType->AsETSObjectType())) {
        return;
    }

    while (returnType->IsETSObjectType() && returnType->AsETSObjectType()->SuperType() != nullptr) {
        returnType = returnType->AsETSObjectType()->SuperType();
        if (returnType->IsETSObjectType() && HasIteratorInterface(returnType->AsETSObjectType())) {
            return;
        }
    }

    checker->LogError(diagnostic::RETURN_ISNT_ITERATOR, {scriptFunc->Id()->Name()}, position);
}

static void SwitchMethodCallToFunctionCall(checker::ETSChecker *checker, ir::CallExpression *expr, Signature *signature)
{
    ES2PANDA_ASSERT(expr->Callee()->IsMemberExpression());
    auto *memberExpr = expr->Callee()->AsMemberExpression();
    if (expr->Arguments().empty() || expr->Arguments()[0] != memberExpr->Object()) {
        expr->Arguments().insert(expr->Arguments().begin(), memberExpr->Object());
    }
    expr->SetSignature(signature);
    expr->SetCallee(memberExpr->Property());
    memberExpr->Property()->AsIdentifier()->SetParent(expr);
    expr->Arguments()[0]->SetParent(expr);
    checker->HandleUpdatedCallExpressionNode(expr);
    // Set TsType for new Callee(original member expression's Object)
    expr->Callee()->Check(checker);
}

checker::Signature *ResolveCallExtensionFunction(checker::Type *functionType, checker::ETSChecker *checker,
                                                 ir::CallExpression *expr, const TypeRelationFlag reportFlag)
{
    // We have to ways to call ExtensionFunction `function foo(this: A, ...)`:
    // 1. Make ExtensionFunction as FunctionCall: `foo(a,...);`
    // 2. Make ExtensionFunction as MethodCall: `a.foo(...)`, here `a` is the receiver;
    auto &signatures = expr->IsETSConstructorCall()
                           ? functionType->AsETSObjectType()->ConstructSignatures()
                           : functionType->AsETSFunctionType()->CallSignaturesOfMethodOrArrow();
    if (expr->Callee()->IsMemberExpression()) {
        // when handle extension function as MethodCall, we temporarily transfer the expr node from `a.foo(...)` to
        // `foo(a, ...)` and resolve the call. If we find the suitable signature, then switch the expr node to
        // function call.
        auto *memberExpr = expr->Callee()->AsMemberExpression();
        expr->Arguments().insert(expr->Arguments().begin(), memberExpr->Object());
        auto *signature = checker->ResolveCallExpressionAndTrailingLambda(signatures, expr, expr->Start(), reportFlag);
        if (signature == nullptr) {
            expr->Arguments().erase(expr->Arguments().begin());
            return nullptr;
        }
        if (!signature->HasSignatureFlag(SignatureFlags::EXTENSION_FUNCTION)) {
            checker->LogError(diagnostic::PROPERTY_NONEXISTENT,
                              {memberExpr->Property()->AsIdentifier()->Name(), memberExpr->ObjType()->Name()},
                              memberExpr->Property()->Start());
            return nullptr;
        }

        SwitchMethodCallToFunctionCall(checker, expr, signature);
        return signature;
    }

    return checker->ResolveCallExpressionAndTrailingLambda(signatures, expr, expr->Start());
}

checker::Signature *ResolveCallForClassMethod(checker::ETSExtensionFuncHelperType *type, checker::ETSChecker *checker,
                                              ir::CallExpression *expr, const TypeRelationFlag reportFlag)
{
    ES2PANDA_ASSERT(expr->Callee()->IsMemberExpression());

    auto signature = checker->ResolveCallExpressionAndTrailingLambda(type->ClassMethodType()->CallSignatures(), expr,
                                                                     expr->Start(), reportFlag);
    if (signature != nullptr) {
        auto *memberExpr = expr->Callee()->AsMemberExpression();
        auto *var = type->ClassMethodType()->Variable();
        memberExpr->Property()->AsIdentifier()->SetVariable(var);
    }
    return signature;
}

checker::Signature *GetMostSpecificSigFromExtensionFuncAndClassMethod(checker::ETSExtensionFuncHelperType *type,
                                                                      checker::ETSChecker *checker,
                                                                      ir::CallExpression *expr)
{
    // We try to find the most suitable signature for a.foo(...) both in ExtensionFunctionType and ClassMethodType.
    // So we temporarily transfer expr node from `a.foo(...)` to `a.foo(a, ...)`.
    // For allCallSignatures in ClassMethodType, temporarily insert the dummyReceiver into their signatureInfo,
    // otherwise we can't get the most suitable classMethod signature if all the extensionFunction signature mismatched.
    ArenaVector<Signature *> signatures(checker->Allocator()->Adapter());
    signatures.insert(signatures.end(), type->ClassMethodType()->CallSignatures().begin(),
                      type->ClassMethodType()->CallSignatures().end());
    signatures.insert(signatures.end(), type->ExtensionMethodType()->CallSignatures().begin(),
                      type->ExtensionMethodType()->CallSignatures().end());

    auto *memberExpr = expr->Callee()->AsMemberExpression();
    auto *dummyReceiver = memberExpr->Object();
    auto *dummyReceiverVar = type->ExtensionMethodType()->CallSignatures()[0]->Params()[0];
    expr->Arguments().insert(expr->Arguments().begin(), dummyReceiver);
    const bool typeParamsNeeded = dummyReceiverVar->TsType()->IsETSObjectType();

    for (auto *methodCallSig : type->ClassMethodType()->CallSignatures()) {
        methodCallSig->GetSignatureInfo()->minArgCount++;
        auto &paramsVar = methodCallSig->Params();
        paramsVar.insert(paramsVar.begin(), dummyReceiverVar);
        auto &params = methodCallSig->Function()->Params();
        params.insert(params.begin(), dummyReceiver);
        if (typeParamsNeeded) {
            auto &typeParams = methodCallSig->TypeParams();
            typeParams.insert(typeParams.end(), dummyReceiverVar->TsType()->AsETSObjectType()->TypeArguments().begin(),
                              dummyReceiverVar->TsType()->AsETSObjectType()->TypeArguments().end());
        }
    }

    auto *signature = checker->ResolveCallExpressionAndTrailingLambda(signatures, expr, expr->Start(),
                                                                      checker::TypeRelationFlag::NO_THROW);

    for (auto *methodCallSig : type->ClassMethodType()->CallSignatures()) {
        methodCallSig->GetSignatureInfo()->minArgCount--;
        auto &paramsVar = methodCallSig->Params();
        paramsVar.erase(paramsVar.begin());
        auto &params = methodCallSig->Function()->Params();
        params.erase(params.begin());
        if (typeParamsNeeded) {
            auto &typeParams = methodCallSig->TypeParams();
            typeParams.resize(typeParams.size() -
                              dummyReceiverVar->TsType()->AsETSObjectType()->TypeArguments().size());
        }
    }
    expr->Arguments().erase(expr->Arguments().begin());

    if (signature != nullptr) {
        if (signature->Owner()->Name() == compiler::Signatures::ETS_GLOBAL) {
            SwitchMethodCallToFunctionCall(checker, expr, signature);
        } else {
            auto *var = type->ClassMethodType()->Variable();
            memberExpr->Property()->AsIdentifier()->SetVariable(var);
        }
    }
    return signature;
}

checker::Signature *ResolveCallForETSExtensionFuncHelperType(checker::ETSExtensionFuncHelperType *type,
                                                             checker::ETSChecker *checker, ir::CallExpression *expr)
{
    ES2PANDA_ASSERT(expr->Callee()->IsMemberExpression());
    auto *calleeObj = expr->Callee()->AsMemberExpression()->Object();
    bool isCalleeObjETSGlobal =
        calleeObj->IsIdentifier() && calleeObj->AsIdentifier()->Name() == compiler::Signatures::ETS_GLOBAL;
    // for callExpr `a.foo`, there are 3 situations:
    // 1.`a.foo` is private method call of class A;
    // 2.`a.foo` is extension function of `A`(function with receiver `A`)
    // 3.`a.foo` is public method call of class A;
    Signature *signature = nullptr;
    if (checker->IsTypeIdenticalTo(checker->Context().ContainingClass(), calleeObj->TsType()) || isCalleeObjETSGlobal) {
        // When called `a.foo` in `a.anotherFunc`, we should find signature through private or protected method firstly.
        signature = ResolveCallForClassMethod(type, checker, expr, checker::TypeRelationFlag::NO_THROW);
        if (signature != nullptr) {
            return signature;
        }
    }

    signature = GetMostSpecificSigFromExtensionFuncAndClassMethod(type, checker, expr);
    if (signature == nullptr) {
        checker->ThrowSignatureMismatch(type->ExtensionMethodType()->CallSignatures(), expr->Arguments(), expr->Start(),
                                        "call");
    }

    return signature;
}

ArenaVector<checker::Signature *> GetUnionTypeSignatures(ETSChecker *checker, checker::ETSUnionType *etsUnionType)
{
    ArenaVector<checker::Signature *> callSignatures(checker->Allocator()->Adapter());

    for (auto *constituentType : etsUnionType->ConstituentTypes()) {
        if (constituentType->IsETSFunctionType()) {
            ArenaVector<checker::Signature *> tmpCallSignatures(checker->Allocator()->Adapter());
            tmpCallSignatures = constituentType->AsETSFunctionType()->CallSignatures();
            callSignatures.insert(callSignatures.end(), tmpCallSignatures.begin(), tmpCallSignatures.end());
        }
        if (constituentType->IsETSUnionType()) {
            ArenaVector<checker::Signature *> tmpCallSignatures(checker->Allocator()->Adapter());
            tmpCallSignatures = GetUnionTypeSignatures(checker, constituentType->AsETSUnionType());
            callSignatures.insert(callSignatures.end(), tmpCallSignatures.begin(), tmpCallSignatures.end());
        }
    }

    return callSignatures;
}

void ProcessExclamationMark(ETSChecker *checker, ir::UnaryExpression *expr, checker::Type *operandType)
{
    if (checker->IsNullLikeOrVoidExpression(expr->Argument())) {
        auto tsType = checker->CreateETSBooleanType(true);
        tsType->AddTypeFlag(checker::TypeFlag::CONSTANT);
        expr->SetTsType(tsType);
        return;
    }

    if (operandType == nullptr || !operandType->IsConditionalExprType()) {
        checker->LogError(diagnostic::ASSERT_NOT_LOGICAL, {}, expr->Argument()->Start());
        expr->SetTsType(checker->GlobalTypeError());
        return;
    }

    auto exprRes = operandType->ResolveConditionExpr();
    if (std::get<0>(exprRes)) {
        auto tsType = checker->CreateETSBooleanType(!std::get<1>(exprRes));
        tsType->AddTypeFlag(checker::TypeFlag::CONSTANT);
        expr->SetTsType(tsType);
        return;
    }
    expr->SetTsType(checker->GlobalETSBooleanType());
}

void SetTsTypeForUnaryExpression(ETSChecker *checker, ir::UnaryExpression *expr, checker::Type *operandType)
{
    switch (expr->OperatorType()) {
        case lexer::TokenType::PUNCTUATOR_MINUS:
        case lexer::TokenType::PUNCTUATOR_PLUS: {
            if (operandType == nullptr || !operandType->HasTypeFlag(checker::TypeFlag::ETS_CONVERTIBLE_TO_NUMERIC)) {
                checker->LogError(diagnostic::OPERAND_NOT_NUMERIC, {}, expr->Argument()->Start());
                expr->SetTsType(checker->GlobalTypeError());
                break;
            }

            if (operandType->HasTypeFlag(checker::TypeFlag::CONSTANT) &&
                expr->OperatorType() == lexer::TokenType::PUNCTUATOR_MINUS) {
                expr->SetTsType(checker->NegateNumericType(operandType, expr));
                break;
            }

            expr->SetTsType(operandType);
            break;
        }
        case lexer::TokenType::PUNCTUATOR_TILDE: {
            if (operandType == nullptr || !operandType->HasTypeFlag(checker::TypeFlag::ETS_CONVERTIBLE_TO_NUMERIC)) {
                checker->LogError(diagnostic::OPERAND_NOT_NUMERIC, {}, expr->Argument()->Start());
                expr->SetTsType(checker->GlobalTypeError());
                break;
            }

            expr->Argument()->SetTsType(expr->SetTsType(checker->SelectGlobalIntegerTypeForNumeric(operandType)));
            break;
        }
        case lexer::TokenType::PUNCTUATOR_EXCLAMATION_MARK: {
            ProcessExclamationMark(checker, expr, operandType);
            break;
        }
        default: {
            ES2PANDA_UNREACHABLE();
        }
    }
}

checker::Type *GetIteratorType(ETSChecker *checker, checker::Type *elemType, ir::AstNode *left)
{
    // Just to avoid extra nested level(s)
    // CC-OFFNXT(G.FMT.14-CPP) project code style
    auto const getIterType = [checker, elemType](ir::VariableDeclarator *const declarator) -> checker::Type * {
        if (declarator->TsType() == nullptr) {
            if (auto *resolved = checker->FindVariableInFunctionScope(declarator->Id()->AsIdentifier()->Name());
                resolved != nullptr) {
                resolved->SetTsType(elemType);
                return elemType;
            }
        } else {
            return declarator->TsType();
        }
        return checker->GlobalTypeError();
    };

    checker::Type *iterType = nullptr;
    if (left->IsIdentifier()) {
        auto *const variable = left->Variable();
        ES2PANDA_ASSERT(variable != nullptr && variable->Declaration() != nullptr);

        auto *decl = variable->Declaration();
        if (decl->IsConstDecl() || decl->IsReadonlyDecl()) {
            const auto &errorMsg =
                decl->IsConstDecl() ? diagnostic::INVALID_CONST_ASSIGNMENT : diagnostic::INVALID_READONLY_ASSIGNMENT;
            checker->LogError(errorMsg, {variable->Name()}, decl->Node()->Start());
        }
        iterType = left->AsIdentifier()->TsType();
    } else if (left->IsVariableDeclaration()) {
        if (auto const &declarators = left->AsVariableDeclaration()->Declarators(); !declarators.empty()) {
            iterType = getIterType(declarators.front());
        }
    }

    if (iterType == nullptr) {
        checker->LogError(diagnostic::ITERATOR_TYPE_ABSENT, {}, left->Start());
        return checker->GlobalTypeError();
    }
    return iterType;
}

bool CheckArgumentVoidType(checker::Type *&funcReturnType, ETSChecker *checker, const std::string &name,
                           ir::ReturnStatement *st)
{
    if (name.find(compiler::Signatures::ETS_MAIN_WITH_MANGLE_BEGIN) != std::string::npos) {
        if (!funcReturnType->IsETSVoidType() && !funcReturnType->IsIntType()) {
            checker->LogError(diagnostic::MAIN_BAD_RETURN, {}, st->Start());
        }
    }
    return true;
}

bool CheckReturnType(ETSChecker *checker, checker::Type *funcReturnType, checker::Type *argumentType,
                     ir::Expression *stArgument, ir::ScriptFunction *containingFunc)
{
    if (funcReturnType->IsETSVoidType() || funcReturnType == checker->GlobalVoidType()) {
        if (argumentType != checker->GlobalVoidType()) {
            checker->LogError(diagnostic::UNEXPECTED_VALUE_RETURN, {}, stArgument->Start());
            return false;
        }
        if (!checker::AssignmentContext(checker->Relation(), stArgument, argumentType, funcReturnType,
                                        stArgument->Start(), std::nullopt,
                                        checker::TypeRelationFlag::DIRECT_RETURN | checker::TypeRelationFlag::NO_THROW)
                 // CC-OFFNXT(G.FMT.02) project code style
                 .IsAssignable()) {
            checker->LogError(diagnostic::RETURN_TYPE_MISMATCH, {}, stArgument->Start());
            return false;
        }
        return true;
    }

    if (containingFunc->IsAsyncFunc() && funcReturnType->IsETSObjectType() &&
        funcReturnType->AsETSObjectType()->GetOriginalBaseType() == checker->GlobalBuiltinPromiseType()) {
        auto promiseArg = funcReturnType->AsETSObjectType()->TypeArguments()[0];
        checker::AssignmentContext(checker->Relation(), stArgument, argumentType, promiseArg, stArgument->Start(),
                                   std::nullopt,
                                   checker::TypeRelationFlag::DIRECT_RETURN | checker::TypeRelationFlag::NO_THROW);
        if (checker->Relation()->IsTrue()) {
            return true;
        }
    }

    if (!checker::AssignmentContext(checker->Relation(), stArgument, argumentType, funcReturnType, stArgument->Start(),
                                    std::nullopt,
                                    checker::TypeRelationFlag::DIRECT_RETURN | checker::TypeRelationFlag::NO_THROW)
             // CC-OFFNXT(G.FMT.02) project code style
             .IsAssignable()) {
        checker->LogError(diagnostic::ARROW_TYPE_MISMATCH, {argumentType, funcReturnType}, stArgument->Start());
        return false;
    }
    return true;
}

void InferReturnType(ETSChecker *checker, ir::ScriptFunction *containingFunc, checker::Type *&funcReturnType,
                     ir::Expression *stArgument)
{
    //  First (or single) return statement in the function:
    funcReturnType =
        stArgument == nullptr ? checker->GlobalVoidType() : checker->GetNonConstantType(stArgument->Check(checker));
    if (funcReturnType->IsTypeError()) {
        containingFunc->Signature()->RemoveSignatureFlag(checker::SignatureFlags::NEED_RETURN_TYPE);
        return;
    }

    /*
    when st_argment is ArrowFunctionExpression, need infer type for st_argment
    example code:
    ```
    return () => {}
    ```
    */
    if (stArgument != nullptr && stArgument->IsArrowFunctionExpression()) {
        auto arrowFunc = stArgument->AsArrowFunctionExpression();
        auto typeAnnotation = arrowFunc->CreateTypeAnnotation(checker);

        auto *argumentType = arrowFunc->TsType();
        funcReturnType = typeAnnotation->GetType(checker);
        if (!checker::AssignmentContext(checker->Relation(), arrowFunc, argumentType, funcReturnType,
                                        stArgument->Start(), std::nullopt,
                                        checker::TypeRelationFlag::DIRECT_RETURN | checker::TypeRelationFlag::NO_THROW)
                 // CC-OFFNXT(G.FMT.02) project code style
                 .IsAssignable()) {
            checker->LogError(diagnostic::ARROW_TYPE_MISMATCH, {argumentType, funcReturnType}, stArgument->Start());
            funcReturnType = checker->GlobalTypeError();
            return;
        }
    }

    containingFunc->Signature()->SetReturnType(funcReturnType);
    containingFunc->Signature()->RemoveSignatureFlag(checker::SignatureFlags::NEED_RETURN_TYPE);
    containingFunc->Signature()->AddSignatureFlag(checker::SignatureFlags::INFERRED_RETURN_TYPE);
    checker->VarBinder()->AsETSBinder()->BuildFunctionName(containingFunc);

    if (stArgument != nullptr && stArgument->IsObjectExpression()) {
        stArgument->AsObjectExpression()->SetPreferredType(funcReturnType);
    }
}

bool IsArrayExpressionValidInitializerForType(ETSChecker *checker, const Type *const arrayExprPreferredType)
{
    const auto validForTarget = arrayExprPreferredType == nullptr  // preferred type will be inferred from elements
                                || arrayExprPreferredType->IsETSArrayType()                    // valid for array type
                                || arrayExprPreferredType->IsETSTupleType()                    // valid for tuple type
                                || checker->Relation()->IsSupertypeOf(arrayExprPreferredType,  // valid for 'Object'
                                                                      checker->GlobalETSObjectType());

    return validForTarget;
}

void CastPossibleTupleOnRHS(ETSChecker *checker, ir::AssignmentExpression *expr)
{
    if (expr->Left()->IsMemberExpression() &&
        expr->Left()->AsMemberExpression()->Object()->TsType()->IsETSTupleType() &&
        expr->OperatorType() == lexer::TokenType::PUNCTUATOR_SUBSTITUTION) {
        auto *storedTupleType = expr->Left()->AsMemberExpression()->Object()->TsType();

        const checker::CastingContext tupleCast(
            checker->Relation(), diagnostic::CAST_FAIL_UNREACHABLE, {},
            checker::CastingContext::ConstructorData {expr->Right(), expr->Right()->TsType(), storedTupleType,
                                                      expr->Right()->Start(), TypeRelationFlag::NO_THROW});
    }
}

void ProcessReturnStatements(ETSChecker *checker, ir::ScriptFunction *containingFunc, checker::Type *&funcReturnType,
                             ir::ReturnStatement *st, ir::Expression *stArgument)
{
    funcReturnType = containingFunc->Signature()->ReturnType();

    if (stArgument == nullptr) {
        // previous return statement(s) have value
        if (!funcReturnType->IsETSVoidType() && funcReturnType != checker->GlobalVoidType()) {
            checker->LogError(diagnostic::MIXED_VOID_NONVOID, {}, st->Start());
            return;
        }
    } else {
        if (stArgument->IsObjectExpression()) {
            stArgument->AsObjectExpression()->SetPreferredType(funcReturnType);
        }

        if (stArgument->IsMemberExpression()) {
            checker->SetArrayPreferredTypeForNestedMemberExpressions(stArgument->AsMemberExpression(), funcReturnType);
        }

        checker::Type *argumentType = checker->GetNonConstantType(stArgument->Check(checker));

        //  previous return statement(s) don't have any value
        if (funcReturnType->IsETSVoidType() && !argumentType->IsETSVoidType()) {
            checker->LogError(diagnostic::MIXED_VOID_NONVOID, {}, stArgument->Start());
            return;
        }

        const auto name = containingFunc->Scope()->InternalName().Mutf8();
        if (!CheckArgumentVoidType(funcReturnType, checker, name, st)) {
            return;
        }

        auto *const relation = checker->Relation();
        relation->SetNode(stArgument);

        if (!relation->IsIdenticalTo(funcReturnType, argumentType)) {
            checker->ResolveReturnStatement(funcReturnType, argumentType, containingFunc, st);
        }

        relation->SetNode(nullptr);
        relation->SetFlags(checker::TypeRelationFlag::NONE);
    }
}

bool CheckReturnTypeNecessity(ir::MethodDefinition *node)
{
    bool needReturnType = true;
    auto *scriptFunc = node->Function();
    needReturnType &= (node->IsNative() || node->IsDeclare());
    needReturnType &= !node->IsConstructor();
    needReturnType &= !scriptFunc->IsSetter();
    return needReturnType;
}

}  // namespace ark::es2panda::checker
