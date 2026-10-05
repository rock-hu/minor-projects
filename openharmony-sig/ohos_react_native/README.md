# React Native for OpenHarmony (RNOH)

Welcome to the React Native for OpenHarmony documentation.

## 启动

### 前提条件

#### 安装了HarmonyOS NEXT的模拟器或真机
该项目需要HarmonyOS NEXT上进行测试。

#### 华为开发者账号
运行应用程序时，需要用到华为开发者账号进行签名。

#### DevEco Studio
从[官网](https://developer.huawei.com/consumer/cn/deveco-studio/)下载并安装最新版本 DevEco Studio。

### 启动项目

1. 打开终端（命令行工具）并导航到react-native-harmony-cli目录，然后执行 `npm i && npm pack`。
1. 在终端中导航到react-native-harmony-sample-package目录，然后执行 `npm i && npm pack`。
1. 导航到tester目录，然后执行 `npm run i` （不是 npm i）。
1. 在tester目录中运行 `npm start` 以启动Metro服务器。
1. 设置`RNOH_C_API_ARCH`环境变量的值为`1`，具体参考[配置CAPI版本环境变量](docs/en/environment-setup.md#set_capi_path)。
1. 在DevEco Studio中打开 `tester/harmony` 项目并等待后台同步完成。
1. 连接设备。
1. 点击 File > Project Structure > Signing Configs，登录并完成签名。
1. 选择 `entry`（右上角）。
1. 点击【Run 'entry'】按钮（右上角）运行项目。

## Usage

- React Native的使用问题可查阅[React Native 官网](https://reactnative.dev/)或[React Native 中文网](https://reactnative.cn/)。
- RNOH的使用问题可查阅[中文文档](./docs/zh-cn/README.md)或[英文文档](./docs/en/README.md)。