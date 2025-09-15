# UIAbility生命周期

## 介绍

本示例通过用户触发应用的生命周期，再使用EventHub监听，在首页打印已触发过的生命周期。

## 效果预览

![lifecycle_zh.gif](screenshots/device/lifecycle_zh.gif)

## 使用说明

1. 用户点击应用，在首页打印出已触发的生命周期。
2. 用户返回桌面，再点击应用，在首页追加打印后续触发的生命周期。

## 工程目录

```
├──entry/src/main/ets/
│  ├──entryability
│  │  └──EntryAbility.ets               // 程序入口类
│  ├──entrybackupability
│  │  └──EntryBackupAbility.ets         // 备份恢复类
│  ├──model                                  
│  │  └──DataModel.ets                  // 数据操作类
│  └──pages                 
│     └──Index.ets                      // 首页
└──entry/src/main/resources             // 应用资源目录
```

## 具体实现

首页打印生命周期是通过在EntryAbility.ets文件中添加eventHub监听实现：

- 首次打开首页，触发onCreate、onWindowStageCreate、onForeground生命周期，通过eventHub监听，在页面打印。
- 切换到桌面，在返回应用，触发onBackground、onForeground生命周期，通过eventHub监听，在页面追加打印。

## 相关权限

不涉及

## 依赖

不涉及

## 约束与限制

1. 本示例仅支持标准系统上运行，支持设备：华为手机。
2. HarmonyOS系统：HarmonyOS 5.1.1 Release及以上。
3. DevEco Studio版本：DevEco Studio 5.1.1 Release及以上。
4. HarmonyOS SDK版本：HarmonyOS 5.1.1 Release SDK及以上。