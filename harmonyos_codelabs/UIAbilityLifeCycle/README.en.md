# UIAbility Lifecycle

## Overview

This example triggers the lifecycle of the application by the user, and then uses EventHub to listen and print the
triggered lifecycle on the homepage.

## Preview

![lifecycle_en.gif](screenshots/device/lifecycle_en.gif)

## How to Use

1. The user clicks on the application and prints out the triggered lifecycle on the homepage.
2. The user returns to the desktop, clicks on the application, and adds the lifecycle triggered by printing on the
   homepage.

## Project Directory

```
├──entry/src/main/ets/
│  ├──entryability
│  │  └──EntryAbility.ets               // Entry ability
│  ├──entrybackupability
│  │  └──EntryBackupAbility.ets         // Backup and restoration
│  ├──model                                  
│  │  └──DataModel.ets                  // DataModel operation
│  └──pages                 
│     └──Index.ets                      // Home page
└──entry/src/main/resources             // Application resource directory
```

## Implementation Details

The homepage printing lifecycle is achieved by adding eventHub listener in the ElementAbility.ts file:

- Open the homepage for the first time, trigger the onCreate, onWindowStageCreate, and onForeground lifecycles, listen
  through eventHub, and print on the page.
- Switch to the desktop, return to the application, trigger the onBackground and onForeground lifecycles, listen through
  eventHub, and add printing to the page.

## Permissions

N/A

## Dependency

N/A

## Constraints

1. The sample is only supported on Huawei phones with standard systems.
2. The HarmonyOS version must be HarmonyOS 5.1.1 Release or later.
3. The DevEco Studio version must be DevEco Studio 5.1.1 Release or later.
4. The HarmonyOS SDK version must be HarmonyOS 5.1.1 Release SDK or later.