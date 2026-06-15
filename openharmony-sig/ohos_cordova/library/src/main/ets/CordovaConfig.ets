/**
 * Cordova全局配置
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2024/5/19 14:02
 */
export class CordovaConfig {
  /**
   * 是否调试模式，默认为生产发布模式
   * Debug模式下遇到错误将以异常形式抛出，Release模式遇到错误将尽力兜底，避免异常导致Crash
   */
  private debug: boolean = false
  /**
   * 是否可输出日志
   * 注意：仅在debug为true调试模式时生效，生产发布时禁止输出日志
   */
  private canLog: boolean = false
  /**
   * 是否开启WebView Inspect调试
   * 注意：仅在debug为true调试模式时生效，生产发布时禁止输出Inspect调试
   */
  private inspect: boolean = false
  /**
   * 是否忽略Web加载过程中的SSL握手错误
   * 注意：仅在调试模式下生效，生产发布模式强制完成SSL握手以及校验
   */
  private ignoreSslError: boolean = false
  /**
   * 设置自定义的UA
   */
  private userAgent: string = ''

  /**
   * 设置是否为调试模式，默认为Release生产发布模式
   * Debug模式下错误将以异常形式抛出，以强烈的信号通知开发者，便于开发者发现问题
   * Release模式下将尽力规避异常，保障程序尽力运行，避免Crash
   *
   * @param debug true表示开发调试模式，否则为生产发布模式
   * @returns 构建器本身，便于级联调用
   */
  setDebug(debug: boolean): CordovaConfig {
    this.debug = debug
    return this
  }

  /**
   * 是否为开发调试模式
   *
   * @returns true表示开发调试模式，否则为生产发布模式
   */
  isDebug(): boolean {
    return this.debug
  }

  /**
   * 设置是否可以打印输出日志
   * 注意：Release生产发布模式强制不输出日志
   *
   * @param canLog true表示可以输出日志，否则不输出日志
   * @returns 构建器本身，便于级联调用
   */
  setCanLog(canLog: boolean): CordovaConfig {
    this.canLog = canLog
    return this
  }

  /**
   * 是否可以输出打印日志
   *
   * @returns true表示可以输出日志，否则不输出日志
   */
  isCanLog(): boolean {
    return this.canLog
  }

  /**
   * 设置是否可以Inspect调试Web组件
   * 注意：Release生产发布模式强制不得Inspect贴士
   *
   * @param inspect true表示可以Inspect调试，否则不得Inspect调试
   * @returns 构建器本身，便于级联调用
   */
  setInspect(inspect: boolean): CordovaConfig {
    this.inspect = inspect
    return this
  }

  /**
   * 是否可以Inspect调试Web组件
   *
   * @returns true表示可以Inspect调试，否则不得Inspect调试
   */
  enableInspect(): boolean {
    return this.inspect
  }

  /**
   * 设置是否忽略Web加载过程中的SSL错误
   * 注意：Release生产发布模式强制不得忽略
   *
   * @param ignoreSslError true表示可以忽略，否则不得忽略
   * @returns 构建器本身，便于级联调用
   */
  setIgnoreSslError(ignoreSslError: boolean): CordovaConfig {
    this.ignoreSslError = ignoreSslError
    return this
  }

  /**
   * 是否可以忽略Web加载过程中的SSL错误
   *
   * @returns true表示可以忽略，否则不得忽略
   */
  isIgnoreSslError(): boolean {
    return this.ignoreSslError
  }

  /**
   * 设置自定义的UA
   *
   * @param userAgent 接入方设置的自定义UA
   * @returns 构建器本身，便于级联调用
   */
  setCustomUserAgent(userAgent: string): CordovaConfig {
    this.userAgent = userAgent
    return this
  }

  /**
   * 获取当前接入方设置的自定义UA
   *
   * @returns 接入方设置的自定义UA
   */
  getCustomUserAgent(): string {
    return this.userAgent
  }
}