/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

abstract class A {
    str: string = "A"

    getA():string {
        return this.str;
    }
}

abstract class B extends A {
    getB():string {
        return this.str;
    }

    public get_str(): string {
        return this.str;
    }

    public get_super_str(): string {
        return super.getA();
    }
}

class BB extends B {}

abstract class C extends B {
    str: string = "C"

    public override get_str(): string {
        return this.str;
    }

    public override get_super_str(): string {
        return super.getB();
    }
}

class CC extends C {}

function main(): void {
  let b: B = new BB();
  let c: C = new CC();

  assertEQ(b.str, "A")
  assertEQ(b.get_str(), "A")
  assertEQ(b.get_super_str(), "A")
  assertEQ(c.str, "C")
  assertEQ(c.get_str(), "C")
  assertEQ(c.get_super_str(), "A")

  assertEQ((b as B).str, "A")
  assertEQ((b as A).str, "A")
  assertEQ((b as B).get_str(), "A")
  assertEQ((b as B).get_super_str(), "A")

  assertEQ((c as C).str, "C")
  assertEQ((c as B).str, "A")
  assertEQ((c as A).str, "A")

  assertEQ((c as C).get_str(), "C")
  assertEQ((c as C).get_super_str(), "A")
  assertEQ((c as B).get_str(), "C")
  assertEQ((c as B).get_super_str(), "A")
}
