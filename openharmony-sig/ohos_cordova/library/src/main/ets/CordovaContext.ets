import { CordovaConfig } from './CordovaConfig'

/**
 * Cordova模块的全部上下文
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2024/5/19 14:03
 */
export class CordovaContext {
  // 单例
  private static readonly INSTANCE: CordovaContext = new CordovaContext()

  private constructor() {
  }

  // 实际初始化的配置
  private config?: CordovaConfig

  /**
   * 初始化Cordova全部上下文
   * 注意：只可进行一次初始化，重复初始化会报错
   *
   * @param config
   */
  static init(config: CordovaConfig): void {
    if (CordovaContext.INSTANCE.config === undefined) {
      CordovaContext.INSTANCE.config = config
    } else {
      throw new Error('the cordova context has been init, plz check')
    }
  }

  /**
   * 是否为开发调试模式
   *
   * @returns true表示为开发调试模式，否则为生产发布模式
   */
  static isDebugMode(): boolean {
    const config = CordovaContext.INSTANCE.config
    return config !== undefined && config.isDebug()
  }

  /**
   * 是否为生产发布模式
   *
   * @returns true表示为生产发布模式，否则为开发调试模式
   */
  static isReleaseMode(): boolean {
    const config = CordovaContext.INSTANCE.config
    return config === undefined || !config.isDebug()
  }

  /**
   * 是否可以输出日志
   *
   * @returns true表示可以输出日志，否则不可以输出日志
   */
  static canLog(): boolean {
    const config = CordovaContext.INSTANCE.config
    return config !== undefined && config.isDebug() && config.isCanLog()
  }

  /**
   * 是否可以开启Web Inspect调试
   *
   * @returns true表示可以开启Inspect调试，否则不得开启
   */
  static enableInspect(): boolean {
    const config = CordovaContext.INSTANCE.config
    return config !== undefined && config.isDebug() && config.enableInspect()
  }

  /**
   * 是否可以忽略Web加载过程中的SSL错误
   *
   * @returns true表示可以忽略，否则不得忽略
   */
  static isIgnoreSslError(): boolean {
    const config = CordovaContext.INSTANCE.config
    return config !== undefined && config.isDebug() && config.isIgnoreSslError()
  }

  /**
   * 获取接入方设置的自定义UA
   *
   * @returns 接入方设置的自定义UA
   */
  static getCustomUserAgent(): string {
    return CordovaContext.INSTANCE.config?.getCustomUserAgent() ?? ''
  }
}