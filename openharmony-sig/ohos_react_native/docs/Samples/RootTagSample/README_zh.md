# 说明
这是获取RNSurface对应RNInstance与rootTag的demo工程。


# 目录结构

```md
RootTagSample
├── ReactProject RN前端工程
├── NativeProject 鸿蒙工程
└── README.md
```


# 环境搭建
1. 在 `ReactProject` 目录下执行 **npm i @react-native-oh/react-native-harmony@x.x.x**或**yarn add @react-native-oh/react-native-harmony@x.x.x** 安装依赖，执行 **npm run dev** 生成 bundle；
2. 在 `entry` 目录下执行 **ohpm i @rnoh/react-native-openharmony@x.x.x** 安装依赖；
3. 检查 `NativeProject`、`entry` 目录下是否生成 `oh-modules` 文件夹；
4. 用 DevEco Studio 打开 `NativeProject`，执行 **Sync and Refresh Project**；
5. 点击右上角的 **run** 启动项目。
