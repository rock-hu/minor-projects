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

export class Level {
    public static FATAL_INT = 50000;
    public static ERROR_INT = 40000;
    public static WARN_INT = 30000;
    public static INFO_INT = 20000;
    public static DEBUG_INT = 10000;
    public static TRACE_INT = 5000;
    public static FATAL = new Level(Level.FATAL_INT, "FATAL");
    public static ERROR = new Level(Level.ERROR_INT, "ERROR");
    public static WARN = new Level(Level.WARN_INT, "WARN");
    public static INFO = new Level(Level.INFO_INT, "INFO");
    public static DEBUG = new Level(Level.DEBUG_INT, "DEBUG");
    public static TRACE = new Level(Level.TRACE_INT, "TRACE");
    private levelInt: number
    private levelStr: string

    private constructor(levelInt: number, levelStr: string) {
        this.levelInt = levelInt;
        this.levelStr = levelStr;
    }

    getLevelInt(): number{
        return this.levelInt
    }

    getLevelStr(): string{
        return this.levelStr
    }
}
