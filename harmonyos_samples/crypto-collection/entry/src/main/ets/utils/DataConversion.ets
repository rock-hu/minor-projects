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

import { cryptoFramework } from '@kit.CryptoArchitectureKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { buffer, util } from '@kit.ArkTS';
import { SM2Sequence } from './SM2Sequence';

export class DataConversion {
  /**
   * Generate SM2 public key based on key parameters.
   *
   * @param keyStr Hexadecimal public key。
   * @returns Promise<cryptoFramework.PubKey>.
   */
  context: UIContext;

  constructor(context: UIContext) {
    this.context = context
  }

  public static async convertStrToPubKey(keyStr: string): Promise<cryptoFramework.PubKey> {
    const puKeyStr = keyStr.startsWith('04') ? keyStr.slice(2) : keyStr;
    const pkPart1 = puKeyStr.slice(0, puKeyStr.length / 2);
    const pkPart2 = puKeyStr.slice(puKeyStr.length / 2);
    const pks: cryptoFramework.Point = {
      x: BigInt('0x' + pkPart1),
      y: BigInt('0x' + pkPart2)
    };
    const pubKeySpec: cryptoFramework.ECCPubKeySpec = {
      params: cryptoFramework.ECCKeyUtil.genECCCommonParamsSpec('NID_sm2'),
      pk: pks,
      algName: 'SM2',
      specType: cryptoFramework.AsyKeySpecType.PUBLIC_KEY_SPEC
    };
    const keypairGenerator = cryptoFramework.createAsyKeyGeneratorBySpec(pubKeySpec);
    let pksData = keypairGenerator.generatePubKeySync();
    return pksData;
  }

  /**
   * Generate SM2 private key based on key parameters.
   *
   * @param keyStr Hexadecimal private key。
   * @returns Promise<cryptoFramework.PriKey>.
   */
  public static async convertStrToPriKey(keyStr: string): Promise<cryptoFramework.PriKey> {
    const sks = BigInt('0x' + keyStr);
    const priKeySpec: cryptoFramework.ECCPriKeySpec = {
      params: cryptoFramework.ECCKeyUtil.genECCCommonParamsSpec('NID_sm2'),
      sk: sks,
      algName: 'SM2',
      specType: cryptoFramework.AsyKeySpecType.PRIVATE_KEY_SPEC
    };
    const priKeypairGenerator = cryptoFramework.createAsyKeyGeneratorBySpec(priKeySpec);
    let sksData = priKeypairGenerator.generatePriKeySync();
    return sksData;
  }

  getLenHex(data: string): string {
    let byte: number = Number.parseInt('0x' + data.slice(0, 2));
    let len_size: number = byte > 127 ? byte - 0x80 + 1 : 1;
    return data.slice(0, len_size * 2);
  }

  lenHex2number(lenHex: string): number {
    if (lenHex.length === 2) {
      return Number.parseInt('0x' + lenHex);
    }
    return Number.parseInt('0x' + lenHex.slice(2, lenHex.length));
  }

  readC1(sm2_sequence: SM2Sequence, data: string): string {
    let xy: string[] = [];
    for (let i = 0; i < 2; i++) {
      if (data.startsWith('0220')) {
        xy[i] = data.slice(4, 68);
        data = data.slice(68, data.length);
      } else if (data.startsWith('022100')) {
        xy[i] = data.slice(6, 70);
        data = data.slice(70, data.length);
      } else {
        hilog.error(0, 'd2i_SM2_Ciphertext',
          this.context.getHostContext()!.resourceManager.getStringSync($r('app.string.format_error').id));
      }
    }
    sm2_sequence.C1x = xy[0];
    sm2_sequence.C1y = xy[1];
    return data;
  }

  readC2(sm2_sequence: SM2Sequence, data: string): string {
    if (data.startsWith('04')) {
      data = data.slice('04'.length, data.length);
      let C2_lenHex = this.getLenHex(data);
      data = data.slice(C2_lenHex.length, data.length);
      if (this.lenHex2number(C2_lenHex) !== data.length / 2) {
        hilog.error(0, 'd2i_SM2_Ciphertext',
          this.context.getHostContext()!.resourceManager.getStringSync($r('app.string.format_error').id));
      }
      sm2_sequence.C2 = data;
    } else {
      hilog.error(0, 'd2i_SM2_Ciphertext',
        this.context.getHostContext()!.resourceManager.getStringSync($r('app.string.format_error').id));
    }
    return data;
  }

  readC3(sm2_sequence: SM2Sequence, data: string): string {
    if (data.startsWith('0420')) {
      sm2_sequence.C3 = data.slice(4, 68);
      data = data.slice(68, data.length);
    } else {
      hilog.error(0, 'd2i_SM2_Ciphertext',
        this.context.getHostContext()!.resourceManager.getStringSync($r('app.string.format_error').id));
    }
    return data;
  }

  genLenHex(content: string): string {
    let size: number = content.length / 2;
    let lenHex: string;
    if (size.toString(16).length % 2 === 1) {
      lenHex = '0' + size.toString(16);
    } else {
      lenHex = size.toString(16);
    }

    if (size < 0x80) {
      return lenHex;
    }
    let lenHex_size: number = lenHex.length / 2;
    return (lenHex_size | 0x80).toString(16) + lenHex;
  }

  /**
   * Convert standard SM2 to C1C2C3.
   *
   * @param standard_data standard SM2 ciphertext.
   * @returns string.
   */
  d2i_SM2_Ciphertext(standard_data: string): string {
    let message: string = standard_data;
    if (!message.startsWith('30')) {
      hilog.error(0, 'd2i_SM2_Ciphertext',
        this.context.getHostContext()!.resourceManager.getStringSync($r('app.string.format_error').id));
    }
    message = message.slice('30'.length, message.length);

    // The length of the SM2 sequence content.
    let sequence_lexHex: string = this.getLenHex(message);
    message = message.slice(sequence_lexHex.length, message.length);
    let sequence_len: number = this.lenHex2number(sequence_lexHex);

    if (sequence_len !== message.length / 2) {
      hilog.error(0x0000, 'd2i_SM2_Ciphertext',
        this.context.getHostContext()!.resourceManager.getStringSync($r('app.string.format_error').id));
    }

    let sm2_sequence = new SM2Sequence();
    message = this.readC1(sm2_sequence, message);
    message = this.readC3(sm2_sequence, message);
    message = this.readC2(sm2_sequence, message);
    hilog.info(0x0000, 'sm2_sequence', sm2_sequence.toString());

    let primal_data: string = sm2_sequence.C1x + sm2_sequence.C1y + sm2_sequence.C3 + sm2_sequence.C2;
    return primal_data;
  }

  /**
   * Convert to ASN. 1 format.
   *
   * @param standard_data C1C3C2 ciphertext.
   * @returns string.
   */
  i2dSM2CipherText(primal_data: string): string {
    let sm2_sequence = new SM2Sequence();
    sm2_sequence.C1x = primal_data.slice(0, 64);
    primal_data = primal_data.slice(64, primal_data.length);
    sm2_sequence.C1y = primal_data.slice(0, 64);
    primal_data = primal_data.slice(64, primal_data.length);
    sm2_sequence.C3 = primal_data.slice(0, 64);
    primal_data = primal_data.slice(64, primal_data.length);
    sm2_sequence.C2 = primal_data;

    let C1x_title: string = (Number.parseInt('0x' + sm2_sequence.C1x.slice(0, 2)) > 127) ? '022100' : '0220';
    let C1y_title: string = (Number.parseInt('0x' + sm2_sequence.C1y.slice(0, 2)) > 127) ? '022100' : '0220';
    let C3_title: string = '0420';
    let C2_title: string = '04' + this.genLenHex(sm2_sequence.C2);
    let sequence_message: string =
      C1x_title + sm2_sequence.C1x + C1y_title + sm2_sequence.C1y + C2_title + sm2_sequence.C2 + C3_title +
      sm2_sequence.C3;
    let sequence_lenHex: string = this.genLenHex(sequence_message);

    let standard_data = '30' + sequence_lenHex + sequence_message;
    return standard_data;
  }

  // Convert string to uint8Array.
  public static stringToUint8Array(str: string): Uint8Array {
    let textEncoder = util.TextEncoder.create('utf-8');
    return textEncoder.encodeInto(str);
  }

  // Convert uint8Array to string.
  public static uint8ArrayToString(input: Uint8Array): string {
    let textDecoder = util.TextDecoder.create('utf-8');
    return textDecoder.decodeToString(input);
  }

  // Convert base64 to uint8Array.
  public static base64ToUint8Array(str: string): Uint8Array {
    let base64Helper = new util.Base64Helper();
    return base64Helper.decodeSync(str);
  }

  // Convert uint8Array to base64.
  public static uint8ArrayToBase64(input: Uint8Array): string {
    let base64Helper = new util.Base64Helper();
    return base64Helper.encodeToStringSync(input);
  }

  // Convert hex to uint8Array.
  public static hexStrToUint8Array(data: string): Uint8Array {
    return new Uint8Array(buffer.from(data, 'hex').buffer);
  }

  // Convert uint8Array to hex.
  public static uint8ArrayToHexStr(input: Uint8Array): string {
    let hexString = '';
    for (let i = 0; i < input.length; i++) {
      let char = ('00' + input[i].toString(16)).slice(-2);
      hexString += char;
    }
    return hexString;
  }
}