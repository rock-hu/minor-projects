import util from '@ohos.util'
import webview from '@ohos.web.webview'
import { SystemWebView } from '../cordova/engine/SystemWebView'
import { AlertEvent, ConfirmEvent, PromptEvent, ErrorEvent, SslErrorEvent, LoadInterceptEvent, HttpErrorEvent, ShowFileSelectorEvent, OnTitleReceiveEvent, OnInterceptRequestEvent } from '../model/HarmonyWebModel'
import { ICordovaWebContainer } from '../cordova/model/ICordovaWebContainer'
import { BridgePermission } from '../model/BridgePermission'
import { CordovaContext } from '../CordovaContext'
import { Constants, CordovaLog, Preconditions, StringUtils } from '../utils/Index'
import { IPageContainer } from '../cordova/model/IPageContainer'

// 网页加载状态
export enum PageLoadState {
  // 初始化
  INIT = 1,
  // 加载中
  LOADING = 2,
  // 完成
  COMPLETE = 3,
  // 异常
  ERROR = 4
}

/**
 * 基于系统Web封装增加Cordova能力支持Q
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/22 16:28
 */
@Component
export struct Index {
  /**
   * 所属页面，必填
   * 使用函数定义便于后续升级使用NodeContainer创建离线CordovaWeb
   */
  @Require
  private page?: () => object
  /**
   * 当前CordovaWeb组件挂载的Page页面，必填
   */
  @Require
  private parent?: IPageContainer
  /**
   * 待加载的url，必填，不得为空
   */
  @Prop
  @Require
  private url: string
  /**
   * 当前页面加载状态
   */
  @State
  @Watch('notifyLoadStateChanged')
  private loadState: PageLoadState = PageLoadState.INIT
  /**
   * 当前Web组件的唯一Id
   */
  private readonly webId: string = util.generateRandomUUID()
  /**
   * 当前加载Web页面可用桥权限配置
   */
  private bridges?: BridgePermission[]
  /**
   * 混合模式
   */
  @State
  private mixedMode: MixedMode = MixedMode.None
  /**
   * 设置是否开启文档对象模型存储接口权限
   */
  private domStorageEnabled?: boolean
  /**
   * 是否支持手势进行缩放
   */
  private supportZoom?: boolean
  /**
   * 字体缩放比
   */
  private textZoom?: number
  /**
   * 是否使用缓存策略
   */
  private isCacheStrategy?: boolean
  /**
   * 是否开启获取位置权限
   */
  private isGeolocationEnabled?: boolean
  /**
   * 是否支持混合加载
   */
  private enableMixedMode?: boolean
  /**
   * 设置深色模式
   */
  @State
  private darkMode?: number = 0
  /**
   * 强制设置为深色模式
   */
  @State
  private forceDarkAccess: boolean = false
  /**
   * 隐私模式
   */
  @State
  private incognitoMode?: boolean = false
  /**
   * 控制器
   */
  private readonly controller: webview.WebviewController = new webview.WebviewController()
  /**
   * Web加载收到title标签事件
   */
  private onTitleReceive?: (event?: OnTitleReceiveEvent) => void
  /**
   * WebUri加载及重定向拦截器
   */
  private onLoadIntercept?: (event: LoadInterceptEvent) => boolean
  /**
   * 文件选择器
   */
  private onShowFileSelector?: (event: ShowFileSelectorEvent) => boolean
  private onInterceptRequest?: (event?: OnInterceptRequestEvent) => WebResourceResponse | null | undefined
  /**
   * Web页面加载状态回调监听
   * 接入方使用该监听自行处理Loader和Error的展示
   */
  private onLoadStateChanged?: (loadState: PageLoadState) => void
  /**
   * Cordova框架基于Web组件二次封装
   */
  private systemWebView?: SystemWebView

  aboutToAppear() {
    this.parent?.onCordovaWebAppear(this as ICordovaWebContainer)
    this.systemWebView?.onAboutToAppear()
    if (CordovaContext.enableInspect()) {
      webview.WebviewController.setWebDebuggingAccess(true)
    }
  }

  aboutToDisappear() {
    this.parent?.onCordovaWebDisappear(this as ICordovaWebContainer)
    this.systemWebView?.onAboutToDisappear()
  }

  /**
   * 返回按钮点击事件
   */
  onBackPress(): boolean {
    // 如果页面已经在加载中或者加载完成，则将back事件派发给Web处理，否则由页面自行处理
    if (this.loadState === PageLoadState.LOADING || this.loadState === PageLoadState.COMPLETE) {
      return this.systemWebView?.onBackPress() ?? false
    }
    return false
  }

  /**
   * 页面展示事件
   */
  onPageShow(): void {
    this.systemWebView?.onPageShow()
  }

  /**
   * 页面隐藏事件
   */
  onPageHide(): void {
    this.systemWebView?.onPageHide()
  }

  /**
   * 待Controller和WebView关联后开始异步加载Url
   */
  private startLoad() {
    this.loadState = PageLoadState.LOADING
    this.initWeb()
      .catch(() => {
        // do nothing
      })
  }

  onPageEnd(url?: string) {
    CordovaLog.e(Constants.TAG, `onPageEnd url: ${url}`)
    if (this.loadState === PageLoadState.LOADING) {
      this.loadState = PageLoadState.COMPLETE
    }
  }

  /**
   * 获取当前Web组件挂载在视图树上的Id
   * {@link IWebContainer} 鸭式编程接口
   *
   * @returns 当前Web组件的Id
   */
  getWebId(): string {
    return this.webId
  }

  /**
   * 获取当前webview加载的url
   * {@link IWebContainer} 鸭式编程接口
   *
   * @returns url链接
   */
  getUrl(): string {
    return this.url
  }

  /**
   * 获取当前webview的bridge权限
   * {@link IWebContainer} 鸭式编程接口
   *
   * @returns bridge权限数组
   */
  getBridgePermission(): BridgePermission[] | undefined {
    return this.bridges
  }

  /**
   * Web页面加载状态发生变化，对外通知加载状态
   */
  private notifyLoadStateChanged(): void {
    this.onLoadStateChanged && this.onLoadStateChanged(this.loadState)
  }

  onAlert = (event?: AlertEvent): boolean => {
    event && this.systemWebView?.onAlert(event)
    return true
  }
  onConfirm = (event?: ConfirmEvent): boolean => {
    event && this.systemWebView?.onConfirm(event)
    return true
  }
  onPrompt = (event?: PromptEvent): boolean => {
    event && this.systemWebView?.onPrompt(event)
    return true
  }
  onErrorReceive = (event?: ErrorEvent): void => {
    if (event?.request.isMainFrame() && this.loadState === PageLoadState.LOADING) {
      event && this.systemWebView?.onErrorReceive(event)
      CordovaLog.e(Constants.TAG, `onErrorReceive url:${event?.request.getRequestUrl()}, error info: ${event?.error.getErrorInfo()}, error code: ${event?.error.getErrorCode()}`)
      this.loadState = PageLoadState.ERROR
    }
  }
  onHttpErrorReceive = (event?: HttpErrorEvent): void => {
    if (event?.request.getRequestUrl() !== this.url || this.loadState === PageLoadState.COMPLETE) {
      return
    }
    if ((event?.response.getResponseCode() === 404 || event?.response.getResponseCode() === 500) && this.loadState === PageLoadState.LOADING) {
      CordovaLog.e(Constants.TAG, `onHttpErrorReceive url:${event?.request.getRequestUrl()}, reason message: ${event?.response.getReasonMessage()}, response code: ${event?.response.getResponseCode()}`)
      this.loadState = PageLoadState.ERROR
    }
  }
  onSslErrorReceive = async (event: SslErrorEvent) => {
    CordovaLog.e(Constants.TAG, `onSslErrorReceive ssl error`)
    if (CordovaContext.isIgnoreSslError()) {
      event.handler.handleConfirm()
    } else {
      event.handler.handleCancel()
    }
  }

  /**
   * 初始化Web配置，加载真正的URL地址
   */
  private async initWeb() {
    this.mixedMode = this.enableMixedMode ? MixedMode.All : MixedMode.None
    this.systemWebView = new SystemWebView(this.controller, Preconditions.checkNonNull(this.page), this)
    const originUA = StringUtils.wrapEmptyString(this.controller.getCustomUserAgent(), this.controller.getUserAgent())
    this.controller.setCustomUserAgent(`${originUA} ${CordovaContext.getCustomUserAgent()}}`)
    this.systemWebView.loadUrl(this.url)
  }

  build() {
    Web({ src: '', controller: this.controller, incognitoMode: this.incognitoMode })
      .width('100%')
      .height('100%')
      .id(this.webId)
      .imageAccess(true)
      .onlineImageAccess(true)
      .overviewModeAccess(false)
      .databaseAccess(true)
      .mediaPlayGestureAccess(false)
      .domStorageAccess(this.domStorageEnabled)
      .zoomAccess(this.supportZoom)
      .textZoomRatio(this.textZoom)
      .cacheMode(this.isCacheStrategy ? CacheMode.Default : CacheMode.Online)
      .javaScriptAccess(true)
      .geolocationAccess(this.isGeolocationEnabled)
      .mixedMode(this.mixedMode)
      .darkMode(this.darkMode)
      .forceDarkAccess(this.forceDarkAccess)
      .onPageEnd((event) => this.onPageEnd(event?.url))
      .onLoadIntercept(this.onLoadIntercept)
      .onTitleReceive(this.onTitleReceive)
      .onShowFileSelector(this.onShowFileSelector)
      .onInterceptRequest(this.onInterceptRequest)
      .onAlert(this.onAlert)
      .onConfirm(this.onConfirm)
      .onPrompt(this.onPrompt)
      .onErrorReceive(this.onErrorReceive)
      .onHttpErrorReceive(this.onHttpErrorReceive)
      .onSslErrorEventReceive(this.onSslErrorReceive)
      .visibility(this.loadState === PageLoadState.ERROR ? Visibility.Hidden : Visibility.Visible)
      .onControllerAttached(() => {
        this.startLoad()
      })
  }
}
