/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

declare class UIContext {
  instanceId_: number;
}

declare interface TouchEvent {
}

declare interface Position { x?: number; y?: number; }

declare type UpdateFunc = (elmtId: number, isFirstRender: boolean, param?: Object) => void;

interface UpdateFuncRecord {
  updateFunc: UpdateFunc;
  componentName: string;
  node?: object;
}

declare class ObserveV2 {
  static getObserve(): ObserveV2;
  public stopRecordDependencies(): void;
  public startRecordDependencies(cmp: BaseNode, id: number, doClearBinding: boolean): void;
}

declare class ConfigureStateMgmt {
  public static get instance(): ConfigureStateMgmt;
  public needsV2Observe(): boolean;
}

declare function wrapBuilder<Args extends Object[]>(
  builder: (...args: Args) => void
): WrappedBuilder<Args>;

declare class WrappedBuilder<Args extends Object[]> {
  builder: (...args: Args) => void;
  constructor(builder: (...args: Args) => void);
}

declare class __JSScopeUtil__ {
  static syncInstanceId(instanceId: number): void;
  static restoreInstanceId(): void;
}

declare interface Size {
  width: number;
  height: number;
}

declare enum XComponentType {
  SURFACE = 0,
  COMPONENT,
  TEXTURE
}

declare interface XComponentController { }
declare interface BuildOptions {
  nestingBuilderSupported?: boolean;
  lazyBuildSupported?: boolean;
}
declare abstract class ViewPU {
  id__(): number;
  aboutToUpdate?: (updateParams: Object) => void;
  updateStateVars(params: {}): void;
  aboutToReuseInternal(param?: Object): void;
  aboutToRecycleInternal(): void;
  updateDirtyElements(): void;
  forceCompleteRerender(deep?: boolean): void
}

/**
 * WeakRef
 * ref to an Object that does not prevent the Object from getting GC'ed
 * current version of tsc does not know about WeakRef
 * but Ark runtime supports it
 *
 */
declare class WeakRef<T extends Object> {
  constructor(o: T);
  deref(): T;
}

type RemovedElementInfo = { elmtId: number, tag: string };

declare class UINodeRegisterProxy {
  public static instance_: UINodeRegisterProxy;
  public removeElementsInfo_: Array<number>;
  public static ElementIdToOwningViewPU_: Map<number, WeakRef<JSBuilderNode>>;
  public unregisterElmtIdsFromIViews(): void;
  private obtainDeletedElmtIds(): void;
  public static unregisterElmtIdsFromIViews(): void;
  public static obtainDeletedElmtIds(): void;
  public static unregisterRemovedElmtsFromViewPUs(removedElements: Array<number>): void;
}

declare interface RegisterParams {
  name: string;
  idOfNode: Symbol
}

declare class FinalizationRegistry {
  constructor(fun: (arg0: RegisterParams | number) => void);
  register(target: BuilderNode | FrameNode, heldValue: RegisterParams | number): undefined;
}

declare function getUINativeModule(): any;

declare function applyUIAttributes(modifier: AttributeModifier<CommonAttribute>, nativeNode: NodePtr, component: ArkComponent): void;

declare interface CommonAttribute { }

declare interface AttributeModifier<T> { }

declare enum ModifierType {
  ORIGIN = 0,
  STATE = 1,
  FRAME_NODE = 2,
}

declare class ArkComponent {
  nativePtr: NodePtr;
  constructor(nativePtr: NodePtr, classType?: ModifierType);
  setNodePtr(noed: NodePtr);
  setInstanceId(instanceId: number): void;
  initialize(...args: Object[]);
  allowChildCount?(): number;
  allowChildTypes?(): string[];
}
declare class ArkTextInputComponent extends ArkComponent {}

declare class ArkTextComponent extends ArkComponent {}

declare class ArkColumnComponent extends ArkComponent {}

declare class ArkRowComponent extends ArkComponent {}

declare class ArkStackComponent extends ArkComponent {}

declare class ArkGridRowComponent extends ArkComponent {}

declare class ArkGridColComponent extends ArkComponent {}

declare class ArkBlankComponent extends ArkComponent {}

declare class ArkImageComponent extends ArkComponent {}

declare class ArkFlexComponent extends ArkComponent {}

declare class ArkSwiperComponent extends ArkComponent {}

declare class ArkProgressComponent extends ArkComponent {}

declare class ArkScrollComponent extends ArkComponent {}

declare class ArkRelativeContainerComponent extends ArkComponent {}

declare class ArkListComponent extends ArkComponent {}

declare class ArkListItemComponent extends ArkComponent {}

declare class ArkDividerComponent extends ArkComponent {}

declare class ArkLoadingProgressComponent extends ArkComponent {}

declare class ArkSearchComponent extends ArkComponent {}

declare class ArkButtonComponent extends ArkComponent {}

declare class ArkXComponentComponent extends ArkComponent {}

declare class ArkListItemGroupComponent extends ArkComponent {}

declare class ArkWaterFlowComponent extends ArkComponent {}

declare class ArkFlowItemComponent extends ArkComponent {}

declare class ArkSymbolGlyphComponent extends ArkComponent {}

declare class ArkQRCodeComponent extends ArkComponent {}

declare class ArkBadgeComponent extends ArkComponent {}

declare class ArkGridComponent extends ArkComponent {}

declare class ArkGridItemComponent extends ArkComponent {}

declare class ArkTextClockComponent extends ArkComponent {}

declare class ArkTextTimerComponent extends ArkComponent {}

declare class ArkMarqueeComponent extends ArkComponent {}

declare class ArkTextAreaComponent extends ArkComponent {}

declare class ArkCheckboxComponent extends ArkComponent {}

declare class ArkCheckboxGroupComponent extends ArkComponent {}

declare class ArkRadioComponent extends ArkComponent {}

declare class ArkRatingComponent extends ArkComponent {}

declare class ArkSliderComponent extends ArkComponent {}

declare class ArkSelectComponent extends ArkComponent {}

declare class ArkToggleComponent extends ArkComponent {}

declare class UICommonEvent {
  private _nodePtr: NodePtr;
  private _instanceId: number;
  constructor(nodePtr: NodePtr);
  setInstanceId(instanceId: number): void;
  setNodePtr(nodePtr: NodePtr): void;
}

declare class ModifierWithKey<T extends number | string | boolean | object> {
  stageValue?: T;
  value?: T;
  constructor(value: T);
  applyStage(node: NodePtr): boolean;
  applyPeer(node: NodePtr, reset: boolean): void;
  checkObjectDiff(): boolean;
}

declare class NativeStrongRef {
  getNativeHandle(): NodePtr;
  dispose(): void;
}

declare class NativeWeakRef {
  invalid(): boolean;
  upgrade(): NativeStrongRef;
  getNativeHandle(): NodePtr;
  dispose?(): void;
}

declare class NativeUtils {
  createNativeStrongRef(nodePtr: NodePtr): NativeStrongRef;
  createNativeWeakRef(nodePtr: NodePtr): NativeWeakRef;
}

declare function __getCustomProperty__(nodeId: number, key: string): Object | undefined;
declare function __getCustomPropertyString__(nodeId: number, key: string): string | undefined;
