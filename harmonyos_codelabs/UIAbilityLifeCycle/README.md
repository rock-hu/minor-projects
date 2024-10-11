# UIAbility和自定义组件生命周期

### 简介

本篇Codelab主要介绍UIAbility和自定义组件生命周期。

### 相关概念

- UIAbility：UIAbility组件是一种包含UI界面的应用组件，主要用于和用户交互。UIAbility组件是系统调度的基本单元，为应用提供绘制界面的窗口；一个UIAbility组件中可以通过多个页面来实现一个功能模块。每一个UIAbility组件实例，都对应于一个最近任务列表中的任务。
- 自定义组件的生命周期：自定义组件的生命周期回调函数用于通知用户该自定义组件的生命周期，这些回调函数是私有的，在运行时由开发框架在特定的时间进行调用，不能从应用程序中手动调用这些回调函数。
- 窗口开发指导：窗口模块用于在同一块物理屏幕上，提供多个应用界面显示、交互的机制。

### 相关权限

不涉及

### 使用说明

1. 通过日志打印可以在控制台检索关键字“[LifeCyclePage]”用来观察生命周期的调用过程。

### 约束与限制

1. 本示例仅支持标准系统上运行，支持设备：华为手机。
2. HarmonyOS系统：HarmonyOS NEXT Developer Beta1及以上。
3. DevEco Studio版本：DevEco Studio NEXT Developer Beta1及以上。
4. HarmonyOS SDK版本：HarmonyOS NEXT Developer Beta1 SDK及以上。
