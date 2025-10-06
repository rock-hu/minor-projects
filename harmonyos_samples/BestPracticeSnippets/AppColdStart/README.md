# 应用冷启动时延优化同源示例代码

### 介绍

本示例代码为最佳实践《应用冷启动时延优化》配套示例代码。

#### 使用说明

不涉及

## 工程目录

``` 
├──entry/src/main/ets                      
│  ├──entryability
│  │  └──EntryAbility.ets                   // 程序入口类
│  ├──entrybackupability
│  │  └──EntryBackupAbility.ets             // 数据备份恢复类
│  ├──pages
│  │  ├──Index.ets                          // 使用columnStart/columnEnd设置GridItem大小示例：反例
│  │  └──RightIndex.ets                     // 使用GridLayoutOptions设置GridItem大小：正例
│  └──pages         
│     ├──ColdStartSlow.ets                  // 冷启动缓慢示例
│     ├──ColdStartSpeedOptimization.ets     // 冷启快速示例
│     ├──ComputeTaskAsync.ets               // 异步任务示例
│     ├──ImportMainPage.ets                 // 页面导入示例
│     ├──ImportMainPageDemo.ets             // 减少导入文件示例
│     ├──ImportOthers.ets                   // 导入大量hsp示例
│     ├──ImportSubPage.ets                  // 导入次级页面
│     ├──Index.ets                          // 应用首页
│     ├──NewIndex.ets                       // 展示网路图片首页
│     ├──NotRecommendDemo.ets               // 不推荐iport示例
│     ├──ReduceImport.ets                   // 减少import导入示例
│     ├──ScenariosExample.ets               // 网络二次刷新示例
│     └──SecondPage.ets                     // 跳转页面
└──entry/src/main/resources                 // 应用资源目录
```

### 相关权限

不涉及

## 约束与限制

* 本示例仅支持标准系统上运行，支持设备：华为手机。
* HarmonyOS系统：HarmonyOS 5.0.5 Release及以上。
* DevEco Studio版本：DevEco Studio 5.0.5 Release及以上。
* HarmonyOS SDK版本：HarmonyOS 5.0.5 Release SDK及以上。