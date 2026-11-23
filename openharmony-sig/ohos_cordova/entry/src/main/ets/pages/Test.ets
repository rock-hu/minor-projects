import router from '@ohos.router'
import List from '@ohos.util.List'
import { CordovaWeb, ICordovaWebContainer, IPageContainer } from '@ohos/cordova/Index'

/**
 * 测试Cordova
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2024/5/19 16:11
 */
@Entry
@Component
struct Index {
  private readonly schemeMap = new Map([
    ["https://www.baidu.com/js/test.html", "test.html"],
    ["https://www.baidu.com/js/cordova.harmony.js", "cordova.harmony.js"],
  ])
  private readonly mimeMap = new Map([
    ["test.html", "text/html"],
    ["cordova.harmony.js", "text/javascript"],
  ])
  /**
   * 一个页面支持有多个CordovaWeb容器，使用集合记录
   */
  private readonly cordovaWebList: List<ICordovaWebContainer> = new List()

  onPageShow(): void {
    // 通知当前页面下的所有CordovaWeb容器页面显示
    this.cordovaWebList.forEach(it => it.onPageShow && it.onPageShow())
  }

  onPageHide(): void {
    // 通知当前页面下的所有CordovaWeb容器页面隐藏
    this.cordovaWebList.forEach(it => it.onPageHide && it.onPageHide())
  }

  onBackPress(): boolean | void {
    // 优先派发返回按钮点击事件给CordovaWeb容器(Cordova优先交给H5处理，其次自行退history栈)
    for (let index = 0; index < this.cordovaWebList.length; index++) {
      if (this.cordovaWebList.get(index).onBackPress && this.cordovaWebList.get(index).onBackPress!()) {
        // Web组件消费了返回事件
        return true
      }
    }
    // 如果CordovaWeb组件没有消费返回事件，则页面自行处理，即页面本身退栈
    router.back()
    return true
  }

  /**
   * CordovaWeb挂载的页面需要实现的 {@link IPageContainer} 的接口
   *
   * @param cordovaWeb 新创建的CordovaWeb实例
   */
  onCordovaWebAppear(cordovaWeb: ICordovaWebContainer): void {
    // 记录CordovaWeb实例，用于通知onBackPress、onPageShow、onPageHide等事件
    if (!this.cordovaWebList.has(cordovaWeb)) {
      this.cordovaWebList.add(cordovaWeb)
    }
  }

  /**
   * CordovaWeb挂载的页面需要实现的 {@link IPageContainer} 的接口
   *
   * @param cordovaWeb 即将销毁的CordovaWeb实例
   */
  onCordovaWebDisappear(cordovaWeb: ICordovaWebContainer): void {
    this.cordovaWebList.remove(cordovaWeb)
  }

  build() {
    Column() {

      Row() {
        Image($r('app.media.ic_navigation_back'))
          .width(48)
          .height(48)
          .padding(10)
          .objectFit(ImageFit.Contain)
          .onClick(() => {
            this.onBackPress()
          })
          .id('back_img')

        Row() {
          Text('测试Cordova')
            .fontSize(18)
            .fontColor('#F0F0F0')
            .maxLines(1)
        }
        .layoutWeight(1)
        .margin({
          right: 48
        })

      }
      .width('100%')
      .height(48)
      .backgroundColor('#333')


      CordovaWeb({
        page: () => this,
        url: 'https://www.baidu.com/js/test.html',
        parent: this as IPageContainer,
        bridges: [
          {
            name: 'HelloBridge',
            methods: 'allAllow'
          }
        ],
        onInterceptRequest: ((event) => {
          if (!event) {
            return
          }
          if (this.schemeMap.has(event.request.getRequestUrl())) {
            const response = new WebResourceResponse()
            response.setResponseIsReady(false)
            this.loadResponse(event.request.getRequestUrl(), response)
            return response
          }
          return null
        })
      })


    }
    .width('100%')
    .height('100%')
  }

  private async loadResponse(url: string, response: WebResourceResponse): Promise<void> {
    try {
      const path = this.schemeMap.get(url)
      const bytes: Uint8Array = await getContext(this).resourceManager.getRawFileContent(path!)
      response.setResponseData(bytes.buffer)
      response.setResponseEncoding('utf-8')
      response.setResponseMimeType(this.mimeMap.get(path!))
      response.setReasonMessage('ok')
      response.setResponseCode(200)
    } catch (e) {
      response.setResponseCode(404)
    } finally {
      response.setResponseIsReady(true)
    }

  }
}