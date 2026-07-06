import { CordovaInterface } from './CordovaInterface'
import { CordovaWebView } from './CordovaWebView'
import { CallbackContext } from './CallbackContext'
import { Preconditions } from '../utils/Index'

/**
 * Cordova桥基类
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/25 10:58
 */
export class CordovaPlugin {
  protected webView?: CordovaWebView
  protected cordova?: CordovaInterface
  private serviceName?: string

  constructor() {
  }

  privateInitialize(serviceName: string, cordova: CordovaInterface, webView: CordovaWebView): void {
    this.serviceName = Preconditions.checkNonNull(serviceName)
    this.cordova = Preconditions.checkNonNull(cordova)
    this.webView = Preconditions.checkNonNull(webView)

    this.initialize(this.cordova!, this.webView!)
  }

  initialize(cordova: CordovaInterface, webView: CordovaWebView): void {
  }

  pluginInitialize(): void {
  }

  getServiceName(): string | undefined {
    return this.serviceName
  }

  execute(action: string, args: ESObject[], callbackContext: CallbackContext): Promise<boolean> {
    return Promise.resolve(false)
  }

  onPageShow(multitasking: boolean) {
  }

  onPageHide(multitasking: boolean) {
  }

  onAboutToDisappear() {

  }

  onMessage(id: string, data: object | null): object | null {
    return null
  }

  shouldAllowNavigation(url: string): boolean | null {
    return null
  }

  shouldAllowBridgeAccess(url: string): boolean | null {
    return null
  }
}