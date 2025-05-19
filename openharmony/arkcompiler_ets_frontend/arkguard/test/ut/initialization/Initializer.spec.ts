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

import {
  clearHistoryUnobfuscatedMap,
  historyAllUnobfuscatedNamesMap,
  historyUnobfuscatedPropMap,
  initObfuscationConfig
} from '../../../src/initialization/Initializer';
import {
  expect,
  assert
} from 'chai';
import {
  describe,
  it
} from 'mocha';
import {
  HvigorErrorInfo,
  MergedConfig,
} from '../../../src/ArkObfuscator';

describe('Tester Cases for <Initializer.ts>.', function () {
  it('Tester Cases for <clearHistoryUnobfuscatedMap>.', function () {
    historyAllUnobfuscatedNamesMap.set('key', {'prop1': 'aaa'});
    historyUnobfuscatedPropMap.set('key', ['value']);
    expect(historyAllUnobfuscatedNamesMap.size).to.equal(1);
    expect(historyUnobfuscatedPropMap.size).to.equal(1);
    clearHistoryUnobfuscatedMap();
    expect(historyAllUnobfuscatedNamesMap.size).to.equal(0);
    expect(historyUnobfuscatedPropMap.size).to.equal(0);
  });
});

function printObfLogger(errorInfo: string, errorCodeInfo: HvigorErrorInfo | string, level: string): void {
  switch (level) {
    case 'warn':
      console.warn(errorInfo);
      break;
    case 'error':
      console.error(errorInfo);
      break;
    default:
      break;
  }
}
const projectConfig = {
  'obfuscationOptions': {
    'selfConfig': {
      'ruleOptions': {
        'enable': true,
        'rules': ['./test/testData/obfuscation/Configs/bytecodeObf/bytecodeObf_enable.txt']
      },
      'consumerRules': [],
    },
    'dependencies': {
      'libraries': [],
      'hars': []
    },
    obfuscationCacheDir: ""
  }
};
interface ArkProjectConfig {
  isBytecodeObfEnabled?: boolean;
  isArkguardEnabled?: boolean;
  allowEtsAnnotations?: boolean;
  obfuscationMergedObConfig?: MergedConfig;
}

describe('test for set arkguard mode property correctly for arkProjectConfig', function () {
  describe('test for set arkguard mode property correctly for arkProjectConfig', () => {
    it('should set isBytecodeObfEnabled when enable bytecodeObf', () => {
      const arkProjectConfig: ArkProjectConfig = {};
      initObfuscationConfig(projectConfig, arkProjectConfig, printObfLogger);
      expect(arkProjectConfig.isBytecodeObfEnabled).to.be.true;
      expect(arkProjectConfig.isArkguardEnabled).to.be.false;
    });

    it('should set isArkguardEnabled when enable arkguardObf', () => {
      const arkProjectConfig: ArkProjectConfig = {};
      projectConfig.obfuscationOptions.selfConfig.ruleOptions.rules = ['./test/testData/obfuscation/Configs/bytecodeObf/arkguard_enable.txt'];
      initObfuscationConfig(projectConfig, arkProjectConfig, printObfLogger);
      expect(arkProjectConfig.isArkguardEnabled).to.be.true;
      expect(arkProjectConfig.isBytecodeObfEnabled).to.be.false;
    });

    it('should not set isArkguardEnabled and isBytecodeObfEnabled when obf is disabled', () => {
      const arkProjectConfig: ArkProjectConfig = {};
      projectConfig.obfuscationOptions.selfConfig.ruleOptions.enable = false;
      initObfuscationConfig(projectConfig, arkProjectConfig, printObfLogger);
      expect(arkProjectConfig.isBytecodeObfEnabled).to.be.undefined;
      expect(arkProjectConfig.isArkguardEnabled).to.be.undefined;
    });

    it('should not set enableEtsAnnotation if allowEtsAnnotations is enabled in arkProjectConfig', () => {
      const arkProjectConfig: ArkProjectConfig = {
        allowEtsAnnotations: true,
        obfuscationMergedObConfig: new MergedConfig(),
      };
      projectConfig.obfuscationOptions.selfConfig.ruleOptions.enable = true;
      projectConfig.obfuscationOptions.selfConfig.ruleOptions.rules = ['./test/testData/obfuscation/Configs/bytecodeObf/arkguard_enable.txt'];
      initObfuscationConfig(projectConfig, arkProjectConfig, printObfLogger);
      expect(arkProjectConfig.obfuscationMergedObConfig?.options.enableEtsAnnotation).to.be.true;
    });
  });
});
