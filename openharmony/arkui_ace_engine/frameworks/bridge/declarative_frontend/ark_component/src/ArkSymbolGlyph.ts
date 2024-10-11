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

/// <reference path='./import.ts' />
class SymbolFontColorModifier extends ModifierWithKey<object> {
    constructor(value: object) {
      super(value);
    }
    static identity: Symbol = Symbol('symbolGlyphFontColor');
    applyPeer(node: KNode, reset: boolean): void {
      if (reset) {
        getUINativeModule().symbolGlyph.resetFontColor(node);
      } else {
        getUINativeModule().symbolGlyph.setFontColor(node, this.value);
      }
    }
    
    checkObjectDiff(): boolean {
      if (isResource(this.stageValue) && isResource(this.value)) {
        let ret: boolean = !isResourceEqual(this.stageValue, this.value);
        return ret;
      } else {
        return true;
      }
    }
  }
    
  class SymbolFontSizeModifier extends ModifierWithKey<number | string | Resource> {
    constructor(value: number | string | Resource) {
      super(value);
    }
    static identity: Symbol = Symbol('symbolGlyphFontSize');
    applyPeer(node: KNode, reset: boolean): void {
      if (reset) {
        getUINativeModule().symbolGlyph.resetFontSize(node);
      } else {
        getUINativeModule().symbolGlyph.setFontSize(node, this.value);
      }
    }
  
    checkObjectDiff(): boolean {
      if (isResource(this.stageValue) && isResource(this.value)) {
        return !isResourceEqual(this.stageValue, this.value);
      } else {
        return true;
      }
    }
  }
    
  class SymbolFontWeightModifier extends ModifierWithKey<number | FontWeight | string> {
    constructor(value: number | FontWeight | string) {
      super(value);
    }
    static identity: Symbol = Symbol('symbolGlyphFontWeight');
    applyPeer(node: KNode, reset: boolean): void {
      if (reset) {
        getUINativeModule().symbolGlyph.resetFontWeight(node);
      } else {
        getUINativeModule().symbolGlyph.setFontWeight(node, this.value);
      }
    }
  
    checkObjectDiff(): boolean {
      if (isResource(this.stageValue) && isResource(this.value)) {
        return !isResourceEqual(this.stageValue, this.value);
      } else {
        return true;
      }
    }
  }
    
  class RenderingStrategyModifier extends ModifierWithKey<SymbolRenderingStrategy> {
    constructor(value: SymbolRenderingStrategy) {
      super(value);
    }
    static identity: Symbol = Symbol('symbolGlyphRenderingStrategy');
    applyPeer(node: KNode, reset: boolean): void {
      if (reset) {
        getUINativeModule().symbolGlyph.resetRenderingStrategy(node);
      } else {
        getUINativeModule().symbolGlyph.setRenderingStrategy(node, this.value);
      }
    }
  
    checkObjectDiff(): boolean {
      if (isResource(this.stageValue) && isResource(this.value)) {
        return !isResourceEqual(this.stageValue, this.value);
      } else {
        return true;
      }
    }
  }
  
  class EffectStrategyModifier extends ModifierWithKey<SymbolEffectStrategy> {
    constructor(value: SymbolEffectStrategy) {
      super(value);
    }
    static identity: Symbol = Symbol('symbolGlyphEffectStrategy');
    applyPeer(node: KNode, reset: boolean): void {
      if (reset) {
        getUINativeModule().symbolGlyph.resetEffectStrategy(node);
      } else {
        getUINativeModule().symbolGlyph.setEffectStrategy(node, this.value);
      }
    }
  
    checkObjectDiff(): boolean {
      if (isResource(this.stageValue) && isResource(this.value)) {
        return !isResourceEqual(this.stageValue, this.value);
      } else {
        return true;
      }
    }
  }

  class SymbolEffectModifier extends ModifierWithKey<ArkSymbolEffect> {
    constructor(value: ArkSymbolEffect) {
      super(value);
    }
    static identity: Symbol = Symbol('symbolGlyphSymbolEffect');
    applyPeer(node: KNode, reset: boolean): void {
      if (reset) {
        getUINativeModule().symbolGlyph.resetSymbolEffectOptions(node);
      } else {
        getUINativeModule().symbolGlyph.setSymbolEffectOptions(node, this.value.symbolEffect, this.value.action);
      }
    }
  }

  class SymbolContentModifier extends ModifierWithKey<Resource> {
    constructor(value: Resource) {
      super(value);
    }
    static identity: Symbol = Symbol('symbolContent');
    applyPeer(node: KNode, reset: boolean): void {
      if (reset) {
        getUINativeModule().symbolGlyph.resetSymbolGlyphInitialize(node);
      } else {
        getUINativeModule().symbolGlyph.setSymbolGlyphInitialize(node, this.value);
      }
    }
  }
    
  class ArkSymbolGlyphComponent extends ArkComponent implements SymbolGlyphAttribute {
    constructor(nativePtr: KNode, classType?: ModifierType) {
      super(nativePtr, classType);
    }
    initialize(value: Object[]): this {
      if (value[0] !== undefined) {
        modifierWithKey(this._modifiersWithKeys, SymbolContentModifier.identity, SymbolContentModifier, value[0]);
      } else {
        modifierWithKey(this._modifiersWithKeys, SymbolContentModifier.identity, SymbolContentModifier, undefined);
      }
      return this;
    }
    fontColor(value: object): SymbolGlyphAttribute {
      modifierWithKey(this._modifiersWithKeys, SymbolFontColorModifier.identity, SymbolFontColorModifier, value);
      return this;
    }
    fontSize(value: number | string | Resource): SymbolGlyphAttribute {
      modifierWithKey(this._modifiersWithKeys, SymbolFontSizeModifier.identity, SymbolFontSizeModifier, value);
      return this;
    }
    fontWeight(value: number | FontWeight | string): SymbolGlyphAttribute {
      let fontWeightStr: string = '400';
      if (isNumber(value)) {
        fontWeightStr = value.toString();
      } else if (isString(value)) {
        fontWeightStr = String(value);
      }
      modifierWithKey(this._modifiersWithKeys, SymbolFontWeightModifier.identity, SymbolFontWeightModifier, fontWeightStr);
      return this;
    }
    renderingStrategy(value: SymbolRenderingStrategy): SymbolGlyphAttribute {
      modifierWithKey(this._modifiersWithKeys, RenderingStrategyModifier.identity, RenderingStrategyModifier, value);
      return this;
    }
    effectStrategy(value: SymbolEffectStrategy): SymbolGlyphAttribute {
      modifierWithKey(this._modifiersWithKeys, EffectStrategyModifier.identity, EffectStrategyModifier, value);
      return this;
    }
    symbolEffect(effect: SymbolEffect, action?: boolean | number): SymbolGlyphAttribute {
      let symbolEffect = new ArkSymbolEffect();
      symbolEffect.symbolEffect = effect;
      symbolEffect.action = action;
      modifierWithKey(this._modifiersWithKeys, SymbolEffectModifier.identity, SymbolEffectModifier, symbolEffect);
      return this;
    }
  }
    
  // @ts-ignore
  globalThis.SymbolGlyph.attributeModifier = function (modifier: ArkComponent): void {
    attributeModifierFunc.call(this, modifier, (nativePtr: KNode) => {
      return new ArkSymbolGlyphComponent(nativePtr);
    }, (nativePtr: KNode, classType: ModifierType, modifierJS: ModifierJS) => {
      return new modifierJS.SymbolGlyphModifier(undefined, nativePtr, classType);
    });
  };