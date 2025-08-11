# 实现相机分段式拍照功能

### 介绍

本示例基于List组件实现常见列表操作功能，主要涉及循环滚动、上拉加载、滚动到底部、左右滚动刷新、单边回弹效果、保持可见区域内容、边缘模糊效果等。


### 效果图预览
![](./screenshots/device/effect.jpg)

### 工程目录
```
├──entry/src/main/ets
│  ├──entryability
│  │  └──EntryAbility.ets                   // Ability的生命周期回调内容
│  ├──entrybackupability
│  │  └──EntryBackupAbility.ets             // 程序入口类
│  └──pages 
│     ├──EdgeBlur.ets                       // 视图层-边缘模糊效果
│     ├──Index.ets                          // 视图层-首页
│     ├──LoopScrolling.ets                  // 视图层-循环滚动
│     ├──MaintainVisibleAreaContent.ets     // 视图层-保持可见区域内容
│     ├──PullUpLoading.ets                  // 视图层-上拉加载
│     ├──ScrollLeftAndRightToRefresh.ets    // 视图层-左右滚动刷新
│     ├──ScrollToTheBottom.ets              // 视图层-滚动到底部
│     └──UnilateralRebound.ets              // 视图层-单边回弹效果
└──entry/src/main/resources                 // 应用静态资源目录
```

## 相关权限

不涉及。

## 约束与限制

* 本示例仅支持标准系统上运行，支持设备：华为手机。

* HarmonyOS系统：HarmonyOS 5.0.5 Release及以上。

* DevEco Studio版本：DevEco Studio 5.0.5 Release及以上。

* HarmonyOS SDK版本：HarmonyOS 5.0.5 Release SDK及以上。