# EditableTitleBar


编辑型标题栏，适用于多选界面或者内容的编辑界面，一般采取左叉右勾的形式。


> ![icon-note.gif](public_sys-resources/icon-note.gif) **说明：**
> 该组件从API Version 10开始支持。后续版本如有新增内容，则采用上角标单独标记该内容的起始版本。


## 导入模块

```
import { EditableTitleBar } from "@ohos.arkui.advanced.EditableTitleBar"
```


## 子组件

无


## 接口

EditableTitleBar({leftIconType: EditableLeftIconType, title: ResourceStr, menuItems?: Array&lt;EditableTitleBarMenuItem&gt;, onSave?: () =&gt; void, onCancel?: () =&gt;void})

**装饰器类型：**\@Component

**系统能力：** SystemCapability.ArkUI.ArkUI.Full

**参数：**

| 参数名 | 参数类型 | 必选 | 参数描述 | 
| -------- | -------- | -------- | -------- |
| leftIconStyle | [ERROR:Invalid&nbsp;link:zh-cn_topic_0000001665632009.xml#xref19441410133613,link:zh-cn_topic_0000001658583341.xml#section1340683083317](zh-cn_topic_0000001658583341.xml#section1340683083317) | 是 | 左侧按钮类型 | 
| title | [ResourceStr](https://docs.openharmony.cn/pages/v4.0/zh-cn/application-dev/reference/arkui-ts/ts-types.md/#resourcestr) | 是 | 标题 | 
| menuItems | Array&lt;[ERROR:Invalid&nbsp;link:zh-cn_topic_0000001665632009.xml#xref81499144365,link:zh-cn_topic_0000001658583341.xml#section1287821819325](zh-cn_topic_0000001658583341.xml#section1287821819325)&gt; | 否 | 右侧菜单项目列表 | 
| onSave | ()&nbsp;=&gt;&nbsp;void | 否 | 保存时的动作闭包 | 
| onCancel | ()&nbsp;=&gt;&nbsp;void | 否 | 当左侧按钮类型为&nbsp;Left，触发取消时的动作闭包 | 


### EditableLeftIconType

| 名称 | 描述 | 
| -------- | -------- |
| Back | 返回按钮 | 
| Cancel | 取消按钮 | 


### EditableTitleBarMenuItem

| 名称 | 值 | 是否必填 | 描述 | 
| -------- | -------- | -------- | -------- |
| value | [ResourceStr](https://docs.openharmony.cn/pages/v4.0/zh-cn/application-dev/reference/arkui-ts/ts-types.md/#resourcestr) | 是 | 图标资源 | 
| isEnabled | boolean | 是 | 是否启用，默认启用 | 
| action | ()&nbsp;=&gt;&nbsp;void | 否 | 触发时的动作闭包 | 


## 示例 1

```
import { EditableLeftIconType } from "@ohos.arkui.advanced.EditableTitleBar"
import { EditableTitleBar } from "@ohos.arkui.advanced.EditableTitleBar"
import Prompt from '@system.prompt'

@Entry
@Component
struct Index {
  build() {
    Row() {
      Column() {
        EditableTitleBar({
          leftIconStyle: EditableLeftIconType.Cancel,
          title: "编辑页面",
          menuItems: [],
          onCancel: () => {
            Prompt.showToast({ message: "on cancel" })
          },
          onSave: () => {
            Prompt.showToast({ message: "on save" })
          }
        })
      }.width('100%')
    }.height('100%')
  }
}
```


![zh-cn_image_0000001617073302](figures/zh-cn_image_0000001617073302.png)


## 示例 2

```
import { EditableLeftIconType } from "@ohos.arkui.advanced.EditableTitleBar"
import { EditableTitleBar } from "@ohos.arkui.advanced.EditableTitleBar"
import Prompt from '@system.prompt'

@Entry
@Component
struct Index {
  build() {
    Row() {
      Column() {
        EditableTitleBar({
          leftIconStyle: EditableLeftIconType.Back,
          title: "编辑页面",
          menuItems: [
            { value: $r('app.media.ic_public_reduce'),
              isEnabled: false,
              action: () => {
                Prompt.showToast({ message: "show toast index 2" })
              }
            }
          ],
          onCancel: () => {
            Prompt.showToast({ message: "on cancel" })
          },
          onSave: () => {
            Prompt.showToast({ message: "on save" })
          }
        })
      }.width('100%')
    }.height('100%')
  }
}
```

![zh-cn_image_0000001665393297](figures/zh-cn_image_0000001665393297.png)
