import RNCodegen from '@react-native/codegen/lib/generators/RNCodegen';
import { AbsolutePath } from '../../core';
import { CodeGenerator, UberSchema } from '../core';
import {
  BaseComponentInstanceHTemplate,
  ComponentJSIBinderHTemplate,
} from '../templates';
import { GlueCodeComponentDataV2 } from './GlueCodeGenerator';

/**
 * NPM package that extends RNOH capabilities.
 */
export type LibraryData = {
  /**
   * C++ directory name which is used in #include statements
   */
  name: string;
  uberSchema: UberSchema;
};

export class ComponentCodeGeneratorCAPI implements CodeGenerator<LibraryData> {
  private glueCodeComponentsData: GlueCodeComponentDataV2[] = [];

  constructor(
    /// "shared" — platform independent code that can be shared between platforms
    private onGetSharedOutputPath: (filename: string) => AbsolutePath,
    private onGetRNOHOutputPath: (filename: string) => AbsolutePath
  ) {}

  getGlueCodeData() {
    return this.glueCodeComponentsData;
  }

  generate({
    name: libraryName,
    uberSchema,
  }: LibraryData): Map<AbsolutePath, string> {
    const fileContentByPath = new Map<AbsolutePath, string>();
    const generatorFns = [
      RNCodegen.allGenerators.generatePropsH,
      RNCodegen.allGenerators.generatePropsCpp,
      RNCodegen.allGenerators.generateEventEmitterH,
      RNCodegen.allGenerators.generateEventEmitterCpp,
      RNCodegen.allGenerators.generateComponentDescriptorH,
      RNCodegen.allGenerators.generateStateH,
      RNCodegen.allGenerators.generateStateCpp,
      RNCodegen.allGenerators.generateShadowNodeH,
      RNCodegen.allGenerators.generateShadowNodeCpp,
    ];
    generatorFns.forEach((generate) => {
      const filesOutput = generate(
        libraryName,
        uberSchema.getValue(),
        'UNUSED_packageName',
        true
      );
      filesOutput.forEach((fileContent, filename) => {
        fileContentByPath.set(
          this.onGetSharedOutputPath(filename),
          fileContent
        );
      });
    });
    for (const [filename, schema] of uberSchema
      .getSpecSchemaByFilenameMap()
      .entries()) {
      if (schema.type === 'Component') {
        Object.entries(schema.components).map(([componentName, shape]) => {
          const componentJSIBinderHTemplate = new ComponentJSIBinderHTemplate(
            componentName,
            2
          );
          const componentInstanceSpecHTemplate =
            new BaseComponentInstanceHTemplate(componentName, libraryName);
          shape.props.forEach((prop) => {
            componentJSIBinderHTemplate.addProp({
              name: prop.name,
              isObject: prop.typeAnnotation.type === 'ObjectTypeAnnotation',
            });
          });
          shape.events.forEach((event) => {
            componentJSIBinderHTemplate.addEvent({
              type: event.bubblingType,
              eventHandlerName: event.name,
            });
          });
          shape.commands.forEach((command) => {
            componentInstanceSpecHTemplate.addCommand({
              name: command.name,
              args: command.typeAnnotation.params.map((param) => ({
                name: param.name,
                typeAnnotation: param.typeAnnotation,
              })),
            });
          });
          fileContentByPath.set(
            this.onGetRNOHOutputPath(`${componentName}JSIBinder.h`),
            componentJSIBinderHTemplate.build()
          );
          fileContentByPath.set(
            this.onGetRNOHOutputPath(`Base${componentName}ComponentInstance.h`),
            componentInstanceSpecHTemplate.build()
          );
          this.glueCodeComponentsData.push({
            name: componentName,
            libraryName,
          });
        });
      }
    }

    return fileContentByPath;
  }
}
