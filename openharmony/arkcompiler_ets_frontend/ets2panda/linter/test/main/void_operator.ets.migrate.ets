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

(() => {
    0.0;
    return undefined;
})();

(() => {
    'hello';
    return undefined;
})();

(() => {
    (1.0 + 2.0);
    return undefined;
})();

interface GeneratedObjectLiteralInterface_1 {
    a: number;
    b: number;
}
(() => {
    ({ a: 1.0, b: 2.0 } as GeneratedObjectLiteralInterface_1);
    return undefined;
})();

(() => {
    [1.0, 2.0, 3.0];
    return undefined;
})();

(() => {
    console.log('expression evaluated');
    return undefined;
})();

const undefined_value = (() => {
    1.0;
    return undefined;
})();

const undefined_value2: undefined = (() => {
    2.0;
    return undefined;
})();

const undefined_value3: number | undefined = (() => {
    3.0;
    return undefined;
})();

(() => {
    (() => {
    1.0;
    return undefined;
})();
    return undefined;
})();

(() => {
    (() => {
    console.log('foo');
});
    return undefined;
})();

(() => {
    (() => {
    console.log("bar!");
})();
    return undefined;
})();

(() => {
    (() => {
    console.log('baz!');
})();
    return undefined;
})();

(() => {
    (class {
    });
    return undefined;
})();

(() => {
    (class {
    });
    return undefined;
})();

(() => {
    (() => { });
    return undefined;
})();

function foo() {
  let a: number = 1.0;
  (() => {
    a++;
    return undefined;
})();

  let b: number[] = [1.0, 2.0, 3.0];
  (() => {
    console.log(b.filter(x => x % 2.0 !== 0.0));
    return undefined;
})();
  
  (() => {
    (() => {
    console.log('foo');
});
    return undefined;
})();

  (() => {
    (() => {
    console.log('foo');
});
    return undefined;
})();
  
  (() => {
    (class {
    });
    return undefined;
})();

  (() => {
    (class localClass {
    });
    return undefined;
})();
}
