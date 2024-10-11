# SelectTitleBar


普通型标题栏的一种，含有一个 Select 控件，可用于页面之间的切换；可用于一级页面、二级及其以上界面（配置返回键）。


> ![icon-note.gif](public_sys-resources/icon-note.gif) **说明：**
> 该组件从API Version 10开始支持。后续版本如有新增内容，则采用上角标单独标记该内容的起始版本。


## 导入模块

```
import { SelectTitleBar } from "@ohos.arkui.advanced.SelectTitleBar"
```


## 子组件

无


## 接口

SelectTitleBar({selected: number, options: Array&lt;SelectOption&gt;, menuItems?: Array&lt;SelectTitleBarMenuItem&gt;, subtitle?: ResourceStr, badgeValue?: number, hidesBackButton?: boolean, onSelected?: (index: number) =&gt; void})

**装饰器类型：**\@Component

**系统能力：** SystemCapability.ArkUI.ArkUI.Full

**参数：**

| 参数名 | 参数类型 | 必选 | 装饰器类型 | 参数描述 |
| -------- | -------- | -------- | -------- | -------- |
| selected | number | 是 | \@Prop | 当前选中项目的索引 |
| options | Array&lt;SelectOption&gt; | 是 | - | 下拉菜单中的项目 |
| menuItems | Array<SelectTitleBarMenuItem&gt; | 否 | - | 右侧菜单项目列表，定义标题栏右侧的菜单项目。 |
| subtitle | [ResourceStr](https://docs.openharmony.cn/pages/v4.0/zh-cn/application-dev/reference/arkui-ts/ts-types.md/#resourcestr) | 否 | - | 子标题 |
| badgeValue | number | 否 | - | 新事件标记 |
| hidesBackButton | boolean | 否 | - | 是否隐藏 |
| onSelected | (index:&nbsp;number)&nbsp;=&gt;&nbsp;void | 否 | - | 下拉菜单项目选中触发的闭包，传入选中项的索引 |


### SelectTitleBarMenuItem

| 名称 | 值 | 是否必填 | 描述 | 
| -------- | -------- | -------- | -------- |
| value | [ResourceStr](https://docs.openharmony.cn/pages/v4.0/zh-cn/application-dev/reference/arkui-ts/ts-types.md/#resourcestr) | 是 | 图标资源 | 
| isEnabled | boolean | 是 | 是否启用 | 
| action | ()&nbsp;=&gt;&nbsp;void | 否 | 触发时的动作闭包 | 


## 示例 1 - 一级页面下拉菜单

```
import { SelectTitleBar } from "@ohos.arkui.advanced.SelectTitleBar"
import Prompt from '@system.prompt'

@Entry
@Component
struct Index {
  build() {
    Row() {
      Column() {
        SelectTitleBar({
          options: [
            { value: '所有照片' },
            { value: '本地（设备）' },
            { value: '本地本地本地本地本地（储存卡）' }
          ],
          selected: 0,
          onSelected: (index) => Prompt.showToast({ message: 'page index ' + index }),
          hidesBackButton: true
        })
      }.width('100%')
    }.height('100%')
  }
}
```

![zh-cn_image_0000001665793709](figures/zh-cn_image_0000001665793709.png)


## 示例 2 - 二级页面下拉菜单

```
import { SelectTitleBar } from "@ohos.arkui.advanced.SelectTitleBar"
import Prompt from '@system.prompt'

@Entry
@Component
struct Index {
  build() {
    Row() {
      Column() {
        Divider().height(2).color(0xCCCCCC)
        SelectTitleBar({
          options: [
            { value: '所有照片' },
            { value: '本地（设备）' },
            { value: '本地本地本地本地本地（储存卡）' }
          ],
          selected: 0,
          onSelected: (index) => Prompt.showToast({ message: 'page index ' + index }),
          hidesBackButton: false
        })
        Divider().height(2).color(0xCCCCCC)
        SelectTitleBar({
          options: [
            { value: '所有照片' },
            { value: '本地（设备）' },
            { value: '本地本地本地本地本地（储存卡）' }
          ],
          selected: 1,
          onSelected: (index) => Prompt.showToast({ message: 'page index ' + index }),
          subtitle: "example@openharmony.cn"
        })
        Divider().height(2).color(0xCCCCCC)
        SelectTitleBar({
          options: [
            { value: '所有照片' },
            { value: '本地（设备）' },
            { value: '本地本地本地本地本地（储存卡）' }
          ],
          selected: 1,
          onSelected: (index) => Prompt.showToast({ message: 'page index ' + index }),
          subtitle: "example@openharmony.cn",
          menuItems: [ { isEnabled: true, value: $r('app.media.ic_public_save'),
            action: () => Prompt.showToast({ message: "show toast index 1" })
          } ]
        })
        Divider().height(2).color(0xCCCCCC)
      }.width('100%')
    }.height('100%')
  }
}
```

![zh-cn_image_0000001617439304](figures/zh-cn_image_0000001617439304.png)


## 示例 3 - 新事件标记

```
import { SelectTitleBar } from "@ohos.arkui.advanced.SelectTitleBar"
import Prompt from '@system.prompt'

@Entry
@Component
struct Index {
  menuItems: { value: Resource, isEnabled: boolean, action: () => void }[] =
    [ { isEnabled: true, value: $r('app.media.ic_public_save'),
        action: () => prompt.showToast({ message: "show toast index 1" })
      },
      { isEnabled: true, value: $r('app.media.ic_public_reduce'),
        action: () => prompt.showToast({ message: "show toast index 2" })
      },
      { isEnabled: true, value: $r('app.media.ic_public_edit'),
        action: () => prompt.showToast({ message: "show toast index 3" })
      },
      { isEnabled: true, value: $r('app.media.ic_public_reduce'),
        action: () => prompt.showToast({ message: "show toast index 4" })
      } ]
  build() {
    Row() {
      Column() {
        SelectTitleBar({
          options: [
            { value: '所有照片' },
            { value: '本地（设备）' },
            { value: '本地本地本地本地本地（储存卡）' }
          ],
          selected: 0,
          onSelected: (index) => Prompt.showToast({ message: 'page index ' + index }),
          subtitle: "example@openharmony.cn",
          menuItems: this.menuItems,
          badgeValue: 99,
          hidesBackButton: true
        })
      }.width('100%')
    }.height('100%')
  }
}
```

![zh-cn_image_0000001616959836](figures/zh-cn_image_0000001616959836.png)
