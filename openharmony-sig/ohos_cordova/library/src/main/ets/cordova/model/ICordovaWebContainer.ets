import { BridgePermission } from '../../model/BridgePermission'

/**
 * CordovaWeb容器应实现的接口
 *
 * @author kexin
 * @email zkx2590372_xa@bank-of-china.com
 * @date 2024/4/1 10:35
 */
export interface ICordovaWebContainer {

  /**
   * 用于接受CordovaWeb所属的Page页面通知Back按钮点击事件
   *
   * @returns true表示CordovaWeb消费了此事件，否则页面容器自行处理
   */
  onBackPress(): boolean

  /**
   * 用于接受CordovaWeb所属的Page页面通知onPageShow事件
   * 此事件后续将继续分发给Cordova Plugin
   */
  onPageShow(): void

  /**
   * 用于接受CordovaWeb所属的Page页面通知onPageHide事件
   * 此事件后续将继续分发给Cordova Plugin
   */
  onPageHide(): void

  /**
   * 获取当前Cordova所关联的WebView容器Id
   *
   * @returns Web容器Id
   */
  getWebId(): string

  /**
   * 获取当前Cordova所关联的WebView容器的url
   *
   * @returns 当前webview加载url链接
   */
  getUrl(): string

  /**
   * 获取当前Cordova所关联的WebView容器的桥权限
   *
   * @returns 当前webview的桥权限
   */
  getBridgePermission(): BridgePermission[] | undefined

}