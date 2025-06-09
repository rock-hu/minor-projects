# Application Privacy Protection

### Overview

This sample implements functions such as requesting approximate application location, selecting gallery files, and requesting sensitive permissions.

### Preview

| Requesting the Approximate Location Permission | Using Picker to Obtain Photos                | Requesting the Camera Permission             |
|------------------------------------------------|----------------------------------------------|----------------------------------------------|
| ![pic1.png](screenshots/devices/pic1.en.png)   | ![pic2.png](screenshots/devices/pic3.en.png) | ![pic3.png](screenshots/devices/pic4.en.png) |


### Project Directory

```
├──entry/src/main/ets/
│  ├──entryability
│  │  └──EntryAbility.ets                 // Entry ability lifecycle callbacks
│  ├──pages
│  │  ├──ApproximatelyLocationDemo.ets    // Approximate location demo
│  │  ├──Index.ets                        // Application entry
│  │  ├──LocationButtonDemo.ets           // LocationButton demo
│  │  └──PickerDemo.ets                   // PhotoViewPicker demo
│  └──utils   
│     └──Logger.ets                       // Log utility
└──entry/src/main/resources              // Static resources
```

### How to Implement

* Call [getCurrentLocation](https://developer.huawei.com/consumer/en/doc/harmonyos-references/js-apis-geolocationmanager#geolocationmanagergetcurrentlocation) in the **geoLocationManager** module to obtain the location information. For details, see [Obtaining Device Location Information](https://developer.huawei.com/consumer/en/doc/harmonyos-guides/location-guidelines).
* Use **PhotoViewPicker** to obtain the user photo. For details, see [Selecting User Files](https://developer.huawei.com/consumer/en/doc/harmonyos-guides/select-user-file).

### Required Permissions

**ohos.permission.APPROXIMATELY_LOCATION**: allows an application to obtain the approximate location information of a device.

**ohos.permission.CAMERA**: allows an application to use the camera.


### Dependencies

N/A

### Constraints

1. This sample is supported only on Huawei phones running the standard system.

2. The HarmonyOS version must be HarmonyOS NEXT Beta 1 or later.

3. The DevEco Studio version must be DevEco Studio NEXT Beta1 or later.

4. The HarmonyOS SDK version must be HarmonyOS NEXT Beta1 SDK or later.
