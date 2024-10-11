# 说明
这是一个使用FrameNode进行bundle预加载的demo工程。


# 目录结构
GenerateBundle -- RN侧工程  
FrameNodeSampleApp -- 原生工程


# 环境搭建
1. 将 rnoh-react-native-harmony-xx.tgz 放到 GenerateBundle/react-native-harmony 文件夹下；
2. 将 rnoh-react-native-harmony-cli-xx.tgz 放到 GenerateBundle/react-native-harmony-cli 文件夹下；
3. 在GenerateBundle中运行**npm i**安装依赖，运行**npm run dev**生成bundle；
4. 将GenerateBundle\harmony\entry\src\main\resources\rawfile\bundle.harmony.js放置到FrameNodeSample\FrameNodeSampleApp\entry\src\main\resources\rawfile\bundle下；
5. 将 react_native_openharmony-xxx.har 放到 NativeProject/libs 文件夹下；
6. 用 DevEco Studio 打开 NativeProject，执行 **Sync and Refresh Project**；
7. 点击右上角的 **run** 启动项目。

