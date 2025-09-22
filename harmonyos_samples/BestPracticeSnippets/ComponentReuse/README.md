# 基于组件复用实现应用界面优化

### 介绍

HarmonyOS应用框架提供了组件复用能力：可复用组件树上移除时，会进入一个回收缓存区，后续创建新组件节点时，会复用缓存区中的节点，节约组件重新创建的时间。
本工程配套官网[组件复用最佳实践](https://developer.huawei.com/consumer/cn/doc/best-practices/bpta-component-reuse)，
文章介绍如何使用组件复用机制提升应用帧率。

### 预览效果

| 减少组件复用的嵌套层级                             | 精准控制组件刷新范围                              |
|-----------------------------------------|-----------------------------------------|
| ![image](screenshots/device/mode_1.gif) | ![image](screenshots/device/mode_2.gif) |

| 使用reuseId标记不同的组件                        | 复用组件采用@State变量入参                        |
|-----------------------------------------|-----------------------------------------|
| ![image](screenshots/device/mode_3.gif) | ![image](screenshots/device/mode_4.gif) |

#### 使用说明

1. 点击“减少组件复用的嵌套层级”按钮，进入二级页面“附近的人”， 滑动列表。
2. 点击“精准控制组件刷新范围”按钮，进入二级页面“附近的人”， 滑动列表。
3. 点击“使用reuseId标记不同的组件”按钮，进入二级页面“附近的人”， 滑动列表。
4. 点击“复用组件创建时的@State变量入参”按钮，进入二级页面“附近的人”， 滑动列表。

## 工程目录

``` 
├──entry/src/main/ets                          
│  ├──common
│  │  ├──Constants.ets                  // 公共常量
│  │  └──GlobalBuilderContext.ets       // 缓存全局@Builder
│  ├──entryability
│  │  └──EntryAbility.ets               // 程序入口类
│  ├──entrybackupability
│  │  └──EntryBackupAbility.ets         // 自定义应用数据转换和迁移模板类
│  ├──model                              
│  │  ├──BasicDataSource.ets            // 数据适配器基类
│  │  ├──ColorData.ets                  // 二级页面“文字列表”的数据适配器
│  │  ├──FriendMomentData.ets           // 二级页面“附近的人”/“图文列表”/“网名列表”的数据适配器
│  │  ├──ItemData.ets                   // 组件复用问题诊断分析场景的数据类
│  │  └──ItemDataSource.ets             // 组件复用问题诊断分析场景的数据适配器
│  ├──pages.ets
│  │  └──Index.ets                      // 首页
│  └──view        
│     ├──OneMoment.ets                  // 二级页面“附近的人”中列表的每条item UI
│     ├──PageListSlideToHistory.ets     // 二级页面“附近的人”UI                    
│     ├──UpdaterComponent.ets           // 二级页面“文字列表”UI    
│     ├──WithFuncParam.ets              // 二级页面“网名列表”UI
│     └──WithReuseId.ets                // 二级页面“图文列表”UI
└──entry/src/main/resources             // 应用资源目录
```

### 相关权限

不涉及

## 约束与限制

* 本示例仅支持标准系统上运行，支持设备：华为手机。
* HarmonyOS系统：HarmonyOS 5.0.5 Release及以上。
* DevEco Studio版本：DevEco Studio 5.0.5 Release及以上。
* HarmonyOS SDK版本：HarmonyOS 5.0.5 Release SDK及以上。