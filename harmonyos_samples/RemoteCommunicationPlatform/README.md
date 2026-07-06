# 基于RCP的网络请求能力
## 介绍
RCP指的是远程通信平台（remote communication platform），RCP提供网络数据请求功能，相较于Network Kit中HTTP请求能力，RCP更具易用性，且拥有更多的功能。例如：
* 支持发送PATCH请求

* 支持设置BaseURL

* 支持取消请求，取消指定或正在进行的会话请求

* 支持自定义证书校验，可以根据自身业务需要，对证书进行校验

* 支持自定义DNS解析

* 支持自定义拦截器实现响应缓存

* 支持在会话中的HTTP请求期间捕获详细的跟踪信息
## 预览效果
![](screenshots/device/rcp.gif)
## 工程目录
``` 
├──entry/src/main/ets                                   // 代码区
│  ├──common
│  |  ├──CommonConstants.ets                            // 日志工具类
│  |  └──Logger.ets                                     // 常量
│  ├──entryability
│  |  └──EntryAbility.ets                               // 程序入口类
│  ├──entrybackupability
│  |  └──EntryBackupAbility.ets
│  ├──pages
│  |  ├──BaseRequest.ets                                // 基础网络请求
│  |  ├──DNSSetting.ets                                 // DNS相关设置
│  |  ├──Index.ets                                      // 首页
│  |  ├──MultipartForm.ets                              // 多表单提交
│  |  ├──RequestAndResponse.ets                         // 请求与响应拦截示例
│  |  └──TracingPoint.ets                               // 捕获有关HTTP请求/响应流的详细信息
|  └──utils.ets                            
│     └──Interceptors.ets                               // 请求与响应拦截
└──entry/src/main/resources                             // 应用资源目录
``` 
## 相关权限
应用需要访问互联网，需要使用网络权限。

`ohos.permission.INTERNET`
## 使用说明
1. 点击“基础请求”按钮跳转至二级页面，点击“发起PATCH请求”按钮，可发起PATCH请求；点击“设置会话中URL的基地址”按钮，可设置请求的BaseUrl。
2. 点击“多表单提交”按钮跳转至二级页面，在表单一中输入姓名；切换至表单二中选择兴趣爱好。点击“提交”按钮，可进行表单提交并展示相关参数。
3. 点击“请求与响应拦截”按钮跳转至二级页面，点击“获取图片”按钮，获取未经拦截的图片地址；开启请求缓存开关，再点击“获取图片”按钮，可获取经拦截处理后的图片地址。
4. 点击“DNS设置”按钮跳转至二级页面，点击“自定义DNS服务器”按钮，可对DNS服务器进行自定义；点击“自定义静态DNS”按钮，可设置静态的DNS服务器；点击“配置HTTP上的DNS”按钮，可配置HTTP上的DNS设置，从而通过HTTPS端点实现安全的DNS解析。
5. 点击“捕获有关HTTP请求/响应流的详细信息”按钮跳转至二级页面，点击“配置详细信息收集”按钮后，可进行相关设置并展示设置的参数。
## 约束与限制
* 本示例仅支持标准系统上运行，支持设备：华为手机。

* HarmonyOS系统：HarmonyOS 5.0.5 Release及以上。

* DevEco Studio版本：DevEco Studio 5.0.5 Release及以上。
  
* HarmonyOS SDK版本：HarmonyOS 5.0.5 Release SDK及以上。