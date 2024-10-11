import { AbsolutePath } from '../../core';
import {
  CodeGenerator,
  CodegenError,
  SpecSchema,
  SpecSchemaType,
  UberSchema,
} from '../core';
import { GlueCodeDataV1 } from './GlueCodeGenerator';
import { NativeModuleCodeGenerator } from './NativeModuleCodeGenerator';
import { ComponentCodeGeneratorArkTS } from './ComponentCodeGeneratorArkTS';

/**
 * Generates code for libraries built on top of RNOH's ArkTS architecture.
 */
export class UberGeneratorV1 implements CodeGenerator<UberSchema> {
  private glueCodeData: GlueCodeDataV1 = { components: [], turboModules: [] };

  constructor(
    private cppOutputPath: AbsolutePath,
    private etsOutputPath: AbsolutePath
  ) {}

  getGlueCodeData(): GlueCodeDataV1 {
    return this.glueCodeData;
  }

  generate(uberSchema: UberSchema) {
    const fileContentByPath = new Map<AbsolutePath, string>();
    const componentCodeGeneratorArkTS = new ComponentCodeGeneratorArkTS(
      this.cppOutputPath,
      this.etsOutputPath.copyWithNewSegment('components')
    );
    const nativeModuleCodeGenerator = new NativeModuleCodeGenerator(
      this.cppOutputPath,
      this.etsOutputPath.copyWithNewSegment('turboModules'),
      1
    );
    const generatorBySpecType = {
      Component: componentCodeGeneratorArkTS,
      NativeModule: nativeModuleCodeGenerator,
    } satisfies Record<SpecSchemaType, CodeGenerator<SpecSchema>>;
    for (const [_fileName, specSchema] of uberSchema
      .getSpecSchemaByFilenameMap()
      .entries()) {
      if (!(specSchema.type in generatorBySpecType)) {
        throw new CodegenError({
          whatHappened: `Could not find a code generator for a specification of type '${specSchema.type}'`,
          unexpected: true,
        });
      }
      generatorBySpecType[specSchema.type]
        .generate(specSchema)
        .forEach((fileContent, filePath) => {
          fileContentByPath.set(filePath, fileContent);
        });
    }
    this.glueCodeData = {
      components: componentCodeGeneratorArkTS.getGlueCodeData(),
      turboModules: nativeModuleCodeGenerator.getGlueCodeData(),
    };
    return fileContentByPath;
  }
}
