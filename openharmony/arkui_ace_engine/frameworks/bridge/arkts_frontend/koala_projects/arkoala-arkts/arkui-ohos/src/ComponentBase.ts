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

import { PeerNode } from './PeerNode'
import { ArkUINativeModule } from "#components"
import { AnimateParam, UIGestureEvent  } from './component'
import { _animationEnd, _animationStart } from './handwritten'
import { unsafeCast } from "@koalaui/common"

export class ComponentBase {
    protected peer?: PeerNode
    protected isFirstBuild: boolean = true
    protected gestureEvent: UIGestureEvent | undefined = undefined
    setPeer(peer: PeerNode) {
        this.peer = peer
    }
    setGestureEvent(gestureEvent: UIGestureEvent) {
        this.gestureEvent = gestureEvent
    }
    protected checkPriority(name: string): boolean {
        return true
    }
    public applyAttributesFinish(): void {
        ArkUINativeModule._ApplyModifierFinish(this.peer!.peer.ptr)
    }
    public applyAttributes(attrs: Object): void {
        this.applyAttributesFinish()
    }
    public animationStart(param: AnimateParam): this {
        _animationStart(param, this.isFirstBuild)
        return this
    }

    public animationEnd(): this {
        _animationEnd(this.isFirstBuild, () => {
            this.isFirstBuild = false
        })
        return this;
    }

    /** @memo */
    public __applyStyle<T extends ComponentBase, A>(
        /** @memo */
        func: (instance: T, arg: A) => T,  // should be ...args: A[], but that doesn't currently compile
        arg: A
    ): T {
        func(unsafeCast<T>(this), arg)
        return unsafeCast<T>(this)
    }

    /** @memo */
    public __applyAnimatableExtend<T extends ComponentBase, A>(
        /** @memo */
        func: (instance: T, arg: A) => T,
        arg: A
    ): T {
        func(unsafeCast<T>(this), arg)
        return unsafeCast<T>(this)
    }
}
