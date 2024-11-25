import { TurboModuleRegistry } from 'react-native';
import type { TurboModule } from 'react-native/Libraries/TurboModule/RCTExport';

export interface Spec extends TurboModule {
  runComputeIntensiveTask(): void;
  runAsyncTask(): Promise<number>;
  getRNVersionFromUIContext(prefix: string): Promise<string>;
  showAlert(): void;
  emitDeviceEvent(eventName: string, payload: number): void;
  testCommunicationBetweenWorkerAndCpp(): Promise<boolean>;
  getArchitecture(): string;
  testCommunicatingWithOtherWorkerTurboModules(): Promise<boolean>;
}

export default TurboModuleRegistry.get<Spec>('SampleWorkerTurboModule')!;
