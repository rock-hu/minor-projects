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

import { mediaquery } from '@kit.ArkUI';
import { BreakpointConstants } from '@ohos/constantsCommon';

const uiContext: UIContext | undefined = AppStorage.get('uiContext');

declare interface BreakPointTypeOption<T> {
  sm?: T,
  md?: T,
  lg?: T
}

export class BreakpointType<T> {
  options: BreakPointTypeOption<T>;

  constructor(option: BreakPointTypeOption<T>) {
    this.options = option;
  }

  getValue(currentPoint: string): T {
    if (currentPoint === 'sm') {
      return this.options.sm as T;
    }
    else if (currentPoint === 'md') {
      return this.options.md as T;
    } else {
      return this.options.lg as T;
    }
  }
}

export class BreakpointSystem {
  private currentBreakpoint: string = BreakpointConstants.BREAKPOINT_SM;
  private smListener: mediaquery.MediaQueryListener = uiContext!.getMediaQuery().matchMediaSync(BreakpointConstants.RANGE_SM);
  private mdListener: mediaquery.MediaQueryListener = uiContext!.getMediaQuery().matchMediaSync(BreakpointConstants.RANGE_MD);
  private lgListener: mediaquery.MediaQueryListener = uiContext!.getMediaQuery().matchMediaSync(BreakpointConstants.RANGE_LG);

  private updateCurrentBreakpoint(breakpoint: string): void {
    if (this.currentBreakpoint !== breakpoint) {
      this.currentBreakpoint = breakpoint;
      AppStorage.setOrCreate<string>(BreakpointConstants.CURRENT_BREAKPOINT, this.currentBreakpoint);
    }
  }

  private isBreakpointSM = (mediaQueryResult: mediaquery.MediaQueryResult): void => {
    if (mediaQueryResult.matches) {
      this.updateCurrentBreakpoint(BreakpointConstants.BREAKPOINT_SM);
    }
  }
  private isBreakpointMD = (mediaQueryResult: mediaquery.MediaQueryResult): void => {
    if (mediaQueryResult.matches) {
      this.updateCurrentBreakpoint(BreakpointConstants.BREAKPOINT_MD);
    }
  }
  private isBreakpointLG = (mediaQueryResult: mediaquery.MediaQueryResult): void => {
    if (mediaQueryResult.matches) {
      this.updateCurrentBreakpoint(BreakpointConstants.BREAKPOINT_LG);
    }
  }

  public register(): void {
    this.smListener = uiContext!.getMediaQuery().matchMediaSync(BreakpointConstants.RANGE_SM);
    this.smListener.on('change', this.isBreakpointSM);
    this.mdListener = uiContext!.getMediaQuery().matchMediaSync(BreakpointConstants.RANGE_MD);
    this.mdListener.on('change', this.isBreakpointMD);
    this.lgListener = uiContext!.getMediaQuery().matchMediaSync(BreakpointConstants.RANGE_LG);
    this.lgListener.on('change', this.isBreakpointLG);
  }

  public unregister(): void {
    this.smListener.off('change', this.isBreakpointSM);
    this.mdListener.off('change', this.isBreakpointMD);
    this.lgListener.off('change', this.isBreakpointLG);
  }
}