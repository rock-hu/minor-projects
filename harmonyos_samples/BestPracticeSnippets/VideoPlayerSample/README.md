# 视频播放类应用

### 简介

本示例主要展示通过HarmonyOS提供的系统播放器AVPlayer和媒体会话等能力，实现视频类应用的开发。

### 效果预览

![](screenshots/videoPlayer.gif)

使用说明：

1.启动应用后显示视频播放列表及首个视频自动播放

2.点击页面按钮可切换竖屏或横屏模式

3.上下滑动切换视频，并能看到历史播放记录

4.拖动进度条或全屏手势滑动可调节播放进度

5.应用切换到后台可以持续播放并且可以播控中心进行控制


### 工程目录

```
├──entry/src/main/ets                              
│  ├──entryability  
│  │  └──EntryAbility.ets               // 程序入口
│  ├──model
│  │  ├──BasicDataSource.ets            // 懒加载DataSource
│  │  └──DataModel.ets                  // 数据类
│  ├──pages                                     
│  │  └──IndexPage.ets                  // 首页
│  ├──utils                                     
│  │  └──WindowUtil.ets                 // 窗口工具类
│  └──view
│     ├──AVPlayer.ets                   // 视频组件
│     ├──VideoList.ets                  // 首页视频列表
│     ├──VideoSide.ets                  // 视频滑动组件
│     └──VideoDetails.ets               // 视频详情信息组件
│──entry/src/main/resources             // 应用资源目录
│
└──MediaService/src/main/ets
   ├──common  
   │  └──CommonConstants.ets            //常量类
   ├──controller
   │  ├──AvPlayerController.ets         //视频播放控制
   │  └──AvSessionController.ets        //媒体会话控制
   ├──model
   │  └──VideoData.ets                  //视频数据类
   └──utils
       ├──BackgroundTaskManager.ets     // 后台播放功能
       ├──CommUtils.ets                 // 工具类
       ├──ImageUtil.ets                 // 图片像素处理类
       └──Logger.ets                    // 日志
```

### 相关权限

1.后台任务权限：ohos.permission.KEEP_BACKGROUND_RUNNING。

2.Internet网络权限：ohos.permission.INTERNET。

### 约束与限制

1.本示例仅支持标准系统上运行，支持设备：华为手机。

2.HarmonyOS系统：HarmonyOS NEXT Beta1及以上。

3.DevEco Studio版本：DevEco Studio NEXT Beta1及以上。

4.HarmonyOS SDK版本：HarmonyOS NEXT Beta1 SDK及以上。
