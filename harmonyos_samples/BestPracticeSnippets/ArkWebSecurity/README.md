# ArkWeb组件安全开发示例代码

## 项目简介

本示例是ArkWeb组件安全开发最佳实践中的代码示例片段。

## 效果预览

无

## 使用说明

本示例为最佳实践示例代码，开发者可以查阅官网最佳实践下ArkWeb组件安全开发最佳实践学习。

## 工程目录
```
├── ArkWebSecurity
│   ├── src/main/ets
│   │   ├── pages
│   │   │   ├── GeolocationShow.ets                     // 地理位置权限
│   │   │   ├── getURL.ets                              // 自定义白名单校验
│   │   │   ├── Index.ets                               // 主页入口
│   │   │   ├── LoadURL.ets                             // 外部输入过滤
│   │   │   ├── MixedMode.ets                           // 混合内容加载
│   │   │   ├── OnInterceptRequest.ets                  // 校验文件URL
│   │   │   ├── OnSslErrorEventReceive.ets              // SSL错误事件处理
│   │   │   ├── PermissionRequest.ets                   // 多媒体权限
│   │   │   ├── PostMessage.ets                         // 网页消息传递
│   │   │   ├── RunJavaScript2.ets                      // JS代码拼接加载
│   │   │   ├── SetCookie.ets                           // Cookie管理
│   │   │   └── SetURLTrustList.ets                     // URL白名单设置
│   │   ├── ErrorSamples
│   │   │   ├── GeolocationShow.ets                     // 地理位置权限
│   │   │   ├── GetURL.ets                              // 自定义白名单校验
│   │   │   ├── JavaScriptProxy.ets                     // JSBridge权限
│   │   │   ├── LoadURL.ets                             // 外部输入过滤
│   │   │   ├── LoadURLWithSensitiveData.ets            // URL拼接敏感信息
│   │   │   ├── MixedMode.ets                           // 混合内容加载
│   │   │   ├── OnInterceptRequest.ets                  // 校验文件URL
│   │   │   ├── OnOverrideUrlLoading.ets                // 覆写页面重载
│   │   │   ├── OnSslErrorEventReceive.ets              // SSL错误事件处理
│   │   │   ├── PermissionRequest.ets                   // 多媒体权限
│   │   │   ├── PostMessage.ets                         // 网页消息传递
│   │   │   ├── Redirection.ets                         // 不安全页面跳转
│   │   │   ├── RunJavaScript.ets                       // 外部JS脚本加载
│   │   │   ├── RunJavaScript2.ets                      // JS代码拼接加载
│   │   │   ├── SetCookie.ets                           // Cookie管理
│   │   │   ├── SetURLTrustList.ets                     // URL白名单设置
│   │   │   └── WebDebug.ets                            // 资源目录内容不展开
│   │   └── arkwebsecuritybackupability
│   │       └── ArkWebSecurityBackupAbility.ets         // 备份能力
│   └── resources                                       // 资源目录内容不展开
```

## 具体实现

该示例使用ArkWeb组件进行开发，实现页面加载、注册与使用JSBridge、数据传输等功能。

## 相关权限

不涉及。

## 约束与限制

1. 本示例仅支持标准系统上运行，支持设备：华为手机。

2. HarmonyOS系统：HarmonyOS 6.0.0 Release及以上。

3. DevEco Studio版本：DevEco Studio 6.0.0 Release及以上。

4. HarmonyOS SDK版本：HarmonyOS 6.0.0 Release SDK及以上。

## 下载

如需单独下载本工程，执行如下命令：
```
git clone --filter=blob:none --no-checkout https://gitee.com/harmonyos_samples/BestPracticeSnippets.git
cd BestPracticeSnippets
git sparse-checkout init --cone
git sparse-checkout set ArkWebSecurity
git checkout
```