import { CordovaPlugin } from './CordovaPlugin'

/**
 *
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/25 14:24
 */
export interface PluginEntry {
  service: string
  onload: boolean
  plugin?: CordovaPlugin
  pluginFactory?: () => CordovaPlugin
}