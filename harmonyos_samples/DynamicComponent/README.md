# 组件动态创建

### 介绍

本示例介绍了在声明式UI中实现组件动态创建的方法

### 效果图预览
| 主页面                                  | 广告关闭弹窗                                  |
|--------------------------------------|-----------------------------------------|
| ![](screenshots/device/MainPage.png) | ![](screenshots/device/CloseDialog.png) |

**使用说明**

1. 进入应用后，广告组件随即被动态创建。其分为两种，分别为图片广告与视频广告。

### 实现思路

1. 构建广告组件。源码参考[AdBuilder.ets](./entry/src/main/ets/components/AdBuilder.ets)。
2. 模拟从云端初始化卡片列表，根据云端数据生成普通卡片对象和广告卡片对象。源码参考代码可参考[MainPage.ets](./entry/src/main/ets/pages/MainPage.ets)中的aboutToAppear()。
3. 在布局中，需要判断节点是否是广告节点，若是则预埋NodeContainer节点。当NodeContainer节点要进行渲染时，会调用getAdNodeController方法获取对应的NodeController，通过NodeController中的makeNode接口完成组件上树。源码参考代码可参考[MainPage.ets](./entry/src/main/ets/pages/MainPage.ets)中的build()。
4. 在getAdNodeController中通过queryAdById(adId)模拟了广告信息/布局获取，并在完成信息获取后构建了相应NodeController，并实现makeNode接口。 AdNodeController继承自NodeController，目的是为了给广告模块提供Controller，其中initAd方法通过 this.adNode.build(…)将广告组件添加到rootNode上。
   当NodeContrainer进行绘制时，会调用AdNodeController的makeNode方法，将构建好的rootNode返回后，组件实现上树。源码参考代码可参考[AdController.ets](./entry/src/main/ets/components/AdController.ets)。
5. 在广告组件中，当点击关闭按钮时，通过node.remove()通知Controller需要移除广告，再通过node.rebuild();触发组件重绘，此时系统会再次执行makeNode接口。通过在AdNodeController的makeNode方法中返回null，实现组件下树，达到隐藏广告的目的。源码参考代码可参考[AdBuilder.ets](./entry/src/main/ets/components/AdBuilder.ets)和[AdController.ets](./entry/src/main/ets/components/AdController.ets)。

### 工程结构&模块类型

   ```
    ├───entry/src/main/ets                             
    │   ├───common
    │   │   └───TypeCasting.ets                          // 类型转换工具
    │   ├───components
    │   │   ├───AdBuilder.ets                            // 广告组件的Builder
    │   │   ├───AdController.ets                         // 广告组件的Controller
    │   │   └───CardComponent.ets                        // 卡片组件
    │   ├───entryability
    │   │   └───EntryAbility.ets                         // Ability的生命周期回调内容
    │   ├───model
    │   │   ├───AdParams.ets                             // 广告组件参数
    │   │   └───CardData.ets                             // 卡片参数
    │   └───pages
    │       ├───Index.ets                                // 应用入口
    │       ├───MainPage.ets                             // 主页面
    │       └───TitleBar.ets                             // 标题头
    └───entry/src/main/resources                         // 应用静态资源目录
   ```

### 模块依赖

**不涉及**

### 约束与限制

1.本示例仅支持标准系统上运行，支持设备：华为手机。

2.HarmonyOS系统：HarmonyOS NEXT Developer Beta3及以上。

3.DevEco Studio版本：DevEco Studio NEXT Developer Beta3及以上。

4.HarmonyOS SDK版本：HarmonyOS NEXT Developer Beta3 SDK 及以上。