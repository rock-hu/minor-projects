# 跨线程序列化耗时问题分析样例代码工程

### 介绍

本示例为跨线程序列化耗时问题分析样例代码工程，包含最佳实践文档中示例代码等内容的局部样例代码。工程本身不具备实际功能，开发者请直接阅读具体源码结合文档来理解。


### 效果预览

不涉及

### 工程目录
```
├──entry/src/main/ets
│  ├──entryability
│  │  └──EntryAbility.ets                   // 程序入口类
│  ├──entrybackupability
│  │  └──EntryBackupAbility.ets             // 应用数据备份恢复类
│  └──pages
│      └──Index.ets                         // 首页
└──entry/src/main/resources                 // 应用静态资源目录
```

### 具体实现

不涉及。

### 相关权限

不涉及。

### 依赖

不涉及。

###  约束与限制

1. 本示例仅支持标准系统上运行，支持设备：华为手机、华为PC/2in1设备、华为平板。

2. HarmonyOS系统：HarmonyOS NEXT 5.0.5 Release及以上。

3. DevEco Studio版本：DevEco Studio NEXT 5.0.5 Release及以上。

4. HarmonyOS SDK版本：HarmonyOS NEXT 5.0.5 Release SDK及以上。

### 下载

如需单独下载本工程，执行如下命令：
```
git init
git config core.sparsecheckout true
echo PerformanceAnalysis/CrossThreadSerializationDelay/ > .git/info/sparse-checkout
git remote add origin https://gitee.com/harmonyos_samples/BestPracticeSnippets.git
git pull origin master
```