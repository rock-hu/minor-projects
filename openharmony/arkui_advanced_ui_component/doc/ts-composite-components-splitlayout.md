# SplitLayout


上下结构布局介绍了常用的页面布局样式。主要分为上下文本和上下图文两种类型。


> ![icon-note.gif](public_sys-resources/icon-note.gif) **说明：**
> 该组件从API Version 10开始支持。后续版本如有新增内容，则采用上角标单独标记该内容的起始版本。


## 导入模块

```
import { SplitLayout } from '@ohos.arkui.advanced.SplitLayout'
```


## 子组件

无


## 接口

SplitLayout({mainImage: Resource, primaryText: string, secondaryText?: string, tertiaryText?: string})

**装饰器类型：**\@Component

**系统能力：** SystemCapability.ArkUI.ArkUI.Full

**参数：**

| 参数名 | 参数类型 | 必选 | 参数描述 | 
| -------- | -------- | -------- | -------- |
| mainImage | [ResourceStr](https://docs.openharmony.cn/pages/v4.0/zh-cn/application-dev/reference/arkui-ts/ts-types.md/#resourcestr) | 是 | 传入图片。 | 
| primaryText | [ResourceStr](https://docs.openharmony.cn/pages/v4.0/zh-cn/application-dev/reference/arkui-ts/ts-types.md/#resourcestr) | 是 | 标题内容。 | 
| secondaryText | [ResourceStr](https://docs.openharmony.cn/pages/v4.0/zh-cn/application-dev/reference/arkui-ts/ts-types.md/#resourcestr) | 否 | 副标题内容。 | 
| tertiaryText | [ResourceStr](https://docs.openharmony.cn/pages/v4.0/zh-cn/application-dev/reference/arkui-ts/ts-types.md/#resourcestr) | 否 | 辅助文本。 | 
| container | ()&nbsp;=&gt;&nbsp;void | 否 | 容器内组件。 | 


## 示例

```
import { SplitLayout } from '@ohos.arkui.advanced.SplitLayout'
@Entry
@Component
struct Index {
  @State demoImage: Resource = $r("app.media.music")
  @State primaryText: string = "主标题"
  @State secondaryText: string = "副标题"

  build() {
      Column() {
        SplitLayout({
          mainImage: this.demoImage,
          primaryText: '新歌推荐',
          secondaryText: '私人订制新歌精选站，为你推荐专属优质新歌;',
          tertiaryText: "每日更新",
        }) {
          Text('示例：空白区域容器内可添加组件')
            .margin({top:36})
        }
      }
      .justifyContent(FlexAlign.SpaceBetween)
      .height('100%')
      .width('100%')
  }
}
```


小于600Vp布局：


![zh-cn_image_0000001665553957](figures/zh-cn_image_0000001665553957.png)


大于600Vp小于840Vp布局：


![zh-cn_image_0000001616957408](figures/zh-cn_image_0000001616957408.png)


大于840Vp布局：


![zh-cn_image_0000001617116972](figures/zh-cn_image_0000001617116972.png)
