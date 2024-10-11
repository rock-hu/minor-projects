# Codegen

## Codegen Configuration

- Before using Codegen, you need a React Native app for generating code, even if you need to manually call Codegen. To run Codegen of OpenHarmony, you must set the `harmony.codegenConfig` field in `package.json` of the custom TurboModule or Fabric, and set the `src` option in the field to configure the resource path of the file.

   `"version": 1` indicates the `ArkTS` version. The Codegen file of the `ArkTS` version is generated.

   `"version": 2` indicates the `CAPI` version. The Codegen file of the `CAPI` version is generated.

   ```json
    ...
    "harmony": {
      "alias": <Module name>,
      "codegenConfig": [
        {
          "version": 1,
          "specPaths": [
            "./src/specs/v1"
          ]
        },
        {
          "version": 2,
          "specPaths": [
            "./src/specs/v2"
          ]
        }
      ]
    }
    ...
   ```

- Add the configuration option of the Codegen file path to `CMakeLists.txt` of the `entry` module.

    The path of the generated Codegen file can be configured as a command parameter in the `scripts` field in the `{RN project}\package.json` file.

    ```json
    "scripts": {
      ...
      "codegen": "react-native codegen-harmony --cpp-output-path ./entry/src/main/cpp/generated --rnoh-module-path ./entry/oh_modules/@rnoh/react-native-openharmony"
    }
    ```

    Description of the **codegen-harmony** parameters:
    - --**rnoh-module-path**: path of the generated ArkTS code
    - --**cpp-output-path**: relative path of the output path for storing the generated C++ file. The default value is **./harmony/entry/src/main/cpp/generated**.
    - --**project-root-path**: relative path of the module root directory

- Set `CMakeLists.txt` of the `entry` module to the path of the Codegen file.

    ```diff
    + set(RNOH_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated")
    ...

    + file(GLOB GENERATED_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/generated/*.cpp") # this line is needed by codegen v1
    ...
    ```

- Configure the path of the generated Codegen file in `CMakeLists.txt` of the native code module.

   ```diff
  ...
  + set(rnoh_xxx_generated_dir "${RNOH_GENERATED_DIR}/xxx")

  + file(GLOB_RECURSE rnoh_xxx_generated_SRC "${rnoh_xxx_generated_dir}/**/*.cpp")
  ...
  + add_library(rnoh_xxx SHARED ${rnoh_xxx_SRC} ${rnoh_xxx_generated_SRC})

  + target_include_directories(rnoh_xxx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${rnoh_xxx_generated_dir})
  ...
   ```

## Codegen File Generation and Structure

For an RN project, you can run the `npm run codegen` command to run Codegen. By default, the generated Codegen file is stored in the `{RN project}/entry` directory. The ArkTS code is stored in the `entry\oh_modules\@rnoh\react-native-harmony\generated` directory, and the CPP code is stored in the `entry\src\main\cpp\generated` directory. The structure of the generated Codegen file is as follows.

> The configuration may vary according to the actual situation. The following describes how to configure both v1 and v2 for reference.

```
  ├─entry
  |   └─src
  |     └─main
  |       └─cpp
  |         └─generated
  |             ├─GeneratedSampleViewArkTSComponentDescriptor.h
  |             ├─GeneratedSampleViewArkTSJSIBinder.h
  |             ├─RNOHGeneratedPackage.h
  |             └─react_native_harmony_sample
  |                 ├─RNOH
  |                 |   └─generated
  |                 |       ├─BaseReactNativeHarmonySamplePackage.h
  |                 |       ├─components
  |                 |       |   ├─BaseGeneratedSampleViewComponentInstance.h
  |                 |       |   └─GeneratedSampleViewJSIBinder.h
  |                 |       └─turbo_modules
  |                 |           ├─GeneratedSampleTurboModule.cpp
  |                 |           └─GeneratedSampleTurboModule.h
  |                 └─react
  |                     └─renderer
  |                         └─components
  |                             └─react_native_harmony_sample
  │                                 ├─ComponentDescriptors.h
  │                                 ├─EventEmitters.cpp
  │                                 ├─EventEmitters.h
  │                                 ├─Props.cpp
  │                                 ├─Props.h
  │                                 ├─ShadowNodes.cpp
  │                                 ├─ShadowNodes.h
  │                                 ├─States.cpp
  │                                 └─States.h
  └─oh_modules
      └─@rnoh
          └─react-native-harmony
              └─generated
                  ├─index.ets
                  ├─ts.ts
                  ├─components
                  │  ├─GeneratedSampleViewArkTS.ts
                  │  └─ts.ts
                  └─turboModules
                      ├─GeneratedSampleTurboModule.ts
                      └─ts.ts
```

After the execution is complete, copy the Codegen code to the OpenHarmony native project, `{RN project}\entry\oh_modules\@rnoh\react-native-harmony\generated` to the `{native project}\entry\oh_modules\@rnoh\react-native-harmony` directory, and `{RN project}\entry\src\main\cpp\generated` to the `{native project}\entry\src\main\cpp` directory.

## Integration Modes

The glue code of the `TurboModule` and `Fabric` components generated by running the Codegen command contains the `C++` code and `ts` code, which need to be integrated into the framework in different integration modes.

- `C++` integration

  Import the `Package` class in the manually implemented `cpp` file to `PackageProvider.cpp` and write the package to the returned object.

     ```cpp
     // {Native project}\entry\src\main\cpp\PackageProvider.cpp
     #include "RNOH/PackageProvider.h"
     #include "generated/xxx_module/RNOH/generated/xxxPackage.h"
     
     using namespace rnoh;
     
     std::vector<std::shared_ptr<Package>> PackageProvider::getPackages(Package::Context ctx) {
       return {
           std::make_shared<xxxPackage>(ctx)
       };
     }
     ```
  > Note that **xxx** in the implementation code indicates the module name and package class name.

- `ts` integration

  Import the `Package` class in the manually implemented `ts` file to `RNPackagesFactory.ts`, create a package, and add it to the array returned by `createRNPackages`.

  ```typescript
  import type {RNPackageContext, RNPackage} from '@rnoh/react-native-openharmony/ts';
  import { xxxPackage } from 'xxxPackage';
  
  export function createRNPackages(ctx: RNPackageContext): RNPackage[] {
    return [new xxxPackage(ctx)];
  }
  ```
  > Note that **xxx** in the implementation code indicates the package class name.
