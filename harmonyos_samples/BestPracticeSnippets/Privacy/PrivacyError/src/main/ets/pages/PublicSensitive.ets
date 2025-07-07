/**
 * 最佳实践：应用安全编码实践
 * 场景十：建议对涉及敏感功能的公共事件进行访问权限控制错误示例
 */

// [Start public_sensitive_error]
import { commonEventManager } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

let subscriber: commonEventManager.CommonEventSubscriber;
//Subscriber information
let subscribeInfo: commonEventManager.CommonEventSubscribeInfo = {
  events: ['event']
};

// Create a subscriber
try {
  commonEventManager.createSubscriber(subscribeInfo, (err, commonEventSubscriber) => {
    if (!err) {
      hilog.info(0x0000, 'privacy', 'createSubscriber Succeed');
      subscriber = commonEventSubscriber;
      //Subscribe to public events
      try {
        commonEventManager.subscribe(subscriber, (err, data) => {
          if (err) {
            hilog.error(0x0000, 'privacy', `subscribe failed, code is ${err.code}, message is ${err.message}`);
          } else {
            //Event processing after receiving public events
            // doSomeDangerousThing(data)
          }
        });
      } catch (err) {
        hilog.error(0x0000, 'privacy', `subscribe failed, code is ${err.code}, message is ${err.message}`);
      }
    } else {
      hilog.error(0x0000, 'privacy', `createSubscriber failed, code is ${err.code}, message is ${err.message}`);
    }
  });
} catch (err) {
  hilog.error(0x0000, 'privacy', `createSubscriber failed, code is ${err.code}, message is ${err.message}`);
}
// [End public_sensitive_error]