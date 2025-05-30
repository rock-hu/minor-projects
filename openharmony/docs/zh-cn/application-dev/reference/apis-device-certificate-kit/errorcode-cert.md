# 证书错误码

> **说明：**
>
> 以下仅介绍本模块特有错误码，通用错误码请参考[通用错误码说明文档](../errorcode-universal.md)。

## 19020001 内存错误

**错误信息**

Memory error.

**可能原因**

当前系统内存分配失败。

**处理步骤**

1. 检查当前系统功能是否正常。
2. 业务检查数据是否超长，导致系统无法分配内存。

## 19020002 运行时错误

**错误信息**

Runtime error.

**可能原因**

系统出现的不可预料的错误。

**处理步骤**

检查当前系统功能是否正常。

## 19020003 参数检查失败

**错误信息**

Indicates that parameter check failed.

**可能原因**

输入枚举值不在范围内。

**处理步骤**

检查输入参数的正确性。

## 19030001 算法相关的操作错误，调用三方算法库API出错

**错误信息**

Crypto operation error.

**可能原因**

加解密算法框架与三方算法库交互时，出现错误。

**处理步骤**

1. 检查输入参数的正确性。
2. 检查三方算法库功能是否正常。

## 19030002 证书签名验证错误

**错误信息**

The certificate signature verification failed.

**可能原因**

证书校验过程中，证书的签名信息不对。

**处理步骤**

检查证书的正确性。

## 19030003 证书尚未生效

**错误信息**

The certificate has not taken effect.

**可能原因**

证书尚未生效。证书的生效时间晚于当前系统时间或者业务指定的时间点。

**处理步骤**

1. 检查证书的正确性。
2. 检查获取的系统时间是否正确。
3. 与当前系统时间进行比对，检查证书的生效时间。

## 19030004 证书过期

**错误信息**

The certificate has expired.

**可能原因**

证书失效。基于当前系统时间或者业务指定的时间点，证书已经失效。

**处理步骤**

1. 检查证书的正确性。
2. 检查获取的系统时间是否正确。
3. 与当前系统时间进行比对，检查证书的失效时间。

## 19030005 无法获取证书的颁发者

**错误信息**

Failed to obtain the certificate issuer.

**可能原因**

证书校验过程中，无法获取当前证书的颁发者。

**处理步骤**

检查证书的正确性。

## 19030006 证书的秘钥用途不包括证书签名

**错误信息**

The key cannot be used for signing a certificate.

**可能原因**

证书的密钥用途不包括证书签名。

**处理步骤**

检查证书的正确性。

## 19030007 证书的秘钥用途不包括数字签名

**错误信息**

The key cannot be used for digital signature.

**可能原因**

证书的密钥用途不包括数字签名。

**处理步骤**

检查证书的正确性。

## 19030008 私钥密码错误

**错误信息**

The password may be wrong.

**可能原因**

私钥密码错误。

**处理步骤**

检查私钥的密码是否正确。