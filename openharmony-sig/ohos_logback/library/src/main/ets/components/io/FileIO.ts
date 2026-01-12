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

import fileio from '@ohos.file.fs'
import { Context } from '@ohos.abilityAccessCtrl'

export class FileIO {
    private filePath: string = ''
    private fd: number = 0
    private context: Context

    constructor(context: Context) {
        this.context = context;
    }

    /**
     * data/app/el2/100/base/cn.openharmony.logback/haps/entry/files
     */
    create(file: string) {
        if (!!!this.context)
            return;
        this.filePath = this.context.filesDir + '/' + file
        let fl= fileio.openSync(this.filePath,fileio.OpenMode.CREATE|fileio.OpenMode.READ_WRITE)
        this.fd =fl.fd;
    }

    write(msg: string) {
        fileio.writeSync(this.fd, `${msg}\n`);
    }

    rename(newPath: string): Promise<boolean> {
        let self = this
        return new Promise<boolean>(function (resolve) {
            if(this.context)
            fileio.rename(self.filePath, this.context.filesDir + '/' + newPath).then(function () {
                resolve(true)
            }).catch(function () {
                resolve(false)
            });
            else resolve(false)
        });
    }
}