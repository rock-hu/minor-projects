/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

class AA {
  a = 1;
  test() {
    let cb = () => {
      this.a;
      new.target;
      print('cb');
    }
    print('test');
    return cb;
  }

  testVariableScopeRefrenceThis() {
    let cb = () => {
      for(let a = 1; a < 10; a++) {
        this.a;
        print('cb');
      }
    }
    print('test');
    return cb;
  }

  testVariableScopeReferenceTarget() {
    let cb = () => {
      for(let a = 1; a < 10; a++) {
        new.target;
        print('cb');
      }
    }
    print('test');
    return cb;
  }
}
