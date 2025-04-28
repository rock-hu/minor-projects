# **主线程耗时操作优化指导**
## 介绍
在应用开发实践中，有效避免主线程执行冗余与易耗时操作是至关重要的策略。此举能有效降低主线程负载，提升UI的响应速度。面对高频回调接口在短时间内密集触发的场景，需要避免接口内的耗时操作，尽量保证主线程不被长时间占用，从而防止阻塞UI渲染，引发界面卡顿或掉帧现象。
本实例结束开发过程中常见的冗余操作，常见的高频回调场景以及其他主线程优化思路。

## 预览效果
![](screenshots/output-15_53_54.gif)
## 工程目录
``` 
├──entry/src/main/ets                                   // 代码区
│  ├──common
│  │  └──Item.ets                                       // 公共区
│  ├──entryability
│  │  └──EntryAbility.ets                               // 程序入口类
│  ├──entrybackupability
│  │  └──EntryBackupAbility.ets
│  ├──pages                              
│  │  └──Index.ets                                      // 首页
│  └──views.ets
│     ├──ConditionalRendering.ets                       // 条件渲染页
│     ├──GetStrOfId.ets                                 // ID资源获取页
│     ├──GetStrOfResource.ets                           // 资源获取页
│     ├──NegativeOfGrid.ets                             // 组件复用反例
│     ├──NegativeOfLazyForEach.ets                      // 循环渲染反例
│     ├──NegativeOfOnScroll.ets                         // 高频事件回调执行耗时操作页
│     ├──NegativeOfProperty.ets                         // 组件属性反例页
│     ├──NoRedundantOperation.ets                       // 不含有冗余操作页
│     ├──PositiveOfGrid.ets                             // 组件复用正例页
│     ├──PositiveOfLazyForEach.ets                      // 循环渲染正例页
│     ├──PositiveOfOnScroll.ets                         // 高频事件回调不执行耗时操作页
│     ├──PositiveOfProperty.ets                         // 组件属性正例页
│     ├──RedundantOperation.ets                         // 含有冗余操作页
│     ├──UseAsync.ets                                   // 使用异步页
│     ├──UseTaskPool.ets                                // 使用多线程页
│     └──WaterFlowDataSource.ets                        // 瀑布流懒加载数据源页
└──entry/src/main/resources                             // 应用资源目录
``` 
## 使用说明
“避免冗余操作”的正例和反例，反例中包含release版本中冗余日志打印、Trace打点以及无业务代码的空回调，降低了性能。

“高频回调场景中避免执行耗时操作”的正例和反例，以下常见高频回调场景中需要避免执行耗时操作:
* 高频事件回调
* 组件复用回调
* 组件生命周期回调
* 循环渲染
* 组件属性

反例中在高频事件中执行了上述耗时操作，降低了性能。

“避免使用耗时接口”的正例和反例，反例中使用了没有指定资源id的接口，导致耗时增加，从而影响了性能。

“使用多线程”是正例，开启多线程后，渲染页面速度加快，提升了性能。
## 相关权限
无

## 约束与限制
* 本示例仅支持标准系统上运行，支持设备：华为手机。

* HarmonyOS系统：HarmonyOS NEXT Release及以上。

* DevEco Studio版本：DevEco Studio NEXT Release及以上。

* HarmonyOS SDK版本：HarmonyOS NEXT Release SDK及以上。