# React Native for OpenHarmony (RNOH)

欢迎阅读React Native for OpenHarmony文档，参与React Native for OpenHarmony开发者文档开源项目，与我们一起完善开发者文档。

## 启动

### 前提条件

#### 安装了HarmonyOS NEXT的模拟器或真机

该项目需要HarmonyOS NEXT上进行测试。

#### 华为开发者账号

运行应用程序时，需要用到华为开发者账号进行签名。

#### DevEco Studio

从[官网](https://developer.huawei.com/consumer/cn/deveco-studio/)下载并安装最新版本 DevEco Studio。

### 启动项目

> 如果是编译源码，在拉取工程后需要在工程的根目录执行`git submodule update --init --recursive`命令，拉取三方库依赖。

1. 打开终端（命令行工具）并导航到react-native-harmony-cli目录，然后执行 `npm i && npm pack`。
1. 在终端中导航到react-native-harmony-sample-package目录，然后执行 `npm i && npm pack`。
1. 导航到tester目录，然后执行 `npm run i` （不是 npm i）。
1. 在tester目录中运行 `npm start` 以启动Metro服务器。
1. 设置`RNOH_C_API_ARCH`环境变量的值为`1`，具体参考[配置CAPI版本环境变量](docs/zh-cn/环境搭建.md#set_capi_path)。
1. 在DevEco Studio中打开 `tester/harmony` 项目并等待后台同步完成。
1. 连接设备。
1. 点击 File > Project Structure > Signing Configs，登录并完成签名。
1. 选择 `entry`（右上角）。
1. 点击【Run 'entry'】按钮（右上角）运行项目。

## 使用说明

- React Native的使用问题可查阅[React Native 官网](https://reactnative.dev/)或[React Native 中文网](https://reactnative.cn/)。
- RNOH的使用问题可查阅[中文文档](./docs/zh-cn/README.md)或[英文文档](./docs/en/README.md)。

## 版本说明

当前发布的最新正式版本：

- [RNOH v5.0.0.813（2024-12-26）](./docs/zh-cn/release-notes/react-native-harmony-v5.0.0.813.md)

历史发布的正式版本：

- [RNOH v5.0.0.812（2024-12-14）](./docs/zh-cn/release-notes/react-native-harmony-v5.0.0.812.md)
- [RNOH v5.0.0.715（2024-10-25）](./docs/zh-cn/release-notes/react-native-harmony-v5.0.0.715.md)
- [RNOH v5.0.0.601（2024-09-26）](./docs/zh-cn/release-notes/react-native-harmony-v5.0.0.601.md)
- [RNOH v5.0.0.500（2024-08-27）](./docs/zh-cn/release-notes/react-native-harmony-v5.0.0.500.md)

## 分支说明

- 0.72.5-ohos-5.0-release分支：release分支，如需下载源码，请从此分支下载上述正式版本对应的tag点源码。
- master分支：主分支，请勿从此分支拉取源码用于商用版本，不保证质量。
- dev分支/partner-dev分支：开发分支，请勿从此分支拉取源码，不保证质量。
- 其余分支(若存在):项目内部运行过程分支，请勿从相关分支拉取源码，不保证质量。

**说明：** 0.72.5-ohos-5.0-release分支在2025/01/23进行了开源整改，整改后的CommitID与整改前不一致，请开发者重新拉取仓库最新代码。