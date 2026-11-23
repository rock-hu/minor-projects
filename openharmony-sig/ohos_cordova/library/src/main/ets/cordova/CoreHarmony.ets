import { KeyCode } from '@ohos.multimodalInput.keyCode'
import { CallbackContext } from './CallbackContext'
import { CordovaPlugin } from './CordovaPlugin'
import { PluginResult } from './PluginResult'
import { MessageStatus } from './enumeration/MessageStatus'
import { BooleanUtils, ObjectUtils, StringUtils } from '../utils/Index'

/**
 * Cordova框架的核心基础功能桥，包括管理H5退栈、劫持物理按键、退出App以及管理信道等
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/26 10:28
 */
export class CoreHarmony extends CordovaPlugin {
  private messageChannel?: CallbackContext
  private pendingResume?: PluginResult

  fireJavascriptEvent(action: string) {
    this.sendEventMessage(PluginResult.createByJson(MessageStatus.OK, {
      'action': action
    } as Record<string, string>))
  }

  async execute(action: string, args: ESObject[], callbackContext: CallbackContext): Promise<boolean> {
    try {
      if (action === 'clearCache') {
        this.webView?.clearCache(true)
      } else if (action === 'show') {
        // todo
      } else if (action === 'loadUrl') {
        // todo
      } else if (action === 'clearHistory') {
        this.webView?.clearHistory()
      } else if (action === 'backHistory') {
        this.webView?.backHistory()
      } else if (action === 'overrideButton') {
        this.overrideButton(StringUtils.convertString(args[0]), BooleanUtils.convertBoolean(args[1]))
      } else if (action === 'overrideBackbutton') {
        this.overrideBackButton(BooleanUtils.convertBoolean(args[0]))
      } else if (action === 'exitApp') {
        this.webView?.getPluginManager().postMessage('exit', null)
      } else if (action === 'messageChannel') {
        this.messageChannel = callbackContext
        if (!ObjectUtils.isNull(this.pendingResume)) {
          this.sendEventMessage(this.pendingResume!)
          this.pendingResume = undefined
        }
      }
      return true
    } catch (e) {
      callbackContext.sendPluginResult(PluginResult.create(MessageStatus.ERROR))
      return false
    }
  }

  private overrideBackButton(over: boolean) {
    this.webView?.setButtonPlumbedToJs(KeyCode.KEYCODE_BACK, over)
  }

  private overrideButton(button: string, over: boolean) {
    if ('volumeup' === button) {
      this.webView?.setButtonPlumbedToJs(KeyCode.KEYCODE_VOLUME_UP, over)
    } else if ('volumedown' === button) {
      this.webView?.setButtonPlumbedToJs(KeyCode.KEYCODE_VOLUME_DOWN, over)
    } else if ('menubutton' === button) {
      this.webView?.setButtonPlumbedToJs(KeyCode.KEYCODE_MENU, over)
    }
  }

  private sendEventMessage(payload: PluginResult): void {
    payload.setKeepCallback(true)
    this.messageChannel?.sendPluginResult(payload)
  }

  sendResumeEvent(resumeEvent: PluginResult) {
    if (!ObjectUtils.isNull(this.messageChannel)) {
      this.sendEventMessage(resumeEvent)
    } else {
      this.pendingResume = resumeEvent
    }
  }
}