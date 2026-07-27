import HashMap from '@ohos.util.HashMap'
import { CordovaConfig } from './CordovaConfig'
import { CordovaContext } from './CordovaContext'
import { CordovaBridgeAlias } from './model/CordovaBridgeAlias'
import { CordovaBridgeEntry } from './model/CordovaBridgeEntry'
import { ObjectUtils, StringUtils } from './utils/Index'
import { CordovaBridgeName } from './model/CordovaBridgeName'

/**
 * Cordova全局管理类
 * 管理包括配置管理、域名白名单、自定义Bridge、自定义别名等
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2024/5/19 14:05
 */
export class CordovaManager {
  // 单例
  private static readonly INSTANCE: CordovaManager = new CordovaManager()
  /**
   * 白名单域名
   * 当加载的Web页面处于白名单域名之内时将拥有全部的Cordova权限
   */
  private readonly whiteDomains: string[] = []
  /**
   * Cordova桥
   */
  private readonly bridgeEntries: CordovaBridgeEntry[] = []
  /**
   * Cordova桥别名
   */
  private readonly bridgeAliases: HashMap<string, HashMap<string, CordovaBridgeName>> = new HashMap()

  private constructor() {
  }

  /**
   * 初始化Cordova框架
   * 注意：初始化动作只可以执行一次，重复初始化将报错
   *
   * @param config 待初始化的配置
   */
  static init(config: CordovaConfig): void {
    return CordovaContext.init(config)
  }

  /**
   * 设置一组Bridge权限白名单域名
   *
   * @param domains 白名单域名，可为空数组，空数组表示清理原先设置的白名单
   * @returns true表示白名单域名设置成功，否则表示设置失败
   */
  static setWhiteDomains(domains: string[]): boolean {
    if (ObjectUtils.isNull(domains)) {
      return false
    }
    const manager = CordovaManager.INSTANCE
    manager.whiteDomains.length = 0
    if (domains.length > 0) {
      manager.whiteDomains.push(...domains)
    }
    return true
  }

  /**
   * 追加一个白名单域名
   *
   * @param domain 白名单域名，不得为空
   * @returns true表示追加成功，否则表示追加失败
   */
  static appendWhiteDomain(domain: string): boolean {
    if (!StringUtils.isEmptyString(domain)) {
      CordovaManager.INSTANCE.whiteDomains.push(domain)
      return true
    } else {
      return false
    }
  }

  /**
   * 获取当前配置的Cordova权限域名白名单
   *
   * @returns 域名白名单
   */
  static getWhiteDomains(): string[] {
    return CordovaManager.INSTANCE.whiteDomains
  }

  /**
   * 设置一组Cordova桥配置
   *
   * @param bridgeEntries Cordova桥配置，可谓空数组，空数组表示清理原设置的Cordova桥配置
   * @returns true表示设置成功，否则表示设置失败
   */
  static setBridgeEntries(bridgeEntries: CordovaBridgeEntry[]): boolean {
    if (ObjectUtils.isNull(bridgeEntries)) {
      return false
    }
    const manager = CordovaManager.INSTANCE
    manager.bridgeEntries.length = 0
    if (bridgeEntries.length > 0) {
      manager.bridgeEntries.push(...bridgeEntries)
    }
    return true
  }

  /**
   * 追加一个Cordova桥配置
   *
   * @param bridgeEntry 新追加的Cordova桥配置
   * @returns true表示操作成功，否则表示操作失败
   */
  static appendBridgeEntry(bridgeEntry: CordovaBridgeEntry): boolean {
    if (ObjectUtils.isNull(bridgeEntry)) {
      return false
    }
    const manager = CordovaManager.INSTANCE
    const exist = manager.bridgeEntries.some(it => it.service === bridgeEntry.service)
    if (!exist) {
      manager.bridgeEntries.push(bridgeEntry)
      return true
    } else if (CordovaContext.isDebugMode()) {
      throw new Error(`there is the same name bridge, witch name is ${bridgeEntry.service}`)
    } else {
      return false
    }
  }

  /**
   * 获取当前配置的所有Cordova桥
   *
   * @returns 所有配置的Cordova桥
   */
  static getBridgeEntries(): CordovaBridgeEntry[] {
    return CordovaManager.INSTANCE.bridgeEntries
  }

  /**
   * 设置一组Cordova别名配置
   *
   * @param bridgeAliases Cordova别名配置，可谓空数组，空数组表示清理原设置的Cordova别名配置
   * @returns true表示设置成功，否则表示设置失败
   */
  static setBridgeAliases(bridgeAliases: CordovaBridgeAlias[]): boolean {
    if (ObjectUtils.isNull(bridgeAliases)) {
      return false
    }
    const manager = CordovaManager.INSTANCE
    manager.bridgeAliases.clear()
    bridgeAliases.forEach(it => {
      let serviceMapper: HashMap<string, CordovaBridgeName> | undefined = manager.bridgeAliases.get(it.aliasService)
      if (ObjectUtils.isNull(serviceMapper)) {
        serviceMapper = new HashMap()
        manager.bridgeAliases.set(it.aliasService, serviceMapper)
      }
      serviceMapper.set(it.aliasAction, {
        service: it.service,
        action: it.action
      })
    })
    return true
  }

  /**
   * 追加一个Cordova别名配置
   *
   * @param bridgeAlias 新追加的Cordova别名配置
   * @returns true表示操作成功，否则表示操作失败
   */
  static appendBridgeAlias(bridgeAlias: CordovaBridgeAlias): boolean {
    if (ObjectUtils.isNull(bridgeAlias)) {
      return false
    }
    const manager = CordovaManager.INSTANCE
    let serviceMapper: HashMap<string, CordovaBridgeName> | undefined = manager.bridgeAliases.get(bridgeAlias.aliasService)
    if (ObjectUtils.isNull(serviceMapper)) {
      serviceMapper = new HashMap()
      manager.bridgeAliases.set(bridgeAlias.aliasService, serviceMapper)
    }
    if (serviceMapper.hasKey(bridgeAlias.aliasAction)) {
      if (CordovaContext.isDebugMode()) {
        throw new Error(`there is the same bridge alias, witch is ${bridgeAlias.aliasService}_${bridgeAlias.aliasAction}}`)
      } else {
        return false
      }
    } else {
      serviceMapper.set(bridgeAlias.aliasAction, {
        service: bridgeAlias.service,
        action: bridgeAlias.action
      })
      return true
    }
  }

  /**
   * 根据当前的Cordova调用桥和方法名获取对应的真实桥和方法
   *
   * @returns 如果存在形参所指的别名则返回对应的真实桥和方法，否则返回undefined表示不存在别名映射
   */
  static getBridgeAlias(aliasService: string, aliasAction: string): CordovaBridgeName | undefined {
    return CordovaManager.INSTANCE.bridgeAliases.get(aliasService)?.get(aliasAction)
  }
}