/**
 * 最佳实践：应用安全编码实践
 * 场景九：避免使用携带个人数据未设置权限的动态公共事件
 */

// [Start public_safe_info]
import { commonEventManager } from '@kit.BasicServicesKit';

function publishEventWithData() {
  let options: commonEventManager.CommonEventPublishData = {
    code: 1,
    data: "ContactData", // Sending sensitive data
    subscriberPermissions: ["ohos.permission.READ_CONTACTS"], // Set the permission
  }
  commonEventManager.publish("MyCommonEvent", options, (err) => {
    if (err.code) {
      console.error("publish event error: " + err.code + ", " + err.message + ", " + err.name + ", " + err.stack);
    } else {
      console.info("publish event with data Succeeded");
    }
  })
}
// [End public_safe_info]