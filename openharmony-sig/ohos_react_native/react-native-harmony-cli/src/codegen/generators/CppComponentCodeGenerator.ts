import { AbsolutePath } from '../../core';
import { CodeGenerator, UberSchema } from '../core';
import {
  BaseComponentInstanceHTemplate,
  ComponentJSIBinderHTemplate,
} from '../templates';
import { GlueCodeComponentDataV2 } from './AppBuildTimeGlueCodeGenerator';

export type LibraryData = {
  libraryCppName: string;
  uberSchema: UberSchema;
};

export class CppComponentCodeGenerator implements CodeGenerator<LibraryData> {
  private glueCodeComponentsData: GlueCodeComponentDataV2[] = [];

  constructor(
    private onGetRNOHOutputPath: (filename: string) => AbsolutePath,
    private codegenNoticeLines: string[]
  ) {}

  getGlueCodeData() {
    return this.glueCodeComponentsData;
  }

  generate({
    libraryCppName,
    uberSchema,
  }: LibraryData): Map<AbsolutePath, string> {
    const fileContentByPath = new Map<AbsolutePath, string>();
    for (const [_filename, schema] of uberSchema
      .getSpecSchemaByFilenameMap()
      .entries()) {
      if (schema.type === 'Component') {
        Object.entries(schema.components).map(([componentName, shape]) => {
          const componentJSIBinderHTemplate = new ComponentJSIBinderHTemplate(
            componentName,
            this.codegenNoticeLines
          );
          const componentInstanceSpecHTemplate =
            new BaseComponentInstanceHTemplate(
              componentName,
              libraryCppName,
              this.codegenNoticeLines
            );
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
            libraryCppName,
          });
        });
      }
    }

    return fileContentByPath;
  }
}
