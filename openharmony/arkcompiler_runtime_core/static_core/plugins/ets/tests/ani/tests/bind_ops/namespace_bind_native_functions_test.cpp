/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"
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

#include "ani_gtest.h"
#include <iostream>
#include <array>

// NOLINTBEGIN(readability-identifier-naming)
namespace ark::ets::ani::testing {

class NamespaceBindNativeFunctionsTest : public AniTest {};

static ani_int Sum([[maybe_unused]] ani_env *env, ani_int a, ani_int b)
{
    return a + b;
}

static ani_int SumA([[maybe_unused]] ani_env *env, ani_int a, ani_int b, ani_int c)
{
    return a + b + c;
}

static ani_string Concat(ani_env *env, ani_string s0, ani_string s1)
{
    std::string str0;
    AniTest::GetStdString(env, s0, str0);
    std::string str1;
    AniTest::GetStdString(env, s1, str1);

    std::string newStr = str0 + str1;
    ani_string result;
    ani_status status = env->String_NewUTF8(newStr.c_str(), newStr.length(), &result);
    if (status != ANI_OK) {
        std::cerr << "Cannot create new utf8 string" << std::endl;
        std::abort();
    }
    return result;
}

static const char *MODULE_NAME = "L@defModule/namespace_bind_native_functions_test;";
static const char *MODULE_DESCRIPTOR = "@defModule/namespace_bind_native_functions_test";
static const char *CONCAT_SIGNATURE = "Lstd/core/String;Lstd/core/String;:Lstd/core/String;";
static const ani_native_function NATIVE_FUNC_SUM = {"sum", "II:I", reinterpret_cast<void *>(Sum)};
static const ani_native_function NATIVE_FUNC_SUM_A = {"sum", "III:I", reinterpret_cast<void *>(SumA)};
static const ani_native_function NATIVE_FUNC_CONCAT = {"concat", CONCAT_SIGNATURE, reinterpret_cast<void *>(Concat)};

TEST_F(NamespaceBindNativeFunctionsTest, bind_native_functions)
{
    ani_module module {};
    ASSERT_EQ(env_->FindModule(MODULE_NAME, &module), ANI_OK);
    ASSERT_NE(module, nullptr);

    ani_namespace ns {};
    ASSERT_EQ(env_->Module_FindNamespace(module, "Lops;", &ns), ANI_OK);
    ASSERT_NE(ns, nullptr);

    std::array functions = {
        NATIVE_FUNC_SUM,
        NATIVE_FUNC_CONCAT,
    };
    ASSERT_EQ(env_->Namespace_BindNativeFunctions(ns, functions.data(), functions.size()), ANI_OK);

    ASSERT_EQ(CallEtsFunction<ani_boolean>(MODULE_DESCRIPTOR, "checkSum"), ANI_TRUE);
    ASSERT_EQ(CallEtsFunction<ani_boolean>(MODULE_DESCRIPTOR, "checkConcat"), ANI_TRUE);
}

TEST_F(NamespaceBindNativeFunctionsTest, already_binded_function)
{
    ani_module module {};
    ASSERT_EQ(env_->FindModule(MODULE_NAME, &module), ANI_OK);
    ASSERT_NE(module, nullptr);

    ani_namespace ns {};
    ASSERT_EQ(env_->Module_FindNamespace(module, "Lops;", &ns), ANI_OK);
    ASSERT_NE(ns, nullptr);

    std::array functions = {
        NATIVE_FUNC_SUM,
        NATIVE_FUNC_CONCAT,
    };
    ASSERT_EQ(env_->Namespace_BindNativeFunctions(ns, functions.data(), functions.size()), ANI_OK);
    ASSERT_EQ(env_->Namespace_BindNativeFunctions(ns, functions.data(), functions.size()), ANI_ALREADY_BINDED);

    ASSERT_EQ(CallEtsFunction<ani_boolean>(MODULE_DESCRIPTOR, "checkSum"), ANI_TRUE);
    ASSERT_EQ(CallEtsFunction<ani_boolean>(MODULE_DESCRIPTOR, "checkConcat"), ANI_TRUE);
}

TEST_F(NamespaceBindNativeFunctionsTest, invalid_ns)
{
    const char *concatSignature = "Lstd/core/String;Lstd/core/String;:Lstd/core/String;";
    std::array functions = {
        ani_native_function {"sum", "II:I", reinterpret_cast<void *>(Sum)},
        ani_native_function {"concat", concatSignature, reinterpret_cast<void *>(Concat)},
    };
    ASSERT_EQ(env_->Namespace_BindNativeFunctions(nullptr, functions.data(), functions.size()), ANI_INVALID_ARGS);
}

TEST_F(NamespaceBindNativeFunctionsTest, invalid_functions)
{
    const char *concatSignature = "Lstd/core/String;Lstd/core/String;:Lstd/core/String;";
    std::array functions = {
        ani_native_function {"sum", "II:I", reinterpret_cast<void *>(Sum)},
        ani_native_function {"concat", concatSignature, reinterpret_cast<void *>(Concat)},
    };
    ASSERT_EQ(env_->Namespace_BindNativeFunctions(nullptr, nullptr, functions.size()), ANI_INVALID_ARGS);
}

TEST_F(NamespaceBindNativeFunctionsTest, function_not_found)
{
    ani_module module {};
    ASSERT_EQ(env_->FindModule(MODULE_NAME, &module), ANI_OK);
    ASSERT_NE(module, nullptr);

    ani_namespace ns {};
    ASSERT_EQ(env_->Module_FindNamespace(module, "Lops;", &ns), ANI_OK);
    ASSERT_NE(ns, nullptr);

    const char *concatSignature = "Lstd/core/String;Lstd/core/String;:Lstd/core/String;";
    std::array functions = {
        ani_native_function {"sumX", "II:I", reinterpret_cast<void *>(Sum)},
        ani_native_function {"concat", concatSignature, reinterpret_cast<void *>(Concat)},
    };
    ASSERT_EQ(env_->Namespace_BindNativeFunctions(ns, functions.data(), functions.size()), ANI_NOT_FOUND);
}

TEST_F(NamespaceBindNativeFunctionsTest, namespace_bind_native_functions_001)
{
    ani_module module {};
    ASSERT_EQ(env_->FindModule(MODULE_NAME, &module), ANI_OK);
    ASSERT_NE(module, nullptr);

    ani_namespace ns {};
    ASSERT_EQ(env_->Module_FindNamespace(module, "Lops;", &ns), ANI_OK);
    ASSERT_NE(ns, nullptr);

    std::array functions = {NATIVE_FUNC_SUM, NATIVE_FUNC_SUM_A, NATIVE_FUNC_CONCAT};
    ASSERT_EQ(env_->Namespace_BindNativeFunctions(ns, functions.data(), functions.size()), ANI_OK);

    ASSERT_EQ(CallEtsFunction<ani_boolean>(MODULE_DESCRIPTOR, "checkSum"), ANI_TRUE);
    ASSERT_EQ(CallEtsFunction<ani_boolean>(MODULE_DESCRIPTOR, "checkSumA"), ANI_TRUE);
    ASSERT_EQ(CallEtsFunction<ani_boolean>(MODULE_DESCRIPTOR, "checkConcat"), ANI_TRUE);
}

TEST_F(NamespaceBindNativeFunctionsTest, namespace_bind_native_functions_002)
{
    ani_module module {};
    ASSERT_EQ(env_->FindModule(MODULE_NAME, &module), ANI_OK);
    ASSERT_NE(module, nullptr);

    ani_namespace ns {};
    ASSERT_EQ(env_->Module_FindNamespace(module, "Lops;", &ns), ANI_OK);
    ASSERT_NE(ns, nullptr);

    std::array functions = {NATIVE_FUNC_SUM, NATIVE_FUNC_SUM_A, NATIVE_FUNC_CONCAT};
    ASSERT_EQ(env_->c_api->Namespace_BindNativeFunctions(nullptr, ns, functions.data(), functions.size()),
              ANI_INVALID_ARGS);
    ASSERT_EQ(env_->Namespace_BindNativeFunctions(nullptr, functions.data(), functions.size()), ANI_INVALID_ARGS);
    ASSERT_EQ(env_->Namespace_BindNativeFunctions(ns, nullptr, functions.size()), ANI_INVALID_ARGS);
    ASSERT_EQ(env_->Namespace_BindNativeFunctions(ns, functions.data(), 0), ANI_OK);
}

TEST_F(NamespaceBindNativeFunctionsTest, namespace_bind_native_functions_003)
{
    ani_module module {};
    ASSERT_EQ(env_->FindModule(MODULE_NAME, &module), ANI_OK);
    ASSERT_NE(module, nullptr);

    ani_namespace ns {};
    ASSERT_EQ(env_->Module_FindNamespace(module, "Lops;", &ns), ANI_OK);
    ASSERT_NE(ns, nullptr);

    ASSERT_EQ(env_->Namespace_BindNativeFunctions(ns, &NATIVE_FUNC_SUM, 1), ANI_OK);
    ASSERT_EQ(CallEtsFunction<ani_boolean>(MODULE_DESCRIPTOR, "checkSum"), ANI_TRUE);

    ASSERT_EQ(env_->Namespace_BindNativeFunctions(ns, &NATIVE_FUNC_SUM_A, 1), ANI_OK);
    ASSERT_EQ(CallEtsFunction<ani_boolean>(MODULE_DESCRIPTOR, "checkSumA"), ANI_TRUE);

    ASSERT_EQ(env_->Namespace_BindNativeFunctions(ns, &NATIVE_FUNC_CONCAT, 1), ANI_OK);
    ASSERT_EQ(CallEtsFunction<ani_boolean>(MODULE_DESCRIPTOR, "checkConcat"), ANI_TRUE);
}

TEST_F(NamespaceBindNativeFunctionsTest, namespace_bind_native_functions_004)
{
    ani_module module {};
    ASSERT_EQ(env_->FindModule(MODULE_NAME, &module), ANI_OK);
    ASSERT_NE(module, nullptr);

    ani_namespace nsA {};
    ani_namespace nsB {};
    ani_function fn {};
    ASSERT_EQ(env_->Module_FindNamespace(module, "LA;", &nsA), ANI_OK);
    ASSERT_NE(nsA, nullptr);
    ASSERT_EQ(env_->Module_FindNamespace(module, "LA/B;", &nsB), ANI_OK);
    ASSERT_NE(nsB, nullptr);
    ASSERT_EQ(env_->Namespace_FindFunction(nsA, "checkSumB", ":Z", &fn), ANI_OK);
    ASSERT_NE(fn, nullptr);

    std::array functions = {
        ani_native_function {"sumB", "II:I", reinterpret_cast<void *>(Sum)},
    };
    ASSERT_EQ(env_->Namespace_BindNativeFunctions(nsB, functions.data(), functions.size()), ANI_OK);
    ani_boolean value = ANI_FALSE;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    ASSERT_EQ(env_->Function_Call_Boolean(fn, &value), ANI_OK);
    ASSERT_EQ(value, ANI_TRUE);
}

TEST_F(NamespaceBindNativeFunctionsTest, namespace_bind_native_functions_005)
{
    ani_module module {};
    ASSERT_EQ(env_->FindModule(MODULE_NAME, &module), ANI_OK);
    ASSERT_NE(module, nullptr);

    ani_namespace ns {};
    ASSERT_EQ(env_->Module_FindNamespace(module, "Lops;", &ns), ANI_OK);
    ASSERT_NE(ns, nullptr);

    std::array functions = {
        ani_native_function {"undefined", "II:I", reinterpret_cast<void *>(Sum)},
    };
    ASSERT_EQ(env_->Namespace_BindNativeFunctions(ns, functions.data(), functions.size()), ANI_NOT_FOUND);
}
}  // namespace ark::ets::ani::testing

// NOLINTEND(readability-identifier-naming)