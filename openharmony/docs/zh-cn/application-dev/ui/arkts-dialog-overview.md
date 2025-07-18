# 弹窗概述
弹窗一般指打开应用时自动弹出或者用户行为操作时弹出的UI界面，用于短时间内展示用户需关注的信息或待处理的操作。

## 弹窗的种类

根据用户交互操作场景，弹窗可分为**模态弹窗**和**非模态弹窗**两种类型，其区别在于用户是否必须对其做出响应。

* **模态弹窗：** 为强交互形式，会中断用户当前的操作流程，要求用户必须做出响应才能继续其他操作，通常用于需要向用户传达重要信息的场景。
* **非模态弹窗：** 为弱交互形式，不会影响用户当前操作行为，用户可以不对其进行回应，通常都有时间限制，出现一段时间后会自动消失。一般用于告诉用户信息内容外还需要用户进行功能操作的场景。

> **说明：** 
>
> 当前模态弹窗通过设置指定属性变成非模态，例如AlertDialog，可以设置[isModal](../reference//apis-arkui/arkui-ts/ts-methods-alert-dialog-box.md#alertdialogparam对象说明)为false变成非模态，其他弹窗详见API说明。
>
> 不建议在非前台状态下，调用弹窗显示接口。
>
> 系统弹窗由系统弹出，出于安全考虑，不支持自定义样式。
>
> 系统弹窗出现时，调用非系统弹窗的显示接口（例如：promptAction的openCustomDialog、CustomDialogController的open等），禁止该类弹窗显示。

## 使用场景

开发者可根据实际应用场景选择合适的弹窗进行页面开发。

弹窗名称 | 应用场景 |
| --- | --- |
| [弹出框（Dialog）](arkts-base-dialog-overview.md) | 当需要展示用户当前需要或必须关注的信息内容或操作时，例如二次退出应用等，应优先考虑使用此弹出框。 |
| [菜单（Menu/openMenu）](arkts-menu-overview.md)  | 当需要给用户提供可执行的操作时，例如长按图标展示操作选项等，应优先考虑使用此弹窗。 |
| [气泡提示（Popup/openPopup）](arkts-popup-overview.md)  | 当需要给用户提供提示时，例如点击一个问号图标弹出一段帮助提示等，应优先考虑使用此弹窗。 |
| [绑定模态页面（bindContentCover/bindSheet）](arkts-modal-overview.md)  | 当需要新的界面覆盖在旧的界面上，且旧的界面不消失的一种转场方式时，例如缩略图片点击后查看大图等，应优先考虑使用此弹窗。 |
|  [即时反馈（Toast）](arkts-create-toast.md)  | 当需要在一个小的窗口中提供用户当前操作的简单反馈时，例如提示文件保存成功等，应用优先考虑使用此弹窗。 |
|  [设置浮层（OverlayManager）](arkts-create-overlaymanager.md)  | 当需要完全自定义内容、行为、样式时，可以使用浮层将UI展示在页面之上，例如音乐/语音播放悬浮球/胶囊等，应优先考虑使用此弹窗。 |

## 规格约束

* 多个弹窗组件先后弹出时，后弹出的组件的层级高于先弹出的层级，退出时按照层级从高到低的顺序逐次退出。
* 在移动设备中，子窗模式的弹窗当前无法超出主窗口。而在2in1设备上，使用模态类弹窗时，会出现需要超出主窗口显示的场景，开发者可以通过设置showInSubWindow为true实现超出主窗口的显示效果。如下图所示：

  ![image](figures/Dialog01.png)


