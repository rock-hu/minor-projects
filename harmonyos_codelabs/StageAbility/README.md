# Stage模型下Ability的创建和使用

### 简介

本篇Codelab是基于Stage模型，以购物应用为例，讲解Ability的创建和使用。效果图如下：

![](screenshots/device/screenshots.gif)

### 相关概念

- UIAbility：UIAbility组件是系统调度的基本单元，为应用提供绘制界面的窗口；一个UIAbility组件中可以通过多个页面来实现一个功能模块。每一个UIAbility组件实例，都对应于一个最近任务列表中的任务。
- UIAbilityContext：UIAbilityContext模块提供允许访问特定Ability的资源的能力，包括对Ability的启动、停止的设置、获取caller通信接口、拉起弹窗请求用户授权等。
- Want：Want是对象间信息传递的载体, 可以用于应用组件间的信息传递。 Want的使用场景之一是作为startAbility的参数, 其包含了指定的启动目标, 以及启动时需携带的相关数据。
- HiLog：HiLog日志系统，让应用可以按照指定类型、指定级别、指定格式字符串输出日志内容，帮助开发者了解应用的运行状态，更好地调试程序。

### 相关权限

不涉及

### 使用说明

1. 点击商品列表中的商品，跳转详情页。
2. 点击底部导航栏中的导航按钮，可以切换页面。
3. 点击详情页面顶部的返回按钮，关闭当前页面。

### 约束与限制

1. 本示例仅支持标准系统上运行，支持设备：华为手机。
2. HarmonyOS系统：HarmonyOS NEXT Developer Beta1及以上。
3. DevEco Studio版本：DevEco Studio NEXT Developer Beta1及以上。
4. HarmonyOS SDK版本：HarmonyOS NEXT Developer Beta1 SDK及以上。
