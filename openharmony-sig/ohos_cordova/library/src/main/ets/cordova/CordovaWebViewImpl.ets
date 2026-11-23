import HashSet from '@ohos.util.HashSet'
import webview from '@ohos.web.webview'
import common from '@ohos.app.ability.common'
import { ObjectUtils, Preconditions, StringUtils } from '../utils/Index'
import { CordovaInterface } from './CordovaInterface'
import { CordovaWebView } from './CordovaWebView'
import { Client, CordovaWebViewEngine } from './CordovaWebViewEngine'
import { PluginEntry } from './PluginEntry'
import { PluginManager } from './PluginManager'
import { PluginResult } from './PluginResult'
import { EvalBridgeMode, NativeToJsMessageQueue, NoOpBridgeMode } from './NativeToJsMessageQueue'
import { KeyEvent } from '@ohos.multimodalInput.keyEvent'
import { CoreHarmony } from './CoreHarmony'
import { KeyCode } from '@ohos.multimodalInput.keyCode'
import { CordovaPlugin } from './CordovaPlugin'
import { ICordovaWebContainer } from './model/ICordovaWebContainer'

const LEGAL_PLUMBED_KEY_CODE: HashSet<number> = new HashSet()
LEGAL_PLUMBED_KEY_CODE.add(KeyCode.KEYCODE_BACK)
LEGAL_PLUMBED_KEY_CODE.add(KeyCode.KEYCODE_VOLUME_UP)
LEGAL_PLUMBED_KEY_CODE.add(KeyCode.KEYCODE_VOLUME_DOWN)
LEGAL_PLUMBED_KEY_CODE.add(KeyCode.KEYCODE_MENU)

const CORE_HARMONY_PLUGIN_NAME = 'CoreHarmony'

/**
 *
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/26 16:35
 */
export class CordovaWebViewImpl implements CordovaWebView {
  private readonly engine: CordovaWebViewEngine
  private cordova?: CordovaInterface
  private pluginManager?: PluginManager
  private appPlugin?: CoreHarmony
  private nativeToJsMessageQueue?: NativeToJsMessageQueue
  private engineClient: EngineClient = new EngineClient(this)
  private loadUrlTimeout: number = 0
  private readonly boundKeyCodes: HashSet<number> = new HashSet()
  private hasPausedEver: boolean = false
  // 当前已经加载的url
  private loadedUrl?: string

  constructor(cordovaWebViewEngine: CordovaWebViewEngine) {
    this.engine = cordovaWebViewEngine
  }

  init(cordova: CordovaInterface, pluginEntries: PluginEntry[]): void {
    Preconditions.checkNullable(this.cordova)
    this.cordova = cordova
    this.pluginManager = new PluginManager(this, cordova, pluginEntries)
    this.nativeToJsMessageQueue = new NativeToJsMessageQueue()
    this.nativeToJsMessageQueue.addBridgeMode(new NoOpBridgeMode())
    this.nativeToJsMessageQueue.addBridgeMode(new EvalBridgeMode(this.engine, cordova))
    this.engine.init(this, cordova, this.engineClient, this.pluginManager, this.nativeToJsMessageQueue)

    // 保底注册CoreHarmony插件
    this.pluginManager.addService({
      service: CORE_HARMONY_PLUGIN_NAME,
      onload: false,
      pluginFactory: () => new CoreHarmony()
    })
    this.pluginManager.init()
  }

  isInitialized(): boolean {
    return !ObjectUtils.isNull(this.cordova)
  }

  getWebViewController(): webview.WebviewController {
    return this.engine!.getWebViewController()
  }

  loadUrl(url: string): void {
    this.loadUrlInfoView(url, true)
  }

  loadUrlInfoView(url: string, recreatePlugins: boolean): void {
    if (url === 'about:blank' || url.startsWith('javascript:')) {
      this.engine.loadUrl(url, false)
    } else {
      recreatePlugins = recreatePlugins || StringUtils.isEmptyString(this.loadedUrl)
      if (recreatePlugins) {
        if (!StringUtils.isEmptyString(this.loadedUrl)) {
          this.appPlugin = undefined
          this.pluginManager!.init()
        }
        this.loadedUrl = url
      }
      // todo 计时并处理超时
      this.engine.loadUrl(url, recreatePlugins)
    }
  }

  stopLoading(): void {
    this.loadUrlTimeout++
  }

  canGoBack(): boolean {
    return this.engine!.canGoBack()
  }

  clearCache(b: boolean): void {
    this.engine?.clearCache()
  }

  clearHistory(): void {
    this.engine?.clearHistory()
  }

  backHistory(): boolean {
    return this.engine!.goBack()
  }

  handleAppear(): void {
  }

  handleDisappear(): void {
    if (this.isInitialized()) {
      this.engine.setPaused(true)
      this.pluginManager!.onAboutToDisappear()
    }
  }

  handlePageShow(keepRunning: boolean): void {
    if (this.isInitialized()) {
      this.engine.setPaused(false)
      this.pluginManager!.onPageShow(keepRunning)
      if (this.hasPausedEver) {
        this.sendJavascriptEvent('resume')
      }
    }
  }

  handlePageHide(keepRunning: boolean): void {
    if (this.isInitialized()) {
      this.hasPausedEver = true
      this.pluginManager!.onPageHide(keepRunning)
      this.sendJavascriptEvent('pause')
      if (keepRunning) {
        this.engine.setPaused(true)
      }
    }
  }

  sendJavascriptEvent(event: string): void {
    if (ObjectUtils.isNull(this.appPlugin)) {
      const appPlugin: CordovaPlugin | null | undefined = this.pluginManager?.getPlugin(CORE_HARMONY_PLUGIN_NAME)
      if (!ObjectUtils.isNull(appPlugin)) {
        this.appPlugin = appPlugin! as CoreHarmony
      }
    }
    if (!ObjectUtils.isNull(this.appPlugin)) {
      this.appPlugin!.fireJavascriptEvent(event)
    } else {
      // todo log
    }
  }

  setButtonPlumbedToJs(keyCode: number, over: boolean): void {
    if (LEGAL_PLUMBED_KEY_CODE.has(keyCode)) {
      if (over) {
        this.boundKeyCodes.add(keyCode)
      } else {
        this.boundKeyCodes.remove(keyCode)
      }
    } else {
      throw new Error()
    }
  }

  isButtonPlumbedToJs(keyCode: number): boolean {
    return this.boundKeyCodes.has(keyCode)
  }

  sendPluginResult(pluginResult: PluginResult, callbackId: string): void {
    this.nativeToJsMessageQueue?.addPluginResult(pluginResult, callbackId)
  }

  getPluginManager(): PluginManager {
    return this.pluginManager!
  }

  getEngine(): CordovaWebViewEngine {
    return this.engine
  }

  getPage(): object {
    return this.engine.getPage()
  }

  getWebContainer(): ICordovaWebContainer {
    return this.engine.getWeb() as ICordovaWebContainer
  }

  getContext(): common.Context {
    return this.engine.getContext()
  }

  postMessage(id: string, data: object): object | null | undefined {
    return this.pluginManager?.postMessage(id, data)
  }
}

class EngineClient implements Client {
  private readonly webView: CordovaWebViewImpl

  constructor(webView: CordovaWebViewImpl) {
    this.webView = webView
  }

  onBackPress(): boolean {
    if (this.webView.isButtonPlumbedToJs(KeyCode.KEYCODE_BACK)) {
      this.webView.sendJavascriptEvent('backbutton')
      return true
    }
    return false
  }

  onDispatchKeyEvent(event: KeyEvent): boolean {
    throw new Error('Method not implemented.')
  }

  clearLoadTimeoutTimer(): void {
    throw new Error('Method not implemented.')
  }

  onPageStarted(newUrl: string): void {
    throw new Error('Method not implemented.')
  }

  onReceivedError(errorCode: number, desciption: string, failingUrl: string): void {
    throw new Error('Method not implemented.')
  }

  onPageFinishedLoading(url: string): void {
    throw new Error('Method not implemented.')
  }

  onNavigationAttempt(url: string): boolean {
    throw new Error('Method not implemented.')
  }
}