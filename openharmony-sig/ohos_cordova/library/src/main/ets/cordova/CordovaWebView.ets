import common from '@ohos.app.ability.common'
import { CordovaInterface } from './CordovaInterface'
import { PluginEntry } from './PluginEntry'
import { PluginManager } from './PluginManager'
import { PluginResult } from './PluginResult'
import webview from '@ohos.web.webview'
import { CordovaWebViewEngine } from './CordovaWebViewEngine'
import { ICordovaWebContainer } from './model/ICordovaWebContainer'

/**
 * Cordova封装接管的WebView容器
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/24 16:55
 */
export interface CordovaWebView {

  init(cordova: CordovaInterface, pluginEntries: PluginEntry[]): void

  isInitialized(): boolean

  getWebViewController(): webview.WebviewController

  loadUrl(url: string): void

  loadUrlInfoView(url: string, recreatePlugins: boolean): void

  stopLoading(): void

  canGoBack(): boolean

  clearCache(b: boolean): void

  clearHistory(): void

  backHistory(): boolean

  handleAppear(): void

  handleDisappear(): void

  handlePageShow(keepRunning: boolean): void

  handlePageHide(keepRunning: boolean): void

  /**
   * 拦截或放行原生物理键
   *
   * @param keyCode 物理键码
   * @param over true表示拦截，false表示放行，不处理
   */
  setButtonPlumbedToJs(keyCode: number, over: boolean): void

  isButtonPlumbedToJs(keyCode: number): boolean

  sendPluginResult(pluginResult: PluginResult, callbackId: string): void

  getPluginManager(): PluginManager

  getEngine(): CordovaWebViewEngine

  getPage(): object

  getWebContainer(): ICordovaWebContainer

  getContext(): common.Context

  postMessage(id: string, data: object | null): object | null | undefined

}