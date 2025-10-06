# 应用内存优化
## 介绍
介绍了应用内存优化的多种方式，包括LRU等。

## 工程目录
``` 
├──entry/src/main/cpps                                  // C++代码区
│  └──AddCpp.cpp                                        // PurgeableMemory
├──entry/src/main/ets                                   // 代码区
│  ├──entryability
│  │  └──EntryAbility.ets                               // 程序入口类
│  ├──entrybackupability
│  │  └──EntryBackupAbility.ets
│  ├──pages    
│  │  ├──Demo.ets                                       // LRU页面
│  │  ├──ImagePage.ets                                  // 图片组件            
│  │  └──Index.ets                                      // 首页      
│  └──utils
│     ├──Loggers.ets                                    // Logger工具类
│     └──LRUCacheUtil.ets                               // LRU工具类
└──entry/src/main/resources                             // 应用资源目录
```

## 相关权限
无

## 约束与限制
* 本示例仅支持标准系统上运行，支持设备：华为手机。

* HarmonyOS系统：HarmonyOS 5.0.5 Release及以上。

* DevEco Studio版本：DevEco Studio 5.0.5 Release及以上。

* HarmonyOS SDK版本：HarmonyOS 5.0.5 Release SDK及以上。