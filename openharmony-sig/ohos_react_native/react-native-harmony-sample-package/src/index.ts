export * from './SampleComponent';
export * from './GeneratedSampleComponentArkTS';
export * from './GeneratedSampleComponentCAPI';
export * from './SimpleText';
export * from './CodegenLibSampleComponent';
import NativeSampleTurboModule from './NativeSampleTurboModule';
import NativeGeneratedSampleTurboModule from './specs/v2/NativeGeneratedSampleTurboModule';
import NativeSampleWorkerTurboModule from './specs/v2/NativeSampleWorkerTurboModule';
import NativeCodegenLibSampleTurboModule from './specs/codegen-lib/NativeCodegenLibSampleModule';

export const SampleTurboModule = NativeSampleTurboModule;
export const GeneratedSampleTurboModule = NativeGeneratedSampleTurboModule;
export const SampleWorkerTurboModule = NativeSampleWorkerTurboModule;
export const CodegenLibSampleTurboModule = NativeCodegenLibSampleTurboModule;
