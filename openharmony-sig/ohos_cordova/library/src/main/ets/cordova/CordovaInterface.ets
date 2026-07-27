import common from '@ohos.app.ability.common'

/**
 * Cordova通用接口
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/25 11:10
 */
export interface CordovaInterface {

  getPage(): object

  getWeb(): object

  getContext(): common.Context

  onMessage(id: string, data: object | null): object | null

}