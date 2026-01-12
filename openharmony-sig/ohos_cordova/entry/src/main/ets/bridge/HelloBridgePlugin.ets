import { CallbackContext, CordovaPlugin } from '@ohos/cordova/Index'
import { CordovaErrorType, getGeneralCordovaError } from '../utils/ErrorUtils'

enum Actions {
  SAY = 'say'
}

/**
 * 测试Cordova桥
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2024/5/19 16:22
 */
export class HelloBridgePlugin extends CordovaPlugin {
  async execute(action: string, args: ESObject[], callbackContext: CallbackContext): Promise<boolean> {
    if (action === Actions.SAY) {
      this.say(args[0], callbackContext)
    } else {
      return false
    }
    return true
  }

  private say(params: object, callbackContext: CallbackContext) {
    try {
      const name = params['name'] as string
      if (name === null || name === undefined || name.length === 0) {
        throw new Error()
      }
      callbackContext.successByString(`Hi ${name}, I'm Harmony`)
    } catch (e) {
      callbackContext.errorByJson(getGeneralCordovaError(CordovaErrorType.PARAM_ILLEGAL))
    }
  }
}