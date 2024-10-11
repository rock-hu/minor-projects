# Codegen

## 配置Codegen

- 使用Codegen之前，您需要一个用于生成代码的React Native App，即便您要自己手动调用 Codegen。为了运行harmony平台的Codegen，需要在自定义TurboModule或Fabric的`package.json`中设置`harmony.codegenConfig`字段，并在字段中设置`src`选项，用于配置文件的资源路径。

   `"version": 1`为`ArkTS`版本，生成`ArkTS`版本的Codegen文件；

   `"version": 2`为`CAPI`版本，生成`CAPI`版本的Codegen文件。

   ```json
    ...
    "harmony": {
      "alias": <模块名称>,
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

- 在`entry`入口模块的`CMakeLists.txt`中添加Codegen文件路径配置项。

    Codegen生成的路径可以在`{RN工程}\package.json`的`scripts`字段以命令参数形式配置：

    ```json
    "scripts": {
      ...
      "codegen": "react-native codegen-harmony --cpp-output-path ./entry/src/main/cpp/generated --rnoh-module-path ./entry/oh_modules/@rnoh/react-native-openharmony"
    }
    ```

    codegen-harmony参数介绍：
    - --rnoh-module-path：生成的ArkTS代码的路径
    - --cpp-output-path：指定⽤于存储⽣成的C++文件的输出⽬录的相对路径，默认./harmony/entry/src/main/cpp/generated
    - --project-root-path：模块根⽬录的相对路径

- 根据配置的路径将codegen文件路径配置到`entry`入口模块的`CMakeLists.txt`中。

    ```diff
    + set(RNOH_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated")
    ...

    + file(GLOB GENERATED_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/generated/*.cpp") # this line is needed by codegen v1
    ...
    ```

- 在原生代码模块的`CMakeLists.txt`中配置Codegen生成文件的路径。

   ```diff
  ...
  + set(rnoh_xxx_generated_dir "${RNOH_GENERATED_DIR}/xxx")

  + file(GLOB_RECURSE rnoh_xxx_generated_SRC "${rnoh_xxx_generated_dir}/**/*.cpp")
  ...
  + add_library(rnoh_xxx SHARED ${rnoh_xxx_SRC} ${rnoh_xxx_generated_SRC})

  + target_include_directories(rnoh_xxx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${rnoh_xxx_generated_dir})
  ...
   ```

## codegen生成及结构

RN工程可以执行`npm run codegen`命令执行Codegen，Codegen生成的文件默认放在`{RN工程}/entry`目录下，其中ArkTS的代码存放在了`entry\oh_modules\@rnoh\react-native-harmony\generated`目录中，cpp的代码存放在`entry\src\main\cpp\generated`目录中。Codegen生成的文件结构如下：

> 根据您具体的实现情况会存在出入，下面展示的是同时配置v1和v2的情况，仅供参考。

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

执行完成后，需要把Codegen代码复制到鸿蒙原生工程中，`{RN工程}\entry\oh_modules\@rnoh\react-native-harmony\generated`复制到`{原生工程}\entry\oh_modules\@rnoh\react-native-harmony`目录中，还需要把`{RN工程}\entry\src\main\cpp\generated`复制到`{原生工程}\entry\src\main\cpp`目录中。

## 集成方式

通过Codegen命令生成的`TurboModule`和`Fabric`组件胶水代码会包含`C++`代码和`ts`代码，需要分别通过不同的集成方式集成到框架中。

- `C++`集成方式

  将手动实现的`cpp`文件中的`Package`类在`PackageProvider.cpp`中引入，写在返回的对象中。

     ```cpp
     //{原生工程}\entry\src\main\cpp\PackageProvider.cpp
     #include "RNOH/PackageProvider.h"
     #include "generated/xxx_module/RNOH/generated/xxxPackage.h"
     
     using namespace rnoh;
     
     std::vector<std::shared_ptr<Package>> PackageProvider::getPackages(Package::Context ctx) {
       return {
           std::make_shared<xxxPackage>(ctx)
       };
     }
     ```
  > 注意实现代码中的xxx，分别代表模块名称与Package的类名。

- `ts`集成方式

  将手动实现的`ts`文件中的`Package`类在`RNPackagesFactory.ets`中引入，创建并添加在`createRNPackages`接口返回的数组中。

  ```typescript
  import type {RNPackageContext, RNPackage} from '@rnoh/react-native-openharmony/ts';
  import { xxxPackage } from 'xxxPackage';
  
  export function createRNPackages(ctx: RNPackageContext): RNPackage[] {
    return [new xxxPackage(ctx)];
  }
  ```
  > 注意实现代码中的xxx，代表Package的类名。