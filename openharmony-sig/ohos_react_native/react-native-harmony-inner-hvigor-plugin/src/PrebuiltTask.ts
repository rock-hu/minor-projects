/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { OhosHarContext } from '@ohos/hvigor-ohos-plugin';
import fse from 'fs-extra';
import fs from 'node:fs';
import { BuildOptionSetOfABC, BuildOptionSetOfRelease, BuildProfile, CMakeContent } from './Template';

// RNOH module path
const RNOH_PATH = 'react_native_openharmony';
// main path of RNOH
const MAIN_PATH = `${RNOH_PATH}/src/main`;
// build mode: release
const RELEASE_BUILD_MODE = 'release';
// product name: abc
const ABC_PRODUCT_NAME = 'abc';
// build-profile.json5 content type
const BUILD_PROFILE_TYPE = {
  DEFAULT: 0,
  ABC: 1,
  RELEASE: 2,
};

/**
 * Logger interface.
 * @interface Logger
 * @method info Log an informational message.
 * @method warn Log a warning message.
 * @method error Log an error message.
 */
export interface Logger {
  info(message: string): void;

  warn(message: string): void;

  error(message: string): void;
}

interface Subtask {
  run(): void;
}

/**
 * Adjust the build-profile.json5 file of RNOH according to the
 * mode(eg: ABC, Release, or default).
 */
class OverwriteBuildProfileSubtask implements Subtask {
  constructor(
    private context: OhosHarContext,
    private logger: Logger,
    private type: number,
  ) {}
  
  run(): void {
    const buildProfileOpt = Object.assign({}, BuildProfile);
    if (this.type === BUILD_PROFILE_TYPE.ABC) {
      // @ts-ignore
      buildProfileOpt.buildOptionSet = BuildOptionSetOfABC;
    } else if (this.type === BUILD_PROFILE_TYPE.RELEASE) {
      // @ts-ignore
      buildProfileOpt.buildOptionSet = BuildOptionSetOfRelease;
    }
    // @ts-ignore
    this.context.setBuildProfileOpt(buildProfileOpt);
    this.logger.info(`[OverwriteBuildProfile]\nbuild-profile.json5 has been successfully overwritten.`);
  }
}

/**
 * Adjust the CMakeLists.txt of RNOH based on whether it is in release mode.
 */
class OverwriteMakefileSubtask implements Subtask {
  constructor(
    private mainPath: string,
    private logger: Logger,
    private isRealseMode: boolean,
  ) {}

  run(): void {
    fs.readFile(`${this.mainPath}/cpp/CMakeLists.txt`, 'utf8', (err, data) => {
      if (err) {
        this.logger.error(`[OverwriteMakefile]\n${JSON.stringify(err)}`);
        return;
      }
      if (this.isRealseMode && !/if\(USE_HERMES\)/gm.test(data)) {
        this.logger.info(`[OverwriteMakefile]\nThe CMakeLists.txt has been converted to release mode.`);
        return;
      }
      if (!this.isRealseMode && /if\(USE_HERMES\)/gm.test(data)) {
        this.logger.info(`[OverwriteMakefile]\nThe CMakeLists.txt has been converted to default mode.`)
        return;
      }

      let replaceData = ''
      if (this.isRealseMode) {
        replaceData = data
          .replace(/if\(USE_HERMES\)([\s\S]*?)endif\(\)/, 'add_hermes_executor(OFF)\nadd_jsvm_executor(OFF)')
          .replace(/if\(\"\$ENV\{RNOH_C_API_ARCH\}\" STREQUAL \"1\"\)([\s\S]*?)endif\(\)/, '$1');
      } else {
        replaceData = data
          .replace(/add_hermes_executor\(OFF\)\s*add_jsvm_executor\(OFF\)/, 'if(USE_HERMES)\n    add_hermes_executor(ON)\nelse()\n    add_jsvm_executor(ON)\nendif()');
      }
      fs.writeFile(`${this.mainPath}/cpp/CMakeLists.txt`, replaceData, 'utf8', err => {
        if (err) {
          this.logger.error(`[OverwriteMakefile]\n${JSON.stringify(err)}`);
          return;
        }
        this.logger.info(`[OverwriteMakefile]\nSuccessfully converted CMakeLists.txt to ${this.isRealseMode ? 'release' : 'default'} mode.`);
      });
    });
  }
}

const filterFileByExt = (dir: string, exts: string[] = [], target: string, logger: Logger) => {
  const files = fs.readdirSync(dir);
  files.forEach(filename => {
    const stat = fs.lstatSync(`${dir}/${filename}`);
    if (`${dir}/${filename}` === target) {
      // Skip the target folder
    } else if (stat.isDirectory()) {
      filterFileByExt(`${dir}/${filename}`, exts, `${target}/${filename}`, logger);
    } else {
      for (let j = 0; j < exts.length; j++) {
        let ext = exts[j];
        if (filename.split('.').pop()?.toLowerCase() === ext.trim().toLowerCase()) {
          try {
            fse.copySync(`${dir}/${filename}`, `${target}/${filename}`);
          } catch (err) {
            logger.error(`${JSON.stringify(err)}`);
          }
          break;
        }
      }
    }
  });
}

/**
 * Extract all header files from the RNOH source code.
 */
class ExtractHeadFileSubtask implements Subtask {
  constructor(
    private mainPath: string,
    private logger: Logger,
  ) {}

  run(): void {
    this.logger.info('[ExtractHeadFile]');
    filterFileByExt(`${this.mainPath}/cpp`, ['h', 'hpp', 'ipp'], `${this.mainPath}/cpp/include`, this.logger);
  }
}

class CopyOtherFileSubtask implements Subtask {
  constructor(
    private mainPath: string,
    private logger: Logger,
  ) {}

  run(): void {
    const sourceList = [
      'RNOHAppNapiBridge.cpp',
    ];
    sourceList.forEach(filePath => {
      try {
        fse.copySync(`${this.mainPath}/cpp/${filePath}`, `${this.mainPath}/cpp/include/${filePath}`);
      } catch (err) {
        this.logger.error(`[CopyOtherFile]\n${JSON.stringify(err)}`);
      }
    });

    fse.writeFile(`${this.mainPath}/cpp/include/react-native-harmony.cmake`, CMakeContent).catch((err) => {
      this.logger.error(`[CopyOtherFile]\n${JSON.stringify(err)}`);
    });
  }
}

/**
 * Delete a Specified Folder.
 */
class CleanSubtask implements Subtask {
  constructor(
    private folder: string,
    private logger: Logger,
  ) {}

  run(): void {
    try {
      fse.removeSync(this.folder);
      this.logger.info(`[Clean]\nFolder(${this.folder}) deleted successfully`);
    } catch (err) {
      this.logger.error(`[Clean]\nError while deleting folder(${this.folder}):\n${JSON.stringify(err)}`);
    }
  }
}

export type RealsePrebuildOptions = {
  productName: string;
  buildMode: string;
};

export class RealsePrebuildTask {
  constructor(
    private logger: Logger,
    private context: OhosHarContext,
    private options: RealsePrebuildOptions
  ) {}
  
  run(): void {
    const { productName, buildMode } = this.options;
    const type = buildMode === RELEASE_BUILD_MODE ?
                 (productName === ABC_PRODUCT_NAME ? BUILD_PROFILE_TYPE.ABC : BUILD_PROFILE_TYPE.RELEASE) :
                 BUILD_PROFILE_TYPE.DEFAULT;
    const isRelease = type !== BUILD_PROFILE_TYPE.DEFAULT;
    
    let subtasks: Subtask[] = [
      new OverwriteBuildProfileSubtask(this.context, this.logger, type),
      new OverwriteMakefileSubtask(MAIN_PATH, this.logger, isRelease),
      new CleanSubtask(`${RNOH_PATH}/generated`, this.logger)
    ];
    if (isRelease) {
      subtasks = subtasks.concat([
        new ExtractHeadFileSubtask(MAIN_PATH, this.logger),
        new CopyOtherFileSubtask(MAIN_PATH, this.logger)
      ]);
    } else {
      subtasks.push(new CleanSubtask(`${MAIN_PATH}/cpp/include`, this.logger));
    }
    
    subtasks.forEach((subtask) => subtask.run());
  }
}
