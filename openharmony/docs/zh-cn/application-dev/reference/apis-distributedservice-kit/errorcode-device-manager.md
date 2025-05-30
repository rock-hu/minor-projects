# 设备管理错误码

> **说明：**
>
> 以下仅介绍本模块特有错误码，通用错误码请参考[通用错误码说明文档](../errorcode-universal.md)。

## 11600101 服务调用异常

**错误信息**

Failed to execute the function.

**可能原因**

服务内部调用异常。

**处理步骤**

重新调用接口再次触发调用。

## 11600102 获取服务失败

**错误信息**

Failed to obtain the service.

**可能原因**

服务未启动或者服务启动失败。

**处理步骤**

检查服务是否正常启动，重新获取服务。

## 11600103 认证业务不可用

**错误信息**

Authentication unavailable.

**可能原因**

上一次认证业务未结束。

**处理步骤**

等待上一次认证业务结束，重新发起认证调用。

## 11600104 发现业务不可用

**错误信息**

Discovery unavailable.

**可能原因**

上一次发现业务未结束。

**处理步骤**

等待上一次发现业务结束，重新发起发现调用。

## 11600105 发布业务不可用

**错误信息**

Publish unavailable.

**可能原因**

上一次发布业务未结束。

**处理步骤**

等待上一次发布业务结束，重新发起发布调用。

## 11600106 从云端获取数据失败

**错误信息**

Failed to get data from the cloud.

**可能原因**

网络链路异常，或网络请求返回异常信息。

**处理步骤**

确保网络链路正常，并使用正确的参数重新调用接口。

## 11600107 需要登录账号

**错误信息**

A login account is required.

**可能原因**

设备没有登录账号。

**处理步骤**

使用正确账号登录设备后，重新调用接口。

## 11600108 设备名称含非法信息

**错误信息**

The device name contains non-compliant content.

**可能原因**

修改的设备名称中含有非法的信息。

**处理步骤**

确保设备名称符合法律法规。

## 32300001 重复创建传输流

**错误信息**

Only one stream can be created for the current session.

**可能原因**

每个会话只能创建一个传输流。当前会话已存在传输流。

**处理步骤**

确认上一个传输流的任务已完成，使用destroyStream接口关闭此传输流后，重新创建。

## 32300002 流接收端未启动

**错误信息**

The stream at the receive end is not started.

**可能原因**

接收端未启动。

**处理步骤**

等待接收端启动之后，启动传输流。

## 32300003 比特率不支持

**错误信息**

Bitrate not supported.

**可能原因**

配置的比特率与设备支持的比特率不匹配。

**处理步骤**

根据网络条件和需求选择合适的比特率。

## 32300004 色彩空间不支持

**错误信息**

Color space not supported.

**可能原因**

色彩空间输出类型指定为OH_COLORSPACE_BT2020_HLG_LIMIT，当前输出类型不是指定类型。

**处理步骤**

在设置色彩空间时，确保其位于支持范围内。
