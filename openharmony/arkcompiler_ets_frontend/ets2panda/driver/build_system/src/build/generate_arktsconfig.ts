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

import * as path from 'path';
import * as fs from 'fs';

import {
  Logger,
  LogData,
  LogDataFactory
} from '../logger';
import {
  ErrorCode
} from '../error_code';
import {
  changeFileExtension,
  ensurePathExists,
  isSubPathOf,
  toUnixPath
} from '../utils';
import {
  BuildConfig,
  ModuleInfo,
  PathsConfig
} from '../types';
import {
  LANGUAGE_VERSION,
  SYSTEM_SDK_PATH_FROM_SDK,
} from '../pre_define';

interface DynamicPathItem {
  language: string,
  declPath: string,
  ohmUrl: string
}

interface ArkTSConfigObject {
  compilerOptions: {
    package: string,
    baseUrl: string,
    paths: Record<string, string[]>;
    dependencies: string[] | undefined;
    entry?: string;
    dynamicPaths: Record<string, DynamicPathItem>;
    useEmptyPackage?: boolean;
  }
};

export class ArkTSConfigGenerator {
  private static instance: ArkTSConfigGenerator | undefined;
  private stdlibStdPath: string;
  private stdlibEscompatPath: string;
  private systemSdkPath: string;
  private externalApiPaths: string[];

  private moduleInfos: Map<string, ModuleInfo>;
  private pathSection: Record<string, string[]>;

  private logger: Logger;

  private constructor(buildConfig: BuildConfig, moduleInfos: Map<string, ModuleInfo>) {
    let pandaStdlibPath: string = buildConfig.pandaStdlibPath ??
                                  path.resolve(buildConfig.pandaSdkPath!!, 'lib', 'stdlib');
    this.stdlibStdPath = path.resolve(pandaStdlibPath, 'std');
    this.stdlibEscompatPath = path.resolve(pandaStdlibPath, 'escompat');
    this.systemSdkPath = path.resolve(buildConfig.buildSdkPath, SYSTEM_SDK_PATH_FROM_SDK);
    this.externalApiPaths = buildConfig.externalApiPaths;

    this.moduleInfos = moduleInfos;
    this.pathSection = {};

    this.logger = Logger.getInstance();
  }

  public static getInstance(buildConfig?: BuildConfig, moduleInfos?: Map<string, ModuleInfo>): ArkTSConfigGenerator {
    if (!ArkTSConfigGenerator.instance) {
      if (!buildConfig || !moduleInfos) {
        throw new Error(
          'buildConfig and moduleInfos is required for the first instantiation of ArkTSConfigGenerator.');
      }
      ArkTSConfigGenerator.instance = new ArkTSConfigGenerator(buildConfig, moduleInfos);
    }
    return ArkTSConfigGenerator.instance;
  }

  public static destroyInstance(): void {
    ArkTSConfigGenerator.instance = undefined;
  }

  private generateSystemSdkPathSection(pathSection: Record<string, string[]>): void {
    function traverse(currentDir: string, relativePath: string = '', isExcludedDir: boolean = false, allowedExtensions: string[] = ['.d.ets']): void {
      const items = fs.readdirSync(currentDir);
      for (const item of items) {
        const itemPath = path.join(currentDir, item);
        const stat = fs.statSync(itemPath);
        const isAllowedFile = allowedExtensions.some(ext => item.endsWith(ext));
        if (stat.isFile() && !isAllowedFile) {
          continue;
        }

        if (stat.isFile()) {
          const basename = path.basename(item, '.d.ets');
          const key = isExcludedDir ? basename : (relativePath ? `${relativePath}.${basename}` : basename);
          pathSection[key] = [changeFileExtension(itemPath, '', '.d.ets')];
        }
        if (stat.isDirectory()) {
          // For files under api dir excluding arkui/runtime-api dir,
          // fill path section with `"pathFromApi.subdir.fileName" = [${absolute_path_to_file}]`;
          // For @koalaui files under arkui/runtime-api dir,
          // fill path section with `"fileName" = [${absolute_path_to_file}]`.
          const isCurrentDirExcluded = path.basename(currentDir) === 'arkui' && item === 'runtime-api';
          const newRelativePath = isCurrentDirExcluded ? '' : (relativePath ? `${relativePath}.${item}` : item);
          traverse(path.resolve(currentDir, item), newRelativePath, isCurrentDirExcluded || isExcludedDir);
        }
      }
    }

    if (this.externalApiPaths && this.externalApiPaths.length !== 0) {
      this.externalApiPaths.forEach((sdkPath: string) => {
        fs.existsSync(sdkPath) ? traverse(sdkPath) : this.logger.printWarn(`sdk path ${sdkPath} not exist.`);
      });
    } else {
      // Search openharmony sdk only, we keep them for ci compatibility.
      let apiPath: string = path.resolve(this.systemSdkPath, 'api');
      fs.existsSync(apiPath) ? traverse(apiPath) : this.logger.printWarn(`sdk path ${apiPath} not exist.`);

      let arktsPath: string = path.resolve(this.systemSdkPath, 'arkts');
      fs.existsSync(arktsPath) ? traverse(arktsPath) : this.logger.printWarn(`sdk path ${arktsPath} not exist.`);

      let kitsPath: string = path.resolve(this.systemSdkPath, 'kits');
      fs.existsSync(kitsPath) ? traverse(kitsPath) : this.logger.printWarn(`sdk path ${kitsPath} not exist.`);
    }
  }

  private getPathSection(): Record<string, string[]> {
    if (Object.keys(this.pathSection).length !== 0) {
        return this.pathSection;
    }

    this.pathSection.std = [this.stdlibStdPath];
    this.pathSection.escompat = [this.stdlibEscompatPath];

    this.generateSystemSdkPathSection(this.pathSection);

    this.moduleInfos.forEach((moduleInfo: ModuleInfo, packageName: string) => {
      if (moduleInfo.language !== LANGUAGE_VERSION.ARKTS_1_2 && moduleInfo.language !== LANGUAGE_VERSION.ARKTS_HYBRID) {
        return;
      }
      if (!moduleInfo.entryFile) {
        return;
      }
      this.handleEntryFile(moduleInfo);
    });
    return this.pathSection;
  }

  private handleEntryFile(moduleInfo: ModuleInfo): void {
    try {
      const stat = fs.statSync(moduleInfo.entryFile);
      if (!stat.isFile()) {
        return;
      }
      const entryFilePath = moduleInfo.entryFile;
      const firstLine = fs.readFileSync(entryFilePath, 'utf-8').split('\n')[0];
      // If the file is an ArkTS 1.2 implementation, configure the path in pathSection.
      if (moduleInfo.language === LANGUAGE_VERSION.ARKTS_1_2 || moduleInfo.language === LANGUAGE_VERSION.ARKTS_HYBRID && firstLine.includes('use static')) {
        this.pathSection[moduleInfo.packageName] = [
          path.resolve(moduleInfo.moduleRootPath, moduleInfo.sourceRoots[0])
        ];
      }
    } catch (error) {
      const logData: LogData = LogDataFactory.newInstance(
        ErrorCode.BUILDSYSTEM_HANDLE_ENTRY_FILE,
        `Error handle entry file for module ${moduleInfo.packageName}`
      );
      this.logger.printError(logData);
    }
  }

  private getDependenciesSection(moduleInfo: ModuleInfo, dependenciesSection: string[]): void {
    let depModules: Map<string, ModuleInfo> = moduleInfo.staticDepModuleInfos;
    depModules.forEach((depModuleInfo: ModuleInfo) => {
      dependenciesSection.push(depModuleInfo.arktsConfigFile);
    });
  }

  private getOhmurl(file: string, moduleInfo: ModuleInfo): string {
    let unixFilePath: string = file.replace(/\\/g, '/');
    let ohmurl: string = moduleInfo.packageName + '/' + unixFilePath;
    return changeFileExtension(ohmurl, '');
  }

  private getDynamicPathSection(moduleInfo: ModuleInfo, dynamicPathSection: Record<string, DynamicPathItem>): void {
    let depModules: Map<string, ModuleInfo> = moduleInfo.dynamicDepModuleInfos;

    depModules.forEach((depModuleInfo: ModuleInfo) => {
      if (!depModuleInfo.declFilesPath || !fs.existsSync(depModuleInfo.declFilesPath)) {
        console.error(`Module ${moduleInfo.packageName} depends on dynamic module ${depModuleInfo.packageName}, but
          decl file not found on path ${depModuleInfo.declFilesPath}`);
        return;
      }
      let declFilesObject = JSON.parse(fs.readFileSync(depModuleInfo.declFilesPath, 'utf-8'));
      Object.keys(declFilesObject.files).forEach((file: string)=> {
        let ohmurl: string = this.getOhmurl(file, depModuleInfo);
        dynamicPathSection[ohmurl] = {
          language: 'js',
          declPath: declFilesObject.files[file].declPath,
          ohmUrl: declFilesObject.files[file].ohmUrl
        };

        let absFilePath: string = path.resolve(depModuleInfo.moduleRootPath, file);
        let entryFileWithoutExtension: string = changeFileExtension(depModuleInfo.entryFile, '');
        if (absFilePath === entryFileWithoutExtension) {
          dynamicPathSection[depModuleInfo.packageName] = dynamicPathSection[ohmurl];
        }
      });
    });
  }

  public writeArkTSConfigFile(
    moduleInfo: ModuleInfo,
    enableDeclgenEts2Ts: boolean,
    buildConfig: BuildConfig
  ): void {
    if (!moduleInfo.sourceRoots || moduleInfo.sourceRoots.length === 0) {
      const logData: LogData = LogDataFactory.newInstance(
        ErrorCode.BUILDSYSTEM_SOURCEROOTS_NOT_SET_FAIL,
        'SourceRoots not set from hvigor.'
      );
      this.logger.printErrorAndExit(logData);
    }
    let pathSection = this.getPathSection();
    let dependenciesSection: string[] = [];
    this.getDependenciesSection(moduleInfo, dependenciesSection);
    this.getAllFilesToPathSectionForHybrid(moduleInfo, buildConfig);

    let dynamicPathSection: Record<string, DynamicPathItem> = {};

    if (!enableDeclgenEts2Ts) {
      this.getDynamicPathSection(moduleInfo, dynamicPathSection);
    }

    let baseUrl: string = path.resolve(moduleInfo.moduleRootPath, moduleInfo.sourceRoots[0]);
    if (buildConfig.paths) {
      Object.entries(buildConfig.paths).map(([key, value]) => {
        pathSection[key] = value
      });
    }
    let arktsConfig: ArkTSConfigObject = {
      compilerOptions: {
        package: moduleInfo.packageName,
        baseUrl: baseUrl,
        paths: pathSection,
        dependencies: dependenciesSection.length === 0 ? undefined : dependenciesSection,
        entry: moduleInfo.entryFile,
        dynamicPaths: dynamicPathSection
      }
    };

    if (moduleInfo.entryFile && moduleInfo.language === LANGUAGE_VERSION.ARKTS_HYBRID) {
      const entryFilePath = moduleInfo.entryFile;
      const stat = fs.statSync(entryFilePath);
      if (fs.existsSync(entryFilePath) && stat.isFile()) {
        const firstLine = fs.readFileSync(entryFilePath, 'utf-8').split('\n')[0];
        // If the entryFile is not an ArkTS 1.2 implementation, remove the entry property field.
        if (!firstLine.includes('use static')) {
          delete arktsConfig.compilerOptions.entry;
        }
      }
    }

    if (moduleInfo.frameworkMode) {
      arktsConfig.compilerOptions.useEmptyPackage = moduleInfo.useEmptyPackage;
    }

    ensurePathExists(moduleInfo.arktsConfigFile);
    fs.writeFileSync(moduleInfo.arktsConfigFile, JSON.stringify(arktsConfig, null, 2), 'utf-8');
  }

  public getAllFilesToPathSectionForHybrid(
    moduleInfo: ModuleInfo,
    buildConfig: BuildConfig
  ): void {
    if (moduleInfo?.language !== LANGUAGE_VERSION.ARKTS_HYBRID) {
      return;
    }

    const projectRoot = toUnixPath(buildConfig.projectRootPath) + '/';
    const moduleRoot = toUnixPath(moduleInfo.moduleRootPath);

    for (const file of buildConfig.compileFiles) {
      const unixFilePath = toUnixPath(file);

      if (!isSubPathOf(unixFilePath, moduleRoot)) {
        continue;
      }

      let relativePath = unixFilePath.startsWith(projectRoot)
        ? unixFilePath.substring(projectRoot.length)
        : unixFilePath;
      const keyWithoutExtension = relativePath.replace(/\.[^/.]+$/, '');
      this.pathSection[keyWithoutExtension] = [file];
    }
  }
}
