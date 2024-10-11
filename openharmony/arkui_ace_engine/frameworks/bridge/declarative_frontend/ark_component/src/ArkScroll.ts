/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

/// <reference path='./import.ts' />

class ScrollNestedScrollModifier extends ModifierWithKey<ArkNestedScrollOptions> {
  static identity: symbol = Symbol('nestedScroll');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetNestedScroll(node);
    } else {
      getUINativeModule().scroll.setNestedScroll(node, this.value.scrollForward, this.value.scrollBackward);
    }
  }

  checkObjectDiff(): boolean {
    return !isBaseOrResourceEqual(this.stageValue.scrollForward, this.value.scrollForward) ||
      !isBaseOrResourceEqual(this.stageValue.scrollBackward, this.value.scrollBackward);
  }
}

class ScrollEnableScrollInteractionModifier extends ModifierWithKey<boolean> {
  static identity: symbol = Symbol('enableScrollInteraction');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetEnableScroll(node);
    } else {
      getUINativeModule().scroll.setEnableScroll(node, this.value);
    }
  }

  checkObjectDiff(): boolean {
    return !isBaseOrResourceEqual(this.stageValue, this.value);
  }
}

class ScrollFrictionModifier extends ModifierWithKey<number | Resource> {
  static identity: symbol = Symbol('friction');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetFriction(node);
    } else {
      getUINativeModule().scroll.setFriction(node, this.value);
    }
  }

  checkObjectDiff(): boolean {
    return !isBaseOrResourceEqual(this.stageValue, this.value);
  }
}

class ScrollScrollSnapModifier extends ModifierWithKey<ArkScrollSnapOptions> {
  static identity: symbol = Symbol('scrollSnap');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetScrollSnap(node);
    } else {
      let snapPagination = [];
      let isArray = true;
      if (Array.isArray(this.value.snapPagination)) {
        for (let i = 0; i < this.value.snapPagination.length; i++) {
          let item = this.value.snapPagination[i];
          snapPagination.push(item);
        }
      } else {
        isArray = false;
      }

      if (isArray) {
        getUINativeModule().scroll.setScrollSnap(node, this.value.snapAlign, snapPagination, this.value.enableSnapToStart, this.value.enableSnapToEnd);
      } else {
        getUINativeModule().scroll.setScrollSnap(node, this.value.snapAlign,
          this.value.snapPagination, this.value.enableSnapToStart, this.value.enableSnapToEnd);
      }
    }
  }

  checkObjectDiff(): boolean {
    return !((this.stageValue.snapAlign === this.value.snapAlign) &&
      (this.stageValue.enableSnapToStart === this.value.enableSnapToStart) &&
      (this.stageValue.enableSnapToEnd === this.value.enableSnapToEnd) &&
      (this.stageValue.snapPagination === this.value.snapPagination));
  }
}

class ScrollScrollBarModifier extends ModifierWithKey<number> {
  static identity: symbol = Symbol('scrollBar');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetScrollBar(node);
    } else {
      getUINativeModule().scroll.setScrollBar(node, this.value);
    }
  }
  checkObjectDiff(): boolean {
    return !isBaseOrResourceEqual(this.stageValue, this.value);
  }
}

class ScrollScrollableModifier extends ModifierWithKey<ScrollDirection> {
  constructor(value: ScrollDirection) {
    super(value);
  }
  static identity: Symbol = Symbol('scrollable');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetScrollable(node);
    } else {
      getUINativeModule().scroll.setScrollable(node, this.value);
    }
  }

  checkObjectDiff(): boolean {
    return this.stageValue !== this.value;
  }
}

class ScrollEdgeEffectModifier extends ModifierWithKey<ArkScrollEdgeEffect> {
  constructor(value: ArkScrollEdgeEffect) {
    super(value);
  }
  static identity: Symbol = Symbol('edgeEffect');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetEdgeEffect(node);
    } else {
      getUINativeModule().scroll.setEdgeEffect(node, this.value.value, this.value.options?.alwaysEnabled);
    }
  }

  checkObjectDiff(): boolean {
    return !((this.stageValue.value === this.value.value) &&
      (this.stageValue.options === this.value.options));
  }
}

class ScrollFadingEdgeModifier extends ModifierWithKey<ArkFadingEdge> {
  constructor(value: ArkFadingEdge) {
    super(value);
  }
  static identity: Symbol = Symbol('scrollFadingEdge');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetFadingEdge(node);
    } else {
      getUINativeModule().scroll.setFadingEdge(node, this.value.value!, this.value.options?.fadingEdgeLength);
    }
  }
  checkObjectDiff(): boolean {
    return !((this.stageValue.value === this.value.value) &&
      (this.stageValue.options === this.value.options));
  }
}

class ScrollScrollBarWidthModifier extends ModifierWithKey<string | number> {
  constructor(value: string | number) {
    super(value);
  }
  static identity: Symbol = Symbol('scrollBarWidth');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetScrollBarWidth(node);
    } else {
      getUINativeModule().scroll.setScrollBarWidth(node, this.value);
    }
  }

  checkObjectDiff(): boolean {
    return this.stageValue !== this.value;
  }
}

class ScrollScrollBarColorModifier extends ModifierWithKey<ResourceColor> {
  constructor(value: ResourceColor) {
    super(value);
  }
  static identity: Symbol = Symbol('scrollBarColor');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetScrollBarColor(node);
    } else {
      getUINativeModule().scroll.setScrollBarColor(node, this.value);
    }
  }

  checkObjectDiff(): boolean {
    return !isBaseOrResourceEqual(this.stageValue, this.value);
  }
}

class ScrollEnablePagingModifier extends ModifierWithKey<boolean> {
  constructor(value: boolean) {
    super(value);
  }
  static identity: Symbol = Symbol('scrollEnablePaging');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetEnablePaging(node);
    } else {
      getUINativeModule().scroll.setEnablePaging(node, this.value);
    }
  }
}

class ScrollClipModifier extends ModifierWithKey<boolean | object> {
  constructor(value: boolean | object) {
    super(value);
  }
  static identity: Symbol = Symbol('scrollClip');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().common.resetClipWithEdge(node);
    } else {
      getUINativeModule().common.setClipWithEdge(node, this.value);
    }
  }

  checkObjectDiff(): boolean {
    return true;
  }
}

class ScrollInitialOffsetModifier extends ModifierWithKey<ArkScrollOffsetOptions> {
  static identity: Symbol = Symbol('initialOffset');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetInitialOffset(node);
    } else {
      getUINativeModule().scroll.setInitialOffset(node, this.value.xOffset, this.value.yOffset);
    }
  }

  checkObjectDiff(): boolean {
    return !((this.stageValue.xOffset === this.value.xOffset) &&
    (this.stageValue.yOffset === this.value.yOffset));
  }
}

class ScrollFlingSpeedLimitModifier extends ModifierWithKey<number> {
  static identity: symbol = Symbol('flingSpeedLimit');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetFlingSpeedLimit(node);
    } else {
      getUINativeModule().scroll.setFlingSpeedLimit(node, this.value);
    }
  }
  checkObjectDiff(): boolean {
    return !isBaseOrResourceEqual(this.stageValue, this.value);
  }
}

class ScrollInitializeModifier extends ModifierWithKey<Scroller> {
  constructor(value: Scroller) {
    super(value);
  }
  static identity: Symbol = Symbol('scrollInitialize');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetScrollInitialize(node);
    } else {
      getUINativeModule().scroll.setScrollInitialize(node, this.value);
    }
  }
}
class ScrollOnScrollStartModifier extends ModifierWithKey<() => void> {
  constructor(value: () => void) {
    super(value);
  }
  static identity: Symbol = Symbol('scrollOnScrollStart');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetScrollOnScrollStart(node);
    } else {
      getUINativeModule().scroll.setScrollOnScrollStart(node, this.value);
    }
  }
}

class ScrollOnScrollEndModifier extends ModifierWithKey<() => void> {
  constructor(value: () => void) {
    super(value);
  }
  static identity: Symbol = Symbol('scrollOnScrollEnd');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetScrollOnScrollEnd(node);
    } else {
      getUINativeModule().scroll.setScrollOnScrollEnd(node, this.value);
    }
  }
}

class ScrollOnScrollStopModifier extends ModifierWithKey<() => void> {
  constructor(value: () => void) {
    super(value);
  }
  static identity: Symbol = Symbol('scrollOnScrollStop');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetScrollOnScrollStop(node);
    } else {
      getUINativeModule().scroll.setScrollOnScrollStop(node, this.value);
    }
  }
}

class ScrollOnScrollModifier extends ModifierWithKey<(xOffset: number, yOffset: number) => void> {
  constructor(value: (xOffset: number, yOffset: number) => void) {
    super(value);
  }
  static identity: Symbol = Symbol('scrollOnScroll');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetScrollOnScrollModifier(node);
    } else {
      getUINativeModule().scroll.setScrollOnScrollModifier(node, this.value);
    }
  }
}

class ScrollOnScrollEdgeModifier extends ModifierWithKey<(side: Edge) => void> {
  constructor(value: (side: Edge) => void) {
    super(value);
  }
  static identity: Symbol = Symbol('scrollOnScrollEdge');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetScrollOnScrollEdge(node);
    } else {
      getUINativeModule().scroll.setScrollOnScrollEdge(node, this.value);
    }
  }
}

class ScrollOnDidScrollModifier extends ModifierWithKey<(xOffset: number, 
                                                         yOffset: number, scrollState: ScrollState) => void> {
  constructor(value: (xOffset: number, yOffset: number, scrollState: ScrollState) => void) {
    super(value);
  }
  static identity: Symbol = Symbol('scrollOnDidScroll');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetScrollOnDidScroll(node);
    } else {
      getUINativeModule().scroll.setScrollOnDidScroll(node, this.value);
    }
  }
}

class ScrollOnWillScrollModifier extends ModifierWithKey<(xOffset: number, yOffset: number,
  scrollState: ScrollState, scrollSource: ScrollSource) => void | OffsetResult> {
  constructor(value: (xOffset: number, yOffset: number,
    scrollState: ScrollState, scrollSource: ScrollSource) => void | OffsetResult) {
    super(value);
  }
  static identity: Symbol = Symbol('scrollOnWillScroll');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetScrollOnWillScroll(node);
    } else {
      getUINativeModule().scroll.setScrollOnWillScroll(node, this.value);
    }
  }
}

class ScrollOnScrollFrameBeginModifier extends ModifierWithKey<(offset: number, state: ScrollState) =>
  { offsetRemain: number }> {
  constructor(value: (offset: number, state: ScrollState) =>
    { offsetRemain: number }) {
    super(value);
  }
  static identity: Symbol = Symbol('scrollOnScrollFrameBegin');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      getUINativeModule().scroll.resetScrollOnScrollFrameBegin(node);
    } else {
      getUINativeModule().scroll.setScrollOnScrollFrameBegin(node, this.value);
    }
  }
}

class ArkScrollComponent extends ArkComponent implements ScrollAttribute {
  constructor(nativePtr: KNode, classType?: ModifierType) {
    super(nativePtr, classType);
  }
  initialize(value: Object[]): ScrollAttribute {
    if (value[0] !== undefined) {
      modifierWithKey(this._modifiersWithKeys, ScrollInitializeModifier.identity, ScrollInitializeModifier, value[0]);
    }
    return this;
  }
  allowChildCount(): number {
    return 1;
  }
  scrollable(value: ScrollDirection): this {
    modifierWithKey(this._modifiersWithKeys, ScrollScrollableModifier.identity, ScrollScrollableModifier, value);
    return this;
  }
  onScroll(event: (xOffset: number, yOffset: number) => void): this {
    modifierWithKey(this._modifiersWithKeys, ScrollOnScrollModifier.identity, ScrollOnScrollModifier, event);
    return this;
  }
  onScrollEdge(event: (side: Edge) => void): this {
    modifierWithKey(this._modifiersWithKeys, ScrollOnScrollEdgeModifier.identity, ScrollOnScrollEdgeModifier, event);
    return this;
  }
  onScrollStart(event: () => void): this {
    modifierWithKey(this._modifiersWithKeys, ScrollOnScrollStartModifier.identity, ScrollOnScrollStartModifier, event);
    return this;
  }
  onScrollEnd(event: () => void): this {
    modifierWithKey(this._modifiersWithKeys, ScrollOnScrollEndModifier.identity, ScrollOnScrollEndModifier, event);
    return this;
  }
  onScrollStop(event: () => void): this {
    modifierWithKey(this._modifiersWithKeys, ScrollOnScrollStopModifier.identity, ScrollOnScrollStopModifier, event);
    return this;
  }
  enablePaging(value: boolean): this {
    modifierWithKey(this._modifiersWithKeys, ScrollEnablePagingModifier.identity, ScrollEnablePagingModifier, value);
    return this;
  }
  scrollBar(value: BarState): this {
    if (value in BarState) {
      modifierWithKey(this._modifiersWithKeys, ScrollScrollBarModifier.identity, ScrollScrollBarModifier, value);
    } else {
      modifierWithKey(this._modifiersWithKeys, ScrollScrollBarModifier.identity, ScrollScrollBarModifier, undefined);
    }
    return this;
  }
  scrollBarColor(color: ResourceColor): this {
    modifierWithKey(this._modifiersWithKeys, ScrollScrollBarColorModifier.identity, ScrollScrollBarColorModifier, color);
    return this;
  }
  scrollBarWidth(value: string | number): this {
    modifierWithKey(this._modifiersWithKeys, ScrollScrollBarWidthModifier.identity, ScrollScrollBarWidthModifier, value);
    return this;
  }
  edgeEffect(value: EdgeEffect, options?: EdgeEffectOptions): this {
    let effect: ArkScrollEdgeEffect = new ArkScrollEdgeEffect();
    effect.value = value;
    effect.options = options;
    modifierWithKey(this._modifiersWithKeys, ScrollEdgeEffectModifier.identity, ScrollEdgeEffectModifier, effect);
    return this;
  }
  fadingEdge(value: boolean, options?: FadingEdgeOptions | undefined): this {
    let fadingEdge: ArkFadingEdge = new ArkFadingEdge();
    fadingEdge.value = value;
    fadingEdge.options = options;
    modifierWithKey(this._modifiersWithKeys, ScrollFadingEdgeModifier.identity, ScrollFadingEdgeModifier, fadingEdge);
    return this;
  }
  onScrollFrameBegin(callback: (offset: number, state: ScrollState) => { offsetRemain: number }): this {
    modifierWithKey(this._modifiersWithKeys, ScrollOnScrollFrameBeginModifier.identity, ScrollOnScrollFrameBeginModifier, callback);
    return this;
  }
  onWillScroll(callback: (xOffset: number, yOffset: number,
    scrollState: ScrollState, scrollSource: ScrollSource) => void | OffsetResult): this {
    modifierWithKey(this._modifiersWithKeys, ScrollOnWillScrollModifier.identity, ScrollOnWillScrollModifier, callback);
    return this;
  }

  onDidScroll(callback: (xOffset: number, yOffset: number, scrollState: ScrollState) => void): this {
    modifierWithKey(this._modifiersWithKeys, ScrollOnDidScrollModifier.identity, ScrollOnDidScrollModifier, callback);
    return this;
  }

  nestedScroll(value: NestedScrollOptions): ScrollAttribute {
    let options = new ArkNestedScrollOptions();
    if (value) {
      if (value.scrollForward) {
        options.scrollForward = value.scrollForward;
      }
      if (value.scrollBackward) {
        options.scrollBackward = value.scrollBackward;
      }
      modifierWithKey(this._modifiersWithKeys, ScrollNestedScrollModifier.identity, ScrollNestedScrollModifier, options);
    }
    return this;
  }
  enableScrollInteraction(value: boolean): ScrollAttribute {
    modifierWithKey(this._modifiersWithKeys, ScrollEnableScrollInteractionModifier.identity, ScrollEnableScrollInteractionModifier, value);
    return this;
  }
  friction(value: number | Resource): ScrollAttribute {
    modifierWithKey(this._modifiersWithKeys, ScrollFrictionModifier.identity, ScrollFrictionModifier, value);
    return this;
  }
  scrollSnap(value: ScrollSnapOptions): ScrollAttribute {
    let options = new ArkScrollSnapOptions();
    if (value) {
      if (value.snapAlign) {
        options.snapAlign = value.snapAlign;
      }
      if (value.snapPagination) {
        options.snapPagination = value.snapPagination;
      }
      if (value.enableSnapToStart) {
        options.enableSnapToStart = value.enableSnapToStart;
      }
      if (value.enableSnapToEnd) {
        options.enableSnapToEnd = value.enableSnapToEnd;
      }
      modifierWithKey(this._modifiersWithKeys, ScrollScrollSnapModifier.identity, ScrollScrollSnapModifier, options);
    }
    return this;
  }
  clip(value: boolean | CircleAttribute | EllipseAttribute | PathAttribute | RectAttribute): this {
    modifierWithKey(this._modifiersWithKeys, ScrollClipModifier.identity, ScrollClipModifier, value);
    return this;
  }
  initialOffset(value: OffsetOptions): ScrollAttribute {
    let options = new ArkScrollOffsetOptions();
    if (value) {
      if (value.xOffset) {
        options.xOffset = value.xOffset;
      }
      if (value.yOffset) {
        options.yOffset = value.yOffset;
      }
      modifierWithKey(this._modifiersWithKeys, ScrollInitialOffsetModifier.identity, ScrollInitialOffsetModifier, options);
    }
    return this;
  }
  flingSpeedLimit(value: number): this {
    modifierWithKey(this._modifiersWithKeys, ScrollFlingSpeedLimitModifier.identity, ScrollFlingSpeedLimitModifier, value);
    return this;
  }
}
// @ts-ignore
globalThis.Scroll.attributeModifier = function (modifier: ArkComponent): void {
  attributeModifierFunc.call(this, modifier, (nativePtr: KNode) => {
    return new ArkScrollComponent(nativePtr);
  }, (nativePtr: KNode, classType: ModifierType, modifierJS: ModifierJS) => {
    return new modifierJS.ScrollModifier(nativePtr, classType);
  });
};
