import common from '@ohos.app.ability.common'
import { KeyEvent } from '@ohos.multimodalInput.keyEvent'
import webview from '@ohos.web.webview'
import { CordovaInterface } from './CordovaInterface'
import { CordovaWebView } from './CordovaWebView'
import { PluginManager } from './PluginManager'
import { NativeToJsMessageQueue } from './NativeToJsMessageQueue'
import { CordovaBridge } from './CordovaBridge'

/**
 *
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/25 16:04
 */

export interface Client {
  onBackPress(): boolean

  onDispatchKeyEvent(event: KeyEvent): boolean | null

  clearLoadTimeoutTimer(): void

  onPageStarted(newUrl: string): void

  onReceivedError(errorCode: number, desciption: string, failingUrl: string): void

  onPageFinishedLoading(url: string): void

  onNavigationAttempt(url: string): boolean
}

export interface EngineView {
  getCordovaWebView(): CordovaWebView
}

export interface CordovaWebViewEngine {

  init(parentWebView: CordovaWebView, cordova: CordovaInterface, client: Client, pluginManager: PluginManager, nativeToJsMessageQueue: NativeToJsMessageQueue): void

  getPage(): object

  getWeb(): object

  getContext(): common.Context

  getCordovaWebView(): CordovaWebView

  getCordovaBridge(): CordovaBridge | undefined

  getWebViewController(): webview.WebviewController

  loadUrl(url: string, clearNavigationStack: boolean): void

  stopLoading(): void

  getUrl(): string

  clearCache(): void

  clearHistory(): void

  canGoBack(): boolean

  goBack(): boolean

  setPaused(value: boolean): void

  destroy(): void

  evaluateJavascript(script: string): void

}