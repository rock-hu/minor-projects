/**
 * 最佳实践：应用资产保护设计
 * 场景二：HarmonyOS资产保护关键技术介绍-混淆后
 */

// [Start application_asset_protection_after]
function getAgeInfo() {
  let c = 0x14;
  let b = 'jack';
  if (b) {
    return c;
  } else {
    return -0x1;
  }
}

console['log']('' + getAgeInfo());
// [End application_asset_protection_after]