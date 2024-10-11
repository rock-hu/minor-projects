# Codelab Sample Code Demo

# Overview

Scan Kit is a basic capability provided by the HarmonyOS SDK. It provides functions including directly accessing content through scanning, generating barcode images, scanning barcodes using the default UI, scanning barcodes using a custom UI, and recognizing barcodes based on images.

|           **App Home Page**            |               **Barcode Image Generation UI**               |              **Default Barcode Scan UI**             |              **Custom Barcode Scan UI**             |             **Saved Image-Based Barcode Recognition UI**             |          **Scan-to-Access UI**           |
|:--------------------------------------:|:---------------------------------------:|:--------------------------------------:|:-------------------------------------:|:-------------------------------------:|:----------------------------------------:|
| ![](entry/src/screenshots/home_en.jpg) | ![](entry/src/screenshots/generate_en.jpg) | ![](entry/src/screenshots/default_en.jpg) | ![](entry/src/screenshots/custom_en.jpg) | ![](entry/src/screenshots/detect_en.jpg) | ![](entry/src/screenshots/access_en.jpg) |

## Required Permissions

**ohos.permission.CAMERA**: camera permission required by barcode scanning customization.

# How to Use

Open the demo app. The level-1 menu screen will be displayed, showing buttons for using functions of Scan Kit.

- Follow the [App Linking integration guide](https://developer.huawei.com/consumer/en/doc/harmonyos-guides-V5/app-linking-startup-V5) to register a domain name. After setting up the domain name mapping, install the test app locally. Then, launch the barcode scanner in HarmonyOS (found in the control panel) and scan the barcode provided by the app to go to the **Scan to Access** page. Please note that the **Scan to Access** button is not provided on the level-1 menu page of the demo.
- Tap **Generate barcode image** to call the barcode image generation API to generate different types of barcode images.
- Tap **Scan code on default UI** to scan a barcode using the default UI.
- Tap **Scan code on custom UI** to scan a barcode using the custom UI.
- Tap **Recognize barcode in image**. On the level-2 page, tap **Saved image** and select an image.

# Constraints

1. This sample can only be run on standard-system Huawei phones and tablets.
2. The HarmonyOS version must be HarmonyOS Next Developer Beta 3 or later.
3. The DevEco Studio version must be DevEco Studio Next Developer Beta 3 or later.
4. The HarmonyOS SDK version must be HarmonyOS NEXT Developer Beta3 SDK or later.
