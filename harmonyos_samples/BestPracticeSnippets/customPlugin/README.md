# 定制hvigor插件

### 介绍

在进行编译构建的过程中，开发者可以通过定制hvigor插件，扩展构建逻辑，实现个性化的打包流程。
定制hvigor插件，通常有以下目的：

满足自定义任务需求。
每个项目可能有独特的构建需求和流程，定制插件可以根据项目的具体要求来扩展hvigor构建的功能。

加强构建任务可维护性。
定制插件可以将某些复杂的构建逻辑封装在同一个地方，使得项目的构建配置更加清晰和易于维护。可以自动化执行某些特定任务，减少手动干预，确保构建过程的一致可靠。

提升团队协作效率。
在团队开发中，定制插件可以确保所有团队成员使用相同的构建流程和标准，减少因个人配置差异导致的问题，从而提升团队协作的效率。

### 工程结构&模块类型

```
├──entry/src/main/ets/
│  ├──mock
│  │  └──GoodsMock.ets                       // 模拟数据类
│  ├──entryability
│  │  └──EntryAbility.ets                    // 配置类
│  └──model
│  │  └──GoodsModel.ets                      // 类型声明
│  └──pages
│     └──Index.ets                           // 程序入口类
└──entry/src/main/resource                   // 应用静态资源目录
```


### 约束与限制

* 本示例仅支持标准系统上运行，支持设备：华为手机。

* HarmonyOS系统：HarmonyOS 5.0.5 Release及以上。

* DevEco Studio版本：DevEco Studio 5.0.5 Release及以上。

* HarmonyOS SDK版本：HarmonyOS 5.0.5 Release SDK及以上。
