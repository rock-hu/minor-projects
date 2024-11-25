const path = require('path');
const { mergeConfig, getDefaultConfig } = require('@react-native/metro-config');
const { createHarmonyMetroConfig } = require('@react-native-oh/react-native-harmony/metro.config');
const moduleId = require('./build/moduleId');

const projectRootPath = path.join(__dirname);

const config = {
  serializer: {
    createModuleIdFactory: moduleId.createModuleIdFactoryWrap(
      projectRootPath,
      'hotel',
    ),
    processModuleFilter: moduleId.postProcessModulesFilterWrap(
      projectRootPath,
    ),
  },
};

module.exports = mergeConfig(getDefaultConfig(__dirname), createHarmonyMetroConfig({
  reactNativeHarmonyPackageName: '@react-native-oh/react-native-harmony',
}), config);
