/**
 * 使用系统标准H5 Bridge API定义的一套Cordova协议操作桥接口
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/26 09:02
 */
export interface ExposedJsApi {

  exec(bridgeSecret: number, service: string, action: string, callbackId: string, arg: string): string | null

  setNativeToJsBridgeMode(bridgeSecret: number, value: number): void

  retrieveJsMessages(bridgeSecret: number, fromOnlineEvent: boolean): string | null

}