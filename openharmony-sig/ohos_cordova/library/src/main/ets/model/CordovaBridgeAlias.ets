/**
 * Cordova桥别名
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2024/5/19 15:32
 */
export interface CordovaBridgeAlias {
  // 桥别名
  aliasService: string
  // 方法别名
  aliasAction: string
  // 真实桥名
  service: string
  // 真实方法名
  action: string
}