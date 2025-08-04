# 内存检测样例代码工程

### 介绍

本示例为内存检测样例代码工程，包含最佳实践文档中涉及的分析方法、优化建议、问题案例等内容的局部样例代码。工程本身不具备实际功能，开发者请直接阅读具体源码结合文档来理解地址越界、资源泄漏类问题的产生原因，规避方法等。


### 效果预览

不涉及

### 工程目录
```
.
└── entry
    └── src
        ├── main
            ├── cpp
            │   ├── CMakeLists.txt
            │   ├── address_problems.cpp       C++ 样例代码
            │   ├── address_sanitizer_case1.cpp       C++ 样例代码
            │   ├── address_sanitizer_case2.cpp       C++ 样例代码
            │   ├── address_sanitizer_case3.cpp       C++ 样例代码
            │   ├── address_sanitizer_case4.cpp       C++ 样例代码
            │   ├── napi_init.cpp       C++ 样例代码
            │   ├── resource_leak.cpp       C++ 样例代码
            │   ├── task_demo1.cpp       C++ 样例代码
            │   └── task_demo2.cpp       C++ 样例代码
            └── ets
                └── pages
                    ├── FdLeakDetection.ets       ArkTS样例代码
                    ├── GwpAsan.cpp       C++ 样例代码
                    ├── Index.ets       ArkTS样例代码
                    ├── MemoryLeakDetection.ets       ArkTS样例代码
                    ├── SetAsan.cpp       C++ 样例代码
                    ├── address_overflow_case.cpp       C++ 样例代码
                    ├── addressproblems.cpp       C++ 样例代码
                    ├── resource_leak_case.ets       C++ 样例代码
                    └── setHwAsan.cpp       C++ 样例代码
```

### 具体实现

不涉及。

### 相关权限

不涉及。

### 依赖

不涉及。

###  约束与限制

1. 本示例仅支持标准系统上运行，支持设备：华为手机、华为PC/2in1设备、华为平板。

2. HarmonyOS系统：HarmonyOS NEXT 5.1.1 Release及以上。

3. DevEco Studio版本：DevEco Studio NEXT 5.1.1 Release及以上。

4. HarmonyOS SDK版本：HarmonyOS NEXT 5.1.1 Release SDK及以上。