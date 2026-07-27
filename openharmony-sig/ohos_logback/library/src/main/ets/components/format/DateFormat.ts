/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

export class DateFormat {

    public static format(date: Date, str: string): string{
        let obj = {
            yyyy: date.getFullYear(),
            yy: ('' + date.getFullYear()).slice(-2),
            M: date.getMonth() + 1,
            MM: ('0' + (date.getMonth() + 1)).slice(-2),
            d: date.getDate(),
            dd: ('0' + date.getDate()).slice(-2),
            H: date.getHours(),
            HH: ('0' + date.getHours()).slice(-2),
            h: date.getHours() % 12,
            hh: ('0' + date.getHours() % 12).slice(-2),
            m: date.getMinutes(),
            mm: ('0' + date.getMinutes()).slice(-2),
            s: date.getSeconds(),
            ss: ('0' + date.getSeconds()).slice(-2),
            w: ['日', '一', '二', '三', '四', '五', '六'][date.getDay()]
        };
        return str.replace(/([a-z]+)/ig, function ($1) {
            return obj[$1]
        });
    }
}