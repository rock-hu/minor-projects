# Introduction

This example demonstrates how to implement a [high-performance waterfall flow homepage](https://developer.huawei.com/consumer/cn/forum/topic/0202160237461682658) using React Native.

# Project Directory

```
home-fluency
├── RNHomeFluency   Frontend Project
└── HomeFluency     Native Project
```

# Installation Guide
1. Replace the version of @rnoh/react-native-openharmony in RNHomeFluency/package.json, HomeFluency/oh-package.json5, and HomeFluency/entry/oh-package.json5 (Optional: Upgrade the version).
2. In `RNHomeFluency`, run **npm i --force** to install dependencies, then run **npm run dev** to generate the bundle file, and **npm start** to start the Metro service;
3. Open `HomeFluency` in DevEco Studio, execute **Sync and Refresh Project**, and copy and replace `RNHomeFluency/entry` into `HomeFluency/entry`;
4. Return to DevEco Studio, click the **run** button on the top right to start the project;
5. If the Metro service is running, you will also need to execute `hdc rport tcp:8081 tcp:8081` to forward port 8081, and then reopen the app on the device.