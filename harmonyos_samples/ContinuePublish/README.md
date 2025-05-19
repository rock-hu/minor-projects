# 基于应用接续及多样的分布式功能实现多种数据类型的接续

## 介绍

本示例基于应用接续、分布式数据对象、分布式文件系统等功能，实现文本数据以及图片接续。通过分布式数据对象跨设备传输数据；点击系统Dock栏，使应用接续并进行数据传输。开发者可学会应用接续和分布式数据使用能力。

## 效果图预览

![](screenshots/device/ContinuePublish_CN.gif)

## 使用说明

1. 进入应用首页，点击“添加优质首图”，选择图片后，图片横向显示在列表中。
2. 点击“标题”/“正文”输入框，可以输入相应内容。
3. 点击左下“添加位置”，“添加位置”下方区域会多出一行附近位置信息，选中后，“添加位置”会变成选中的位置信息。
4. 本端打开应用接续（内容发布）应用，对端设备的Dock栏中会新增应用接续（内容发布）应用图标，点击应用接续（内容发布）应用，本端应用会接续到对端设备上去。

## 工程目录

```
├──entry/src/main/ets                         // 代码区
│  ├──constants
│  │  ├──BreakpointConstants.ets              // 断点常量类
│  │  └──CommonConstants.ets                  // 公共常量
│  ├──entryability
│  │  └──EntryAbility.ets       
│  ├──model
│  │  └──CommonModel.ets                      // 公共实体类       
│  ├──pages
│  │  └──Home.ets                             // 主页面       
│  ├──utils
│  │  ├──FileUtil.ets                         // 选择图片工具类
│  │  └──LocationUtil.ets                     // 本地位置工具类
│  └──view
│     ├──AddPic.ets                           // 添加图片模块
│     ├──BottomToolbar.ets                    // 底部工具模块
│     ├──EditorComponent.ets                  // 编辑输入模块
│     └──TopArea.ets                          // 顶部区域模块
└──entry/src/main/resources                   // 应用资源目录
```

## 相关权限

- 申请获取设备位置信息权限:ohos.permission.APPROXIMATELY_LOCATION和ohos.permission.LOCATION。

## 依赖

不涉及

## 约束与限制

1. 本示例仅支持标准系统上运行，支持设备：华为手机。

2. HarmonyOS系统：HarmonyOS 5.0.2 Release及以上。

3. DevEco Studio版本：DevEco Studio 5.0.2 Release及以上。

4. HarmonyOS SDK版本：HarmonyOS 5.0.2 Release SDK及以上。

5. 双端设备需要登录同一华为账号。

6. 双端设备需要打开Wi-Fi和蓝牙开关。条件允许时，建议双端设备接入同一个局域网，可提升数据传输的速度。 

7. 应用接续只能在同应用（UIAbility）之间触发，双端设备都需要有该应用。