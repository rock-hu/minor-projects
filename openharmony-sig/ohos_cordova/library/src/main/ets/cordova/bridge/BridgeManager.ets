import { PluginEntry } from '../PluginEntry'
import { CordovaManager } from '../../CordovaManager'

/**
 * H5桥管理器
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/26 16:04
 */
export class BridgeManager {

  static getPluginEntries(): PluginEntry[] {
    return CordovaManager.getBridgeEntries()
      .map(it => ({
        service: it.service,
        onload: false,
        pluginFactory: it.factory
      } as PluginEntry))
  }
}