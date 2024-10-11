/*
 * Copyright (c) 2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#ifndef ECMASCRIPT_JS_SEGMENTS_H
#define ECMASCRIPT_JS_SEGMENTS_H

#include "ecmascript/ecma_string.h"
#include "ecmascript/ecma_vm.h"
#include "ecmascript/ecma_macros.h"
#include "ecmascript/global_env.h"
#include "ecmascript/js_hclass.h"
#include "ecmascript/js_intl.h"
#include "ecmascript/js_locale.h"
#include "ecmascript/js_object.h"
#include "ecmascript/js_segmenter.h"
#include "ecmascript/js_tagged_value.h"
#include "ecmascript/object_factory.h"

#include "unicode/locdspnm.h"

namespace panda::ecmascript {
class JSSegments : public JSObject {
public:
    CAST_CHECK(JSSegments, IsJSSegments);

    static constexpr size_t ICU_FIELD_OFFSET = JSObject::SIZE;
    ACCESSORS(IcuField, ICU_FIELD_OFFSET, SEGMENTS_STRING_OFFSET)
    ACCESSORS(SegmentsString, SEGMENTS_STRING_OFFSET, UNICODE_STRING_OFFSET)
    ACCESSORS(UnicodeString, UNICODE_STRING_OFFSET, BIT_FIELD_OFFSET)
    ACCESSORS_BIT_FIELD(BitField, BIT_FIELD_OFFSET, LAST_OFFSET)
    DEFINE_ALIGN_SIZE(LAST_OFFSET);

    // define BitField
    static constexpr size_t GRANULARITY_BITS = 3;
    FIRST_BIT_FIELD(BitField, Granularity, GranularityOption, GRANULARITY_BITS)

    DECL_VISIT_OBJECT_FOR_JS_OBJECT(JSObject, ICU_FIELD_OFFSET, BIT_FIELD_OFFSET)
    DECL_DUMP()

    // 18.5.1 CreateSegmentsObject ( segmenter, string )
    static JSHandle<JSSegments> CreateSegmentsObject(JSThread *thread,
                                                     const JSHandle<JSSegmenter> &segmenter,
                                                     const JSHandle<EcmaString> &string);
    // Get icu break iterator from icu field
    icu::BreakIterator *GetIcuBreakIterator() const
    {
        ASSERT(GetIcuField().IsJSNativePointer());
        auto result = JSNativePointer::Cast(GetIcuField().GetTaggedObject())->GetExternalPointer();
        return reinterpret_cast<icu::BreakIterator *>(result);
    }

    static void FreeIcuBreakIterator([[maybe_unused]] void *env, void *pointer, [[maybe_unused]] void* hint)
    {
        if (pointer == nullptr) {
            return;
        }
        auto icuBreakIterator = reinterpret_cast<icu::BreakIterator *>(pointer);
        delete icuBreakIterator;
    }

    static void SetIcuBreakIterator(JSThread *thread, const JSHandle<JSSegments> &segments,
                                    icu::BreakIterator* icuBreakIterator, const NativePointerCallback &callback);

    static void FreeUString([[maybe_unused]] void *env, void *pointer, [[maybe_unused]] void* hint)
    {
        if (pointer == nullptr) {
            return;
        }
        auto unicodeString = reinterpret_cast<icu::UnicodeString *>(pointer);
        delete unicodeString;
    }

    static void SetUString(JSThread *thread, const JSHandle<JSSegments> &segments,
                           icu::UnicodeString* icuUnicodeString, const NativePointerCallback &callback);

    icu::UnicodeString *GetUString() const
    {
        ASSERT(GetUnicodeString().IsJSNativePointer());
        auto result = JSNativePointer::Cast(GetUnicodeString().GetTaggedObject())->GetExternalPointer();
        return reinterpret_cast<icu::UnicodeString *>(result);
    }

    static JSTaggedValue Containing(JSThread *thread, const JSHandle<JSSegments> &segments, double index);

    // 18.7.1 CreateSegmentDataObject ( segmenter, string, startIndex, endIndex )
    static JSHandle<JSObject> CreateSegmentDataObject(JSThread *thread, GranularityOption granularity,
        icu::BreakIterator* breakIterator, const JSHandle<EcmaString> &inputString,
        const icu::UnicodeString& unicodeString, int32_t startIndex, int32_t endIndex);
};
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_JS_SEGMENTS_H