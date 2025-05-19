/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "napi/native_common.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "securec.h"
#include "test.h"
#include "test_common.h"
#include "utils/log.h"
#ifdef FOR_JERRYSCRIPT_TEST
#include "jerryscript-core.h"
#endif

static constexpr int32_t NAPI_UT_BUFFER_SIZE = 64;

class NapiExtTest : public NativeEngineTest {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "NapiExtTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "NapiExtTest TearDownTestCase";
    }

    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: UndefinedTest
 * @tc.desc: Test undefined type.
 * @tc.type: FUNC
 */
HWTEST_F(NapiExtTest, CreateBufferTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;

    napi_value buffer = nullptr;
    void* bufferPtr = nullptr;
    size_t bufferSize = NAPI_UT_BUFFER_SIZE;
    napi_create_buffer(env, bufferSize, &bufferPtr, &buffer);
    void* tmpBufferPtr = nullptr;
    size_t bufferLength = 0;
    napi_get_buffer_info(env, buffer, &tmpBufferPtr, &bufferLength);

    ASSERT_EQ(bufferPtr, tmpBufferPtr);
    ASSERT_EQ(bufferSize, bufferLength);
}

HWTEST_F(NapiExtTest, CreateBufferTest003, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;

    napi_value buffer = nullptr;
    void* bufferPtr = nullptr;
    const char bufferdata[] = "for test";
    const char* data = bufferdata;
    size_t bufferSize = NAPI_UT_BUFFER_SIZE;
    napi_create_buffer_copy(env, bufferSize, data, &bufferPtr, &buffer);

    void* tmpBufferPtr = nullptr;
    size_t bufferLength = 0;
    napi_get_buffer_info(env, buffer, &tmpBufferPtr, &bufferLength);

    ASSERT_EQ(bufferPtr, tmpBufferPtr);
    ASSERT_EQ(bufferSize, bufferLength);
    ASSERT_EQ(0, memcmp(bufferdata, bufferPtr, bufferSize));
}

HWTEST_F(NapiExtTest, CreateBufferTest005, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;

    napi_value buffer = nullptr;
    char testStr[] = "test";
    void* bufferPtr = testStr;

    size_t bufferSize = NAPI_UT_BUFFER_SIZE;
    napi_create_external_buffer(
        env, bufferSize, bufferPtr, [](napi_env env, void* data, void* hint) {}, (void*)testStr, &buffer);

    void* tmpBufferPtr = nullptr;
    size_t bufferLength = 0;
    napi_get_buffer_info(env, buffer, &tmpBufferPtr, &bufferLength);
    bool isBuffer = false;
    napi_is_buffer(env, buffer, &isBuffer);

    ASSERT_EQ(bufferSize, bufferLength);
}

HWTEST_F(NapiExtTest, IsBufferTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;

    napi_value buffer = nullptr;
    void* bufferPtr = nullptr;
    size_t bufferSize = NAPI_UT_BUFFER_SIZE;
    bool isBuffer = false;

    napi_create_buffer(env, bufferSize, &bufferPtr, &buffer);

    void* tmpBufferPtr = nullptr;
    size_t bufferLength = 0;
    napi_get_buffer_info(env, buffer, &tmpBufferPtr, &bufferLength);
    napi_is_buffer(env, buffer, &isBuffer);

    ASSERT_TRUE(isBuffer);
}

HWTEST_F(NapiExtTest, IsBufferTest002, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;

    napi_value buffer = nullptr;
    void* bufferPtr = nullptr;
    size_t bufferSize = -1;
    bool isBuffer = false;

    napi_create_buffer(env, bufferSize, &bufferPtr, &buffer);

    void* tmpBufferPtr = nullptr;
    size_t bufferLength = 0;
    napi_get_buffer_info(env, buffer, &tmpBufferPtr, &bufferLength);
    napi_is_buffer(env, buffer, &isBuffer);

    ASSERT_EQ(isBuffer, false);
}

/**
 * @tc.name: StringTestAce
 * @tc.desc: Test string type.
 * @tc.type: FUNC
 */
HWTEST_F(NapiExtTest, StringTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    const char16_t testStr[] = u"中文,English,123456,!@#$%$#^%&12345";
    int testStrLength = static_cast<int>(std::char_traits<char16_t>::length(testStr));
    napi_value result = nullptr;
    ASSERT_CHECK_CALL(napi_create_string_utf16(env, testStr, testStrLength, &result));
    ASSERT_CHECK_VALUE_TYPE(env, result, napi_string);

    char16_t* buffer = nullptr;
    size_t bufferSize = 0;
    size_t strLength = 0;
    ASSERT_CHECK_CALL(napi_get_value_string_utf16(env, result, nullptr, 0, &bufferSize));
    ASSERT_GT(bufferSize, (size_t)0);
    buffer = new char16_t[bufferSize + 1] { 0 };
    ASSERT_CHECK_CALL(napi_get_value_string_utf16(env, result, buffer, bufferSize + 1, &strLength));
    for (int i = 0; i < testStrLength; i++) {
        ASSERT_EQ(testStr[i], buffer[i]);
    }
    ASSERT_EQ(testStrLength, strLength);
    delete[] buffer;
    buffer = nullptr;
    char16_t* bufferShort = nullptr;
    int bufferShortSize = 3;
    bufferShort = new char16_t[bufferShortSize] { 0 };
    ASSERT_CHECK_CALL(napi_get_value_string_utf16(env, result, bufferShort, bufferShortSize, &strLength));
    for (int i = 0; i < bufferShortSize; i++) {
        if (i == (bufferShortSize - 1)) {
            ASSERT_EQ(0, bufferShort[i]);
        } else {
            ASSERT_EQ(testStr[i], bufferShort[i]);
        }
    }
    ASSERT_EQ(testStrLength, strLength);
    delete[] bufferShort;
    bufferShort = nullptr;
}

#if  (defined(FOR_JERRYSCRIPT_TEST)) &&  (JERRY_API_MINOR_VERSION <= 3)
    // jerryscript 2.3 do nothing
#else
// jerryscript 2.4 or quickjs or V8

/**
 * @tc.name: BigIntTest
 * @tc.desc: Test number type.
 * @tc.type: FUNC
 */
HWTEST_F(NapiExtTest, BigIntTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    // uint64
    {
        uint64_t testValue = UINT64_MAX;
        napi_value result = nullptr;
        bool flag = false;
        ASSERT_CHECK_CALL(napi_create_bigint_uint64(env, testValue, &result));
        ASSERT_CHECK_VALUE_TYPE(env, result, napi_bigint);

        uint64_t resultValue = 0;
        ASSERT_CHECK_CALL(napi_get_value_bigint_uint64(env, result, &resultValue, &flag));
        ASSERT_EQ(resultValue, UINT64_MAX);
        ASSERT_TRUE(flag);
    }
    {
        uint64_t testValue = 0xffffffffffffffff;
        napi_value result = nullptr;
        ASSERT_CHECK_CALL(napi_create_bigint_uint64(env, testValue, &result));
        ASSERT_CHECK_VALUE_TYPE(env, result, napi_bigint);
        bool flag = false;
        uint64_t resultValue = 0;
        ASSERT_CHECK_CALL(napi_get_value_bigint_uint64(env, result, &resultValue, &flag));
        ASSERT_EQ(resultValue, testValue);
        ASSERT_TRUE(flag);
    }
    {
        uint64_t testValue = 9007199254740991;
        napi_value result = nullptr;
        ASSERT_CHECK_CALL(napi_create_bigint_uint64(env, testValue, &result));
        ASSERT_CHECK_VALUE_TYPE(env, result, napi_bigint);
        bool flag = false;
        uint64_t resultValue = 0;
        ASSERT_CHECK_CALL(napi_get_value_bigint_uint64(env, result, &resultValue, &flag));
        ASSERT_EQ(resultValue, testValue);
        ASSERT_TRUE(flag);
    }
}

HWTEST_F(NapiExtTest, BigIntTest002, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    // int64
    {
        int64_t testValue = INT64_MAX;
        napi_value result = nullptr;
        bool flag = false;
        ASSERT_CHECK_CALL(napi_create_bigint_int64(env, testValue, &result));
        ASSERT_CHECK_VALUE_TYPE(env, result, napi_bigint);

        int64_t resultValue = 0;
        ASSERT_CHECK_CALL(napi_get_value_bigint_int64(env, result, &resultValue, &flag));
        ASSERT_EQ(resultValue, INT64_MAX);
        ASSERT_TRUE(flag);
    }
    {
        int64_t testValue = 9007199254740991;
        napi_value result = nullptr;
        ASSERT_CHECK_CALL(napi_create_bigint_int64(env, testValue, &result));
        ASSERT_CHECK_VALUE_TYPE(env, result, napi_bigint);
        bool flag = false;
        int64_t resultValue = 0;
        ASSERT_CHECK_CALL(napi_get_value_bigint_int64(env, result, &resultValue, &flag));
        ASSERT_EQ(resultValue, testValue);
        ASSERT_TRUE(flag);
    }
    {
        int64_t testValue = -1;
        napi_value result = nullptr;
        ASSERT_CHECK_CALL(napi_create_bigint_int64(env, testValue, &result));
        ASSERT_CHECK_VALUE_TYPE(env, result, napi_bigint);
        bool flag = false;
        int64_t resultValue = 0;
        ASSERT_CHECK_CALL(napi_get_value_bigint_int64(env, result, &resultValue, &flag));
        ASSERT_EQ(resultValue, testValue);
        ASSERT_TRUE(flag);
    }
}

HWTEST_F(NapiExtTest, BigIntWordsTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    int signBit = 0;
    size_t wordCount = 4;
    uint64_t words[] = { 0xFFFFFFFFFFFFFFFF, 34ULL, 56ULL, 0xFFFFFFFFFFFFFFFF };
    uint64_t wordsOut[] = { 0ULL, 0ULL, 0ULL, 0ULL };
    napi_value result = nullptr;
    ASSERT_CHECK_CALL(napi_create_bigint_words(env, signBit, wordCount, words, &result));

    ASSERT_CHECK_CALL(napi_get_value_bigint_words(env, result, &signBit, &wordCount, wordsOut));

    ASSERT_EQ(signBit, 0);
    ASSERT_EQ(wordCount, 4);
    ASSERT_EQ(words[0], wordsOut[0]);
    ASSERT_EQ(words[1], wordsOut[1]);
    ASSERT_EQ(words[2], wordsOut[2]);
    ASSERT_EQ(words[3], wordsOut[3]);
}

HWTEST_F(NapiExtTest, BigIntWordsTest002, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    int signBit = 0;
    size_t wordCount = 5;
    uint64_t words[] = { 12ULL, 34ULL, 56ULL, 78ULL, 90ULL };
    uint64_t wordsOut[] = { 0ULL, 0ULL, 0ULL, 0ULL, 0ULL };
    napi_value result = nullptr;
    ASSERT_CHECK_CALL(napi_create_bigint_words(env, signBit, wordCount, words, &result));

    ASSERT_CHECK_CALL(napi_get_value_bigint_words(env, result, &signBit, &wordCount, wordsOut));

    ASSERT_EQ(signBit, 0);
    ASSERT_EQ(wordCount, 5);
    for (size_t i = 0; i < wordCount; i++) {
        ASSERT_EQ(words[i], wordsOut[i]);
    }
}

HWTEST_F(NapiExtTest, BigIntWordsTest003, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    int signBit = 1;
    size_t wordCount = 4;
    uint64_t words[] = { 0xFFFFFFFFFFFFFFFF, 34ULL, 56ULL, 0xFFFFFFFFFFFFFFFF };
    uint64_t wordsOut[] = { 0ULL, 0ULL, 0ULL, 0ULL };
    napi_value result = nullptr;
    ASSERT_CHECK_CALL(napi_create_bigint_words(env, signBit, wordCount, words, &result));

    ASSERT_CHECK_CALL(napi_get_value_bigint_words(env, result, &signBit, &wordCount, wordsOut));

    ASSERT_EQ(signBit, 1);
    ASSERT_EQ(wordCount, 4);
    ASSERT_EQ(words[0], wordsOut[0]);
    ASSERT_EQ(words[1], wordsOut[1]);
    ASSERT_EQ(words[2], wordsOut[2]);
    ASSERT_EQ(words[3], wordsOut[3]);
}

HWTEST_F(NapiExtTest, BigIntWordsTest004, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    int signBit = 1;
    size_t wordCount = 5;
    uint64_t words[] = { 12ULL, 34ULL, 56ULL, 78ULL, 0x000000FF98765432 };
    uint64_t wordsOut[] = { 0ULL, 0ULL, 0ULL, 0ULL, 0ULL };
    napi_value result = nullptr;
    ASSERT_CHECK_CALL(napi_create_bigint_words(env, signBit, wordCount, words, &result));

    ASSERT_CHECK_CALL(napi_get_value_bigint_words(env, result, &signBit, &wordCount, wordsOut));

    ASSERT_EQ(signBit, 1);
    ASSERT_EQ(wordCount, 5);
    for (size_t i = 0; i < wordCount; i++) {
        ASSERT_EQ(words[i], wordsOut[i]);
    }
}

HWTEST_F(NapiExtTest, TagObjectTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    napi_value object = nullptr;
    const napi_type_tag typeTag = { 0xFFFFFFFFFFFFFFFF, 34ULL };

    ASSERT_CHECK_CALL(napi_create_object(env, &object));

    ASSERT_CHECK_CALL(napi_type_tag_object(env, object, &typeTag));

    bool checkResult = false;

    ASSERT_CHECK_CALL(napi_check_object_type_tag(env, object, &typeTag, &checkResult));
    ASSERT_TRUE(checkResult);
}
#endif

HWTEST_F(NapiExtTest, GetDateTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    napi_value createResult = nullptr;
    double time = 202110181203150;

    ASSERT_CHECK_CALL(napi_create_date(env, time, &createResult));

    double getTime = false;

    ASSERT_CHECK_CALL(napi_get_date_value(env, createResult, &getTime));
    bool result = false;
    if (time == getTime) {
        result = true;
    }
    ASSERT_TRUE(result);
}

HWTEST_F(NapiExtTest, IsDateTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    napi_value createResult = nullptr;
    double time = 202110181203150;

    ASSERT_CHECK_CALL(napi_create_date(env, time, &createResult));

    bool result = false;

    ASSERT_CHECK_CALL(napi_is_date(env, createResult, &result));

    ASSERT_TRUE(result);
}

/**
 * @tc.name: ACE_napi_adjust_external_memory_test.
 * @tc.desc: Test napi_adjust_external_memory.
 * @tc.type: FUNC
 */
HWTEST_F(NapiExtTest, AdjustExternalMemoryTest001, testing::ext::TestSize.Level1)
{
    HILOG_INFO("%{public}s", "ACE_napi_adjust_external_memory_test start");
    napi_env env = (napi_env)engine_;
    int64_t changeInBytes = 32;
    int64_t adjustedValue = 32;
    napi_status ret = napi_adjust_external_memory(env, changeInBytes, &adjustedValue);
    ASSERT_EQ(ret, napi_ok);
    HILOG_INFO("%{public}s", "ACE_napi_adjust_external_memory_test end");
}

/**
 * @tc.name: ACE_napi_async_init_Test.
 * @tc.desc: Test napi_async_init, napi_async_destroy.
 * @tc.type: FUNC
 */
HWTEST_F(NapiExtTest, AsyncInitTest001, testing::ext::TestSize.Level1)
{
    HILOG_INFO("ACE_napi_async_init_Test_001 start");

    napi_env env = (napi_env)engine_;

    napi_value resourceName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, "ACE_napi_async_init_Test_001",
        NAPI_AUTO_LENGTH, &resourceName));

    napi_async_context context = nullptr;
    napi_status ret = napi_async_init(env, nullptr, resourceName, &context);
    ASSERT_EQ(ret, napi_ok);
    EXPECT_NE(context, nullptr);

    ret = napi_async_destroy(env, context);
    ASSERT_EQ(ret, napi_ok);

    HILOG_INFO("ACE_napi_async_init_Test_001 end");
}

/**
 * @tc.name: ACE_napi_open_callback_scope_Test
 * @tc.desc: Test napi_open_callback_scope, napi_close_callback_scope.
 * @tc.type: FUNC
 */
HWTEST_F(NapiExtTest, OpenCallbackScopeTest001, testing::ext::TestSize.Level1)
{
    HILOG_INFO("ACE_napi_open_callback_scope_Test_001 start");

    napi_env env = (napi_env)engine_;

    auto callbackScopeManager = engine_->GetCallbackScopeManager();
    ASSERT_NE(callbackScopeManager, nullptr);

    int openCallbackScopesBefore = callbackScopeManager->GetOpenCallbackScopes();
    int asyncCallbackScopeDepthBefore = callbackScopeManager->GetAsyncCallbackScopeDepth();

    napi_value resourceName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, "test", NAPI_AUTO_LENGTH, &resourceName));

    napi_async_context context;
    NAPI_CALL_RETURN_VOID(env, napi_async_init(env, nullptr, resourceName, &context));

    napi_callback_scope scope = nullptr;
    napi_status ret = napi_open_callback_scope(env, NULL, context, &scope);
    EXPECT_EQ(ret, napi_ok);
    EXPECT_NE(scope, nullptr);

    int openCallbackScopes = callbackScopeManager->GetOpenCallbackScopes();
    int asyncCallbackScopeDepth = callbackScopeManager->GetAsyncCallbackScopeDepth();
    EXPECT_EQ(openCallbackScopes, (openCallbackScopesBefore + 1));
    EXPECT_EQ(asyncCallbackScopeDepth, (asyncCallbackScopeDepthBefore + 1));

    ret = napi_close_callback_scope(env, scope);
    EXPECT_EQ(ret, napi_ok);

    int openCallbackScopesAfter = callbackScopeManager->GetOpenCallbackScopes();
    int asyncCallbackScopeDepthAfter = callbackScopeManager->GetAsyncCallbackScopeDepth();
    EXPECT_EQ(openCallbackScopesAfter, openCallbackScopesBefore);
    EXPECT_EQ(asyncCallbackScopeDepthAfter, asyncCallbackScopeDepthBefore);

    NAPI_CALL_RETURN_VOID(env, napi_async_destroy(env, context));

    HILOG_INFO("ACE_napi_open_callback_scope_Test_001 end");
}

/**
 * @tc.name: ACE_napi_open_callback_scope_Test
 * @tc.desc: Test napi_open_callback_scope, napi_close_callback_scope.
 * @tc.type: FUNC
 */
HWTEST_F(NapiExtTest, OpenCallbackScopeTest002, testing::ext::TestSize.Level1)
{
    HILOG_INFO("ACE_napi_open_callback_scope_Test_002 start");

    napi_env env = (napi_env)engine_;

    auto callbackScopeManager = engine_->GetCallbackScopeManager();
    ASSERT_NE(callbackScopeManager, nullptr);

    int openCallbackScopesBefore = callbackScopeManager->GetOpenCallbackScopes();
    int asyncCallbackScopeDepthBefore = callbackScopeManager->GetAsyncCallbackScopeDepth();

    napi_value resourceName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, "test", NAPI_AUTO_LENGTH, &resourceName));

    napi_async_context context;
    NAPI_CALL_RETURN_VOID(env, napi_async_init(env, nullptr, resourceName, &context));

    napi_callback_scope scope = nullptr;
    napi_status ret = napi_open_callback_scope(env, NULL, context, &scope);
    EXPECT_EQ(ret, napi_ok);
    EXPECT_NE(scope, nullptr);

    int openCallbackScopes1 = callbackScopeManager->GetOpenCallbackScopes();
    int asyncCallbackScopeDepth1 = callbackScopeManager->GetAsyncCallbackScopeDepth();

    // Open a internal callback scope
    panda::Local<panda::ObjectRef> obj = panda::ObjectRef::New(env_->GetEcmaVm());
    auto scope2 = callbackScopeManager->Open(engine_, obj, {0, 0});
    int openCallbackScopes2 = callbackScopeManager->GetOpenCallbackScopes();
    int asyncCallbackScopeDepth2 = callbackScopeManager->GetAsyncCallbackScopeDepth();

    EXPECT_NE(scope2, nullptr);
    EXPECT_EQ(openCallbackScopes2, openCallbackScopes1);
    EXPECT_EQ(asyncCallbackScopeDepth2, (asyncCallbackScopeDepth1 + 1));

    callbackScopeManager->Close(scope2);
    obj->Delete(env_->GetEcmaVm(), obj);
    int openCallbackScopes2After = callbackScopeManager->GetOpenCallbackScopes();
    int asyncCallbackScopeDepth2After = callbackScopeManager->GetAsyncCallbackScopeDepth();

    EXPECT_EQ(openCallbackScopes2After, openCallbackScopes1);
    EXPECT_EQ(asyncCallbackScopeDepth2After, asyncCallbackScopeDepth1);

    ret = napi_close_callback_scope(env, scope);
    EXPECT_EQ(ret, napi_ok);

    int openCallbackScopes1After = callbackScopeManager->GetOpenCallbackScopes();
    int asyncCallbackScopeDepth1After = callbackScopeManager->GetAsyncCallbackScopeDepth();

    EXPECT_EQ(openCallbackScopes1After, openCallbackScopesBefore);
    EXPECT_EQ(asyncCallbackScopeDepth1After, asyncCallbackScopeDepthBefore);

    NAPI_CALL_RETURN_VOID(env, napi_async_destroy(env, context));

    HILOG_INFO("ACE_napi_open_callback_scope_Test_002 end");
}

static napi_value TestFatalException(napi_env env, napi_callback_info info)
{
    napi_value err;
    size_t argc = 1;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &err, nullptr, nullptr));
    NAPI_CALL(env, napi_fatal_exception(env, err));
    return nullptr;
}

/**
 * @tc.name: FatalException
 * @tc.desc: Test FatalException Func.
 * @tc.type: FUNC
 */
HWTEST_F(NapiExtTest, FatalExceptionTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    ASSERT_EQ(TestFatalException(env, nullptr), nullptr);
}

HWTEST_F(NapiExtTest, AddFinalizerTest001, testing::ext::TestSize.Level1)
{
    HILOG_INFO("add_finalizer_test_0100 start");
    napi_env env = (napi_env)engine_;

    napi_value object;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &object));

    static bool testValue = false;
    const char* testStr = "test";
    napi_ref ref = nullptr;
    napi_add_finalizer(
        env, object, (void*)testStr, [](napi_env env, void* data, void* hint) {
            testValue = true;
        }, nullptr, &ref);

    napi_delete_reference(env, ref);
    ASSERT_TRUE(testValue);
    HILOG_INFO("add_finalizer_test_0100 end");
}

typedef struct {
    size_t value;
    bool print;
    napi_ref js_cb_ref;
} AddonData;

static void DeleteAddonData(napi_env env, void* raw_data, void* hint)
{
    AddonData* data = (AddonData*)raw_data;
    if (data->print) {
        printf("deleting addon data\n");
    }
    if (data->js_cb_ref != NULL) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, data->js_cb_ref));
    }
    free(data);
}

static napi_value SetPrintOnDelete(napi_env env, napi_callback_info info)
{
    AddonData* data;
    NAPI_CALL(env, napi_get_instance_data(env, (void**)&data));
    data->print = true;
    return NULL;
}

static void TestFinalizer(napi_env env, void* raw_data, void* hint)
{
    (void)raw_data;
    (void)hint;

    AddonData* data;
    napi_value jsResult;
    NAPI_CALL_RETURN_VOID(env, napi_get_instance_data(env, (void**)&data));
    napi_value js_cb, undefined;
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, data->js_cb_ref, &js_cb));
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, js_cb, 0, NULL, &jsResult));

    NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, data->js_cb_ref));
    data->js_cb_ref = NULL;
}

static napi_value ObjectWithFinalizer(napi_env env, napi_callback_info info)
{
    HILOG_INFO("%{public}s", "start.");
    AddonData* data;

    napi_value result, js_cb;
    size_t argc = 1;

    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        HILOG_INFO("%{public}s", "function called");
        return nullptr;
    };

    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &js_cb);

    NAPI_CALL(env, napi_get_instance_data(env, (void**)&data));
    NAPI_ASSERT(env, data->js_cb_ref == NULL, "reference must be NULL");
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &js_cb, NULL, NULL));
    NAPI_CALL(env, napi_create_object(env, &result));
    NAPI_CALL(env, napi_add_finalizer(env, result, NULL, TestFinalizer, NULL, NULL));
    NAPI_CALL(env, napi_create_reference(env, js_cb, 1, &data->js_cb_ref));
    HILOG_INFO("%{public}s", "end.");
    return nullptr;
}

HWTEST_F(NapiExtTest, InstanceDataTest_001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    // Set instance data
    AddonData* data = (AddonData*)malloc(sizeof(*data));
    data->value = 41;
    data->print = false;
    data->js_cb_ref = NULL;
    ASSERT_CHECK_CALL(napi_set_instance_data(env, data, DeleteAddonData, NULL));

    // Test get instance data
    AddonData* get_data = nullptr;
    ASSERT_CHECK_CALL(napi_get_instance_data(env, (void**)&get_data));
    ++get_data->value;
    const size_t expectValue = 42;
    ASSERT_EQ(get_data->value, expectValue);

    // Test finalizer
    SetPrintOnDelete(env, nullptr);
    ObjectWithFinalizer(env, nullptr);
}
