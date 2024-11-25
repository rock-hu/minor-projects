const {mergeConfig, getDefaultConfig} = require('@react-native/metro-config');
const {createHarmonyMetroConfig} = require('react-native-harmony/metro.config');

/**
 * @type {import("metro-config").ConfigT}
 */
const config = {
  // 清理metro缓存
  // resetCache: true,
  transformer: {
    getTransformOptions: async () => ({
      transform: {
        experimentalImportSupport: false,
        inlineRequires: true,
      },
    }),
  },
};

module.exports = mergeConfig(getDefaultConfig(__dirname), createHarmonyMetroConfig({
  reactNativeHarmonyPackageName: 'react-native-harmony',
}), config);
