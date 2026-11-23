/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { AbsolutePath } from '../core';

export abstract class CliExecutor {
  abstract run(
    command: string,
    options?: {
      args?: Record<string, string | number | boolean>;
      cwd?: AbsolutePath;
    }
  ): string;

  protected stringifyCliArgs(
    args: Record<string, string | number | boolean>
  ): string {
    return Object.entries(args)
      .filter(([_, value]) => {
        if (typeof value === 'boolean') {
          return value;
        }
        return value !== undefined && value !== null;
      })
      .map(([key, value]) => {
        const formattedKey = toKebabCase(key);

        if (typeof value === 'boolean') {
          return `--${formattedKey}`;
        }

        return `--${formattedKey} ${value}`;
      })
      .join(' ');
  }
}

function toKebabCase(str: string): string {
  return str
    .replace(/([a-z])([A-Z])/g, '$1-$2')
    .replace(/[\s_]+/g, '-')
    .toLowerCase();
}