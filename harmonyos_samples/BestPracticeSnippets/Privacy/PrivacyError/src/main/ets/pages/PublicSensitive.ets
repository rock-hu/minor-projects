/**
 * 最佳实践：应用安全编码实践
 * 场景十：建议对涉及敏感功能的公共事件进行访问权限控制错误示例
 */

// [Start public_sensitive_error]
import { commonEventManager } from '@kit.BasicServicesKit';

let subscriber: commonEventManager.CommonEventSubscriber;
//Subscriber information
let subscribeInfo: commonEventManager.CommonEventSubscribeInfo = {
  events: ["event"]
};

// Create a subscriber
try {
  commonEventManager.createSubscriber(subscribeInfo, (err, commonEventSubscriber) => {
    if (!err) {
      console.info("createSubscriber Succeed");
      subscriber = commonEventSubscriber;
      //Subscribe to public events
      try {
        commonEventManager.subscribe(subscriber, (err, data) => {
          if (err) {
            console.error(`subscribe failed, code is ${err.code}, message is ${err.message}`);
          } else {
            //Event processing after receiving public events
            // doSomeDangerousThing(data)
          }
        });
      } catch (err) {
        console.error(`subscribe failed, code is ${err.code}, message is ${err.message}`);
      }
    } else {
      console.error(`createSubscriber failed, code is ${err.code}, message is ${err.message}`);
    }
  });
} catch (err) {
  console.error(`createSubscriber failed, code is ${err.code}, message is ${err.message}`);
}
// [End public_sensitive_error]