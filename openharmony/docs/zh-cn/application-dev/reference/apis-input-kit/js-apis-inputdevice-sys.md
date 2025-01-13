# @ohos.multimodalInput.inputDevice (输入设备)(系统接口)


输入设备管理模块，用于监听输入设备连接和断开状态，查询输入设备相关信息。


> **说明**：
>
> 本模块首批接口从API version 8开始支持。后续版本的新增接口，采用上角标单独标记接口的起始版本。
>
> 当前页面仅包含模块的系统接口，其他公开接口请参考[@ohos.multimodalInput.inputDevice (输入设备)](js-apis-inputdevice.md)。


## 导入模块

```js
import { inputDevice } from '@kit.InputKit';
```

## inputDevice.setKeyboardRepeatDelay<sup>10+</sup>

setKeyboardRepeatDelay(delay: number, callback: AsyncCallback&lt;void&gt;): void

设置键盘按键的重复时延，使用AsyncCallback异步方式返回结果。

**系统能力**：SystemCapability.MultimodalInput.Input.InputDevice

**系统API**：此接口为系统接口。

**参数**：

| 参数名     | 类型   | 必填 | 说明                                                         |
| -------- | ------ | ---- | ------------------------------------------------------------ |
| delay    | number                    | 是    | 键盘按键重复延迟时间，默认值500ms，调节范围[300ms，1000ms]。 |
| callback | AsyncCallback&lt;void&gt; | 是    | 回调函数。 |

**错误码**：

以下错误码的详细介绍请参见[通用错误码](../errorcode-universal.md)。

| 错误码ID  | 错误信息             |
| ---- | --------------------- |
| 401  | Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;2. Incorrect parameter types; 3. Parameter verification failed. |

**示例**：

```js
try {
  inputDevice.setKeyboardRepeatDelay(350, (error: Error) => {
    if (error) {
      console.log(`Set keyboard repeat delay failed, error: ${JSON.stringify(error, [`code`, `message`])}`);
      return;
    }
    console.log(`Set keyboard repeat delay success`);
  });
} catch (error) {
  console.log(`Set keyboard repeat delay failed, error: ${JSON.stringify(error, [`code`, `message`])}`);
}
```

## inputDevice.setKeyboardRepeatDelay<sup>10+</sup>

setKeyboardRepeatDelay(delay: number): Promise&lt;void&gt;

设置键盘按键的重复时延，使用Promise异步方式返回结果。

**系统能力**：SystemCapability.MultimodalInput.Input.InputDevice

**系统API**：此接口为系统接口。

**参数**：

| 参数名    | 类型     | 必填   | 说明                                  |
| ----- | ------ | ---- | ----------------------------------- |
| delay | number | 是    | 键盘按键重复延迟时间，默认值500ms，调节范围[300ms，1000ms]。 |

**返回值**：

| 参数                  | 说明               |
| ------------------- | ---------------- |
| Promise&lt;void&gt; | Promise对象。 |

**错误码**：

以下错误码的详细介绍请参见[通用错误码](../errorcode-universal.md)。

| 错误码ID  | 错误信息             |
| ---- | --------------------- |
| 401  | Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;2. Incorrect parameter types; 3. Parameter verification failed. |

**示例**：

```js
try {
  inputDevice.setKeyboardRepeatDelay(350).then(() => {
    console.log(`Set keyboard repeat delay success`);
  });
} catch (error) {
  console.log(`Set keyboard repeat delay failed, error: ${JSON.stringify(error, [`code`, `message`])}`);
}
```

## inputDevice.getKeyboardRepeatDelay<sup>10+</sup>

getKeyboardRepeatDelay(callback: AsyncCallback&lt;number&gt;): void

获取键盘按键的重复时延，使用AsyncCallback异步方式返回结果。

**系统能力**：SystemCapability.MultimodalInput.Input.InputDevice

**系统API**：此接口为系统接口。

**参数**：

| 参数名     | 类型   | 必填 | 说明                                                         |
| -------- | ------ | ---- | ------------------------------------------------------------ |
| callback   | AsyncCallback&lt;number&gt;                    | 是    | 回调函数，异步返回键盘按键重复延迟时间。 |

**错误码**：

以下错误码的详细介绍请参见[通用错误码](../errorcode-universal.md)。

| 错误码ID  | 错误信息             |
| ---- | --------------------- |
| 202  | SystemAPI permission error |
| 401  | Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;2. Incorrect parameter types; 3. Parameter verification failed. |

**示例**：

```js
try {
  inputDevice.getKeyboardRepeatDelay((error: Error, delay: Number) => {
    if (error) {
      console.log(`Get keyboard repeat delay failed, error: ${JSON.stringify(error, [`code`, `message`])}`);
      return;
    }
    console.log(`Get keyboard repeat delay success`);
  });
} catch (error) {
  console.log(`Get keyboard repeat delay failed, error: ${JSON.stringify(error, [`code`, `message`])}`);
}
```

## inputDevice.getKeyboardRepeatDelay<sup>10+</sup>

getKeyboardRepeatDelay(): Promise&lt;number&gt;

获取键盘按键的重复时延，使用Promise异步方式返回结果。

**系统能力**：SystemCapability.MultimodalInput.Input.InputDevice

**系统API**：此接口为系统接口。

**返回值**：

| 参数                    | 说明                  |
| --------------------- | ------------------- |
| Promise&lt;number&gt; | Promise实例，异步返回键盘按键的重复时延。 |

**错误码**：

以下错误码的详细介绍请参见[通用错误码](../errorcode-universal.md)。

| 错误码ID  | 错误信息             |
| ---- | --------------------- |
| 202  | SystemAPI permission error |
| 401  | Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;2. Incorrect parameter types; 3. Parameter verification failed. |

**示例**：

```js
try {
  inputDevice.getKeyboardRepeatDelay().then((delay: Number) => {
    console.log(`Get keyboard repeat delay success`);
  });
} catch (error) {
  console.log(`Get keyboard repeat delay failed, error: ${JSON.stringify(error, [`code`, `message`])}`);
}
```

## inputDevice.setKeyboardRepeatRate<sup>10+</sup>

setKeyboardRepeatRate(rate: number, callback: AsyncCallback&lt;void&gt;): void

设置键盘按键的重复速率，使用AsyncCallback异步方式返回结果。

**系统能力**：SystemCapability.MultimodalInput.Input.InputDevice

**系统API**：此接口为系统接口。

**参数**：

| 参数名     | 类型   | 必填 | 说明                                                         |
| -------- | ------ | ---- | ------------------------------------------------------------ |
| rate    | number                    | 是    | 键盘按键重复速率，默认值50ms/次，调节范围[36ms/次，100ms/次]。 |
| callback | AsyncCallback&lt;void&gt; | 是    | 回调函数。 |

**错误码**：

以下错误码的详细介绍请参见[通用错误码](../errorcode-universal.md)。

| 错误码ID  | 错误信息             |
| ---- | --------------------- |
| 202  | SystemAPI permission error |
| 401  | Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;2. Incorrect parameter types; 3. Parameter verification failed. |

**示例**：

```js
try {
  inputDevice.setKeyboardRepeatRate(60, (error: Error) => {
    if (error) {
      console.log(`Set keyboard repeat rate failed, error: ${JSON.stringify(error, [`code`, `message`])}`);
      return;
    }
    console.log(`Set keyboard repeat rate success`);
  });
} catch (error) {
  console.log(`Set keyboard repeat rate failed, error: ${JSON.stringify(error, [`code`, `message`])}`);
}
```

## inputDevice.setKeyboardRepeatRate<sup>10+</sup>

setKeyboardRepeatRate(rate: number): Promise&lt;void&gt;

设置键盘按键的重复速率，使用Promise异步方式返回结果。

**系统能力**：SystemCapability.MultimodalInput.Input.InputDevice

**系统API**：此接口为系统接口。

**参数**：

| 参数名    | 类型     | 必填   | 说明                                  |
| ----- | ------ | ---- | ----------------------------------- |
| rate | number | 是    | 键盘按键重复速率，默认值50ms/次，调节范围[36ms/次，100ms/次]。 |

**返回值**：

| 参数                  | 说明               |
| ------------------- | ---------------- |
| Promise&lt;void&gt; | Promise对象。 |

**错误码**：

以下错误码的详细介绍请参见[通用错误码](../errorcode-universal.md)。

| 错误码ID  | 错误信息             |
| ---- | --------------------- |
| 202  | SystemAPI permission error |
| 401  | Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;2. Incorrect parameter types; 3. Parameter verification failed. |

**示例**：

```js
try {
  inputDevice.setKeyboardRepeatRate(60).then(() => {
    console.log(`Set keyboard repeat rate success`);
  });
} catch (error) {
  console.log(`Set keyboard repeat rate failed, error: ${JSON.stringify(error, [`code`, `message`])}`);
}
```

## inputDevice.getKeyboardRepeatRate<sup>10+</sup>

getKeyboardRepeatRate(callback: AsyncCallback&lt;number&gt;): void

获取键盘按键的重复速率，使用AsyncCallback异步方式返回结果。

**系统能力**：SystemCapability.MultimodalInput.Input.InputDevice

**系统API**：此接口为系统接口。

**参数**：

| 参数名       | 类型                          | 必填   | 说明             |
| -------- | --------------------------- | ---- | -------------- |
| callback | AsyncCallback&lt;number&gt; | 是    | 回调函数，异步返回键盘按键的重复速率。 |

**错误码**：

以下错误码的详细介绍请参见[通用错误码](../errorcode-universal.md)。

| 错误码ID  | 错误信息             |
| ---- | --------------------- |
| 202  | SystemAPI permission error |
| 401  | Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;2. Incorrect parameter types; 3. Parameter verification failed. |

**示例**：

```js
try {
  inputDevice.getKeyboardRepeatRate((error: Error, rate: Number) => {
    if (error) {
      console.log(`Get keyboard repeat rate failed, error: ${JSON.stringify(error, [`code`, `message`])}`);
      return;
    }
    console.log(`Get keyboard repeat rate success`);
  });
} catch (error) {
  console.log(`Get keyboard repeat rate failed, error: ${JSON.stringify(error, [`code`, `message`])}`);
}
```

## inputDevice.getKeyboardRepeatRate<sup>10+</sup>

getKeyboardRepeatRate(): Promise&lt;number&gt;

获取键盘按键的重复速率，使用Promise异步方式返回结果。

**系统能力**：SystemCapability.MultimodalInput.Input.InputDevice

**系统API**：此接口为系统接口。

**返回值**：

| 参数                    | 说明                  |
| --------------------- | ------------------- |
| Promise&lt;number&gt; | Promise实例，异步返回键盘按键的重复速率。 |

**错误码**：

以下错误码的详细介绍请参见[通用错误码](../errorcode-universal.md)。

| 错误码ID  | 错误信息             |
| ---- | --------------------- |
| 202  | SystemAPI permission error |
| 401  | Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;2. Incorrect parameter types; 3. Parameter verification failed. |

**示例**：

```js
try {
  inputDevice.getKeyboardRepeatRate().then((rate: Number) => {
    console.log(`Get keyboard repeat rate success`);
  });
} catch (error) {
  console.log(`Get keyboard repeat rate failed, error: ${JSON.stringify(error, [`code`, `message`])}`);
}
```

## inputDevice.setInputDeviceEnabled<sup>16+</sup>

setInputDeviceEnabled(deviceId: number, enabled: boolean): Promise&lt;void&gt;

设置输入设备的开关状态。以触摸屏为例：关闭时，点击触摸屏设备不响应；开启时，可正常操作触摸屏。

**需要权限**：ohos.permission.INPUT_DEVICE_CONTROLLER

**系统能力**：SystemCapability.MultimodalInput.Input.InputDevice

**系统API**：此接口为系统接口。

**参数**：

| 参数名   | 类型    | 必填 | 说明                      |
| -------- | ------- | ---- | ------------------------- |
| deviceId | number  | 是   | 目标设备Id。              |
| enabled  | boolean | 是   | true：开启输入设备输入；false：关闭输入设备输入。 |

**错误码**：

以下错误码的详细介绍请参见[通用错误码](../errorcode-universal.md)和[模块错误码](errorcode-inputdevice.md)。


| 错误码ID | 错误信息                                                     |
| -------- | ------------------------------------------------------------ |
| 201      | Permission denied.                                           |
| 202      | Not system application.                                      |
| 401      | Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;2. Incorrect parameter types; 3. Parameter verification failed. |
| 3900001  | The specified device does not exist.                         |

**示例**：

```js
try {
  inputDevice.setInputDeviceEnabled(0, true).then(() => {
    console.info(`Set input device enable success`);
  }).catch((error) => {
    console.info(`Set input device enable failed, error=${JSON.stringify(error)}`);
  });
} catch (error) {
    console.info(`Set input device enable error`);
}
```
## inputDevice.setFunctionKeyEnabled<sup>15+</sup>

setFunctionKeyEnabled(functionKey: FunctionKey, enabled: boolean): Promise&lt;void&gt;

设置是否使能功能键。

**需要权限**：ohos.permission.INPUT_KEYBOARD_CONTROLLER

**系统能力**：SystemCapability.MultimodalInput.Input.InputDevice

**系统API**：此接口为系统接口。

**参数**：

| 参数名   | 类型    | 必填 | 说明                      |
| -------- | ------- | ---- | ------------------------- |
| functionKey | [FunctionKey](js-apis-inputdevice.md#functionkey15) | 是   |  需要设置的功能键id。          |
| enabled  | boolean | 是   | 设置功能键是开启还是关闭状态。true代表开启状态，false代表关闭状态。 |

**错误码**：

以下错误码的详细介绍请参见[通用错误码](../errorcode-universal.md)。


| 错误码ID | 错误信息                                                     |
| -------- | ------------------------------------------------------------ |
| 201      | Permission denied.                                           |
| 401      | Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;2. Incorrect parameter types; 3. Parameter verification failed. |

**示例**：

```js
try {
  inputDevice.setInputDeviceEnabled(1, true).then(() => {
    console.info(`Set capslock state success`);
  }).catch((error) => {
    console.info(`Set capslock state failed, error=${JSON.stringify(error)}`);
  });
} catch (error) {
    console.info(`Set capslock enable error`);
}
```
## 