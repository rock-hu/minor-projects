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

#include "ecmascript/js_generator_object.h"
#include "ecmascript/generator_helper.h"
#include "ecmascript/js_iterator.h"
#include "ecmascript/js_object-inl.h"
#include "ecmascript/js_tagged_value-inl.h"

namespace panda::ecmascript {
JSGeneratorState JSGeneratorObject::GeneratorValidate(JSThread *thread, const JSHandle<JSTaggedValue> &obj)
{
    // 1.Perform ? RequireInternalSlot(generator, [[GeneratorState]]).
    // 2.Assert: generator also has a [[GeneratorContext]] internal slot.
    if (!obj->IsECMAObject()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "Is not object",
            JSGeneratorState::UNDEFINED);
    }
    JSHandle<JSObject> toObj = JSTaggedValue::ToObject(thread, obj);
    if (!toObj->IsGeneratorObject()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "Is not generator object", JSGeneratorState::UNDEFINED);
    }

    // 3.Let state be generator.[[GeneratorState]].
    JSHandle<JSGeneratorObject> generator(thread, JSGeneratorObject::Cast(*(toObj)));
    JSGeneratorState state = generator->GetGeneratorState();
    // 4.If state is executing, throw a TypeError exception.
    if (state == JSGeneratorState::EXECUTING) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "State is executing", JSGeneratorState::UNDEFINED);
    }
    // 5.Return state.
    return state;
}

JSHandle<JSObject> JSGeneratorObject::GeneratorResume(JSThread *thread, const JSHandle<JSGeneratorObject> &generator,
                                                      JSTaggedValue value)
{
    // 1.Let state be ? GeneratorValidate(generator).
    JSHandle<JSTaggedValue> gen(thread, generator.GetTaggedValue());
    JSGeneratorState state = GeneratorValidate(thread, gen);
    RETURN_HANDLE_IF_ABRUPT_COMPLETION(JSObject, thread);

    // 2.If state is completed, return CreateIterResultObject(undefined, true).
    if (state == JSGeneratorState::COMPLETED) {
        JSHandle<JSTaggedValue> valueHandle(thread, JSTaggedValue::Undefined());
        return JSIterator::CreateIterResultObject(thread, valueHandle, true);
    }

    // 3.Assert: state is either suspendedStart or suspendedYield.
    ASSERT_PRINT(state == JSGeneratorState::SUSPENDED_START ||
                     state == JSGeneratorState::SUSPENDED_YIELD,
                 "state is neither suspendedStart nor suspendedYield");

    // 4.Let genContext be generator.[[GeneratorContext]].
    JSHandle<GeneratorContext> genContext(thread, generator->GetGeneratorContext());

    // 5.Let methodContext be the running execution context.
    // 6.Suspend methodContext.

    // 7.Set generator.[[GeneratorState]] to executing.
    generator->SetGeneratorState(JSGeneratorState::EXECUTING);

    // 8.Push genContext onto the execution context stack; genContext is now the running execution context.
    // 9.Resume the suspended evaluation of genContext using NormalCompletion(value) as the result of the operation
    //   that suspended it. Let result be the value returned by the resumed computation.
    // 10.Assert: When we return here, genContext has already been removed from the execution context stack and
    //    methodContext is the currently running execution context.
    // 11.Return Completion(result).
    JSHandle<JSObject> result = GeneratorHelper::Next(thread, genContext, value);
    return result;
}

JSHandle<JSObject> JSGeneratorObject::GeneratorResumeAbrupt(JSThread *thread,
                                                            const JSHandle<JSGeneratorObject> &generator,
                                                            const JSHandle<CompletionRecord> &abruptCompletion)
{
    // 1.Let state be ? GeneratorValidate(generator).
    JSHandle<JSTaggedValue> gen(thread, generator.GetTaggedValue());
    JSGeneratorState state = GeneratorValidate(thread, gen);
    RETURN_HANDLE_IF_ABRUPT_COMPLETION(JSObject, thread);

    // 2.If state is suspendedStart, then
    //     a.Set generator.[[GeneratorState]] to completed.
    //     b.Once a generator enters the completed state it never leaves it and its associated execution context is
    //       never resumed. Any execution state associated with generator can be discarded at this point.
    //     c.Set state to completed.
    if (state == JSGeneratorState::SUSPENDED_START) {
        state = JSGeneratorState::COMPLETED;
        generator->SetGeneratorState(state);
    }

    // 3.If state is completed, then
    //     a.If abruptCompletion.[[Type]] is return, then
    //         i.Return CreateIterResultObject(abruptCompletion.[[Value]], true).
    //     b.Return Completion(abruptCompletion).
    if (state == JSGeneratorState::COMPLETED) {
        JSHandle<JSTaggedValue> valueHandle(thread, abruptCompletion->GetValue());
        JSHandle<JSObject> result = JSIterator::CreateIterResultObject(thread, valueHandle, true);
        if (abruptCompletion->GetType() == CompletionRecordType::RETURN) {
            return result;
        }
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, valueHandle.GetTaggedValue(), result);
    }

    // 4.Assert: state is suspendedYield.
    ASSERT_PRINT(state == JSGeneratorState::SUSPENDED_YIELD, "state is not suspendedYield");

    // 5.Let genContext be generator.[[GeneratorContext]].
    JSHandle<GeneratorContext> genContext(thread, generator->GetGeneratorContext());

    // 6.Let methodContext be the running execution context.
    // 7.Suspend methodContext.

    // 8.Set generator.[[GeneratorState]] to executing.
    generator->SetGeneratorState(JSGeneratorState::EXECUTING);

    // 9.Push genContext onto the execution context stack; genContext is now the running execution context.
    // 10.Resume the suspended evaluation of genContext using abruptCompletion as the result of the operation that
    //    suspended it. Let result be the completion record returned by the resumed computation.
    // 11.Assert: When we return here, genContext has already been removed from the execution context stack and
    //    methodContext is the currently running execution context.
    // 12.Return Completion(result).
    JSHandle<JSObject> result;
    if (abruptCompletion->GetType() == CompletionRecordType::RETURN) {
        result = GeneratorHelper::Return(thread, genContext, abruptCompletion->GetValue());
    } else {
        result = GeneratorHelper::Throw(thread, genContext, abruptCompletion->GetValue());
    }
    return result;
}
}  // namespace panda::ecmascript
