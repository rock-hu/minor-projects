# 不同包类型的字节码混淆建议

由于不同包类型的用途及构建流程的差异，开发者对不同包类型使用混淆有不同的注意事项。本文对[HAP](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/hap-package.md)、[HAR](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/har-package.md)和[HSP](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/in-app-hsp.md)三种包类型分别提供建议，帮助开发者高效使用混淆。
为了对混淆在不同包类型下的行为有更清晰的理解，建议开发者在对不同包类型进行配置前，充分了解混淆原理及混淆开启流程，并优先阅读[Stage模型应用程序包结构](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/application-package-structure-stage.md)（了解不同包类型之间的差异点）。

## 推荐混淆选项

推荐开发者在默认混淆的基础上，在混淆配置文件中开启以下四项基础混淆选项：
    1. 顶层作用域名称混淆（`-enable-toplevel-obfuscation`）。
    2. 属性名称混淆（`-enable-property-obfuscation`）。
    3. 导入导出名称混淆（`-enable-export-obfuscation`）。
    4. 文件名混淆（`-enable-filename-obfuscation`）。

开启混淆功能后，需要配置白名单进行适配，来保证应用运行功能正常。

* 对于新开发的应用，建议直接打开以上选项，在开发迭代过程中增加白名单配置。
* 对于已开发一定功能的应用，建议按照以上顺序逐步打开各个选项，对比不同选项的混淆产物，熟悉新增选项的具体效果，参考[混淆选项配置指导](bytecode-obfuscation-guide.md#混淆选项配置指导)排查适配。

当应用功能调试正常后，还可继续开启日志删除（`-remove-log`）等功能以发布release应用包。

## HAP包混淆建议

1. 开启混淆规则。建议开启[推荐的混淆选项](#推荐混淆选项)，其它选项按需添加。
2. HAP包的开发者需要重点知悉[混淆规则的合并策略](bytecode-obfuscation.md#混淆规则合并策略)，确认清楚了解所有潜在影响本模块编译时混淆规则的情况。
3. 在此基础上，还需了解哪种依赖包内的代码会跟随本模块一起参与编译混淆，因为这会影响混淆新生成的名称，以及部分全局生效（如属性名称）的白名单，进而影响到本模块最终的混淆效果。具体而言：
    1. 当依赖本地HAR包时，它的代码会跟随本模块一起混淆，因此在这部分代码中混淆前后的名称会一起体现在编译中间产物的`obfuscation/nameCache.json`文件中。
    2. 当依赖发布态源码HAR包时，它被安装在工程级的oh_modules目录下的代码会跟随本模块一起混淆，因此在这部分代码中混淆前后的名称也会一起体现在编译中间产物的`obfuscation/nameCache.json`文件中。
    3. 当依赖发布态字节码HAR包或HSP包时，参与编译的是其中的二进制字节码和声明文件，由于ArkGuard工具是字节码混淆，且为了保证声明文件中的接口与二进制中的实现的一致性，不会对这部分代码进行混淆。但是，若三方库未正确提供接口的声明或者未在`consumer-rules`中配置白名单，在HAP中使用这些三方库接口的地方可能会被混淆，此时HAP包开发者可以自行配置，来保证运行时的正确性。

4. 为保证与发布态三方库交互使用的正确性，ArkGuard会自动收集本模块依赖的安装到oh_modules中的模块的export导出名称及其相关属性等名称到不混淆名单中，由于开发者可以引用依赖模块中的任意路径下的文件，因此导出名称的收集不仅限于入口文件（如Index.ets），而是涵盖了`oh_modules`三方库中所有文件的export导出名称。
5. 对于本地源码HAR包和本地HSP包，ArkGuard不会自动收集所有文件中的export导出名称到不混淆名单中。

    下表总结了常见依赖包的编译混淆情况：

    | 依赖模块 | 实际参与编译的文件 | 是否跟随本模块参与混淆 | 是否会传递保留配置给本模块 | 是否收集导出名称到不混淆名单中 |
    | --- | --- | --- | --- | --- |
    | [本地源码HAR包](#本地源码har包) | 模块在本地的源码目录 | 是 | 是 | 否 |
    | [发布态源码HAR包](#发布态源码har包) | 工程级的`oh_modules`下的模块目录 | 是 | 是 | 是 |
    | [发布态字节码HAR包](#发布态字节码har包) | 二进制字节码和声明文件 | 否 | 是 | 是 |
    | [本地源码HSP包](#本地源码hsp包) | NA | 否 | 是 | 否 |
    | [集成态HSP包](#集成态hsp包) | 二进制字节码和声明文件 | 否 | 是 | 是 |

6. 了解需要[配置白名单的场景](bytecode-obfuscation.md#保留选项)，将白名单配置到obfuscation-rules.txt文件中。
7. 构建应用，验证HAP包功能。若功能异常，则继续排查是否遗漏白名单。
8. 应用功能正常，则可以发布应用包。

## HAR包混淆建议

### HAR包通用建议

1. HAR包的开发者需充分了解[三种混淆配置文件](bytecode-obfuscation-guide.md#三种混淆配置文件)以及[混淆规则的合并策略](bytecode-obfuscation.md#混淆规则合并策略)，以及在被HAP包使用时的[HAP包混淆建议](#hap包混淆建议)中的注意事项，确保被应用依赖时本模块所有功能正常。

2. 由于HAR包会影响使用它的主模块的混淆流程，**无论HAR包本身是否开启混淆，都应该配置consumer-rules.txt，来保证主模块在开启任意混淆的情况下，HAR包内部实现的功能都保持正常**。

3. 由于consumer配置的传递性，**HAR包开发者不应该在其中配置开启某种混淆的能力，只应该配置保留某个白名单的规则，并且为减少对依赖方的影响范围，只建议使用`-keep-global-name`和`-keep-property-name`两种白名单配置能力。**

### 本地源码HAR包

作为一个未发布的静态包，本地源码HAR包不会独立进行编译混淆，而是会跟随依赖它的主模块（如HAP）一同进行编译混淆，开发者需参阅[HAP包混淆建议](#hap包混淆建议)了解相关行为。

### 发布态源码HAR包

1. 开启混淆规则。建议开启[推荐的混淆选项](#推荐混淆选项)，其它选项按需添加。
2. 了解需要[配置白名单的场景](bytecode-obfuscation.md#已有保留选项汇总)，配置HAR中的白名单：

    * obfuscation-rules.txt中配置HAR包对外导出接口及其相关属性名称、此次构建过程不能被混淆的名称等。
    * consumer-rules.txt配置不能被二次混淆的接口、属性等名称。

3. HAR包功能验证。需注意，在构建本模块HAR时会进行一次混淆，当发布后的HAR包被依赖时，如果使用方开启混淆，则本HAR包发布后的代码还会跟随使用方被二次混淆，因为需要充分验证使用方开启混淆时HAR包功能是否正常。

4. 发布HAR包。

> **说明：**
>
> 当被其他模块依赖时，有以下两点需要注意：
>
> * ArkGuard会在使用方（如HAP）混淆时收集本发布态HAR包的export导出名称及其相关属性等名称到不混淆名单中。
> * ArkGuard会在使用方（如HAP）混淆时收集本发布态HAR包的obfuscation.txt文件中的白名单，但不会继续收集本HAR包依赖的HAR包（四方库）中的obfuscation.txt文件的白名单（这是由于在构建三方库时，已收集过四方库的名单）。因此，**若发布态HAR包依赖其他HAR包，需要固定其他HAR包的版本号，不应配置自动匹配最新版本。** 否则当四方库升级后，若白名单发生变更，则不会在HAP中生效，易引起稳定性问题。

### 发布态字节码HAR包

字节码HAR包的混淆适配流程与发布态源码HAR包基本一致。但是由于ArkGuard是字节码混淆工具，因此当release编译构建发布一个字节码HAR包时，在构建本模块HAR时会进行一次混淆，当发布后的HAR包被使用方依赖时，如果使用方开启混淆，ArkGuard不会对字节码HAR包进行二次混淆。

## HSP包混淆建议

### HSP包通用建议

1. HSP包的开发者需充分了解[三种混淆配置文件](bytecode-obfuscation-guide.md#三种混淆配置文件)以及[混淆规则的合并策略](bytecode-obfuscation.md#混淆规则合并策略)，以及在被HAP包使用时的[HAP包混淆建议](#hap包混淆建议)中的注意事项，确保被应用依赖时本模块所有功能正常。
2. 由于HSP包是独立构建，并且只会构建一次，因此要重点关注模块内部的混淆效果，同时让其他模块正常调用接口即可。
3. 由于consumer配置的传递性，**HSP包开发者不应该在其中配置开启某种混淆的能力，只应该配置保留某个白名单的规则，并且为减少对依赖方的影响范围，只建议使用`-keep-global-name`和`-keep-property-name`两种白名单配置能力。**

### 本地源码HSP包

1. 本地源码HSP包要明确对外提供哪些接口及其相关属性，并将这些名称配置到obfuscation-rules.txt及consumer-rules.txt中。
2. 对于模块内部混淆效果，开发者可以参阅[HAP包混淆建议](#hap包混淆建议)中的所有建议。
3. 功能验证时，应同时构建主模块和HSP包，然后验证HSP包提供的全部接口功能。

### 集成态HSP包

1. 集成态HSP包要明确对外提供哪些接口及其相关属性，并将这些名称配置到obfuscation-rules.txt中。consumer-rules.txt文件中无需配置，因为打包后的tgz压缩包中包含HAR与HSP两个子压缩包，HAR包中的声明文件内定义的对外导出接口及其属性会被自动收集到白名单中。
2. 对于集成态HSP包的内部混淆效果，开发者可以参阅[HAP包混淆建议](#hap包混淆建议)中的所有建议。
3. 集成态HSP包在发布后会被各方依赖，需要充分验证使用方开启混淆时HSP包接口可以被正常调用。

> **说明**
>
> HSP生成obfuscation.txt的规则来源，仅来自于当前模块的consumer-rules.txt文件，不包含依赖模块的consumer-rules.txt文件或obfuscation.txt文件。
