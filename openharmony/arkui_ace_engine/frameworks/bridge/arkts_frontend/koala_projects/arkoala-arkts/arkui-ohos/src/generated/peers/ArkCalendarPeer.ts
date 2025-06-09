/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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


// WARNING! THIS FILE IS AUTO-GENERATED, DO NOT MAKE CHANGES, THEY WILL BE LOST ON NEXT GENERATION!

import { int32, float32 } from "@koalaui/common"
import { nullptr, KPointer, KInt, KBoolean, KStringPtr } from "@koalaui/interop"
import { runtimeType, RuntimeType } from "@koalaui/interop"
import { Serializer } from "./Serializer"
import { ComponentBase } from "../../ComponentBase"
import { PeerNode } from "../../PeerNode"
import { ArkUIGeneratedNativeModule } from "#components"
import { Axis } from "./../../component/enums"
import { CurrentDayStyle, NonCurrentDayStyle, TodayStyle, WeekStyle, WorkStateStyle, Callback_CalendarSelectedDate_Void, CalendarSelectedDate, Callback_CalendarRequestedData_Void, CalendarRequestedData, Type_CalendarInterface_value, Literal_Number_day_month_year, MonthData, CalendarController, CalendarAttribute } from "./../../component/calendar"
import { CallbackKind } from "./CallbackKind"
import { CallbackTransformer } from "./CallbackTransformer"
import { TypeChecker } from "#components"
import { MaterializedBase, toPeerPtr, wrapCallback } from "@koalaui/interop"
export class ArkCalendarPeer extends PeerNode {
    protected constructor(peerPtr: KPointer, id: int32, name: string = "", flags: int32 = 0) {
        super(peerPtr, id, name, flags)
    }
    public static create(component?: ComponentBase, flags: int32 = 0): ArkCalendarPeer {
        const peerId  = PeerNode.nextId()
        const _peerPtr  = ArkUIGeneratedNativeModule._Calendar_construct(peerId, flags)
        const _peer  = new ArkCalendarPeer(_peerPtr, peerId, "Calendar", flags)
        component?.setPeer(_peer)
        return _peer
    }
    setCalendarOptionsAttribute(value: Type_CalendarInterface_value): void {
        const thisSerializer : Serializer = Serializer.hold()
        const value_date  = value.date
        const value_date_year  = value_date.year
        thisSerializer.writeNumber(value_date_year)
        const value_date_month  = value_date.month
        thisSerializer.writeNumber(value_date_month)
        const value_date_day  = value_date.day
        thisSerializer.writeNumber(value_date_day)
        const value_currentData  = value.currentData
        thisSerializer.writeMonthData(value_currentData)
        const value_preData  = value.preData
        thisSerializer.writeMonthData(value_preData)
        const value_nextData  = value.nextData
        thisSerializer.writeMonthData(value_nextData)
        const value_controller  = value.controller
        let value_controller_type : int32 = RuntimeType.UNDEFINED
        value_controller_type = runtimeType(value_controller)
        thisSerializer.writeInt8(value_controller_type as int32)
        if ((RuntimeType.UNDEFINED) != (value_controller_type)) {
            const value_controller_value  = value_controller!
            thisSerializer.writeCalendarController(value_controller_value)
        }
        ArkUIGeneratedNativeModule._CalendarInterface_setCalendarOptions(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    showLunarAttribute(value: boolean): void {
        ArkUIGeneratedNativeModule._CalendarAttribute_showLunar(this.peer.ptr, value ? 1 : 0)
    }
    showHolidayAttribute(value: boolean): void {
        ArkUIGeneratedNativeModule._CalendarAttribute_showHoliday(this.peer.ptr, value ? 1 : 0)
    }
    needSlideAttribute(value: boolean): void {
        ArkUIGeneratedNativeModule._CalendarAttribute_needSlide(this.peer.ptr, value ? 1 : 0)
    }
    startOfWeekAttribute(value: number): void {
        ArkUIGeneratedNativeModule._CalendarAttribute_startOfWeek(this.peer.ptr, value)
    }
    offDaysAttribute(value: number): void {
        ArkUIGeneratedNativeModule._CalendarAttribute_offDays(this.peer.ptr, value)
    }
    directionAttribute(value: Axis): void {
        ArkUIGeneratedNativeModule._CalendarAttribute_direction(this.peer.ptr, value.valueOf())
    }
    currentDayStyleAttribute(value: CurrentDayStyle): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeCurrentDayStyle(value)
        ArkUIGeneratedNativeModule._CalendarAttribute_currentDayStyle(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    nonCurrentDayStyleAttribute(value: NonCurrentDayStyle): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeNonCurrentDayStyle(value)
        ArkUIGeneratedNativeModule._CalendarAttribute_nonCurrentDayStyle(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    todayStyleAttribute(value: TodayStyle): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeTodayStyle(value)
        ArkUIGeneratedNativeModule._CalendarAttribute_todayStyle(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    weekStyleAttribute(value: WeekStyle): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeWeekStyle(value)
        ArkUIGeneratedNativeModule._CalendarAttribute_weekStyle(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    workStateStyleAttribute(value: WorkStateStyle): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.writeWorkStateStyle(value)
        ArkUIGeneratedNativeModule._CalendarAttribute_workStateStyle(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    onSelectChangeAttribute(value: ((event: CalendarSelectedDate) => void)): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(value)
        ArkUIGeneratedNativeModule._CalendarAttribute_onSelectChange(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
    onRequestDataAttribute(value: ((event: CalendarRequestedData) => void)): void {
        const thisSerializer : Serializer = Serializer.hold()
        thisSerializer.holdAndWriteCallback(value)
        ArkUIGeneratedNativeModule._CalendarAttribute_onRequestData(this.peer.ptr, thisSerializer.asBuffer(), thisSerializer.length())
        thisSerializer.release()
    }
}
export interface ArkCalendarAttributes {
    showLunar?: boolean
    showHoliday?: boolean
    needSlide?: boolean
    startOfWeek?: number
    offDays?: number
    direction?: Axis
    currentDayStyle?: CurrentDayStyle
    nonCurrentDayStyle?: NonCurrentDayStyle
    todayStyle?: TodayStyle
    weekStyle?: WeekStyle
    workStateStyle?: WorkStateStyle
    onSelectChange?: ((event: CalendarSelectedDate) => void)
    onRequestData?: ((event: CalendarRequestedData) => void)
}
