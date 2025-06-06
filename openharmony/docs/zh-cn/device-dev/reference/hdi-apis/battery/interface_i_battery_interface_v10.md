# IBatteryInterface


## 概述

获取、订阅电池信息的接口。

服务获取此对象后，可以调用相关的接口获取、订阅电池信息。

**起始版本：** 3.1

**相关模块：**[Battery](battery_v10.md)


## 汇总


### Public 成员函数

| 名称 | 描述 | 
| -------- | -------- |
| [Register](#register) ([in] [IBatteryCallback](interface_i_battery_callback_v10.md) event) | 注册电池信息的回调。 | 
| [UnRegister](#unregister) () | 取消注册电池信息的回调。 | 
| [ChangePath](#changepath) ([in] String path) | 设置电池信息节点的路径。 | 
| [GetCapacity](#getcapacity) ([out] int capacity) | 获取电池的电量百分比。 | 
| [GetVoltage](#getvoltage) ([out] int voltage) | 获取电池的电压，单位微伏。 | 
| [GetTemperature](#gettemperature) ([out] int temperature) | 获取电池的充电温度，单位0.1摄氏度。 | 
| [GetHealthState](#gethealthstate) ([out] enum [BatteryHealthState](battery_v10.md#batteryhealthstate) healthState) | 获取电池的健康状态。 | 
| [GetPluggedType](#getpluggedtype) ([out] enum [BatteryPluggedType](battery_v10.md#batterypluggedtype) pluggedType) | 获取充电设备类型。 | 
| [GetChargeState](#getchargestate) ([out] enum [BatteryChargeState](battery_v10.md#batterychargestate) chargeState) | 获取充电状态。 | 
| [GetPresent](#getpresent) ([out] boolean present) | 获取是否支持电池或者电池是否在位。 | 
| [GetTechnology](#gettechnology) ([out] String technology) | 获取电池的技术型号。 | 
| [GetTotalEnergy](#gettotalenergy) ([out] int totalEnergy) | 获取电池的总容量。 | 
| [GetCurrentAverage](#getcurrentaverage) ([out] int curAverage) | 获取电池的平均电流。 | 
| [GetCurrentNow](#getcurrentnow) ([out] int curNow) | 获取电池的电流。 | 
| [GetRemainEnergy](#getremainenergy) ([out] int remainEnergy) | 获取电池的剩余容量。 | 
| [GetBatteryInfo](#getbatteryinfo) ([out] struct [BatteryInfo](_battery_info_v10.md) info) | 获取电池的全部信息。 | 


## 成员函数说明


### ChangePath()

```
IBatteryInterface::ChangePath ([in] String path)
```

**描述**

设置电池信息节点的路径。

**起始版本：** 3.1

**参数:**

| 名称 | 描述 | 
| -------- | -------- |
| path | 输入参数，电池信息节点的路径。 | 

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。


### GetBatteryInfo()

```
IBatteryInterface::GetBatteryInfo ([out] struct BatteryInfo info)
```

**描述**

获取电池的全部信息。

**起始版本：** 3.1

**参数:**

| 名称 | 描述 | 
| -------- | -------- |
| info | 输出参数，电池的全部信息。 | 

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。

**参见：**

[BatteryInfo](_battery_info_v10.md)


### GetCapacity()

```
IBatteryInterface::GetCapacity ([out] int capacity)
```

**描述**

获取电池的电量百分比。

**起始版本：** 3.1

**参数:**

| 名称 | 描述 | 
| -------- | -------- |
| capacity | 输出参数，表示电量的百分比值。 | 

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。


### GetChargeState()

```
IBatteryInterface::GetChargeState ([out] enum BatteryChargeState chargeState)
```

**描述**

获取充电状态。

**起始版本：** 3.1

**参数:**

| 名称 | 描述 | 
| -------- | -------- |
| chargeState | 输出参数，表示充电状态。 | 

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。

**参见：**

[BatteryChargeState](battery_v10.md#batterychargestate)


### GetCurrentAverage()

```
IBatteryInterface::GetCurrentAverage ([out] int curAverage)
```

**描述**

获取电池的平均电流。

**起始版本：** 3.1

**参数:**

| 名称 | 描述 | 
| -------- | -------- |
| curAverage | 输出参数，表示电池的平均电流，单位毫安。 | 

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。


### GetCurrentNow()

```
IBatteryInterface::GetCurrentNow ([out] int curNow)
```

**描述**

获取电池的电流。

**起始版本：** 3.1

**参数:**

| 名称 | 描述 | 
| -------- | -------- |
| curNow | 输出参数，表示电池的实时电流，单位毫安。 | 

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。


### GetHealthState()

```
IBatteryInterface::GetHealthState ([out] enum BatteryHealthState healthState)
```

**描述**

获取电池的健康状态。

**起始版本：** 3.1

**参数:**

| 名称 | 描述 | 
| -------- | -------- |
| healthState | 输出参数，表示电池健康状态。 | 

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。

**参见：**

[BatteryHealthState](battery_v10.md#batteryhealthstate)


### GetPluggedType()

```
IBatteryInterface::GetPluggedType ([out] enum BatteryPluggedType pluggedType)
```

**描述**

获取充电设备类型。

**起始版本：** 3.1

**参数:**

| 名称 | 描述 | 
| -------- | -------- |
| pluggedType | 输出参数，表示充电设备类型。 | 

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。

**参见：**

[BatteryPluggedType](battery_v10.md#batterypluggedtype)


### GetPresent()

```
IBatteryInterface::GetPresent ([out] boolean present)
```

**描述**

获取是否支持电池或者电池是否在位。

**起始版本：** 3.1

**参数:**

| 名称 | 描述 | 
| -------- | -------- |
| present | 输出参数，表示是否支持电池或者电池是否在位。true表示支持或在位，false表示不支持或不在位。 | 

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。


### GetRemainEnergy()

```
IBatteryInterface::GetRemainEnergy ([out] int remainEnergy)
```

**描述**

获取电池的剩余容量。

**起始版本：** 3.1

**参数:**

| 名称 | 描述 | 
| -------- | -------- |
| remainEnergy | 输出参数，表示电池的剩余容量，单位毫安时。 | 

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。


### GetTechnology()

```
IBatteryInterface::GetTechnology ([out] String technology)
```

**描述**

获取电池的技术型号。

**起始版本：** 3.1

**参数:**

| 名称 | 描述 | 
| -------- | -------- |
| technology | 输出参数，当前电池技术型号。 | 

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。


### GetTemperature()

```
IBatteryInterface::GetTemperature ([out] int temperature)
```

**描述**

获取电池的充电温度，单位0.1摄氏度。

**起始版本：** 3.1

**参数:**

| 名称 | 描述 | 
| -------- | -------- |
| temperature | 输出参数，表示电池温度。 | 

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。


### GetTotalEnergy()

```
IBatteryInterface::GetTotalEnergy ([out] int totalEnergy)
```

**描述**

获取电池的总容量。

**起始版本：** 3.1

**参数:**

| 名称 | 描述 | 
| -------- | -------- |
| totalEnergy | 输出参数，表示电池的总容量，单位毫安时。 | 

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。


### GetVoltage()

```
IBatteryInterface::GetVoltage ([out] int voltage)
```

**描述**

获取电池的电压，单位微伏。

**起始版本：** 3.1

**参数:**

| 名称 | 描述 | 
| -------- | -------- |
| voltage | 输出参数，表示电池的电压。 | 

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。


### Register()

```
IBatteryInterface::Register ([in] IBatteryCallback event)
```

**描述**

注册电池信息的回调。

**起始版本：** 3.1

**参数:**

| 名称 | 描述 | 
| -------- | -------- |
| event | 输入参数，服务注册的回调。 | 

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。


### UnRegister()

```
IBatteryInterface::UnRegister ()
```

**描述**

取消注册电池信息的回调。

**起始版本：** 3.1

**返回：**

HDF_SUCCESS 表示操作成功。

HDF_FAILED 表示操作失败。
