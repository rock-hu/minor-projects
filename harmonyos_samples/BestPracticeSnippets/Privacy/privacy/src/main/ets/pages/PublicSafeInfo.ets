/**
 * 最佳实践：应用安全编码实践
 * 场景九：避免使用携带个人数据未设置权限的动态公共事件
 */

// [Start public_safe_info]
import { commonEventManager } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

function publishEventWithData() {
  let options: commonEventManager.CommonEventPublishData = {
    code: 1,
    data: 'ContactData', // Sending sensitive data
    subscriberPermissions: ['ohos.permission.READ_CONTACTS'], // Set the permission
  }
  commonEventManager.publish('MyCommonEvent', options, (err) => {
    if (err.code) {
      hilog.error(0x0000, 'privacy', `publish event error: ${err.code} ${err.message} ${err.name} ${err.stack}`);
    } else {
      hilog.info(0x0000, 'privacy', 'publish event with data Succeeded');
    }
  })
}
// [End public_safe_info]