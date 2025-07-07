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

#include "ecmascript/containers/containers_private.h"
#include "ecmascript/ecma_string.h"
#include "ecmascript/ecma_vm.h"
#include "ecmascript/global_env.h"
#include "ecmascript/js_api/js_api_tree_set.h"
#include "ecmascript/js_api/js_api_tree_set_iterator.h"
#include "ecmascript/js_function.h"
#include "ecmascript/js_handle.h"
#include "ecmascript/js_iterator.h"
#include "ecmascript/js_object-inl.h"
#include "ecmascript/js_tagged_value.h"
#include "ecmascript/object_factory.h"
#include "ecmascript/tagged_tree.h"
#include "ecmascript/tests/ecma_test_common.h"

using namespace panda;

using namespace panda::ecmascript;

namespace panda::test {
class JSAPITreeSetTest : public BaseTestWithScope<false> {
protected:
    JSAPITreeSet *CreateTreeSet()
    {
        ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
        auto result = TestCommon::CreateContainerTaggedValue(thread, containers::ContainerTag::TreeSet);
        JSHandle<JSTaggedValue> constructor(thread, result);
        JSHandle<JSAPITreeSet> set(factory->NewJSObjectByConstructor(JSHandle<JSFunction>(constructor), constructor));
        JSTaggedValue internal = TaggedTreeSet::Create(thread);
        set->SetTreeSet(thread, internal);
        return *set;
    }

    JSHandle<JSAPITreeSet> TestCommon(JSMutableHandle<JSTaggedValue>& key, std::string& myKey, uint32_t nums)
    {
        ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
        // test JSAPITreeSet
        JSHandle<JSAPITreeSet> tset(thread, CreateTreeSet());
        for (int i = 0; i < nums; i++) {
            std::string ikey = myKey + std::to_string(i);
            key.Update(factory->NewFromStdString(ikey).GetTaggedValue());
            JSAPITreeSet::Add(thread, tset, key);
        }
        EXPECT_EQ(tset->GetSize(thread), nums);
        return tset;
    }
};

HWTEST_F_L0(JSAPITreeSetTest, TreeSetCreate)
{
    JSAPITreeSet *set = CreateTreeSet();
    EXPECT_TRUE(set != nullptr);
}

HWTEST_F_L0(JSAPITreeSetTest, TreeSetAddAndHas)
{
    constexpr int NODE_NUMBERS = 8;
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSMutableHandle<JSTaggedValue> key(thread, JSTaggedValue::Undefined());

    // test JSAPITreeSet
    std::string myKey("mykey");
    auto tset = TestCommon(key, myKey, NODE_NUMBERS);

    // test Add exception
    key.Update(JSTaggedValue::Hole());
    JSAPITreeSet::Add(thread, tset, key);
    EXPECT_EXCEPTION();

    for (int i = 0; i < NODE_NUMBERS; i++) {
        std::string ikey = myKey + std::to_string(i);
        key.Update(factory->NewFromStdString(ikey).GetTaggedValue());

        // test has
        bool has = JSAPITreeSet::Has(thread, tset, key);
        EXPECT_EQ(has, true);
    }
}

HWTEST_F_L0(JSAPITreeSetTest, TreeSetDeleteAndHas)
{
    constexpr int NODE_NUMBERS = 64;
    constexpr int REMOVE_SIZE = 48;
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSMutableHandle<JSTaggedValue> key(thread, JSTaggedValue::Undefined());

    // test JSAPITreeSet
    std::string myKey("mykey");
    auto tset = TestCommon(key, myKey, NODE_NUMBERS);

    // test delete
    {
        std::string ikey = myKey + std::to_string(NODE_NUMBERS);
        key.Update(factory->NewFromStdString(ikey).GetTaggedValue());
        bool success = JSAPITreeSet::Delete(thread, tset, key);
        EXPECT_EQ(success, false);
    }

    for (int i = 0; i < REMOVE_SIZE; i++) {
        std::string ikey = myKey + std::to_string(i);
        key.Update(factory->NewFromStdString(ikey).GetTaggedValue());
        bool success = JSAPITreeSet::Delete(thread, tset, key);
        EXPECT_EQ(success, true);
    }
    EXPECT_EQ(tset->GetSize(thread), NODE_NUMBERS - REMOVE_SIZE);

    for (int i = 0; i < REMOVE_SIZE; i++) {
        std::string ikey = myKey + std::to_string(i);
        key.Update(factory->NewFromStdString(ikey).GetTaggedValue());

        // test has
        bool has = JSAPITreeSet::Has(thread, tset, key);
        EXPECT_EQ(has, false);
    }

    for (int i = REMOVE_SIZE; i < NODE_NUMBERS; i++) {
        std::string ikey = myKey + std::to_string(i);
        key.Update(factory->NewFromStdString(ikey).GetTaggedValue());

        // test has
        bool has = JSAPITreeSet::Has(thread, tset, key);
        EXPECT_EQ(has, true);
    }
}

HWTEST_F_L0(JSAPITreeSetTest, TreeSetClear)
{
    constexpr int NODE_NUMBERS = 8;
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSMutableHandle<JSTaggedValue> key(thread, JSTaggedValue::Undefined());

    // test TaggedTreeSet
    std::string myKey("mykey");
    auto tset = TestCommon(key, myKey, NODE_NUMBERS);

    JSAPITreeSet::Clear(thread, tset);
    EXPECT_EQ(tset->GetSize(thread), 0);
    for (int i = 0; i < NODE_NUMBERS; i++) {
        std::string ikey = myKey + std::to_string(i);
        key.Update(factory->NewFromStdString(ikey).GetTaggedValue());

        // test has
        bool has = JSAPITreeSet::Has(thread, tset, key);
        EXPECT_EQ(has, false);
    }
}

HWTEST_F_L0(JSAPITreeSetTest, TreeSetPop)
{
    constexpr int NODE_NUMBERS = 8;
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSMutableHandle<JSTaggedValue> key(thread, JSTaggedValue::Undefined());

    // test TaggedTreeSet
    JSHandle<JSAPITreeSet> tset(thread, CreateTreeSet());

    // test popFirst and popLast of empty treeset
    JSTaggedValue fvalue1 = JSAPITreeSet::PopFirst(thread, tset);
    EXPECT_EQ(fvalue1, JSTaggedValue::Undefined());
    JSTaggedValue lvalue1 = JSAPITreeSet::PopFirst(thread, tset);
    EXPECT_EQ(lvalue1, JSTaggedValue::Undefined());

    std::string myKey("mykey");
    for (int i = 0; i < NODE_NUMBERS; i++) {
        std::string ikey = myKey + std::to_string(i);
        key.Update(factory->NewFromStdString(ikey).GetTaggedValue());
        JSAPITreeSet::Add(thread, tset, key);
    }
    EXPECT_EQ(tset->GetSize(thread), NODE_NUMBERS);

    // test popFirst
    std::string fkey = myKey + std::to_string(0);
    key.Update(factory->NewFromStdString(fkey).GetTaggedValue());
    JSTaggedValue fvalue = JSAPITreeSet::PopFirst(thread, tset);
    EXPECT_EQ(tset->GetSize(thread), NODE_NUMBERS - 1);
    EXPECT_EQ(fvalue, key.GetTaggedValue());
    bool has = JSAPITreeSet::Has(thread, tset, key);
    EXPECT_EQ(has, false);

    // test popLast
    std::string lkey = myKey + std::to_string(NODE_NUMBERS - 1);
    key.Update(factory->NewFromStdString(lkey).GetTaggedValue());
    JSTaggedValue lvalue = JSAPITreeSet::PopLast(thread, tset);
    EXPECT_EQ(tset->GetSize(thread), NODE_NUMBERS - 2);
    EXPECT_EQ(lvalue, key.GetTaggedValue());
    has = JSAPITreeSet::Has(thread, tset, key);
    EXPECT_EQ(has, false);
}

HWTEST_F_L0(JSAPITreeSetTest, JSAPITreeSetIterator)
{
    constexpr int NODE_NUMBERS = 8;
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSAPITreeSet> tset(thread, CreateTreeSet());

    JSMutableHandle<JSTaggedValue> key(thread, JSTaggedValue::Undefined());
    for (int i = 0; i < NODE_NUMBERS; i++) {
        key.Update(JSTaggedValue(i));
        JSAPITreeSet::Add(thread, tset, key);
    }

    // test key or value
    JSHandle<JSTaggedValue> keyIter(factory->NewJSAPITreeSetIterator(tset, IterationKind::KEY));
    JSHandle<JSTaggedValue> valueIter(factory->NewJSAPITreeSetIterator(tset, IterationKind::VALUE));
    JSMutableHandle<JSTaggedValue> keyIterResult(thread, JSTaggedValue::Undefined());
    JSMutableHandle<JSTaggedValue> valueIterResult(thread, JSTaggedValue::Undefined());
    for (int i = 0; i < NODE_NUMBERS / 2; i++) {
        keyIterResult.Update(JSIterator::IteratorStep(thread, keyIter).GetTaggedValue());
        valueIterResult.Update(JSIterator::IteratorStep(thread, valueIter).GetTaggedValue());
        EXPECT_EQ(i, JSIterator::IteratorValue(thread, keyIterResult)->GetInt());
        EXPECT_EQ(i, JSIterator::IteratorValue(thread, valueIterResult)->GetInt());
    }

    // test key and value
    JSHandle<JSTaggedValue> indexKey(thread, JSTaggedValue(0));
    JSHandle<JSTaggedValue> elementKey(thread, JSTaggedValue(1));
    JSHandle<JSTaggedValue> iter(factory->NewJSAPITreeSetIterator(tset, IterationKind::KEY_AND_VALUE));
    JSMutableHandle<JSTaggedValue> iterResult(thread, JSTaggedValue::Undefined());
    JSMutableHandle<JSTaggedValue> result(thread, JSTaggedValue::Undefined());
    for (int i = 0; i < NODE_NUMBERS; i++) {
        iterResult.Update(JSIterator::IteratorStep(thread, iter).GetTaggedValue());
        result.Update(JSIterator::IteratorValue(thread, iterResult).GetTaggedValue());
        EXPECT_EQ(i, JSObject::GetProperty(thread, result, indexKey).GetValue()->GetInt());
        EXPECT_EQ(i, JSObject::GetProperty(thread, result, elementKey).GetValue()->GetInt());
    }

    // test delete
    key.Update(JSTaggedValue(NODE_NUMBERS / 2));
    bool success = JSAPITreeSet::Delete(thread, tset, key);
    EXPECT_EQ(success, true);
    for (int i = NODE_NUMBERS / 2 + 1; i < NODE_NUMBERS; i++) {
        keyIterResult.Update(JSIterator::IteratorStep(thread, keyIter).GetTaggedValue());
        valueIterResult.Update(JSIterator::IteratorStep(thread, valueIter).GetTaggedValue());
        EXPECT_EQ(i, JSIterator::IteratorValue(thread, keyIterResult)->GetInt());
        EXPECT_EQ(i, JSIterator::IteratorValue(thread, valueIterResult)->GetInt());
    }

    // test set
    key.Update(JSTaggedValue(NODE_NUMBERS));
    JSAPITreeSet::Add(thread, tset, key);
    keyIterResult.Update(JSIterator::IteratorStep(thread, keyIter).GetTaggedValue());
    EXPECT_EQ(NODE_NUMBERS, JSIterator::IteratorValue(thread, keyIterResult)->GetInt());

    // test end
    keyIterResult.Update(JSIterator::IteratorStep(thread, keyIter).GetTaggedValue());
    EXPECT_EQ(JSTaggedValue::False(), keyIterResult.GetTaggedValue());
}

HWTEST_F_L0(JSAPITreeSetTest, TreeSetGetKey)
{
    constexpr int NODE_NUMBERS = 8;
    JSMutableHandle<JSTaggedValue> key(thread, JSTaggedValue::Undefined());

    // init treeset
    JSHandle<JSAPITreeSet> tset(thread, CreateTreeSet());
    for (int i = 0; i < NODE_NUMBERS; i++) {
        key.Update(JSTaggedValue(i));
        JSAPITreeSet::Add(thread, tset, key);
    }
    EXPECT_EQ(tset->GetSize(thread), NODE_NUMBERS);

    // test GetKey
    for (int i = 0; i < NODE_NUMBERS; i++) {
        EXPECT_EQ(tset->GetKey(thread, i), JSTaggedValue(i));
    }
    EXPECT_EQ(tset->GetKey(thread, -1), JSTaggedValue::Undefined());
}
}  // namespace panda::test
