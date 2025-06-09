/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import { ILogger } from "./Logger";

export const logMessages: string[] = [];

export class SilentLogger implements ILogger {
  constructor(){
    logMessages.length=0;
  }
  private log(level: string,message: string): void{
    void this;
    logMessages.push(`[${level}] ${message}`);
  }
  doTrace(message: string): void {
    this.log("TRACE",message);
  }
  doDebug(message: string): void {
    this.log("DEBUG",message);
  }
  doInfo(message: string): void {
    this.log("INFO",message);
  }
  doWarn(message: string): void {
    this.log("WARN",message);
  }
  doError(message: string): void {
    this.log("ERROR",message);
  }

}
