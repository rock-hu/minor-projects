# 蓝牙资源合理使用

### 介绍

无长时任务的应用退到后台时，不允许进行蓝牙扫描。

### 工程结构&模块类型

```
├──entry/src/main/ets/
│  ├──entryability
│  │  └──EntryAbility.ets                    // 配置类
│  └──pages
│     └──Index.ets                           // 程序入口
│     └──Https.ets                           // HTTP数据请求
│     └──Bluetooth.ets                       // 合理使用蓝牙资源
└──entry/src/main/resource                   // 应用静态资源目录

```

### 相关权限

- 需要申请权限 ohos.permission.ACCESS_BLUETOOTH。

### 约束与限制

1. 本示例仅支持标准系统上运行，支持设备：华为手机。

2. HarmonyOS 系统：HarmonyOS 5.0.5 Release及以上。

3. DevEco Studio 版本：DevEco Studio 5.0.5 Release及以上。

4. HarmonyOS SDK 版本：HarmonyOS 5.0.5 Release SDK及以上。
