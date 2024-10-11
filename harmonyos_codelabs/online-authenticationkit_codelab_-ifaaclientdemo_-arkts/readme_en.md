# IFAA

## Overview

This sample illustrates how to use the APIs for IFAA password-free identity verification.

You can import IFAA APIs through **@kit.OnlineAuthenticationKit** to enable, perform, and disable on-device IFAA password-free identity verification.


## Preview

| App entry| App home page| API call buttons|
| --------------- | --------------- | -------------- |
| App icon for opening the app| App home page displayed on the device screen  | Tapping a button to call the corresponding API |
| <img src=".\image\startIcon.png" style="zoom:50%;" /> | <img src=".\image\homepage.jpeg" style="zoom:25%;" /> | <img src=".\image\results.jpeg" style="zoom:25%;" /> |

## Project Directory

```
├── entry/src/main/ets               // Code area
│    ├── entryability
│    │    └── EntryAbility.ets       // Entry point class.
│    ├── model
│    │    └── IfaaModel.ts           // Enabling, performing, and disabling IFAA password-free identity verification
│    └── pages
│         └── Index.ets              // Home page
```



## How to Use

1. To use fingerprint/3D face authentication for password-free identity verification, record the biometric feature information onto a mobile device.
2. After the fingerprint/3D face information is recorded onto the device, tap **Enable** to enable IFAA password-free identity verification. Biometric authentication is required in this step.
3. Tap the **Authenticate** button to perform IFAA password-free identity verification based on biometric features (face/fingerprint).
4. Tap the **Disable** button to disable IFAA password-free identity verification based on biometric features (face/fingerprint).


## How to Implement
The following uses performing IFAA password-free identity verification as an example. You need to import the following IFAA APIs first:

import { ifaa } from '@kit.OnlineAuthenticationKit'
* function preAuth():Promise<Uint8Array>;
* function auth(authToken: Uint8Array, authData: Uint8Array):Promise<Uint8Array>;

Record the fingerprint/3D face information onto the device first. Then, call the **preAuth()** API to obtain a temporary challenge for the current IFAA authentication. The challenge value must be the same as the one used for biometric authentication. The IFAA service verifies the validity period of the challenge. If the challenge is not used for a long time after being obtained, the authentication will fail.
After obtaining the fingerprint/face information by calling Authentication Kit, call the **auth()** API for identity verification. If the operation is successful, a signed packet is returned. If the operation fails, an exception is thrown.
For details about the code, see **Index.ets**.


## Required Permissions

N/A

## Dependencies

UserAuthenticationKit

## Constraints

1. The sample app is only supported on devices with standard systems.
2. The DevEco Studio version used in this sample must be DevEco Studio NEXT Developer Beta1.
3. The sample code requires packets signed by the IIFAA central server for correct results.
