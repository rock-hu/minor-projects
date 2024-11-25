import { Config } from '@react-native-community/cli-types';
import {
  commandBundleHarmony,
  commandPackHarmony,
  commandUnpackHarmony,
  commandCreateMetroConstantsHarmony,
  commandCodegenHarmony,
  commandVerifyPackageHarmony,
  commandCodegenLibHarmony,
} from './commands';

export const config = {
  commands: [
    commandBundleHarmony,
    commandPackHarmony,
    commandUnpackHarmony,
    commandCreateMetroConstantsHarmony,
    commandCodegenHarmony,
    commandVerifyPackageHarmony,
    commandCodegenLibHarmony,
  ],
} satisfies Partial<Config>;
