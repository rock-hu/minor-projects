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

import { SignatureItem } from './SignatureItem';

export class SignatureViewModel {
  public static getSignatureList(): SignatureItem[] {
    let SignatureList: SignatureItem[] = [];
    SignatureList.push(new SignatureItem($r('app.string.rsa_pkcs1'), 'RSA1024|PKCS1|SHA256', 'RSA1024', false));
    SignatureList.push(new SignatureItem($r('app.string.rsa_pkcs1_seg'), 'RSA1024|PKCS1|SHA256', 'RSA1024', true));
    SignatureList.push(new SignatureItem($r('app.string.rsa_pass'), 'RSA|PASS|SHA256|MGF1_SHA256', '', false));
    SignatureList.push(new SignatureItem($r('app.string.ecdsa'), 'ECC256|SHA256', 'ECC256', false));
    SignatureList.push(new SignatureItem($r('app.string.sm2'), 'SM2_256|SM3', 'SM2_256', false));

    return SignatureList;
  }
}