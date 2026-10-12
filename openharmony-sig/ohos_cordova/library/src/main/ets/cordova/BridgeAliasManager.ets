import HashMap from '@ohos.util.HashMap'
import { ObjectUtils } from '../utils/Index'

interface BridgeName {
  service: string
  action: string
}

/**
 * H5Cordova桥别名管理器
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2024/3/1 08:38
 */
export class BridgeAliasManager {
  // 单例
  private static readonly INSTANCE: BridgeAliasManager = new BridgeAliasManager()
  // 别名集合
  private readonly aliasMapper: HashMap<string, HashMap<string, BridgeName>> = new HashMap()

  private constructor() {
  }

  /**
   * 注册H5 Cordova Bridge别名
   *
   * @param aliasService 桥别名
   * @param aliasAction 方法别名
   * @param service 真实桥名
   * @param action 真实方法名
   */
  static registerAlias(aliasService: string, aliasAction: string, service: string, action: string): void {
    let serviceMapper: HashMap<string, BridgeName> | undefined = BridgeAliasManager.INSTANCE.aliasMapper.get(aliasService)
    if (ObjectUtils.isNull(serviceMapper)) {
      serviceMapper = new HashMap()
      BridgeAliasManager.INSTANCE.aliasMapper.set(aliasService, serviceMapper)
    }
    serviceMapper.set(aliasAction, {
      service,
      action
    } as BridgeName)
  }

  /**
   * 反注册H5 Cordova Bridge别名
   *
   * @param aliasService 桥别名
   * @param aliasAction 方法别名
   */
  static unregisterAlias(aliasService: string, aliasAction: string): void {
    const serviceMapper: HashMap<string, BridgeName> | undefined = BridgeAliasManager.INSTANCE.aliasMapper.get(aliasService)
    serviceMapper?.remove(aliasAction)
    if (serviceMapper?.length === 0) {
      BridgeAliasManager.INSTANCE.aliasMapper.remove(aliasService)
    }
  }

  /**
   * 查询别名桥对应的真实桥
   *
   * @param aliasService 桥别名
   * @param aliasAction 方法别名
   * @returns 若存在别名配置时，返回真实的桥名，否则返回空
   */
  static getBridgeByAlias(aliasService: string, aliasAction: string): BridgeName | undefined {
    return BridgeAliasManager.INSTANCE.aliasMapper.get(aliasService)?.get(aliasAction)
  }
}