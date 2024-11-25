import { setupRNOHWorker } from "@rnoh/react-native-openharmony/src/main/ets/setupRNOHWorker"
import { createRNPackages } from "../pages/Index"

setupRNOHWorker({
  createWorkerRNInstanceConfig: (_rnInstanceName) => {
    return { thirdPartyPackagesFactory: createRNPackages }
  }
})