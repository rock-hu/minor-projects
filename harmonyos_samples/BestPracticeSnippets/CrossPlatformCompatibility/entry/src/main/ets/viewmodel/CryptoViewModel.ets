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

import { CryptoItem } from './CryptoItem';

export class CryptoViewModel {
  public static pkData = new Uint8Array([48, 129, 159, 48, 13, 6, 9, 42, 134, 72, 134, 247, 13, 1, 1, 1, 5, 0, 3, 129,
    141, 0, 48, 129, 137, 2, 129, 129, 0, 197, 64, 10, 198, 14, 110, 65, 92, 206, 35, 28, 123, 153, 24, 134, 255, 145,
    74, 42, 173, 40, 215, 146, 58, 143, 46, 10, 195, 154, 160, 69, 196, 220, 152, 179, 44, 111, 200, 84, 78, 215, 73,
    210, 181, 12, 29, 70, 68, 36, 135, 153, 89, 230, 202, 130, 212, 111, 243, 234, 92, 131, 62, 145, 50, 73, 48, 104,
    245, 46, 70, 45, 157, 147, 143, 140, 162, 156, 216, 220, 49, 121, 142, 194, 33, 223, 201, 0, 16, 163, 210, 240, 118,
    92, 147, 121, 220, 17, 114, 24, 52, 125, 135, 176, 88, 21, 83, 86, 17, 156, 88, 250, 48, 79, 86, 128, 248, 105, 208,
    133, 140, 13, 153, 164, 191, 136, 164, 44, 53, 2, 3, 1, 0, 1]);
  public static skData = new Uint8Array([48, 130, 2, 119, 2, 1, 0, 48, 13, 6, 9, 42, 134, 72, 134, 247, 13, 1, 1, 1, 5,
    0, 4, 130, 2, 97, 48, 130, 2, 93, 2, 1, 0, 2, 129, 129, 0, 197, 64, 10, 198, 14, 110, 65, 92, 206, 35, 28, 123, 153,
    24, 134, 255, 145, 74, 42, 173, 40, 215, 146, 58, 143, 46, 10, 195, 154, 160, 69, 196, 220, 152, 179, 44, 111, 200,
    84, 78, 215, 73, 210, 181, 12, 29, 70, 68, 36, 135, 153, 89, 230, 202, 130, 212, 111, 243, 234, 92, 131, 62, 145,
    50, 73, 48, 104, 245, 46, 70, 45, 157, 147, 143, 140, 162, 156, 216, 220, 49, 121, 142, 194, 33, 223, 201, 0, 16,
    163, 210, 240, 118, 92, 147, 121, 220, 17, 114, 24, 52, 125, 135, 176, 88, 21, 83, 86, 17, 156, 88, 250, 48, 79, 86,
    128, 248, 105, 208, 133, 140, 13, 153, 164, 191, 136, 164, 44, 53, 2, 3, 1, 0, 1, 2, 129, 128, 70, 75, 184, 139, 53,
    1, 94, 17, 240, 244, 218, 101, 193, 253, 215, 190, 164, 204, 197, 192, 200, 89, 107, 39, 171, 119, 65, 38, 204, 168,
    105, 180, 234, 217, 16, 161, 185, 132, 175, 103, 25, 154, 153, 153, 36, 36, 26, 178, 150, 66, 45, 8, 185, 19, 90,
    228, 210, 177, 30, 200, 177, 141, 78, 184, 248, 59, 113, 154, 145, 73, 160, 24, 73, 157, 86, 207, 186, 32, 95, 200,
    106, 252, 107, 69, 170, 193, 216, 196, 181, 142, 74, 203, 15, 18, 89, 228, 152, 19, 239, 21, 233, 98, 121, 214, 57,
    187, 111, 239, 223, 248, 199, 70, 223, 108, 108, 113, 234, 144, 155, 95, 246, 144, 244, 122, 39, 55, 127, 81, 2, 65,
    0, 246, 96, 188, 0, 0, 104, 221, 105, 139, 144, 63, 175, 209, 87, 179, 162, 88, 192, 99, 82, 125, 53, 54, 48, 70,
    245, 239, 37, 15, 242, 247, 84, 115, 187, 196, 95, 156, 40, 165, 60, 64, 102, 13, 229, 243, 2, 149, 0, 232, 226,
    221, 192, 95, 11, 12, 208, 5, 181, 98, 62, 210, 190, 141, 235, 2, 65, 0, 204, 244, 34, 10, 105, 80, 76, 116, 163,
    35, 231, 168, 187, 206, 189, 101, 215, 103, 80, 115, 86, 11, 34, 127, 203, 114, 84, 188, 121, 174, 169, 31, 142, 2,
    182, 27, 140, 225, 157, 227, 71, 98, 15, 203, 187, 213, 5, 190, 20, 121, 8, 30, 193, 100, 232, 101, 141, 8, 124, 20,
    29, 78, 6, 95, 2, 65, 0, 204, 43, 225, 224, 6, 118, 224, 117, 100, 200, 199, 94, 70, 23, 109, 175, 173, 232, 208,
    230, 61, 8, 105, 189, 156, 48, 150, 91, 154, 89, 248, 136, 173, 215, 254, 166, 84, 220, 130, 1, 234, 68, 40, 100,
    84, 251, 224, 202, 254, 51, 115, 28, 198, 38, 124, 25, 175, 129, 94, 199, 61, 17, 216, 189, 2, 64, 72, 230, 129,
    129, 48, 138, 134, 87, 106, 123, 231, 247, 165, 173, 216, 194, 115, 198, 228, 223, 209, 120, 46, 114, 68, 92, 75,
    117, 170, 214, 140, 131, 147, 208, 181, 19, 193, 157, 178, 186, 87, 246, 178, 101, 166, 79, 20, 54, 211, 51, 101,
    199, 2, 197, 48, 192, 134, 84, 193, 69, 170, 82, 201, 131, 2, 65, 0, 213, 165, 55, 166, 131, 210, 195, 56, 250, 147,
    195, 61, 205, 208, 189, 185, 40, 52, 50, 119, 137, 23, 246, 46, 220, 108, 52, 23, 152, 154, 94, 32, 144, 195, 184,
    249, 21, 168, 12, 57, 222, 18, 60, 117, 81, 157, 72, 30, 155, 190, 165, 242, 228, 139, 240, 184, 145, 170, 103, 210,
    160, 161, 135, 13]);
  public static rsaKeyNin = BigInt('0x9260d0750ae117eee55c3f3deaba74917521a262ee76007cdf8a56755ad73a1598a1408410a' +
    '01434c3f5bc54a88b57fa19fc4328daea0750a4c44e88cff3b2382621b80f670464433e4336e6d003e8cd65bff211da144b88291c2259a00' +
    'a72b711c116ef7686e8fee34e4d933c868187bdc26f7be071493c86f7a5941c3510806ad67b0f94d88f5cf5c02a092821d8626e8932b65c5' +
    'bd8c92049c210932b7afa7ac59c0e886ae5c1edb00d8ce2c57633db26bd6639bff73cee82be9275c402b4cf2a4388da8cf8c64eefe1c5a0f' +
    '5ab8057c39fa5c0589c3e253f0960332300f94bea44877b588e1edbde97cf2360727a09b775262d7ee552b3319b9266f05a25');
  public static rsaKeyDIn = BigInt('0x6a7df2ca63ead4dda191d614b6b385e0d9056a3d6d5cfe07db1daabee022db08212d97613d3328e' +
    '0267c9dd23d787abde2afcb306aeb7dfce69246cc73f5c87fdf06030179a2114b767db1f083ff841c025d7dc00cd82435b9a90f695369e94' +
    'df23d2ce458bc3b3283ad8bba2b8fa1ba62e2dce9accff3799aae7c840016f3ba8e0048c0b6cc4339af7161003a5beb864a0164b2c1c9237' +
    'b64bc87556994351b27506c33d4bcdfce0f9c491a7d6b0628c7c852be4f0a9c3132b2ed3a2c8881e9aab07e20e17deb074691be677776a78' +
    'b5c502e05d9bdde72126b3738695e2dd1a0a98a14247c65d8a7ee79432a092cb0721a12df798e44f7cfce0c498147a9b1');

  public static getCryptoList(): CryptoItem[] {
    let cryptoList: CryptoItem[] = [];
    cryptoList.push(new CryptoItem($r('app.string.AES_GCM'), $r('app.string.encrypt'),
      $r('app.string.decrypt'), 'AES_GCM'));
    cryptoList.push(new CryptoItem($r('app.string.AES_CBC'), $r('app.string.encrypt'),
      $r('app.string.decrypt'), 'AES_CBC'));
    cryptoList.push(new CryptoItem($r('app.string.AES_EBC'), $r('app.string.encrypt'),
      $r('app.string.decrypt'), 'AES_ECB'));
    cryptoList.push(new CryptoItem($r('app.string.3DES_EBC'), $r('app.string.encrypt'),
      $r('app.string.decrypt'), '3DES_ECB'));
    cryptoList.push(new CryptoItem($r('app.string.SM4_EBC'), $r('app.string.encrypt'),
      $r('app.string.decrypt'), 'SM4_ECB'));
    cryptoList.push(new CryptoItem($r('app.string.SM4_CBC'), $r('app.string.encrypt'),
      $r('app.string.decrypt'), 'SM4_CBC'));
    cryptoList.push(new CryptoItem($r('app.string.RSA_PKCS1'), $r('app.string.encrypt'),
      $r('app.string.decrypt'), 'RSA_PKCS1'));
    cryptoList.push(new CryptoItem($r('app.string.RSA_PKCS1_OAEP'), $r('app.string.encrypt'),
      $r('app.string.decrypt'),
      'RSA_PKCS1_OAEP'));
    cryptoList.push(new CryptoItem($r('app.string.SM2_crypto'), $r('app.string.encrypt'),
      $r('app.string.decrypt'), 'SM2'));
    cryptoList.push(new CryptoItem($r('app.string.rsa_dissymmetry_pkcs1'),
      $r('app.string.decrypt'), $r('app.string.encrypt'),
      'RSA_PKCS1_BIG'));
    cryptoList.push(new CryptoItem($r('app.string.format_conversion'),
      $r('app.string.encrypt'), $r('app.string.decrypt'), 'SM2'));
    return cryptoList;
  }
}