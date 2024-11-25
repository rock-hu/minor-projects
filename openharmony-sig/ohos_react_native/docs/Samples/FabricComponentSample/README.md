# 说明
这是创建自定义Fabric组件（包括CAPI组件和ArkTs组件）的demo工程。


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
1. 将 `rnoh-react-native-harmony-xx.tgz` 放到 `react-native-harmony` 文件夹下，然后修改 `ReactProject/package.json` 的 `react-native-harmony` 依赖路径；
2. 将 `rnoh-react-native-harmony-cli-xx.tgz` 放到 `react-native-harmony-cli` 文件夹下；
3. 在 `fabric-component-sample-package` 中运行 **npm pack** 生成tgz文件；
4. 将 `react_native_openharmony-xxx.har` 放到 `NativeProject/libs` 文件夹下，并修改 `NativeProject/oh-package.json5` 和 `NativeProject/entry/oh-package.json5` 的依赖路径；
5. 用 DevEco Studio 打开 `NativeProject`，执行 **Sync and Refresh Project**；
6. 在 `ReactProject` 中运行 **npm i** 安装依赖，运行 **npm run dev** 生成bundle文件，运行 **npm start** 启动metro服务；
7. 回到 DevEco Studio，点击右上角的 **run** 启动项目；
8. 如果启动的是metro服务，还需要运行 `hdc rport tcp:8081 tcp:8081` 来转发8081端口，然后再在手机上重新打开应用。

