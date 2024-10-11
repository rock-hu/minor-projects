import { AbsolutePath } from '../../core';
import { CodeGenerator } from '../core';
import {
  ComponentCodeGeneratorCAPI,
  LibraryData,
} from './ComponentCodeGeneratorCAPI';
import { NativeModuleCodeGenerator } from './NativeModuleCodeGenerator';
import { GlueCodeDataV2 } from './GlueCodeGenerator';

/**
 * Generates code for libraries built on top of RNOH's C-API architecture.
 */
export class UberGeneratorV2 implements CodeGenerator<LibraryData[]> {
  private glueCodeData: GlueCodeDataV2 = new Map();

  constructor(
    private cppOutputPath: AbsolutePath,
    private etsOutputPath: AbsolutePath
  ) {}

  getGlueCodeData(): GlueCodeDataV2 {
    return this.glueCodeData;
  }

  generate(npmPackagesInfo: LibraryData[]) {
    const fileContentByPath: Map<AbsolutePath, string> = new Map();

    npmPackagesInfo.forEach(({ name, uberSchema }) => {
      this.glueCodeData.set(name, { components: [], turboModules: [] });
      const libraryGlueCodeData = this.glueCodeData.get(name)!;
      const libraryCppOutput = this.cppOutputPath.copyWithNewSegment(name);
      const componentCodeGeneratorCAPI = new ComponentCodeGeneratorCAPI(
        (filename) => {
          return libraryCppOutput.copyWithNewSegment(
            'react',
            'renderer',
            'components',
            name,
            filename
          );
        },
        (filename) => {
          return libraryCppOutput.copyWithNewSegment(
            'RNOH',
            'generated',
            'components',
            filename
          );
        }
      );
      componentCodeGeneratorCAPI
        .generate({ name, uberSchema })
        .forEach((fileContent, path) => {
          fileContentByPath.set(path, fileContent);
        });
      componentCodeGeneratorCAPI
        .getGlueCodeData()
        .forEach((componentGlueCodeData) => {
          libraryGlueCodeData.components.push(componentGlueCodeData);
        });

      for (const [_fileName, specSchema] of uberSchema
        .getSpecSchemaByFilenameMap()
        .entries()) {
        if (specSchema.type === 'NativeModule') {
          const nativeModuleCodeGenerator = new NativeModuleCodeGenerator(
            libraryCppOutput.copyWithNewSegment(
              'RNOH',
              'generated',
              'turbo_modules'
            ),
            this.etsOutputPath.copyWithNewSegment('turboModules'),
            2
          );
          nativeModuleCodeGenerator
            .generate(specSchema)
            .forEach((fileContent, filePath) => {
              fileContentByPath.set(filePath, fileContent);
            });
          nativeModuleCodeGenerator
            .getGlueCodeData()
            .forEach((turboModuleGlueCodeData) => {
              libraryGlueCodeData.turboModules.push(turboModuleGlueCodeData);
            });
        }
      }
    });

    return fileContentByPath;
  }
}
