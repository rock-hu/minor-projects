/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "ecmascript/builtins/builtins_string_iterator.h"

#include "ecmascript/ecma_string-inl.h"
#include "ecmascript/ecma_string_table.h"
#include "ecmascript/js_iterator.h"
#include "ecmascript/js_string_iterator.h"

namespace panda::ecmascript::builtins {
JSTaggedValue BuiltinsStringIterator::Next(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), StringIterator, Next);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    // 1. Let O be the this value.
    JSHandle<JSTaggedValue> thisValue = GetThis(argv);
    return NextInternal(thread, thisValue);
}

JSTaggedValue BuiltinsStringIterator::NextInternal(JSThread *thread, JSHandle<JSTaggedValue> thisValue)
{
    // 2. If Type(O) is not Object, throw a TypeError exception.
    // 3. If O does not have all of the internal slots of an String Iterator Instance (21.1.5.3),
    // throw a TypeError exception.
    if (!thisValue->IsStringIterator()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "is not StringIterator", JSTaggedValue::Exception());
    }
    // 4. Let s be the value of the [[IteratedString]] internal slot of O.
    JSHandle<JSTaggedValue> string(thread, thisValue.GetObject<JSStringIterator>()->GetIteratedString());
    // 5. If s is undefined, return CreateIterResultObject(undefined, true).
    if (string->IsUndefined()) {
        return JSIterator::CreateIterResultObject(thread, string, true).GetTaggedValue();
    }
    // 6. Let position be the value of the [[StringIteratorNextIndex]] internal slot of O.
    uint32_t position = thisValue.GetObject<JSStringIterator>()->GetStringIteratorNextIndex();

    // 7. Let len be the number of elements in s.
    uint32_t len = EcmaStringAccessor(string.GetObject<EcmaString>()).GetLength();
    // If position ≥ len, then
    // a. Set the value of the [[IteratedString]] internal slot of O to
    // b. Return CreateIterResultObject(undefined, true).
    if (position >= len) {
        thisValue.GetObject<JSStringIterator>()->SetIteratedString(thread, JSTaggedValue::Undefined());
        JSHandle<JSTaggedValue> result(thread, JSTaggedValue::Undefined());
        return JSIterator::CreateIterResultObject(thread, result, true).GetTaggedValue();
    }

    // 9. Let first be the code unit value at index position in s.
    uint16_t first = EcmaStringAccessor(string.GetObject<EcmaString>()).Get<false>(position);
    JSMutableHandle<JSTaggedValue> result(thread, JSTaggedValue::Undefined());
    uint32_t resultSize = 1;
    // 10. If first < 0xD800 or first > 0xDBFF or position+1 = len, let resultString be the string consisting of the
    // single code unit first.
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    if (position + 1 == len || first < common::utf_helper::DECODE_LEAD_LOW ||
        first > common::utf_helper::DECODE_LEAD_HIGH) {
        if (EcmaStringAccessor::CanBeCompressed(&first, 1)) {
            JSHandle<SingleCharTable> singleCharTable(thread, thread->GetSingleCharTable());
            result.Update(singleCharTable->GetStringFromSingleCharTable(first));
        } else {
            std::vector<uint16_t> resultString {first, 0x0};
            result.Update(factory->NewFromUtf16(resultString.data(), 1).GetTaggedValue());
        }
    } else {
        // 11. Else,
        // a. Let second be the code unit value at index position+1 in the String S.
        // b. If second < 0xDC00 or second > 0xDFFF, let resultString be the string consisting of the single code unit
        // first.
        // c. Else, let resultString be the string consisting of the code unit first followed by the code unit second.
        uint16_t second = EcmaStringAccessor(string.GetObject<EcmaString>()).Get<false>(position + 1);
        if (second < common::utf_helper::DECODE_TRAIL_LOW || second > common::utf_helper::DECODE_TRAIL_HIGH) {
            std::vector<uint16_t> resultString {first, 0x0};
            result.Update(factory->NewFromUtf16NotCompress(resultString.data(), 1).GetTaggedValue());
        } else {
            std::vector<uint16_t> resultString {first, second, 0x0};
            result.Update(
                factory->NewFromUtf16NotCompress(resultString.data(), 2).GetTaggedValue());  // 2: two bytes
            resultSize = 2;  // 2: 2 means that two bytes represent a character string
        }
    }
    // 12. Let resultSize be the number of code units in resultString.
    // 13. Set the value of the [[StringIteratorNextIndex]] internal slot of O to position+ resultSize.
    thisValue.GetObject<JSStringIterator>()->SetStringIteratorNextIndex(position + resultSize);

    // 14. Return CreateIterResultObject(resultString, false).
    return JSIterator::CreateIterResultObject(thread, result, false).GetTaggedValue();
}
}  // namespace panda::ecmascript::builtins
