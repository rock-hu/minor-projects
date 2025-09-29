/**
 * 最佳实践：后台合理使用系统资源
 * 场景一：应用直接持锁
 */
// [Start bpta_lock_by_application]
import { runningLock } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { BusinessError } from '@kit.BasicServicesKit';

// Return to the background to release the lock
runningLock.create('running_lock_test', runningLock.RunningLockType.BACKGROUND)
  .then((lock: runningLock.RunningLock) => {
    try {
      lock.unhold();
    } catch (error) {
      let err = error as BusinessError;
      hilog.warn(0x000, 'testTag', `setColorMode failed, code=${err.code}, message=${err.message}`);
    }
  })
  .catch((err: Error) => {
    console.error('create running lock failed, err: ' + err);
  });
// [End bpta_lock_by_application]