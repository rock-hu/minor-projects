# Input Device Error Codes

> **NOTE**
>
> This topic describes only module-specific error codes. For details about universal error codes, see [Universal Error Codes](../errorcode-universal.md).

<!--Del-->
## 3900001 Device Not Exist

**Error Message**

The specified device does not exist.

**Description**

This error code is reported if the specified device cannot be found in the multimodal device list.

**Possible Causes**

1. The ID of the input device has changed.
2. The physical connection of the input device is disconnected.

**Procedure**

1. Use [inputDevice.getDeviceList](js-apis-inputdevice.md#inputdevicegetdevicelist9) to query the device ID, and then pass in the correct device ID.
2. Check whether the keyboard cable is disconnected.<!--DelEnd-->

## 3900002 Keyboard Not Connected

**Error Message**

There is currently no keyboard device connected.

**Description**

This error code is reported if no connected keyboard is detected.

**Possible Causes**

The physical connection of the input device is disconnected.

**Procedure**

Check whether the keyboard cable is disconnected.

## 3900003 API Call Failed for a Non-Input Application

**Error Message**

It is prohibited for non-input applications.

**Description**

Non-input applications are not allowed to call this API.

**Possible Causes**

This API is not supported for a third-party application or a non-input system application.

**Procedure**

Use an input application to call this API.
