import { TurboModule, TurboModuleRegistry } from "react-native";

// 使用自定义的TurboModules
export interface SpecSampleTurboModule extends TurboModule {
  pushStringToHarmony(arg: string, id?: number): string;
  registerFunction(onComplete: (value: string) => void): void;
  doAsyncJob(shouldResolve: boolean): Promise<string>;
  rnLog(info: string): void;
}

export default TurboModuleRegistry.getEnforcing<SpecSampleTurboModule>('SampleTurboModule');