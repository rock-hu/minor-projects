/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

/**
 * NativeViewPartialUpdate aka JSViewPartialUpdate C++ class exposed to JS
 * 
 *  all definitions in this file are framework internal
 */
declare class NativeViewPartialUpdate {
  constructor(thisObj: object);
  markNeedUpdate(): void;
  findChildById(compilerAssignedUniqueChildId: string): View;
  syncInstanceId(): void;
  isFirstRender(): boolean;
  restoreInstanceId(): void;
  // create for NON @Reusable/@ReusableV2 components
  static create(newView: NativeViewPartialUpdate): void;

  // createRecycle for @Reusable/@ReusableV2 components
  static createRecycle(componentCall: any, isRecycling: boolean,
    reuseId: string, callback: () => void): void;
  finishUpdateFunc(elmtId: number): void;
  isLazyItemRender(elmtId : number): boolean;
  setCardId(cardId: number): void;
  getCardId(): number;
  resetRecycleCustomNode(): void;
  sendStateInfo(stateInfo: string): void;
  setIsV2(isV2: boolean): void;
  allowReusableV2Descendant(): boolean;
  getInstanceId(): number;
  markStatic(): void;
  elmtIdExists(elmtId: number): boolean;
  findChildByIdForPreview(viewId: number): object;
  queryNavDestinationInfo(isInner: boolean | undefined): object;
  queryNavigationInfo(): object;
  queryRouterPageInfo(): object;
  getUIContext(): object;
  getUniqueId(): number;
  getDialogController(): object;
}
