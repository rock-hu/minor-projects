# 手势事件冲突解决方案

### 介绍

本代码示例展示了多个组件嵌套时同时绑定手势事件，或者同一个组件同时绑定多个手势等场景下，手势冲突问题的解决。

#### 使用说明

安装运行应用，进入应用首页，点击不同的按钮跳转到不同的手势场景示例。

## 工程目录

``` 
├──entry/src/main/ets                          
│  ├──entryability
│  │  └──EntryAbility.ets               // 程序入口类
│  ├──entrybackupability
│  │  └──EntryBackupAbility.ets         // 数据备份恢复类
│  └──pages        
│     ├──GesturesConflictScene1.ets     // 滚动容器嵌套滚动容器事件冲突
│     ├──GesturesConflictScene2.ets     // 使用组合手势同时绑定多个同类型手势冲突解反例        
│     ├──GesturesConflictScene2A.ets    // 使用组合手势同时绑定多个同类型手势冲突解正例        
│     ├──GesturesConflictScene3.ets     // 系统手势和自定义手势之间冲突反例
│     ├──GesturesConflictScene3A.ets    // 系统手势和自定义手势之间冲突正例
│     ├──GesturesConflictScene4.ets     // 手势事件透传
│     ├──GesturesConflictScene5.ets     // 多点触控场景下手势冲突
│     ├──GesturesConflictScene6.ets     // 动态控制自定义手势是否响应
│     ├──GesturesConflictScene7.ets     // 父组件如何管理子组件手势
│     ├──Index.ets                      // 应用首页
│     └──TestCode.ets                   // Web组件示例
└──entry/src/main/resources             // 应用资源目录
```

### 相关权限

不涉及

## 约束与限制

* 本示例仅支持标准系统上运行，支持设备：华为手机。
* HarmonyOS系统：HarmonyOS 5.0.5 Release及以上。
* DevEco Studio版本：DevEco Studio 5.0.5 Release及以上。
* HarmonyOS SDK版本：HarmonyOS 5.0.5 Release SDK及以上。

### 下载

如需单独下载本工程，执行如下命令：
```
git init
git config core.sparsecheckout true
echo GesturesConfictPractice/ > .git/info/sparse-checkout
git remote add origin https://gitee.com/harmonyos_samples/BestPracticeSnippets.git
git pull origin master
```