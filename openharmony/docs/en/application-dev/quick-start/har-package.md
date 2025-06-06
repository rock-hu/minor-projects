# HAR
A Harmony Archive (HAR) is a static shared package that can contain code, C++ libraries, resource files, and configuration files (also called profiles). It enables modules and projects to share code of ArkUI components, resources, and more.

## When to Use
- Supports intra-application sharing or, after being released, intra-application sharing.
- As a second-party library for internal applications, by being released to the [OHPM private repository](https://developer.huawei.com/consumer/en/doc/harmonyos-guides-V13/ide-ohpm-repo-V13).
- As a third-party library for external applications, by being released to the [OHPM central repository](https://ohpm.openharmony.cn/#/en/home).
- When multiple HAPs or HSPs reference the same HAR, the application package may contain multiple copies of code and resource files for the HAPs or HSPs, resulting in an unwelcome large package size.

## Constraints

- An HAR can only be referenced as a dependency of an application module. It cannot be installed or run independently on a device.
- An HAR does not support the declaration of the [ExtensionAbility](../application-models/extensionability-overview.md) component in the configuration file, but supports the [UIAbility](../application-models/uiability-overview.md) component.
> **NOTE**
>
> If the [startAbility](../reference/apis-ability-kit/js-apis-inner-application-uiAbilityContext.md#uiabilitycontextstartability) API is used to start the UIAbility in the HAR, the value of **moduleName** in the API parameter must be the module name of the [HAP](hap-package.md) or [HSP](in-app-hsp.md) that depends on the HAR.
- An HAR does not support the declaration of the [pages](./module-configuration-file.md#pages) tag in the configuration file. Still, it can include pages, which can be redirected through a [named route](../ui/arkts-routing.md#named-route).
- An HAR does not support referencing resources in the **AppScope** folder. This is because the content in the **AppScope** folder is not packaged into the HAR during building.
- An HAR can depend on other HARs, but does not support cyclic dependency or dependency transfer.

## Creating an HAR
Create an HAR module in DevEco Studio. For details, see [Creating a HAR Module](https://developer.huawei.com/consumer/en/doc/harmonyos-guides-V13/ide-har-V13#section643521083015).


## Developing an HAR

You can export the ArkUI components, APIs, and other resources of an HAR for other applications or intra-application modules to reference.

The **Index.ets** file acts as the entry of the HAR export declaration file and is where the HAR exports APIs. This file is automatically generated by DevEco Studio by default. You can specify another file as the entry declaration file in the **main** field in the **oh-package.json5** file of the module. The code snippet is as follows:
```json
{
  "main": "Index.ets"
}
```
### Exporting ArkUI Components
Use **export** to export the ArkUI components. The code snippet is as follows:
```ts
// library/src/main/ets/components/mainpage/MainPage.ets
@Component
export struct MainPage {
  @State message: string = 'HAR MainPage';

  build() {
    Column() {
      Row() {
        Text(this.message)
          .fontSize(32)
          .fontWeight(FontWeight.Bold)
      }
      .margin({ top: '32px' })
      .height(56)
      .width('624px')

      Flex({ justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center, alignContent: FlexAlign.Center }) {
        Column() {
          Image($r('app.media.pic_empty')).width('33%')
          Text($r('app.string.empty'))
            .fontSize(14)
            .fontColor($r('app.color.text_color'))
        }
      }.width('100%')
      .height('90%')
    }
    .width('100%')
    .height('100%')
    .backgroundColor($r('app.color.page_background'))
  }
}
```
In the **Index.ets** file, declare the APIs that the HAR exposes to external systems. The code snippet is as follows:
```ts
// library/Index.ets
export { MainPage } from './src/main/ets/components/mainpage/MainPage';
```
### Exporting TS Classes and Methods
Use **export** to export TS classes and methods. Multiple TS classes and methods can be exported at the same time. The code snippet is as follows:
```ts
// library/src/main/ts/test.ets
export class Log {
    static info(msg: string) {
        console.info(msg);
    }
}

export function func() {
  return 'har func';
}

export function func2() {
  return 'har func2';
}
```
In the **Index.ets** file, declare the APIs that the HAR exposes to external systems. The code snippet is as follows:
```ts
// library/Index.ets
export { Log } from './src/main/ts/test';
export { func } from './src/main/ts/test';
export { func2 } from './src/main/ts/test';
```

### Exporting Native Methods
The HAR can contain .so files written in C++. Native methods in the .so file can be exported from the HAR in the following way. In the example, the **add** API in the **liblibrary.so** file is exported.
```ts
// library/src/main/ets/utils/nativeTest.ets
import native from 'liblibrary.so';

export function nativeAdd(a: number, b: number): number {
  let result: number = native.add(a, b);
  return result;
}
```
In the **Index.ets** file, declare the APIs that the HAR exposes to external systems. The code snippet is as follows:
```ts
// library/Index.ets
export { nativeAdd } from './src/main/ets/utils/nativeTest';
```

### Exporting Resources
Specifically, DevEco Studio collects resource files from the HAP module and its dependent modules, and overwrites the resource files with the same name (if any) based on the following priorities (in descending order):
- AppScope (supported only by the stage model)
- Modules in the HAP
- Dependent HAR modules<br>If resource conflicts occur between dependent HAR modules, they are overwritten based on the dependency sequence indicated under **dependencies** in the **oh-package.json5** file. The module that is higher in the dependency sequence list has a higher priority. For example, in the following example, if **dayjs** and **lottie** folders contain files with the same name, resources in **dayjs** are used preferentially.
> **NOTE**
> 
> With regard to resources in the internationalization folder of **AppScope**, HAP, and HAR directories, the preceding priority rules also apply to resources with the same internationalization qualifier. In addition, resources with internationalization qualifiers are prioritized over those in the **base** folder. For example, if resources with the same name are configured in both the **base** folder under **AppScope** and the **en_US** folder of an HAR, the one in the **en_US** folder is prioritized for internationalization purposes.
```
// oh-package.json5
{
  "dependencies": {
    "dayjs": "^1.10.4",
    "lottie": "^2.0.0"
  }
}
```

## Using an HAR

You can reference the ArkUI components, APIs, and resources in an HAR.

Before referencing the HAR, you need to configure the dependency on it. For details, see [Referencing a Shared Package](https://developer.huawei.com/consumer/en/doc/harmonyos-guides-V13/ide-har-import-V13).

### Referencing ArkUI Components

After configuring the dependency on the HAR, you can reference ArkUI components exported from the HAR by using **import**. The code snippet is as follows:
```ts
// entry/src/main/ets/pages/IndexSec.ets
import { MainPage } from 'library';

@Entry
@Component
struct IndexSec {
  build() {
    Row() {
      // Reference the ArkUI component in the HAR.
      MainPage()
    }
    .height('100%')
  }
}
```
### Referencing TS Classes and Methods
To reference the TS classes and methods exported from the HAR, use **import** as follows:
```ts
// entry/src/main/ets/pages/Index.ets
import { Log } from 'library';
import { func } from 'library';

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  build() {
    Column() {
      Text(this.message)
        .fontFamily('HarmonyHeiTi')
        .fontWeight(FontWeight.Bold)
        .fontSize(32)
        .fontWeight(700)
        .fontColor($r('app.color.text_color'))
        .textAlign(TextAlign.Start)
        .margin({ top: '32px' })
        .width('624px')

      // Reference TS classes and methods.
      Button($r('app.string.button'))
        .id('button')
        .height(48)
        .width('624px')
        .margin({ top: '4%' })
        .type(ButtonType.Capsule)
        .fontFamily('HarmonyHeiTi')
        .borderRadius($r('sys.float.ohos_id_corner_radius_button'))
        .backgroundColor($r('app.color.button_background'))
        .fontColor($r('sys.color.ohos_id_color_foreground_contrary'))
        .fontSize($r('sys.float.ohos_id_text_size_button1'))
        .onClick(() => {
          // Reference TS classes and methods in the HAR.
          Log.info('har msg');
          this.message = 'func return: ' + func();
        })
    }
    .width('100%')
    .backgroundColor($r('app.color.page_background'))
    .height('100%')
  }
}
```

### Referencing Native Methods
To reference the native methods exported from the HAR, use **import** as follows:
```ts
// entry/src/main/ets/pages/Index.ets
import { nativeAdd } from 'library';

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  build() {
    Column() {
      Text(this.message)
        .fontFamily('HarmonyHeiTi')
        .fontWeight(FontWeight.Bold)
        .fontSize(32)
        .fontWeight(700)
        .fontColor($r('app.color.text_color'))
        .textAlign(TextAlign.Start)
        .margin({ top: '32px' })
        .width('624px')

      // Reference the native method in the HAR.
      Button($r('app.string.native_add'))
        .id('nativeAdd')
        .height(48)
        .width('624px')
        .margin({ top: '4%', bottom: '6%' })
        .type(ButtonType.Capsule)
        .fontFamily('HarmonyHeiTi')
        .borderRadius($r('sys.float.ohos_id_corner_radius_button'))
        .backgroundColor($r('app.color.button_background'))
        .fontColor($r('sys.color.ohos_id_color_foreground_contrary'))
        .fontSize($r('sys.float.ohos_id_text_size_button1'))
        .onClick(() => {
          this.message = 'result: ' + nativeAdd(1, 2);
        })
    }
    .width('100%')
    .backgroundColor($r('app.color.page_background'))
    .height('100%')
  }
}
```

### Referencing Resources
Use **$r** to reference resources in the HAR. For example, add the **name: hello_har** string (defined in the **string.json** file) and **icon_har.png** image to the **src/main/resources** directory of the HAR module, and then reference the string and image in the entry module. The code snippet is as follows:
```ts
// entry/src/main/ets/pages/Index.ets
@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  build() {
    Column() {
      // Reference the string in the HAR.
      Text($r('app.string.hello_har'))
        .id('stringHar')
        .fontFamily('HarmonyHeiTi')
        .fontColor($r('app.color.text_color'))
        .fontSize(24)
        .fontWeight(500)
        .margin({ top: '40%' })

      List() {
        ListItem() {
          // Reference the image in the HAR.
          Image($r('app.media.icon_har'))
            .id('iconHar')
            .borderRadius('48px')
        }
        .margin({ top: '5%' })
        .width('312px')
      }
      .alignListItem(ListItemAlign.Center)
    }
    .width('100%')
    .backgroundColor($r('app.color.page_background'))
    .height('100%')
  }
}
```
## Building an HAR

HAR can be used as a second-party or third-party library for other applications. To protect code assets, you are advised to [enable code obfuscation](../arkts-utils/source-obfuscation-guide.md).

After [code obfuscation](../arkts-utils/source-obfuscation.md) is enabled, DevEco Studio compiles, obfuscates, and compresses code when building HARs to protect code assets.

The obfuscation capability is enabled by default for the HAR module. When the compilation module is release, simple code obfuscation is automatically performed for the HAR module of API version 10 or later. **Since DevEco Studio 5.0.3.600, the code obfuscation is disabled by default when a project is created.** You can enable this feature by setting **enable** in the **ruleOptions** field in the **build-profile.json5** file of the HAR module. For details, see [Code Obfuscation](https://developer.huawei.com/consumer/en/doc/harmonyos-guides-V13/ide-build-obfuscation-V13). The configuration is as follows:

  ```json
  {
    "apiType": "stageMode",
    "buildOption": {
    },
    "buildOptionSet": [
      {
        "name": "release",
        "arkOptions": {
          "obfuscation": {
            "ruleOptions": {
              "enable": true,
              "files": [
                "./obfuscation-rules.txt"
              ]
            },
            "consumerFiles": [
              "./consumer-rules.txt"
            ]
          }
        }
      },
    ],
    "targets": [
      {
        "name": "default"
      }
    ]
  }
  ```

### Building TS Files

> **Scenario Description**
>
>Enable this configuration when using **Sendable** in an HAR.

> **Constraints**
>
>When depend on TS HAR, the ArkUI component in TS HAR cannot be referenced.

After the ArkTS file in the HAR module is built, the product is a JS file by default. To change the product to a TS file, set **name** under the **metadata** field in the **module.json5** file of the HAR module to **UseTsHar**. The configuration is as follows:

  ```json
  {
    "module": {
      "name": "TsClosedHar",
      "type": "har",
      "deviceTypes": [
        "default",
        "tablet",
        "2in1"
      ],
      "metadata": [
        {
          "name": "UseTsHar",
          "value": "true"
        }
      ]
    }
  }
  ```

## Publishing an HAR

For details, see [Publishing a Shared Package](https://developer.huawei.com/consumer/en/doc/harmonyos-guides-V13/ide-har-publish-V13).
