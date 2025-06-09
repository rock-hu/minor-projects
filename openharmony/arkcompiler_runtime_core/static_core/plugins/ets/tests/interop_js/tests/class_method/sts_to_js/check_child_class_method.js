
/**
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

const {
    ChildClass,
    createChildClassFromSts,
} = require('class_method.test.abc');

function checkChildClassMethod() {
    const ETSClass = new ChildClass();

    ASSERT_TRUE(ETSClass.compare(ETSClass));
}

function checkCreateChildClassFromSts() {
    const ETSClass = createChildClassFromSts();

    ASSERT_TRUE(ETSClass.compare(ETSClass));
}

function checkChildClassOwnMethod() {
    const ETSClass = new ChildClass();

    ASSERT_TRUE(ETSClass.childMethodCompare(ETSClass));
}

function checkCreateChildClassOwnMethodFromSts() {
    const ETSClass = createChildClassFromSts();

    ASSERT_TRUE(ETSClass.childMethodCompare(ETSClass));
}


checkChildClassMethod();
checkCreateChildClassFromSts();
checkChildClassOwnMethod();
checkCreateChildClassOwnMethodFromSts();
