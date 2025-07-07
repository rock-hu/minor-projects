/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ecmascript/js_collator.h"
#include "ecmascript/global_env.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda::ecmascript;

namespace panda::test {
class JSCollatorTest : public BaseTestWithScope<true> {
};

/**
 * @tc.name: GetIcuCollatorAndCompare
 * @tc.desc: Call "SetIcuCollator" function set IcuCollator arrributes, then check whether the return value is
 *           expected and Call "CompareStrings" function to compare the two strings to check whether the attribute
 *           setting of ICU Collator meets the requirements.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(JSCollatorTest, GetIcuCollatorAndCompare)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    // test string
    JSHandle<EcmaString> string1 = factory->NewFromASCII("ABC");
    JSHandle<EcmaString> string2 = factory->NewFromASCII("abc");
    JSHandle<EcmaString> string3 = factory->NewFromASCII("cde");
    UErrorCode status = U_ZERO_ERROR;
    JSHandle<JSTaggedValue> ctor = env->GetCollatorFunction();
    JSHandle<JSCollator> collator =
        JSHandle<JSCollator>::Cast(factory->NewJSObjectByConstructor(JSHandle<JSFunction>(ctor), ctor));
    // test Collator for US and set its strength to PRIMARY
    icu::Collator *icuCollator = icu::Collator::createInstance("US", status);
    EXPECT_TRUE(icuCollator != nullptr);
    icuCollator->setStrength(icu::Collator::PRIMARY);
    // Call "SetIcuCollator" function set icu collator
    JSCollator::SetIcuCollator(thread, collator, icuCollator, JSCollator::FreeIcuCollator);
    icu::Collator *icuCollator1 = collator->GetIcuCollator(thread);
    EXPECT_TRUE(icuCollator1 == icuCollator);
    JSTaggedValue result = JSCollator::CompareStrings(thread, icuCollator1, string1, string2);
    EXPECT_EQ(result.GetInt(), 0);  // equivalent
    result = JSCollator::CompareStrings(thread, icuCollator1, string1, string3);
    EXPECT_EQ(result.GetInt(), -1); // less than
    // test Collator is zh-Hans-CN locale
    icu::Locale zhLocale("zh", "Hans", "CN");
    icuCollator = icu::Collator::createInstance(zhLocale, status);
    icuCollator->setStrength(icu::Collator::PRIMARY);
    JSCollator::SetIcuCollator(thread, collator, icuCollator, JSCollator::FreeIcuCollator);
    icu::Collator *icuCollator2 = collator->GetIcuCollator(thread);
    EXPECT_TRUE(icuCollator2 == icuCollator);
    result = JSCollator::CompareStrings(thread, icuCollator2, string1, string2);
    EXPECT_EQ(result.GetInt(), 0);  // equivalent
}

/**
 * @tc.name: InitializeCollatorAndGetIcuCollator
 * @tc.desc: Call "InitializeCollator" function initialize the attributes of Collator,then check whether the
 *           attributes is expected.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(JSCollatorTest, InitializeCollatorAndGetIcuCollator)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSTaggedValue> ctor = env->GetCollatorFunction();
    JSHandle<JSCollator> collator =
        JSHandle<JSCollator>::Cast(factory->NewJSObjectByConstructor(JSHandle<JSFunction>(ctor), ctor));
    JSHandle<JSTaggedValue> localeStr = thread->GlobalConstants()->GetHandledEnUsString();
    JSHandle<JSTaggedValue> undefinedHandle(thread, JSTaggedValue::Undefined());

    JSHandle<JSCollator> initCollator = JSCollator::InitializeCollator(thread, collator, localeStr, undefinedHandle);
    EXPECT_EQ(JSTaggedValue::SameValue(thread, collator.GetTaggedValue(), initCollator.GetTaggedValue()), true);
    // check attributes
    EXPECT_TRUE(initCollator->GetBoundCompare(thread).IsUndefined());
    EXPECT_EQ(initCollator->GetIgnorePunctuation(), false);
    EXPECT_EQ(initCollator->GetSensitivity(), SensitivityOption::VARIANT);
    EXPECT_EQ(initCollator->GetCaseFirst(), CaseFirstOption::UNDEFINED);
    EXPECT_TRUE(initCollator->GetCollation(thread).IsUndefined());
    EXPECT_EQ(initCollator->GetUsage(), UsageOption::SORT);
    EXPECT_TRUE(
        JSTaggedValue::SameValue(thread, JSHandle<JSTaggedValue>(thread, initCollator->GetLocale(thread)), localeStr));
    // check icucollator
    icu::Collator *icuCollator = initCollator->GetIcuCollator(thread);
    EXPECT_EQ(icuCollator->getStrength(), icu::Collator::TERTIARY);
}
}  // namespace panda::test
