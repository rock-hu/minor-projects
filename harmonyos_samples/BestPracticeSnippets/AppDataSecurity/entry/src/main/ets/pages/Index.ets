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
import { common, contextConstant } from '@kit.AbilityKit';
import { fileIo } from '@kit.CoreFileKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { buffer } from '@kit.ArkTS';
import { huks } from '@kit.UniversalKeystoreKit';

@Entry
@Component
struct Index {
  @State message: string = '';

  build() {
    Column() {
      Column({ space: 12 }) {
        Text($r('app.string.file_path'))
          .fontSize(20)
          .fontWeight(700)
        Text(this.message)
          .fontSize(16)
      }
      .width('100%')
      .alignItems(HorizontalAlign.Start)
      Column({ space: 12 }) {
        Button($r('app.string.get_file_path'))
          .width('100%')
          .onClick(() => {
            this.getEl2Path();
          })
        Button($r('app.string.get_el1_file_path'))
          .width('100%')
          .onClick(() => {
            this.getEl1Path();
          })
        Button($r('app.string.encrypted'))
          .width('100%')
          .onClick(async () => {
            await GenerateAesKey();
            await EncryptData();
          })
        Button($r('app.string.decrypted'))
          .width('100%')
          .onClick(() => {
            DecryptData();
          })
      }
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.SpaceBetween)
    .padding(16)
  }

  getEl2Path() {
    let context = getContext(this) as common.UIAbilityContext;
    context.area = contextConstant.AreaMode.EL2;
    let filePath = context.filesDir + '/health_data.txt';
    this.message = filePath;
  }

  getEl1Path() {
    let context = getContext(this) as common.UIAbilityContext;
    context.area = contextConstant.AreaMode.EL1;
    let filePath = context.filesDir + '/health_data.txt';
    this.message = filePath;
  }
}

let aesKeyAlias = 'test_aesKeyAlias';
let handle: number;
let plainText = '123456';
let IV = '001122334455';
let cipherData: Uint8Array;

function writeFile(filePath: string, data: string) {
  let file = fileIo.openSync(filePath, fileIo.OpenMode.READ_WRITE | fileIo.OpenMode.CREATE);
  let writeLen = fileIo.writeSync(file.fd, data);
  hilog.info(0x0000, 'AppDataSecurity', 'The length of str is: ' + writeLen);
  fileIo.closeSync(file);
}

function readFile(filePath: string): object {
  let file = fileIo.openSync(filePath, fileIo.OpenMode.READ_WRITE | fileIo.OpenMode.CREATE);
  let arrayBuffer = new ArrayBuffer(1024);

  class Option {
    public offset: number = 0;
    public length: number = 0;
  }

  let option = new Option();
  option.length = arrayBuffer.byteLength;
  let readLen = fileIo.readSync(file.fd, arrayBuffer, option);
  let buf = buffer.from(arrayBuffer, 0, readLen);
  hilog.info(0x0000, 'AppDataSecurity', `The length of of file: ${readLen}`);
  fileIo.closeSync(file);
  return [readLen, buf.toString()]
}

function StringToUint8Array(str: String) {
  let arr: number[] = new Array();
  for (let i = 0, j = str.length; i < j; ++i) {
    arr.push(str.charCodeAt(i));
  }
  return new Uint8Array(arr);
}

function Uint8ArrayToString(fileData: Uint8Array) {
  let dataString = '';
  for (let i = 0; i < fileData.length; i++) {
    dataString += String.fromCharCode(fileData[i]);
  }
  return dataString;
}

function GetAesGenerateProperties() {
  let properties: Array<huks.HuksParam> = [{
    tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
    value: huks.HuksKeyAlg.HUKS_ALG_AES
  }, {
    tag: huks.HuksTag.HUKS_TAG_KEY_SIZE,
    value: huks.HuksKeySize.HUKS_AES_KEY_SIZE_128
  }, {
    tag: huks.HuksTag.HUKS_TAG_PURPOSE,
    value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT |
    huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT
  }];
  return properties;
}

function GetAesEncryptProperties() {
  let properties: Array<huks.HuksParam> = [{
    tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
    value: huks.HuksKeyAlg.HUKS_ALG_AES
  }, {
    tag: huks.HuksTag.HUKS_TAG_KEY_SIZE,
    value: huks.HuksKeySize.HUKS_AES_KEY_SIZE_128
  }, {
    tag: huks.HuksTag.HUKS_TAG_PURPOSE,
    value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT
  }, {
    tag: huks.HuksTag.HUKS_TAG_PADDING,
    value: huks.HuksKeyPadding.HUKS_PADDING_PKCS7
  }, {
    tag: huks.HuksTag.HUKS_TAG_BLOCK_MODE,
    value: huks.HuksCipherMode.HUKS_MODE_CBC
  }, {
    tag: huks.HuksTag.HUKS_TAG_IV,
    value: StringToUint8Array(IV)
  }];
  return properties;
}

function GetAesDecryptProperties() {
  let properties: Array<huks.HuksParam> = [{
    tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
    value: huks.HuksKeyAlg.HUKS_ALG_AES
  }, {
    tag: huks.HuksTag.HUKS_TAG_KEY_SIZE,
    value: huks.HuksKeySize.HUKS_AES_KEY_SIZE_128
  }, {
    tag: huks.HuksTag.HUKS_TAG_PURPOSE,
    value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT
  }, {
    tag: huks.HuksTag.HUKS_TAG_PADDING,
    value: huks.HuksKeyPadding.HUKS_PADDING_PKCS7
  }, {
    tag: huks.HuksTag.HUKS_TAG_BLOCK_MODE,
    value: huks.HuksCipherMode.HUKS_MODE_CBC
  }, {
    tag: huks.HuksTag.HUKS_TAG_IV,
    value: StringToUint8Array(IV)
  }];
  return properties;
}

async function GenerateAesKey() {
  let genProperties = GetAesGenerateProperties();
  let options: huks.HuksOptions = {
    properties: genProperties
  };

  await huks.generateKeyItem(aesKeyAlias, options)
    .then((data) => {
      hilog.info(0x0000, 'AppDataSecurity', `promise: generate AES Key success, data = ${JSON.stringify(data)}`);
    }).catch((error: Error) => {
      hilog.error(0x0000, 'AppDataSecurity', `promise: generate AES Key failed, ${JSON.stringify(error)}`);
    })
}

async function EncryptData() {
  let encryptProperties = GetAesEncryptProperties();
  let options: huks.HuksOptions = {
    properties: encryptProperties,
    inData: StringToUint8Array(plainText)
  };

  await huks.initSession(aesKeyAlias, options)
    .then((data) => {
      handle = data.handle;
    }).catch((error: Error) => {
      hilog.error(0x0000, 'AppDataSecurity', `promise: init EncryptData failed, ${JSON.stringify(error)}`);
    })

  await huks.finishSession(handle, options)
    .then((data) => {
      hilog.info(0x0000, 'AppDataSecurity',
        `promise: encrypt data success, data is ` + Uint8ArrayToString(data.outData as Uint8Array));
      cipherData = data.outData as Uint8Array;
    }).catch((error: Error) => {
      hilog.error(0x0000, 'AppDataSecurity', `promise: encrypt data failed, ${JSON.stringify(error)}`);
    })
}

async function DecryptData() {
  let decryptOptions = GetAesDecryptProperties()
  let options: huks.HuksOptions = {
    properties: decryptOptions,
    inData: cipherData
  };

  await huks.initSession(aesKeyAlias, options)
    .then((data) => {
      handle = data.handle;
    }).catch((error: Error) => {
      hilog.error(0x0000, 'AppDataSecurity', `promise: init DecryptData failed, ${JSON.stringify(error)}`);
    })

  await huks.finishSession(handle, options)
    .then((data) => {
      hilog.info(0x0000, 'AppDataSecurity',
        `promise: decrypt data success, data is ` + Uint8ArrayToString(data.outData as Uint8Array));
    }).catch((error: Error) => {
      hilog.error(0x0000, 'AppDataSecurity', `promise: decrypt data failed, ${JSON.stringify(error)}`);
    })
}

async function DeleteKey() {
  let emptyOptions: huks.HuksOptions = {
    properties: []
  };

  await huks.deleteKeyItem(aesKeyAlias, emptyOptions)
    .then((data) => {
      hilog.info(0x0000, 'AppDataSecurity', `promise: delete data success`);
    }).catch((error: Error) => {
      hilog.error(0x0000, 'AppDataSecurity', `promise: delete data failed, ${JSON.stringify(error)}`);
    })
}