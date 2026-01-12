import { ICordovaWebContainer } from './ICordovaWebContainer'

/**
 * CordovaWeb容器挂载的Page页面应实现的接口
 * 注意：该接口为鸭式编程接口，Page Component组件可以无需显示继承
 */
export interface IPageContainer {

  /**
   * 通知创建CordovaWeb实例引用
   * 请务必使用该引用向CordovaWeb派发onBackPress、onPageShow、onPageHide等事件
   * 该接口定义方便页面挂载多个CordovaWeb实例
   *
   * @param cordovaWeb 创建好的CordovaWeb实例引用，不为空
   */
  onCordovaWebAppear(cordovaWeb: ICordovaWebContainer): void

  /**
   * 通知销毁CordovaWeb实例引用
   * 当CordovaWeb实例销毁后，不得再向其派发onBackPress、onPageShow、onPageHide等事件
   * 该接口定义方便页面挂载多个CordovaWeb实例
   *
   * @param cordovaWeb 已经销毁的CordovaWeb实例引用，不为空
   */
  onCordovaWebDisappear(cordovaWeb: ICordovaWebContainer): void

}