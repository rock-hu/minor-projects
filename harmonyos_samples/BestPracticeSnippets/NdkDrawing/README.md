# **Drawing自绘制性能提升**
## 介绍
Canvas画布组件是用来显示自绘内容的组件，它具有保留历史绘制内容、增量绘制的特点。Canvas有CanvasRenderingContext2D/OffscreenCanvasRenderingContext2D和DrawingRenderingContext两套API，应用使用两套绘制API绘制的内容都可以在绑定的Canvas组件上显示。其中CanvasRenderingContext2D按照W3C标准封装了Native Drawing接口，可以方便快速复用Web应用的绘制逻辑，因此非常适用于Web应用和游戏、快速原型设计、数据可视化、在线绘图板、教学工具或创意应用等场景。

## 预览效果

<img alt="img.png" src="./screenshots/Screenshot.png" width="300"/>

## 工程目录
``` 
├──entry/src/main/ets                                   // 代码区
│  ├──entryability
│  │  └──EntryAbility.ets                               // 程序入口类
│  ├──entrybackupability
│  │  └──EntryBackupAbility.ets
│  ├──pages                              
│  │  └──Index.ets                                      // 首页
│  └──views
│     ├──GlassCoverView.ets                       
│     └──Index.ets                                 
└──entry/src/main/resources                             // 应用资源目录
```

## 相关权限
无

## 约束与限制
* 本示例仅支持标准系统上运行，支持设备：华为手机。

* HarmonyOS系统：HarmonyOS 5.0.5 Release及以上。

* DevEco Studio版本：DevEco Studio 5.0.5 Release及以上。

* HarmonyOS SDK版本：HarmonyOS 5.0.5 Release SDK及以上。