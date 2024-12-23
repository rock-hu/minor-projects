/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TestSuite, TestCase } from '@rnoh/testerino';
import {Button} from 'react-native';

const SEVERITIES = ['log', 'info', 'warn', 'error', 'debug'] as const;

export const ConsoleTest = () => {
  return (
    <TestSuite name="Console">
      {SEVERITIES.map((severity, idx) => (
        <TestCase key={idx} itShould={`log message to console (${severity})`}>
          <Button
            title="log"
            onPress={() => console[severity](`console.${severity} test`)}
          />
        </TestCase>
      ))}
    </TestSuite>
  );
};
