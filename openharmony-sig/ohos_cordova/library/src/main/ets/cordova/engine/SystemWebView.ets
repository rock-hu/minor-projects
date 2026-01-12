import { CordovaDialogsHelper } from '../CordovaDialogsHelper'
import webview from '@ohos.web.webview'
import { CordovaWebView } from '../CordovaWebView'
import { CordovaWebViewImpl } from '../CordovaWebViewImpl'
import { SystemWebViewEngine } from './SystemWebViewEngine'
import { CordovaInterfaceImpl } from '../CordovaInterfaceImpl'
import { BridgeManager } from '../bridge/BridgeManager'
import { AlertEvent, ConfirmEvent, PromptEvent, ErrorEvent } from '../../model/HarmonyWebModel'
import { ObjectUtils } from '../../utils/Index'

/**
 * 封装系统WebView交互事件处理类，所有系统WebView交互事件均在这里处理
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/26 14:46
 */
export class SystemWebView {
  private readonly controller: webview.WebviewController
  private readonly page: () => object
  private readonly engine: SystemWebViewEngine
  private readonly webView: CordovaWebView
  private readonly cordovaInterface: CordovaInterfaceImpl

  constructor(controller: webview.WebviewController, page: () => object, web: object) {
    this.controller = controller
    this.page = page
    this.engine = new SystemWebViewEngine(controller)
    this.webView = new CordovaWebViewImpl(this.engine)
    this.cordovaInterface = new CordovaInterfaceImpl(page, web)
  }

  loadUrl(url: string) {
    if (!this.webView.isInitialized()) {
      this.webView.init(this.cordovaInterface, BridgeManager.getPluginEntries())
    }
    this.cordovaInterface.onCordovaInit(this.webView.getPluginManager())
    this.webView.loadUrlInfoView(url, true)
  }

  onAlert(event: AlertEvent) {
    CordovaDialogsHelper.showAlert(event.message, (success: boolean, message: string | null) => {
      if (success) {
        event.result.handleConfirm()
      } else {
        event.result.handleCancel()
      }
    })
  }

  onConfirm(event: ConfirmEvent) {
    CordovaDialogsHelper.showConfirm(event.message, (success: boolean, message: string | null) => {
      if (success) {
        event.result.handleConfirm()
      } else {
        event.result.handleCancel()
      }
    })
  }

  onPrompt(event: PromptEvent) {
    const value = this.engine.getCordovaBridge()?.promptOnJsPrompt(event.url, event.message, event.value)
    if (ObjectUtils.isNull(value)) {
      CordovaDialogsHelper.showPrompt(event.message, (success: boolean, message: string | null) => {
        if (success) {
          event.result.handleConfirm()
        } else {
          event.result.handleCancel()
        }
      })
    } else {
      event.result.handlePromptConfirm(value)
    }
  }

  onErrorReceive(event: ErrorEvent): void {
    // todo
  }

  onAboutToAppear() {
    this.webView.handleAppear()
  }

  onAboutToDisappear() {
    this.webView.handleDisappear()
  }

  onPageShow() {
    this.webView.handlePageShow(true)
  }

  onPageHide() {
    this.webView.handlePageHide(true)
  }

  onKeyEvent() {
    // todo
  }

  onBackPress(): boolean {
    // 如果H5通过Cordova注册back事件拦截，则将事件派发给H5处理
    const consumed = this.engine.client?.onBackPress() ?? false
    if (!consumed) {
      // 其次检查H5 history是否可退栈
      return this.engine.goBack()
    }
    return true
  }
}