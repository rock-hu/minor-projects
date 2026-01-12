# UIAbility Lifecycle

## Overview

This example demonstrates how to use **EventHub** to listen for triggered lifecycle events and log the lifecycle on the
home
page.

## Effect

![lifecycle_en.gif](screenshots/device/lifecycle_en.gif)

## How to Use

1. Tap the application. The triggered lifecycle is displayed on the home page.
2. Return to the home screen and tap the application again. The subsequent triggered lifecycle is displayed on the home
   page.

## Project Directory

```
├──entry/src/main/ets/
│  ├──entryability
│  │  └──EntryAbility.ets               // Entry ability
│  ├──entrybackupability
│  │  └──EntryBackupAbility.ets         // Backup and restoration
│  ├──model                                  
│  │  └──ModelData.ets                  // DataModel operation
│  └──pages                 
│     └──Index.ets                      // Home page
└──entry/src/main/resources             // Application resource directory
```

## How to Implement

To log the lifecycle on the home page, you can add **eventHub** listening to the **EntryAbility.ets** file.

- When the home page is first opened, the **onCreate**, **onWindowStageCreate**, and **onForeground** lifecycles are
  triggered. The
  **eventHub** is used to listen for the lifecycle and log the lifecycle on the page.
- When the user switches to the home screen and returns to the application, the **onBackground** and **onForeground** lifecycles
  are triggered. The **eventHub** is used to listen for the lifecycle and log the lifecycle on the page.

## Required Permissions

N/A

## Dependencies

N/A

## Constraints

1. This sample is only supported on Huawei phones running standard systems.
2. The HarmonyOS version must be HarmonyOS 5.1.1 Release or later.
3. The DevEco Studio version must be DevEco Studio 6.0.0 Release or later.
4. The HarmonyOS SDK version must be HarmonyOS 6.0.0 Release SDK or later.