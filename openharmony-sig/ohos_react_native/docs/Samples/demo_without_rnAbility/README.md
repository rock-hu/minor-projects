# 说明
这是一个不继承RNAbilty，由原生页面跳转到RN页面的demo工程。


# 目录结构
RNProject -- RN侧工程  
NativeProject -- 原生工程


# 环境搭建
1. 在 `library` 目录下执行 **ohpm i @rnoh/react-native-openharmony@x.x.x** 安装依赖；
2. 执行完成后，检查是否在 `NativeProject`、`entry`、`library` 目录下生成 `oh-modules` 文件夹；
3. 在 `RNProject/MainProject` 中执行 **npm i @react-native-oh/react-native-harmony@x.x.x** 安装依赖；
4. 在 `RNProject/MainProject` 中执行 **npm run dev** 命令打包；
4. 用 DevEco Studio 打开 `NativeProject`，执行 **Sync and Refresh Project**；
5. 点击右上角的 **run** 启动项目。
   

# 注意事项

1. 主动调用`windowSizeChange`方法监听屏幕尺寸变化，调用`this.rnInstancesCoordinator?.onWindowSizeChange`设置正确的屏幕尺寸，否则会出现字体size不正常的问题。