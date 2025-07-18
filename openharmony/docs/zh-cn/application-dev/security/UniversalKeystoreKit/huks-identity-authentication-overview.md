# 用户身份认证访问控制简介

HUKS提供了完备的密钥访问控制能力，以保证存储在HUKS中的密钥被合法正确地访问：

- 业务只能访问属于自己的密钥，即只能访问通过HUKS生成或导入的密钥。

- 支持密钥的用户身份认证访问控制，对于高安全级别、保护敏感数据的业务密钥，需要在使用密钥的时候，再次要求用户即时的验证锁屏密码或生物特征，验证通过后，才能使用业务密钥。

- 支持严格限制密钥的使用用途，如支持只允许AES密钥进行加密解密，只允许RSA密钥进行签名验签。

> **说明：**
> 轻量级设备不支持用户身份认证访问控制功能。

## 使用场景及开发流程

整个用户身份认证访问控制，涉及多个环节，介绍如下。

### 用户身份认证

生成/导入密钥时，可以指定密钥必须经过用户身份认证后才能使用。应用可以指定用于解锁设备锁屏的凭据（锁屏密码、指纹、人脸）的子集进行身份认证，详情请参考[用户身份认证和授权访问类型详细规格](#用户身份认证和授权访问类型详细规格)。

生成/导入密钥后，即使应用进程被攻击也不会导致未经用户授权的密钥访问，一般用于高敏感且高级别安全保护的场景，比如免密登录、免密支付、自动填充密码保护等。

### 设置授权访问类型

除用户身份认证外，应用还需将密钥的授权访问类型（即失效条件）设置为以下三种类型之一：

- **清除锁屏密码后，密钥永久失效。**
  在生成/导入密钥后，一旦清除锁屏密码，此类密钥将永久失效。修改密码不会导致失效情况发生。

  前提条件：当前用户已经设置了锁屏密码。

  适用场景：保护与用户强相关的数据、锁屏密码授权访问某部分功能。

- **用户录入新生物特征后，该特征下密钥永久失效。**
  在生成/导入密钥后，一旦录入新的生物特征，该特征下此类密钥将永久失效。仅删除生物特征不会导致失效情况发生。

  这表示该授权访问类型下，仅录入同类型的新生物特征会无法使用此类密钥，而其它身份认证方式（其它生物特征、锁屏密码）仍可正常使用此类密钥。需要注意的是，如果清除锁屏密码，该授权访问类型的密钥也将永久失效。

  前提条件：当前用户至少录入了一个生物特征（如指纹）。

  使用场景：免密登录，免密支付等。

- **用户密钥总是有效。**
  使用此模式生成的密钥将永久有效，除非用户主动删除。

  这表示无论用户如何清除、更改锁屏密码和生物特征，只要用户身份认证成功，此类密钥总是可用。

  密钥生成时将绑定前台用户id，使用此模式生成的密钥时，仅对前台用户id进行校验。

### 支持挑战值校验

此外，为了保证密钥使用时用户认证结果的有效性（不可重放），HUKS支持挑战值校验。

在身份认证前，需要从HUKS获取挑战值传给用户身份认证方法，然后在密钥操作时校验认证令牌的挑战值。

### 二次认证密钥访问控制

除此之外，二次认证密钥访问控制还包含以下三种方式：

- **根据AuthToken进行访问控制**

  ![访问控制(2)](figures/访问控制(2).png)

  用户身份认证访问控制流程如上图所示。

  其中，AT KEY（对称密钥）由HUKS TA每次启动时根据设备的安全根密钥重新派生而成。且UserIAM 与HUKS 的交互位于TEE环境内。

- **允许一次认证多次授权**

  密钥管理支持一次认证多次授权，允许多个密钥的访问控制授权通过一次认证完成。

  适用场景：如手机克隆，希望一次性导出数据，只需要校验一次锁屏密码。

  使用方式：HUKS 每次生成的Challenge为8字节，而传入UserIAM中的Challenge可扩展为32字节，因而支持一次授权4个密钥的访问。业务调用HUKS为每个密钥生成8字节的Challenge，而后将多个Challenge拼接为一个Challenge传入UserIAM进行认证。

- **支持基于timestamp的访问控制**

  对于某些特殊场景，如高性能场景，支持基于timestamp的免Challenge实现。在生成密钥时设置超时时间，业务使用时直接请求UserIAM发起认证，而后将包含了timestamp的AuthToken传给HUKS，HUKS校验AuthToken后，对比当前时间与timestamp差值是否大于超时时间，从而判断密钥是否允许使用。

## 用户身份认证和授权访问类型详细规格

为方便查阅，在表格中将使用对应的简写指代具体的用户认证类型，其对应关系如下：

- HUKS_USER_AUTH_TYPE_PIN，即锁屏密码。简写为PIN。
- HUKS_USER_AUTH_TYPE_FACE，即生物特征（人脸）。简写为FACE。
- HUKS_USER_AUTH_TYPE_FINGERPRINT，即生物特征（指纹）。简写为FINGERPRINT。
<!--RP1--><!--RP1End-->

安全访问控制类型简写为：
- HUKS_AUTH_ACCESS_INVALID_CLEAR_PASSWORD，即清除锁屏密码后，密钥永久失效。简写为INVALID_CLEAR_PASSWORD。
- HUKS_AUTH_ACCESS_INVALID_NEW_BIO_ENROLL，即用户录入新生物特征后，该特征下密钥永久失效。简写为 INVALID_NEW_BIO_ENROLL。
- HUKS_AUTH_ACCESS_ALWAYS_VALID，即用户密钥总是有效。简写为ALWAYS_VALID。

| 用户身份认证                      | 授权访问类型     | 说明                                                  |
| -------------------------------| -------------- | -------------------------------------   |
| PIN                            | INVALID_CLEAR_PASSWORD | 当前仅支持锁屏密码进行身份认证，清除锁屏密码后，密钥永久失效。  |
| FACE                           | INVALID_CLEAR_PASSWORD | 当前仅支持人脸进行身份认证，清除锁屏密码后，密钥永久失效。  |
| FINGERPRINT                    | INVALID_CLEAR_PASSWORD | 当前仅支持指纹进行身份认证，清除锁屏密码后，密钥永久失效。  |
| PIN  \|  FACE                  | INVALID_CLEAR_PASSWORD | 当前支持锁屏密码或人脸任一凭据进行身份认证，清除锁屏密码后，密钥永久失效。  |
| PIN  \|  FINGERPRINT           | INVALID_CLEAR_PASSWORD | 当前支持锁屏密码或指纹任一凭据进行身份认证，清除锁屏密码后，密钥永久失效。    |
| PIN  \|  FACE  \|  FINGERPRINT | INVALID_CLEAR_PASSWORD | 当前支持锁屏密码、人脸或指纹任一凭据进行身份认证，清除锁屏密码后，密钥永久失效。    |
| FACE                           | INVALID_NEW_BIO_ENROLL | 当前仅支持人脸进行身份认证，录入新的人脸后，密钥永久失效。  |
| FINGERPRINT                    | INVALID_NEW_BIO_ENROLL | 当前仅支持指纹进行身份认证，录入新的指纹后，密钥永久失效。  |
| PIN  \|  FACE                  | INVALID_NEW_BIO_ENROLL | 当前支持锁屏密码或人脸任一凭据进行身份认证，录入新的人脸后，用户通过人脸凭据无法使用密钥，通过锁屏密码仍可正常使用密钥；清除锁屏密码后，密钥永久失效。   |
| PIN  \|  FINGERPRINT           | INVALID_NEW_BIO_ENROLL | 当前支持锁屏密码或指纹任一凭据进行身份认证，录入新的指纹后，用户通过指纹凭据无法使用密钥，通过锁屏密码仍可正常使用密钥；清除锁屏密码后，密钥永久失效。   |
| FACE  \|  FINGERPRINT          | INVALID_NEW_BIO_ENROLL | 当前支持人脸或指纹任一凭据进行身份认证，录入新的生物特征后，用户通过该生物特征凭据无法使用密钥，其它凭据不受影响仍可正常使用密钥。    |
| PIN  \|  FACE  \|  FINGERPRINT | INVALID_NEW_BIO_ENROLL | 当前支持锁屏密码、人脸或指纹任一凭据进行身份认证，录入新的生物特征后，用户通过该生物特征凭据无法使用密钥，其它凭据不受影响仍可正常使用密钥；清除锁屏密码后，密钥永久失效。    |
| PIN                            | ALWAYS_VALID           | 当前仅支持锁屏密码进行身份认证，密钥永久有效。    |
| FACE                           | ALWAYS_VALID           | 当前仅支持人脸进行身份认证，密钥永久有效。    |
| FINGERPRINT                    | ALWAYS_VALID           | 当前仅支持指纹进行身份认证，密钥永久有效。    |
| PIN  \|  FACE                  | ALWAYS_VALID           | 当前支持锁屏密码或人脸进行身份认证，密钥永久有效。    |
| PIN  \|  FINGERPRINT           | ALWAYS_VALID           | 当前支持锁屏密码或指纹进行身份认证，密钥永久有效。    |
| FACE  \|  FINGERPRINT          | ALWAYS_VALID           | 当前支持人脸或指纹进行身份认证，密钥永久有效。    |
| PIN  \|  FACE  \|  FINGERPRINT | ALWAYS_VALID           | 当前支持锁屏密码、人脸或指纹进行身份认证，密钥永久有效。    |
<!--RP2--><!--RP2End-->

