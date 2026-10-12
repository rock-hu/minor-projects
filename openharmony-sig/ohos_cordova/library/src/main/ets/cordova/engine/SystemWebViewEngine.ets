import common from '@ohos.app.ability.common'
import webview from '@ohos.web.webview'
import { CordovaBridge } from '../CordovaBridge'
import { CordovaInterface } from '../CordovaInterface'
import { CordovaWebView } from '../CordovaWebView'
import { Client, CordovaWebViewEngine } from '../CordovaWebViewEngine'
import { NativeToJsMessageQueue } from '../NativeToJsMessageQueue'
import { PluginManager } from '../PluginManager'
import { SystemExposedJsApi } from './SystemExposedJsApi'
import { CordovaLog, Preconditions, StringUtils } from '../../utils/Index'
import { TAG } from '../../utils/Constants'

/**
 *
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/26 14:25
 */
export class SystemWebViewEngine implements CordovaWebViewEngine {
  private readonly controller: webview.WebviewController
  private bridge?: CordovaBridge
  client?: Client
  private parentWebView?: CordovaWebView
  private cordova?: CordovaInterface

  constructor(controller: webview.WebviewController) {
    this.controller = controller
  }

  init(parentWebView: CordovaWebView, cordova: CordovaInterface, client: Client, pluginManager: PluginManager, nativeToJsMessageQueue: NativeToJsMessageQueue): void {
    Preconditions.checkNullable(this.cordova)
    this.parentWebView = parentWebView
    this.cordova = cordova
    this.client = client
    // todo 补偿在线BridgeMode
    this.bridge = new CordovaBridge(pluginManager, nativeToJsMessageQueue)

    // todo controller必须与Web关联才行
    this.controller.registerJavaScriptProxy(new SystemExposedJsApi(this.bridge), '_cordovaNative', ['exec', 'setNativeToJsBridgeMode', 'retrieveJsMessages'])
  }

  getPage(): object {
    return this.cordova!.getPage()
  }

  getWeb(): object {
    return this.cordova!.getWeb()
  }

  getContext(): common.Context {
    return this.cordova!.getContext()
  }

  getCordovaWebView(): CordovaWebView {
    return this.parentWebView!
  }

  getCordovaBridge(): CordovaBridge | undefined {
    return this.bridge
  }

  getWebViewController(): webview.WebviewController {
    return this.controller
  }

  loadUrl(url: string, clearNavigationStack: boolean): void {
    this.controller.loadUrl(url)
  }

  stopLoading(): void {
    this.controller.stop()
  }

  getUrl(): string {
    return this.controller.getUrl()
  }

  clearCache(): void {
    // todo
    this.controller.removeCache(true)
    this.controller.clearSslCache()
    this.controller.clearClientAuthenticationCache()
  }

  clearHistory(): void {
    this.controller.clearHistory()
  }

  canGoBack(): boolean {
    return this.controller.accessBackward()
  }

  goBack(): boolean {
    if (this.controller.accessForward()) {
      this.controller.backward()
      return true
    } else {
      return false
    }
  }

  setPaused(value: boolean): void {
  }

  destroy(): void {
  }

  evaluateJavascript(script: string): void {
    // 捕获偶现的 17100001 错误
    try {
      this.controller.runJavaScript(script)
        .catch((e: Error) => {
          // do nothing
        })
    } catch (e) {
      CordovaLog.d(TAG, `run javaScript error --> ${StringUtils.convertString(e)}`)
    }
  }
}