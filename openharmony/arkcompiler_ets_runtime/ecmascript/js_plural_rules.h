/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_JS_PLURAL_RULES_H
#define ECMASCRIPT_JS_PLURAL_RULES_H

#include "ecmascript/js_array.h"
#include "ecmascript/js_hclass.h"
#include "ecmascript/js_intl.h"
#include "ecmascript/js_locale.h"
#include "ecmascript/js_object.h"
#include "ecmascript/js_tagged_value.h"

namespace panda::ecmascript {
constexpr int MNFD_DEFAULT = 0;
constexpr int MXFD_DEFAULT = 3;

class JSPluralRules : public JSObject {
public:
    CAST_CHECK(JSPluralRules, IsJSPluralRules);

    static constexpr size_t LOCALE_OFFSET = JSObject::SIZE;
    ACCESSORS(Locale, LOCALE_OFFSET, MINIMUM_INTEGER_DIGITS_OFFSET)
    ACCESSORS(MinimumIntegerDigits, MINIMUM_INTEGER_DIGITS_OFFSET, MINIMUM_FRACTION_DIGITS_OFFSET)
    ACCESSORS(MinimumFractionDigits, MINIMUM_FRACTION_DIGITS_OFFSET, MAXIMUM_FRACTION_DIGITS_OFFSET)
    ACCESSORS(MaximumFractionDigits, MAXIMUM_FRACTION_DIGITS_OFFSET, MININUM_SIGNIFICANT_DIGITS_OFFSET)
    ACCESSORS(MinimumSignificantDigits, MININUM_SIGNIFICANT_DIGITS_OFFSET, MAXINUM_SIGNIFICANT_DIGITS_OFFSET)
    ACCESSORS(MaximumSignificantDigits, MAXINUM_SIGNIFICANT_DIGITS_OFFSET, ICU_PLURAL_RULES_OFFSET)
    ACCESSORS(IcuPR, ICU_PLURAL_RULES_OFFSET, ICU_NUMBER_FORMAT_OFFSET)
    ACCESSORS(IcuNF, ICU_NUMBER_FORMAT_OFFSET, BIT_FIELD_OFFSET)
    ACCESSORS_BIT_FIELD(BitField, BIT_FIELD_OFFSET, LAST_OFFSET)
    DEFINE_ALIGN_SIZE(LAST_OFFSET);

    // define BitField
    static constexpr size_t ROUNDING_TYPE_BITS = 3;
    static constexpr size_t TYPE_BITS = 2;
    FIRST_BIT_FIELD(BitField, RoundingType, RoundingType, ROUNDING_TYPE_BITS)
    NEXT_BIT_FIELD(BitField, Type, TypeOption, TYPE_BITS, RoundingType)

    DECL_VISIT_OBJECT_FOR_JS_OBJECT(JSObject, LOCALE_OFFSET, BIT_FIELD_OFFSET)
    DECL_DUMP()

    icu::number::LocalizedNumberFormatter *GetIcuNumberFormatter() const;

    static void SetIcuNumberFormatter(JSThread *thread, const JSHandle<JSPluralRules> &pluralRules,
        const icu::number::LocalizedNumberFormatter &icuNF, const NativePointerCallback &callback);

    static void FreeIcuNumberFormatter(void *env, void *pointer, void* hint = nullptr);

    icu::PluralRules *GetIcuPluralRules() const;

    static void SetIcuPluralRules(JSThread *thread, const JSHandle<JSPluralRules> &pluralRules,
        const icu::PluralRules &icuPR, const NativePointerCallback &callback);

    static void FreeIcuPluralRules(void *env, void *pointer, void* hint = nullptr);

    static JSHandle<TaggedArray> BuildLocaleSet(JSThread *thread, const std::set<std::string> &icuAvailableLocales);

    static JSHandle<TaggedArray> GetAvailableLocales(JSThread *thread);

    // 15.1.1 InitializePluralRules ( pluralRules, locales, options )
    static JSHandle<JSPluralRules> InitializePluralRules(JSThread *thread, const JSHandle<JSPluralRules> &pluralRules,
                                                         const JSHandle<JSTaggedValue> &locales,
                                                         const JSHandle<JSTaggedValue> &options);

    // 15.1.4 ResolvePlural ( pluralRules, n )
    static JSHandle<EcmaString> ResolvePlural(JSThread *thread, const JSHandle<JSPluralRules> &pluralRules, double n);

    static void ResolvedOptions(JSThread *thread, const JSHandle<JSPluralRules> &pluralRules,
                                const JSHandle<JSObject> &options);
};
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_JS_PLURAL_RULES_H