# 说明
这是一个不继承RNAbilty，由原生页面跳转到RN页面的demo工程。


# 目录结构
RNProject -- RN侧工程  
NativeProject -- 原生工程


# 环境搭建
1. 将 rnoh-react-native-harmony-xx.tgz 放到 RNProject/react-native-harmony 文件夹下；
2. 将 rnoh-react-native-harmony-cli-xx.tgz 放到 RNProject/react-native-harmony-cli 文件夹下；
3. 将 react_native_openharmony-xxx.har 放到 NativeProject/libs 文件夹下；
4. 在 RNProject/MainProject 中执行 **npm i** 安装依赖；
4. 在 RNProject/MainProject 中执行 **npm run dev** 命令打包；
5. 用 DevEco Studio 打开 NativeProject，执行 **Sync and Refresh Project**；
6. 点击右上角的 **run** 启动项目。

