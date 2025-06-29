/*
 * Copyright 2025. Huawei Technologies Co., Ltd. All rights reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

// [Start sampleCode]
import { hilog } from '@kit.PerformanceAnalysisKit';

const TAG: string = 'Ads Demo-TimeOutHandler';

// Timeout interval, in milliseconds. You can change the value based on the actual situation.
const TIME_OUT_DELAY: number = 1000;

export class TimeOutHandler {
  isTimeOut: boolean;

  private timeOutIndex: number;

  constructor(handler: () => void) {
    hilog.info(0x0000, TAG, 'Create timeout handler');
    this.isTimeOut = false;
    this.timeOutIndex = setTimeout(() => {
      hilog.info(0x0000, TAG, 'Timeout');
      this.isTimeOut = true;
      handler();
    }, TIME_OUT_DELAY);
  }

  clear(): void {
    clearTimeout(this.timeOutIndex);
  }
}
// [End sampleCode]