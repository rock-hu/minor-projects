/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
import { Scope } from "./scope/Scope";
import { ArkFile } from "@ArkAnalyzer/src";
import { transfer2UnixPath } from "@ArkAnalyzer/src/utils/pathTransfer";

export class CheckerStorage {
    private static instance: CheckerStorage;
    private scopeMap: Map<string, Scope> = new Map();
    private detectFiles2UnixMap: Map<string, string> = new Map();

    public static getInstance(): CheckerStorage {
        if (!CheckerStorage.instance) {
            CheckerStorage.instance = new CheckerStorage();
        }
        return CheckerStorage.instance;
    }

    public getScope(filePath: string): Scope | undefined {
        return this.scopeMap.get(filePath);
    }

    public setScopeMap(scopeMap: Map<string, Scope>) {
        this.scopeMap = scopeMap;
    }

    public setDetectFiles2UnixMap(detectFiles2UnixMap: Map<string, string>) {
        this.detectFiles2UnixMap = detectFiles2UnixMap;
    }

    public getDetectFiles2UnixMap(originFilePath: string): string | undefined {
        return this.detectFiles2UnixMap.get(originFilePath);
    }

    public getRealFiePath(arkFile: ArkFile): string {
        const filePath = arkFile.getFilePath();
        return this.getDetectFiles2UnixMap(transfer2UnixPath(filePath)) ?? filePath;
    }
}