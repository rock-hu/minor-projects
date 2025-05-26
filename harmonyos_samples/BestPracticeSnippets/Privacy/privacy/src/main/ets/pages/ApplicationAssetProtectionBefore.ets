/**
 * 最佳实践：应用资产保护设计
 * 场景一：HarmonyOS资产保护关键技术介绍-混淆前
 */

// [Start application_asset_protection_before]
function getAgeInfo() {
  let age = 20;
  let name = 'jack';
  if (name) {
    return age;
  } else {
    return -1;
  }
}

console.log('' + getAgeInfo());
// [End application_asset_protection_before]