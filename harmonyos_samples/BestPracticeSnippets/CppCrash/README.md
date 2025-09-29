# Native崩溃（Cpp Crash）样例代码工程

### 介绍

本示例为Native崩溃（Cpp Crash）样例代码工程，包含最佳实践文档中涉及的分析方法、优化建议、问题案例等内容的局部样例代码。工程本身不具备实际功能，开发者请直接阅读具体源码结合文档来理解Native崩溃（Cpp Crash）类问题的产生原因，规避方法等。


### 效果预览

不涉及

### 工程目录
```
├──entry/src/main/
│  └──cpp
│  │   ├── CMakeLists.txt
│  │   ├── CppCrashAdvise1.cpp                 // C++ 样例代码
│  │   ├── CppCrashAdvise2.cpp                 // C++ 样例代码
│  │   ├── CppCrashAdvise3.cpp                 // C++ 样例代码
│  │   ├── CppCrashAdvise4.cpp                 // C++ 样例代码
│  │   ├── CppCrashCaseAnalyse1.cpp            // C++ 样例代码
│  │   ├── CppCrashCaseAnalyse2.cpp            // C++ 样例代码
│  │   ├── CppCrashCaseAnalyse3.cpp            // C++ 样例代码
│  │   ├── CppCrashCaseAnalyse4.cpp            // C++ 样例代码
│  │   ├── CppCrashCaseAnalyse5.cpp            // C++ 样例代码
│  │   ├── CppCrashCaseAnalyse6.cpp            // C++ 样例代码
│  │   ├── CppCrashCaseAnalyse7.cpp            // C++ 样例代码
│  │   ├── CppCrashCaseAnalysis8.cpp           // C++ 样例代码
│  │   ├── CppCrashQuestionCheck1.cpp          // C++ 样例代码
│  │   ├── CppCrashQuestionCheck2.cpp          // C++ 样例代码
│  │   └── napi_init.cpp                       // C++ 样例代码
│  └──ets
│      ├──pages
│      │    └──Index.ets                        // 首页
│      ├──entryability
│      │    └──EntryAbility.ets                 // Ability的生命周期回调内容
│      └──entrybackupability
│           └──EntryBackupAbility.ets           // 应用数据备份恢复类
└──entry/src/main/resources                     // 应用资源目录      
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

3. DevEco Studio版本：DevEco Studio 5.0.5 Release及以上。

4. HarmonyOS SDK版本：HarmonyOS NEXT 5.0.5 Release SDK及以上。