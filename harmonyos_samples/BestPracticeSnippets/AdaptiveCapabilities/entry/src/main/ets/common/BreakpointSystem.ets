/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

export class BreakPointTypeOptionEntity<T> implements BreakPointTypeOption<T> {
  xs?: T;
  sm?: T;
  md?: T;
  lg?: T;
  xl?: T;
  xxl?: T;
}

declare interface BreakPointTypeOption<T> {
  xs?: T;
  sm?: T;
  md?: T;
  lg?: T;
  xl?: T;
  xxl?: T;
}

export class BreakPointType<T> {
  options: BreakPointTypeOption<T>;

  constructor(option: BreakPointTypeOption<T>) {
    this.options = option;
  }

  getValue(currentBreakPoint: string): T {
    if (currentBreakPoint === 'xs') {
      return this.options.xs as T;
    } else if (currentBreakPoint === 'sm') {
      return this.options.sm as T;
    } else if (currentBreakPoint === 'md') {
      return this.options.md as T;
    } else if (currentBreakPoint === 'lg') {
      return this.options.lg as T;
    } else if (currentBreakPoint === 'xl') {
      return this.options.xl as T;
    } else if (currentBreakPoint === 'xxl') {
      return this.options.xxl as T;
    } else {
      return this.options.xs as T;
    }
  }
}

class BreakpointEntity {
  name: string = '';
  size: number = 0;
  mediaQueryListener?: mediaquery.MediaQueryListener;
}

export class BreakpointSystem {
  private currentBreakpoint: string = 'md';
  private breakpoints: BreakpointEntity[] = [
    { name: 'xs', size: 0 },
    { name: 'sm', size: 320 },
    { name: 'md', size: 600 },
    { name: 'lg', size: 840 }
  ]

  private updateCurrentBreakpoint(breakpoint: string) {
    if (this.currentBreakpoint !== breakpoint) {
      this.currentBreakpoint = breakpoint;
      AppStorage.setOrCreate<string>('currentBreakpoint', this.currentBreakpoint);
      console.log('on current breakpoint: ' + this.currentBreakpoint);
    }
  }

  public register() {
    this.breakpoints.forEach((breakpoint: BreakpointEntity, index) => {
      let condition: string = '';
      if (index === this.breakpoints.length - 1) {
        condition = '(' + breakpoint.size + 'vp<=width' + ')';
      } else {
        condition = '(' + breakpoint.size + 'vp<=width<' + this.breakpoints[index + 1].size + 'vp)';
      }
      let uiContext = AppStorage.get('uiContext') as UIContext;
      breakpoint.mediaQueryListener = uiContext.getMediaQuery().matchMediaSync(condition);
      breakpoint.mediaQueryListener.on('change', (mediaQueryResult) => {
        if (mediaQueryResult.matches) {
          this.updateCurrentBreakpoint(breakpoint.name);
        }
      })
    })
  }

  public unregister() {
    this.breakpoints.forEach((breakpoint: BreakpointEntity) => {
      if (breakpoint != undefined && breakpoint.mediaQueryListener != undefined) {
        breakpoint.mediaQueryListener.off('change');
      }
    })
  }
}