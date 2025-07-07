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

#include "builtins_date_time_format.h"

#include "ecmascript/intl/locale_helper.h"
#include "ecmascript/global_env.h"
#include "ecmascript/js_date.h"
#include "ecmascript/js_date_time_format.h"
#include "ecmascript/js_function.h"
#include "ecmascript/js_intl.h"

namespace panda::ecmascript::builtins {
// 13.2.1 Intl.DateTimeFormat ( [ locales [ , options ] ] )
JSTaggedValue BuiltinsDateTimeFormat::DateTimeFormatConstructor(EcmaRuntimeCallInfo *argv)
{
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, DateTimeFormat, Constructor);
    [[maybe_unused]] EcmaHandleScope scope(thread);
    EcmaVM *ecmaVm = thread->GetEcmaVM();
    JSHandle<GlobalEnv> env = ecmaVm->GetGlobalEnv();
    ObjectFactory *factory = ecmaVm->GetFactory();

    // 1. If NewTarget is undefined, let newTarget be the active function object, else let newTarget be NewTarget.
    JSHandle<JSTaggedValue> constructor = GetConstructor(argv);
    JSHandle<JSTaggedValue> newTarget = GetNewTarget(argv);
    if (newTarget->IsUndefined()) {
        newTarget = constructor;
    }

    // 2. Let dateTimeFormat be ? OrdinaryCreateFromConstructor(newTarget, "%DateTimeFormatPrototype%", «
    //    [[InitializedDateTimeFormat]], [[Locale]], [[Calendar]], [[NumberingSystem]], [[TimeZone]], [[Weekday]],
    //    [[Era]], [[Year]], [[Month]], [[Day]], [[Hour]], [[Minute]], [[Second]], [[TimeZoneName]], [[HourCycle]],
    //    [[Pattern]], [[BoundFormat]] »).
    JSHandle<JSObject> newObject = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(constructor), newTarget);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    JSHandle<JSDateTimeFormat> dateTimeFormat = JSHandle<JSDateTimeFormat>::Cast(newObject);

    // 3. Perform ? InitializeDateTimeFormat(dateTimeFormat, locales, options).
    JSHandle<JSTaggedValue> locales = GetCallArg(argv, 0);
    JSHandle<JSTaggedValue> options = GetCallArg(argv, 1);
    JSHandle<JSDateTimeFormat> dtf =
        JSDateTimeFormat::InitializeDateTimeFormat(thread, dateTimeFormat, locales, options);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 4. Let this be the this value.
    JSHandle<JSTaggedValue> thisValue = GetThis(argv);

    // 5. If NewTarget is undefined and Type(this) is Object and ? InstanceofOperator(this, %DateTimeFormat%) is true,
    //    then
    //    a. Perform ? DefinePropertyOrThrow(this, %Intl%.[[FallbackSymbol]], PropertyDescriptor{ [[Value]]:
    //       dateTimeFormat, [[Writable]]: false, [[Enumerable]]: false, [[Configurable]]: false }).
    //    b. Return this.
    if (GetNewTarget(argv)->IsUndefined() && thisValue->IsJSObject()) {
        bool isInstanceOf = JSFunction::OrdinaryHasInstance(thread, env->GetDateTimeFormatFunction(), thisValue);
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
        if (isInstanceOf) {
            PropertyDescriptor descriptor(thread, JSHandle<JSTaggedValue>::Cast(dtf), false, false, false);
            JSHandle<JSTaggedValue> key(thread,
                JSHandle<JSIntl>::Cast(env->GetIntlFunction())->GetFallbackSymbol(thread));
            JSTaggedValue::DefinePropertyOrThrow(thread, thisValue, key, descriptor);
            RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
            return thisValue.GetTaggedValue();
        }
    }

    // 6. Return dateTimeFormat.
    return dtf.GetTaggedValue();
}

// 13.3.2 Intl.DateTimeFormat.supportedLocalesOf ( locales [ , options ] )
JSTaggedValue BuiltinsDateTimeFormat::SupportedLocalesOf(EcmaRuntimeCallInfo *argv)
{
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, DateTimeFormat, SupportedLocalesOf);
    [[maybe_unused]] EcmaHandleScope scope(thread);
    // 1. Let availableLocales be %DateTimeFormat%.[[AvailableLocales]].
    JSHandle<TaggedArray> availableLocales = JSDateTimeFormat::GainAvailableLocales(thread);

    // 2. Let requestedLocales be ? CanonicalizeLocaleList(locales).
    JSHandle<JSTaggedValue> locales = GetCallArg(argv, 0);
    JSHandle<TaggedArray> requestedLocales = intl::LocaleHelper::CanonicalizeLocaleList(thread, locales);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 3. Return ? SupportedLocales(availableLocales, requestedLocales, options).
    JSHandle<JSTaggedValue> options = GetCallArg(argv, 1);
    JSHandle<JSArray> result = JSLocale::SupportedLocales(thread, availableLocales, requestedLocales, options);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    return result.GetTaggedValue();
}

// 13.4.3 get Intl.DateTimeFormat.prototype.format
JSTaggedValue BuiltinsDateTimeFormat::Format(EcmaRuntimeCallInfo *argv)
{
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, DateTimeFormat, Format);
    [[maybe_unused]] EcmaHandleScope scope(thread);

    // 1. Let dtf be this value.
    JSHandle<JSTaggedValue> thisValue = GetThis(argv);

    // 2. If Type(dtf) is not Object, throw a TypeError exception.
    if (!thisValue->IsJSObject()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "dtf is not object", JSTaggedValue::Exception());
    }

    // 3. Let dtf be ? UnwrapDateTimeFormat(dtf).
    JSHandle<JSTaggedValue> dtfValue = JSDateTimeFormat::UnwrapDateTimeFormat(thread, thisValue);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    if (dtfValue->IsUndefined()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "dtfValue is not object", JSTaggedValue::Exception());
    }

    // 4. If dtf.[[BoundFormat]] is undefined, then
    //    a. Let F be a new built-in function object as defined in DateTime Format Functions (13.1.5).
    //    b. Set F.[[DateTimeFormat]] to dtf.
    //    c. Set dtf.[[BoundFormat]] to F.
    // 5. Return dtf.[[BoundFormat]].
    JSHandle<JSDateTimeFormat> dtf = JSHandle<JSDateTimeFormat>::Cast(dtfValue);
    JSHandle<JSTaggedValue> boundFormat(thread, dtf->GetBoundFormat(thread));
    if (boundFormat->IsUndefined()) {
        ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
        JSHandle<JSIntlBoundFunction> intlBoundFunc = factory->NewJSIntlBoundFunction(
            MethodIndex::BUILTINS_DATE_TIME_FORMAT_ANONYMOUS_DATE_TIME_FORMAT);
        intlBoundFunc->SetDateTimeFormat(thread, dtf);
        dtf->SetBoundFormat(thread, intlBoundFunc);
    }
    return dtf->GetBoundFormat(thread);
}

// 13.1.5 DateTime Format Functions
JSTaggedValue BuiltinsDateTimeFormat::AnonymousDateTimeFormat(EcmaRuntimeCallInfo *argv)
{
    // A DateTime format function is an anonymous built-in function that has a [[DateTimeFormat]] internal slot.
    // When a DateTime format function F is called with optional argument date, the following steps are taken:
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, DateTimeFormat, AnonymousDateTimeFormat);
    [[maybe_unused]] EcmaHandleScope scope(thread);
    JSHandle<JSIntlBoundFunction> intlBoundFunc = JSHandle<JSIntlBoundFunction>::Cast(GetConstructor(argv));

    // 1. Let dtf be F.[[DateTimeFormat]].
    JSHandle<JSTaggedValue> dtf(thread, intlBoundFunc->GetDateTimeFormat(thread));

    // 2. Assert: Type(dtf) is Object and dtf has an [[InitializedDateTimeFormat]] internal slot.
    ASSERT_PRINT(dtf->IsJSObject() && dtf->IsJSDateTimeFormat(), "dtf is not object or JSDateTimeFormat");

    // 3. If date is not provided or is undefined, then
    //    a. Let x be Call(%Date_now%, undefined).
    // 4. Else,
    //    a. Let x be ? ToNumber(date).
    JSHandle<JSTaggedValue> date = GetCallArg(argv, 0);
    double x = 0.0;
    if (date->IsUndefined()) {
        x = JSDate::Now().GetNumber();
    } else {
        JSTaggedNumber xNumber = JSTaggedValue::ToNumber(thread, date);
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
        x = xNumber.GetNumber();
    }

    // 5. Return ? FormatDateTime(dtf, x).
    JSHandle<EcmaString> result = JSDateTimeFormat::FormatDateTime(thread, JSHandle<JSDateTimeFormat>::Cast(dtf), x);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    return result.GetTaggedValue();
}

// 13.4.4 Intl.DateTimeFormat.prototype.formatToParts ( date )
JSTaggedValue BuiltinsDateTimeFormat::FormatToParts(EcmaRuntimeCallInfo *argv)
{
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, DateTimeFormat, FormatToParts);
    [[maybe_unused]] EcmaHandleScope scope(thread);
    // 1. Let dtf be this value.
    JSHandle<JSTaggedValue> dtf = GetThis(argv);
    // 2. Perform ? RequireInternalSlot(dtf, [[InitializedDateTimeFormat]]).
    if (!dtf->IsJSDateTimeFormat()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "is not JSDateTimeFormat", JSTaggedValue::Exception());
    }

    // 3. If date is undefined, then
    //    a. Let x be Call(%Date_now%, undefined).
    // 4. Else,
    //    a. Let x be ? ToNumber(date).
    JSHandle<JSTaggedValue> date = GetCallArg(argv, 0);
    double x = 0.0;
    if (date->IsUndefined()) {
        x = JSDate::Now().GetNumber();
    } else {
        JSTaggedNumber xNumber = JSTaggedValue::ToNumber(thread, date);
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
        x = xNumber.GetNumber();
    }

    double xValue = JSDate::TimeClip(x);
    if (std::isnan(xValue)) {
        THROW_RANGE_ERROR_AND_RETURN(thread, "Invalid time value", JSTaggedValue::Exception());
    }

    // 5. Return ? FormatDateTimeToParts(dtf, x).
    JSHandle<JSArray> result =
        JSDateTimeFormat::FormatDateTimeToParts(thread, JSHandle<JSDateTimeFormat>::Cast(dtf), xValue);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    return result.GetTaggedValue();
}

// 13.4.5 Intl.DateTimeFormat.prototype.resolvedOptions ()
JSTaggedValue BuiltinsDateTimeFormat::ResolvedOptions(EcmaRuntimeCallInfo *argv)
{
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, DateTimeFormat, ResolvedOptions);
    [[maybe_unused]] EcmaHandleScope scope(thread);
    // 1. Let dtf be this value.
    JSHandle<JSTaggedValue> thisValue = GetThis(argv);
    // 2. If Type(dtf) is not Object, throw a TypeError exception.
    if (!thisValue->IsJSObject()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "this is not object", JSTaggedValue::Exception());
    }
    // 3. Let dtf be ? UnwrapDateTimeFormat(dtf).
    thisValue = JSDateTimeFormat::UnwrapDateTimeFormat(thread, thisValue);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // Perform ? RequireInternalSlot(dtf, [[InitializedDateTimeFormat]]).
    if (!thisValue->IsJSDateTimeFormat()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "dtf is not JSDateTimeFormat", JSTaggedValue::Exception());
    }

    auto ecmaVm = thread->GetEcmaVM();
    JSHandle<GlobalEnv> env = ecmaVm->GetGlobalEnv();
    ObjectFactory *factory = ecmaVm->GetFactory();
    JSHandle<JSFunction> ctor(env->GetObjectFunction());
    JSHandle<JSObject> options(factory->NewJSObjectByConstructor(ctor));
    JSDateTimeFormat::ResolvedOptions(thread, JSHandle<JSDateTimeFormat>::Cast(thisValue), options);
    // 6. Return options.
    return options.GetTaggedValue();
}

// Intl.DateTimeFormat.prototype.formatRange
JSTaggedValue BuiltinsDateTimeFormat::FormatRange(EcmaRuntimeCallInfo *argv)
{
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, DateTimeFormat, FormatRange);
    [[maybe_unused]] EcmaHandleScope scope(thread);

    // 1. Let dtf be this value.
    JSHandle<JSTaggedValue> thisValue = GetThis(argv);
    // 2. If Type(dtf) is not Object, throw a TypeError exception.
    if (!thisValue->IsJSObject()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "this is not object", JSTaggedValue::Exception());
    }

    // 3. If dtf does not have an [[InitializedDateTimeFormat]] internal slot, throw a TypeError exception.
    if (!thisValue->IsJSDateTimeFormat()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "this is not JSDateTimeFormat", JSTaggedValue::Exception());
    }

    // 4. If startDate is undefined or endDate is undefined, throw a TypeError exception.
    JSHandle<JSTaggedValue> startDate = GetCallArg(argv, 0);
    JSHandle<JSTaggedValue> endDate = GetCallArg(argv, 1);
    if (startDate->IsUndefined() || endDate->IsUndefined()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "startDate or endDate is undefined", JSTaggedValue::Exception());
    }

    // 5. Let x be ? ToNumber(startDate).
    JSTaggedNumber valueX = JSTaggedValue::ToNumber(thread, startDate);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    double x = valueX.GetNumber();

    // 6. Let y be ? ToNumber(endDate).
    JSTaggedNumber valueY = JSTaggedValue::ToNumber(thread, endDate);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    double y = valueY.GetNumber();

    // 8. Return ? FormatDateTimeRange(dtf, x, y)
    JSHandle<JSDateTimeFormat> dtf = JSHandle<JSDateTimeFormat>::Cast(thisValue);
    JSHandle<EcmaString> result = JSDateTimeFormat::NormDateTimeRange(thread, dtf, x, y);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    return result.GetTaggedValue();
}

// Intl.DateTimeFormat.prototype.formatRangeToParts
JSTaggedValue BuiltinsDateTimeFormat::FormatRangeToParts(EcmaRuntimeCallInfo *argv)
{
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, DateTimeFormat, FormatRangeToParts);
    [[maybe_unused]] EcmaHandleScope scope(thread);
    // 1. Let dtf be this value.
    JSHandle<JSTaggedValue> thisValue = GetThis(argv);
    // 2. If Type(dtf) is not Object, throw a TypeError exception.
    if (!thisValue->IsJSObject()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "this is not object", JSTaggedValue::Exception());
    }

    // 3. If dtf does not have an [[InitializedDateTimeFormat]] internal slot,
    //    throw a TypeError exception.
    if (!thisValue->IsJSDateTimeFormat()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "this is not JSDateTimeFormat", JSTaggedValue::Exception());
    }

    // 4. If startDate is undefined or endDate is undefined, throw a TypeError exception.
    JSHandle<JSTaggedValue> startDate = GetCallArg(argv, 0);
    JSHandle<JSTaggedValue> endDate = GetCallArg(argv, 1);
    if (startDate->IsUndefined() || endDate->IsUndefined()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "startDate or endDate is undefined", JSTaggedValue::Exception());
    }

    // 5. Let x be ? ToNumber(startDate).
    JSTaggedNumber valueX = JSTaggedValue::ToNumber(thread, startDate);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    double x = valueX.GetNumber();

    // 6. Let y be ? ToNumber(endDate).
    JSTaggedNumber valueY = JSTaggedValue::ToNumber(thread, endDate);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    double y = valueY.GetNumber();

    // 8. Return ? FormatDateTimeRangeToParts(dtf, x, y)
    JSHandle<JSDateTimeFormat> dtf = JSHandle<JSDateTimeFormat>::Cast(thisValue);
    JSHandle<JSArray> result = JSDateTimeFormat::NormDateTimeRangeToParts(thread, dtf, x, y);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    return result.GetTaggedValue();
}
}  // namespace panda::ecmascript::builtins