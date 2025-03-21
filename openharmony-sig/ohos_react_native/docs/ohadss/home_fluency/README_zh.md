
# 介绍

本示例展示了RN如何实现一个[高性能的瀑布流首页](https://developer.huawei.com/consumer/cn/forum/topic/0202160237461682658)。

# 工程目录
```
home_fluency
├── RNHomeFluency 前端工程
└── HomeFluency 原生工程
```

# 安装教程
1. 在 `RNHomeFluency` 中运行 **npm i @react-native-oh/react-native-harmony@x.x.x**或**yarn add @react-native-oh/react-native-harmony@x.x.x** 安装依赖，运行 **npm run dev** 生成bundle文件，运行 **npm start** 启动metro服务；
2. 在 `entry` 目录下执行 **ohpm i @rnoh/react-native-openharmony@x.x.x** 安装依赖；
3. 用 DevEco Studio 打开 `HomeFluency`，执行 **Sync and Refresh Project**，将RNHomeFluency/HomeFluency/entry拷贝替换HomeFluency/entry；
4. 回到 DevEco Studio，点击右上角的 **run** 启动项目；
5. 如果启动的是metro服务，还需要运行 `hdc rport tcp:8081 tcp:8081` 来转发8081端口，然后再在手机上重新打开应用。
