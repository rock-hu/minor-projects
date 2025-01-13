### README

这个项目包含两个演示，一个演示如何在 OpenHarmony 侧将 raw 文件推送到沙箱并成功加载，另一个演示如何在 React Native 侧加载沙箱中的图片。

#### 目录结构

```
├── RegisterFontNativeProject
|   └── Index.ets
|       ├── fontResourceByFontFamily # OpenHarmony 侧注册字体的方法1
|       └── rnInstance.registerFont #OpenHarmony 侧自定义注册字体的方法2，随时可以注册字体文件
└── RegisterFontRnProject
    └── example.tsx  # React Native 侧展示使用字体的效果

```

#### OpenHarmony 侧

1. **fontResourceByFontFamily**: 常规注册字体方法，创建实例时注册
2. **rnInstance.registerFont**: 找到对应的rnInstance可随时注册字体

#### React Native 侧

1. **使用字体文件**: 使用 `example.tsx` 中的代码，直接使用OpenHarmony 侧注册的字体

#### 运行步骤

1. 在 `RegisterFontRnProject` 目录下执行 **npm i @react-native-oh/react-native-harmony@x.x.x**或**yarn add @react-native-oh/react-native-harmony@x.x.x** 安装依赖，执行 **npm run dev** 生成 bundle；
2. 在 `entry` 目录下执行 **ohpm i @rnoh/react-native-openharmony@x.x.x** 安装依赖；
3. 检查 `RegisterFontRnProject`、`entry` 目录下是否生成 `oh-modules` 文件夹；
4. 用 DevEco Studio 打开 RegisterFontNativeProject **Sync and Refresh Project**；
5. 点击右上角的 **run** 启动项目；
6. 确保鸿蒙和 rn 侧依赖下载无缺，版本配套正常；
7. 打开生成的 app 来确认文件已正确加载。

#### 注意事项

- 确保 OpenHarmony 和 React Native 项目配置正确，以便文件可以在两个系统之间无缝传递与加载。
- 沙箱路径的使用和文件访问权限需要根据具体环境进行调整。

---
