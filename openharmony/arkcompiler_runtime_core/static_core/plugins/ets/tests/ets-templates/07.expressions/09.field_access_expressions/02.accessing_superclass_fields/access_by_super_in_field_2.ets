/*---
Copyright (c) 2024-2025 Huawei Device Co., Ltd.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
---*/

/*---
desc: The forms using the keyword super are valid in instance methods, constructors of a class or in the initializer of an instance variable of a class. Compile-time error occurs otherwise.
---*/

class A {
  private _x: int = 5;

  protected get x(): int{
    return this._x;
  }
}

class B extends A{
  protected readonly y: int = super.x;
  foo(): int {
    return this.y;
  }
}

function main(): void {
  let instance: B = new B();
  assertEQ( instance.foo(), 5 )
}
