import { CodegenLibSampleModule } from "./generated/turboModules/CodegenLibSampleModule"
import { WorkerTurboModule } from "@rnoh/react-native-openharmony/ts"

export class SampleLibGenTurboModule extends WorkerTurboModule implements CodegenLibSampleModule.Spec {
  getNumber(): number {
    return 42
  }
}