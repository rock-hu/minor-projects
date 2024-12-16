import type {RNPackageContext, RNPackage} from '@rnoh/react-native-openharmony/ts';
/**
 * Importing ets files from top level barrel file doesn't work.
 */
import {SamplePackage} from '@rnoh/sample-package/src/main/ets/SamplePackage';

export function createRNPackages(ctx: RNPackageContext): RNPackage[] {
  return [new SamplePackage(ctx)];
}
