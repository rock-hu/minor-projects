/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

// Annotation declaration:
@interface ClassAuthor {
   authorName: string = "Jim"
   authorAge: number = 35
   testBool: boolean = false
   reviewers: string[] = ["Bob", "Jim", "Tom"]
   reviewersAge: number[] = [18, 21, 32]
   testBools: boolean[] = [false, true, false]
}

@interface MyAnno {

}

// Annotation use:
@ClassAuthor({authorName: "Bob", authorAge: 35, testBool: false,
               reviewers: ["Bob", "Jim", "Tom"], reviewersAge: [-18, -3.14, 32],
               testBools: [false, true, false]})
@MyAnno()
class MyClass {}

function main(): void {
   
}