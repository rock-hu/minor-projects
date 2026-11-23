import { CordovaPlugin } from '../cordova/CordovaPlugin'

/**
 * 一个Cordova桥
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2024/5/19 15:29
 */
export interface CordovaBridgeEntry {
  /**
   * 桥名称
   */
  service: string
  /**
   * Cordova桥工厂方法
   * 每个Web实例会调用本工厂方法创建一个新的桥实例对象
   *
   * @returns H5桥实例，返回不得为空
   */
  factory: () => CordovaPlugin
}