# 说明
这是创建自定义Fabric组件（包括CAPI组件和ArkTS组件）的demo工程。


# 目录结构

```md
FabricComponentSample
├── ReactProject 前端工程
├── fabric-component-sample-package RN三方件（自定义Fabric组件）
├── NativeProject 鸿蒙工程
├── react-native-harmony RN鸿蒙化包
├── react-native-harmony-cli RN鸿蒙化手脚架
└── README.md
```


# 环境搭建
1. 在 `fabric-component-sample-package` 中运行 **npm pack** 生成tgz文件；
2. 在 `ReactProject` 目录下执行 **npm i @react-native-oh/react-native-harmony@x.x.x**或**yarn add @react-native-oh/react-native-harmony@x.x.x** 安装依赖，执行 **npm run dev** 生成 bundle；
3. 在 `entry` 目录下执行 **ohpm i @rnoh/react-native-openharmony@x.x.x** 安装依赖；
4. 检查 `NativeProject`、`entry` 目录下是否生成 `oh-modules` 文件夹；
5. 用 DevEco Studio 打开 SampleApp，执行 **Sync and Refresh Project**；
6. 如果启动的是metro服务，还需要运行 `hdc rport tcp:8081 tcp:8081` 来转发8081端口，然后再在手机上重新打开应用。

