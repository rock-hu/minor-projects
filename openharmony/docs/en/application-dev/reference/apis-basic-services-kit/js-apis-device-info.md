# @ohos.deviceInfo (Device Information)

The **deviceInfo** module provides terminal device information query, which cannot be configured by developers.

> **NOTE**
>
> The initial APIs of this module are supported since API version 6. Newly added APIs will be marked with a superscript to indicate their earliest API version.
> Some parameters whose return value is the default value are not yet available.
> The APIs of this module return information about device constants. You're not expected to call these APIs frequently.

## Modules to Import

```ts
import { deviceInfo } from '@kit.BasicServicesKit';
```

## Attributes
> **NOTE**
> Unless otherwise specified, the maximum data length is 96 bytes.

**System capability**: SystemCapability.Startup.SystemInfo

**Required permissions**: The items in the table below require different system capabilities.

| Name| Type| Readable| Writable| Description| Example|
| -------- | -------- | -------- | -------- | -------- | -------- |
| deviceType | string | Yes| No| Device type. For details, see [deviceTypes tag](../../quick-start/module-configuration-file.md#devicetypes).<br>**Atomic service API**: This API can be used in atomic services since API version 11.| phone |
| manufacture | string | Yes| No| Device manufacturer.| HUAWEI |
| brand | string | Yes| No| Device brand.<br>**Atomic service API**: This API can be used in atomic services since API version 11.| HUAWEI |
| marketName | string | Yes| No| Marketing name.| Mate XX |
| productSeries | string | Yes| No| Product series.| TAS |
| productModel | string | Yes| No| Product model.<br>**Atomic service API**: This API can be used in atomic services since API version 11.| TAS-AL00 |
| softwareModel | string | Yes| No| Software model.| TAS-AL00 |
| hardwareModel | string | Yes| No| Hardware model.| TASA00CVN1 |
| hardwareProfile<sup>(deprecated) </sup> | string | Yes| No| Hardware profile.<br>**NOTE**<br>This API is supported since API version 6 and deprecated since API version 9.| NA |
| serial | string | Yes| No| Device SN, available only for system applications.<br>**NOTE**<br>The device SN can be used as the unique identifier of a device.<br>**Required permissions**: ohos.permission.sec.ACCESS_UDID| The device SN varies according to devices.|
| bootloaderVersion | string | Yes| No| Bootloader version.| bootloater |
| abiList | string | Yes| No| Application binary interface (Abi) list.| arm64-v8a |
| securityPatchTag | string | Yes| No| Security patch tag.| 2021-01-01 |
| displayVersion | string | Yes| No| Product version.| XXX X.X.X.X |
| incrementalVersion | string | Yes| No| Incremental version.| NA |
| osReleaseType | string | Yes| No| OS release type. The options are as follows:<br>- **Canary**: Preliminary release open only to specific developers. This release does not promise API stability and may require tolerance of instability.<br>- **Beta**: Release open to all developers. This release does not promise API stability and may require tolerance of instability.<br>- **Release**: Official release open to all developers. This release promises that all APIs are stable.| Canary\\Beta\\Release |
| osFullName | string | Yes| No| System version. The version number is in the format of **OpenHarmony-x.x.x.x**, where **x** is a digit.<br>**Atomic service API**: This API can be used in atomic services since API version 11.| Openharmony-5.0.0.1 |
| majorVersion | number | Yes| No| Major version number, which increments with the main version. The value is the first digit in **osFullName**. You are advised to use **deviceInfo.majorVersion** instead of parsing **osFullName** to obtain the value.| 5 |
| seniorVersion | number | Yes| No| Senior version number, which increments with architecture and feature updates. The value is the second digit in **osFullName**. You are advised to use **deviceInfo.seniorVersion** instead of parsing **osFullName** to obtain the value.| 0 |
| featureVersion | number | Yes| No| Feature version number. The value is the third digit in **osFullName**. You are advised to use **deviceInfo.featureVersion** instead of parsing **osFullName** to obtain the value.| 0 |
| buildVersion | number | Yes| No| Build version number. The value is the fourth digit in **osFullName**. You are advised to use **deviceInfo.buildVersion** instead of parsing **osFullName** to obtain the value.| 1 |
| sdkApiVersion | number | Yes| No| SDK API version.| 12 |
| firstApiVersion | number | Yes| No| First API version.| 3 |
| versionId | string | Yes| No| Version ID. It consists of the following fields: **deviceType**, **manufacture**, **brand**, **productSeries**, **osFullName**, **productModel**, **softwareModel**, **sdkApiVersion**, **incrementalVersion**, and **buildType**.| NA |
| buildType | string | Yes| No| Build type.| release:nolog |
| buildUser | string | Yes| No| Build user.| NA |
| buildHost | string | Yes| No| Build host.| NA |
| buildTime | string | Yes| No| Build time.| NA |
| buildRootHash | string | Yes| No| Build root hash.| NA |
| udid<sup>7+</sup> | string | Yes| No| Device UDID, available only for system applications.<br>**NOTE**<br>The data length is 65 bytes. The UDID can be used as the unique identifier of a device.<br>**Required permissions**: ohos.permission.sec.ACCESS_UDID| NA |
| distributionOSName<sup>10+</sup> | String | Yes| No| Distribution OS name.<!--Del--> It is defined by the issuer.<!--DelEnd-->.| NA |
| distributionOSVersion<sup>10+</sup> | String | Yes| No| Distribution OS version.<!--Del--> It is defined by the issuer.<!--DelEnd-->.<!--RP1--><!--RP1End--> | NA |
| distributionOSApiVersion<sup>10+</sup> | number| Yes| No| Distribution OS API version.<!--Del--> It is defined by the issuer.<!--DelEnd-->.| NA |
| distributionOSReleaseType<sup>10+</sup> | String | Yes| No| Distribution OS release type.<!--Del--> It is defined by the issuer.<!--DelEnd-->.| NA |
| ODID<sup>12+</sup> | String | Yes| No|Vendor Anonymous Identifier<br>An ODID will be regenerated in the following scenarios:<br>Restore a phone to its factory settings.<br>Uninstall and reinstall all apps of one developer on one device.<br>An ODID is generated based on the following rules:<br>For apps from the same developer, which are running on the same device, they have the same ODID.<br>For apps from different developers, which are running on the same device, each of them has its own ODID.<br>For apps from the same developer, which are running on different devices, each of them has its own ODID.<br>For apps from different developers, which are running on different devices, each of them has its own ODID.<br>**NOTE**<br>The data length is 37 bytes.| NA |

**Example**

```ts
    import { deviceInfo } from '@kit.BasicServicesKit';

    let deviceTypeInfo: string = deviceInfo.deviceType;
    console.info('the value of the deviceType is :' + deviceTypeInfo);

    let manufactureInfo: string = deviceInfo.manufacture;
    console.info('the value of the manufactureInfo is :' + manufactureInfo);

    let brandInfo: string = deviceInfo.brand;
    console.info('the value of the device brand is :' + brandInfo);

    let marketNameInfo: string = deviceInfo.marketName;
    console.info('the value of the deviceInfo marketName is :' + marketNameInfo);

    let productSeriesInfo: string = deviceInfo.productSeries;
    console.info('the value of the deviceInfo productSeries is :' + productSeriesInfo);

    let productModelInfo: string = deviceInfo.productModel;
    console.info('the value of the deviceInfo productModel is :' + productModelInfo);

    let softwareModelInfo: string = deviceInfo.softwareModel;
    console.info('the value of the deviceInfo softwareModel is :' + softwareModelInfo);

    let hardwareModelInfo: string = deviceInfo.hardwareModel;
    console.info('the value of the deviceInfo hardwareModel is :' + hardwareModelInfo);

    let serialInfo: string = deviceInfo.serial;
    console.info('the value of the deviceInfo serial is :' + serialInfo);

    let bootloaderVersionInfo: string = deviceInfo.bootloaderVersion;
    console.info('the value of the deviceInfo bootloaderVersion is :' + bootloaderVersionInfo);

    let abiListInfo: string = deviceInfo.abiList;
    console.info('the value of the deviceInfo abiList is :' + abiListInfo);

    let securityPatchTagInfo: string = deviceInfo.securityPatchTag;
    console.info('the value of the deviceInfo securityPatchTag is :' + securityPatchTagInfo);

    let displayVersionInfo: string = deviceInfo.displayVersion;
    console.info('the value of the deviceInfo displayVersion is :' + displayVersionInfo);

    let incrementalVersionInfo: string = deviceInfo.incrementalVersion;
    console.info('the value of the deviceInfo incrementalVersion is :' + incrementalVersionInfo);

    let osReleaseTypeInfo: string = deviceInfo.osReleaseType;
    console.info('the value of the deviceInfo osReleaseType is :' + osReleaseTypeInfo);

    let osFullNameInfo: string = deviceInfo.osFullName;
    console.info('the value of the deviceInfo osFullName is :' + osFullNameInfo);

    let majorVersionInfo: number = deviceInfo.majorVersion;
    console.info('the value of the deviceInfo majorVersion is :' + majorVersionInfo);

    let seniorVersionInfo: number = deviceInfo.seniorVersion;
    console.info('the value of the deviceInfo seniorVersion is :' + seniorVersionInfo);

    let featureVersionInfo: number = deviceInfo.featureVersion;
    console.info('the value of the deviceInfo featureVersion is :' + featureVersionInfo);

    let buildVersionInfo: number = deviceInfo.buildVersion;
    console.info('the value of the deviceInfo buildVersion is :' + buildVersionInfo);

    let sdkApiVersionInfo: number = deviceInfo.sdkApiVersion;
    console.info('the value of the deviceInfo sdkApiVersion is :' + sdkApiVersionInfo);

    let firstApiVersionInfo: number = deviceInfo.firstApiVersion;
    console.info('the value of the deviceInfo firstApiVersion is :' + firstApiVersionInfo);

    let versionIdInfo: string = deviceInfo.versionId;
    console.info('the value of the deviceInfo versionId is :' + versionIdInfo);

    let buildTypeInfo: string = deviceInfo.buildType;
    console.info('the value of the deviceInfo buildType is :' + buildTypeInfo);

    let buildUserInfo: string = deviceInfo.buildUser;
    console.info('the value of the deviceInfo buildUser is :' + buildUserInfo);

    let buildHostInfo: string = deviceInfo.buildHost;
    console.info('the value of the deviceInfo buildHost is :' + buildHostInfo);

    let buildTimeInfo: string = deviceInfo.buildTime;
    console.info('the value of the deviceInfo buildTime is :' + buildTimeInfo);

    let buildRootHashInfo: string = deviceInfo.buildRootHash;
    console.info('the value of the deviceInfo buildRootHash is :' + buildRootHashInfo);

    let udid: string = deviceInfo.udid;
    console.info('the value of the deviceInfo udid is :' + udid);

    let distributionOSName: string = deviceInfo.distributionOSName
    console.info('the value of the deviceInfo distributionOSName is :' + distributionOSName);

    let distributionOSVersion: string = deviceInfo.distributionOSVersion
    console.info('the value of the deviceInfo distributionOSVersion is :' + distributionOSVersion);

    let distributionOSApiVersion: number = deviceInfo.distributionOSApiVersion
    console.info('the value of the deviceInfo distributionOSApiVersion is :' + distributionOSApiVersion);

    let distributionOSReleaseType: string = deviceInfo.distributionOSReleaseType
    console.info('the value of the deviceInfo distributionOSReleaseType is :' + distributionOSReleaseType);

    let odid: string = deviceInfo.ODID;
    console.info('the value of the deviceInfo odid is :' + odid);

```
