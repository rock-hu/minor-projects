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

import {Base64} from '@ohos/base64'
import prompt from '@ohos.promptAction';
import { GlobalContext } from '../entryability/GlobalContext';
import { Context } from '@ohos.abilityAccessCtrl';

@Entry
@Component
struct EncodeByteTest{
  private test:number[] = [1,2,3,4,5,6,7,8,9,10,11,12,13]
  @State private filePath:string = "";

  build(){
    Column(){
      Text('data:'+this.test).fontSize('20fp').width('100%')
        .textAlign(TextAlign.Start)
      Text('encode:'+Base64.encode(new Uint8Array(this.test))).fontSize('20fp').width('100%').margin({top:10})
        .textAlign(TextAlign.Start)
      Text('decode:'+Base64.decode(Base64.encode(new Uint8Array(this.test)))).fontSize('20fp').width('100%').margin({top:10})
        .textAlign(TextAlign.Start)
      Text('encodeBytes:'+Base64.encodeBytes(new Uint8Array(this.test))).fontSize('20fp').width('100%').margin({top:10})
        .textAlign(TextAlign.Start)
      Text('decodeBytes:'+Base64.decode(Base64.encodeBytes(new Uint8Array(this.test)))).fontSize('20fp').width('100%').margin({top:10})
        .textAlign(TextAlign.Start)

      Text('encodeBytesToBytes:'+Base64.encodeBytesToBytes(new Uint8Array(this.test))).fontSize('20fp').width('100%').margin({top:10})
        .textAlign(TextAlign.Start)
      Text('decodeBytes:'+Base64.decode(Base64.encodeBytesToBytes(new Uint8Array(this.test)))).fontSize('20fp').width('100%').margin({top:10})
        .textAlign(TextAlign.Start)
      Text('encodeToFile:').fontSize('20fp').width(200).height(60)
        .textAlign(TextAlign.Center)
        .fontColor('#ffffff')
        .backgroundColor('#ff017fef').margin({top:10})
        .onClick(()=>{
          let context: Context = GlobalContext.getContext().getObject("context") as Context;
          this.filePath = context.filesDir.concat('/test.txt');
          Base64.encodeToFile(this.filePath, new Uint8Array(this.test))
        })
      Text('decodeFromFile:').fontSize('20fp').width(200).height(60)
        .textAlign(TextAlign.Center)
        .fontColor('#ffffff')
        .backgroundColor('#ff017fef').margin({top:10})
        .onClick(()=>{
          if (this.filePath !== "") {
            let result = Base64.decodeFromFile(this.filePath)
            console.info("base64 decodeFromFile:" + result.toString())
            prompt.showToast({
              message: result.toString(),
              duration:3000
            })
          } else {
            prompt.showToast({
              message: "找不到文件",
              duration:3000
            })
          }
        })
    }
  }
}