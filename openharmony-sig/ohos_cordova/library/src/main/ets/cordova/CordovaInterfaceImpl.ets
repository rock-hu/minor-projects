import common from '@ohos.app.ability.common'
import router from '@ohos.router'
import { CordovaInterface } from './CordovaInterface'
import { PluginManager } from './PluginManager'
import { CoreHarmony } from './CoreHarmony'
import { PluginResult } from './PluginResult'
import { MessageStatus } from './enumeration/MessageStatus'
import { ObjectUtils } from '../utils/Index'

/**
 * Cordova接口实现类
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/26 10:13
 */
export class CordovaInterfaceImpl implements CordovaInterface {
  private readonly page: () => object
  private readonly web: object
  private pluginManager?: PluginManager
  private destroyed: boolean = false
  private savedResult?: Object

  constructor(page: () => object, web: object) {
    this.page = page
    this.web = web
  }

  getPage(): object {
    return this.page()
  }

  getWeb(): object {
    return this.web
  }

  getContext(): common.Context {
    return getContext(this.page())
  }

  onMessage(id: string, data: object): object | null {
    if ('exit' === id) {
      router.back()
    }
    return null
  }

  onCordovaInit(pluginManager: PluginManager): void {
    this.pluginManager = pluginManager
    if (!ObjectUtils.isNull(this.savedResult)) {
      this.onPageResult(this.savedResult!)
    } else if (this.destroyed) {
      this.destroyed = false
      const appPlugin: CoreHarmony | undefined = pluginManager?.getPlugin('CoreHarmony') as CoreHarmony
      if (!ObjectUtils.isNull(appPlugin)) {
        appPlugin!.sendResumeEvent(PluginResult.createByJson(MessageStatus.OK, {
          'action': 'resume'
        } as Record<string, string>))
      }
    }
  }

  onPageResult(params: Object) {
    // todo
  }
}