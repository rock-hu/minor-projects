import { AbsolutePath } from '../../core';
import { SpecSchema } from './UberSchema';

type FileContent = string;
export interface CodeGenerator<T> {
  generate(data: T): Map<AbsolutePath, FileContent>;
}

export interface SpecCodeGenerator extends CodeGenerator<SpecSchema> {}
