/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

import { BasePrecreateView } from '../rn/';

@Component
export default struct PrecreateRN {
  private bundlePath = 'bundle/bp/details.harmony.bundle'
  private moduleName = 'Details'
  container: Record<string, string | number> = {
    "backgroundColor": '#E6E6E6',
    "flex": 1,
    "padding": 20,
  }
  apiFontSize: Record<string, string | number> = {
    'fontSize': 30,
    'color': 'white'
  }
  styles: Record<string, object> = {
    'container': this.container,
    'apiFontSize': this.apiFontSize
  }
  initProps: Record<string, string | object> = {
    'stringParam': 'ArkTS传递给RN的参数',
    'styles': this.styles
  };

  build() {
    NavDestination() {
      Column() {
        BasePrecreateView({
          appKey: this.moduleName,
          bundlePath: this.bundlePath,
          initialProps: this.initProps
        })
      }
      .width('100%')
      .height('100%')
    }
    .hideTitleBar(true)
  }
}
