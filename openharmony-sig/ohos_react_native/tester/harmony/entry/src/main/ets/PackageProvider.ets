import type { RNPackageContext, RNOHPackage } from '@rnoh/react-native-openharmony';
import { createRNOHPackages as createRNOHPackagesAutolinking } from "./RNOHPackagesFactory"
import SamplePackage from '@rnoh/sample-package/src/main/ets/SamplePackage';

export function getRNOHPackages(ctx: RNPackageContext): RNOHPackage[] {
  return [
    ...createRNOHPackagesAutolinking(ctx),
    new SamplePackage(ctx), // comment this line out when testing SamplePackage autolinking
  ]
}