# 说明
这是一个多jsBundle和多rnInstance的demo工程。


# 目录结构

```md
MutilBundleSample
├── FlightRN 机票前端工程
├── HotelRN 酒店前端工程
├── NativeProject 鸿蒙工程
└── README.md
```


# 环境搭建
1. 在 `FlightRN` 中运行 **npm i @react-native-oh/react-native-harmony@x.x.x** 安装依赖，运行 **npm run dev:all** 生成机票的jsBundle；
2. 在 `HotelRN` 中运行 **npm i @react-native-oh/react-native-harmony@x.x.x** 安装依赖，运行 **npm run dev:all** 生成酒店的jsBundle；
3. 在 `entry` 目录下执行 **ohpm i @rnoh/react-native-openharmony@x.x.x** 安装依赖；
4. 检查 `FrameNodeSampleApp`、`entry` 目录下是否生成 `oh-modules` 文件夹；
5. 用 DevEco Studio 打开 NativeProject，执行 **Sync and Refresh Project**；
6. 点击右上角的 **run** 启动项目。

