# restool


## Overview

restool is a resource compilation tool that creates resource indexes and parses resources by compiling resource files. You can call the [resource management APIs](../reference/apis-localization-kit/js-apis-resource-manager.md) to obtain resources. The tool is stored in the **toolchains** subdirectory of the SDK installation directory.

## Description

### Command Options

| Option| Default Value Allowed| Argument Carried| Description|
| -------- | -------- | -------- | -------- |
| -i | No| Yes| Resource directory or resource middleware to create.<br>You can specify a compiled Hap resource directory (in the decompressed state) in the resource directory and implement overlay compilation based on the Hap resource directory.<br>For details, see [Compiling Resources](#compiling-resources).|
| -j | No| Yes| Path of the **config.json** or **module.json** file.|
| -o | No| Yes| Output path of the compiled resource.|
| -p | No| Yes| Bundle name of the compiled resource.|
| -r | No| Yes| Header file path of the resource. The header file can be in .txt, .js, or .h format.|
| -e | Yes| Yes| Start ID of the generated resource, for example, **0x01000000**. The value range is [0x01000000, 0x06FFFFFF) and [0x08000000, 0x41FFFFFF).|
| -f | Yes| No| An existing output path will be forcibly deleted and a new one will be generated.|
| -h | Yes| No| Help information.|
| -m | Yes| Yes| Module name. During joint module compilation, multiple module names can be specified, separated by commas (,).|
| -x | Yes| Yes| Resource directory for generating intermediate files or a single resource path. The same command can run multiple times.|
| -z | Yes| No| Compilation result generated based on the resource intermediate file directory.|
| -v | Yes| No| Tool version.|
| --ids | Yes| Yes| Output directory of the generated **id_defined.json** file.|
| --defined-ids | Yes| Yes| Path of the **id_defined.json** file. Generally, the file is generated by using **--ids**.<br>**id_defined.json** contains a list of resource types, names, and IDs.<br>You can customize resource IDs in **id_defined.json**.|
| --icon-check | Yes| No| Whether to enable PNG image verification for icons and startWindowIcons.|
| --thread | Yes| Yes| Number of child threads enabled during resource compilation.|
| --target-config | Yes| Yes| Used together with the **-i** command to support compilation selection.<br>For details, see **target-config Parameters**.|
| --ignored-file | Yes| Yes| Ignoring rule for files and folders during resource compilation. The value is a regular expression. Multiple rules are separated by colons (:\). For example, **\\.git:\\.svn** indicates that the files and folders whose names are **.git** and **.svn** are ignored. |

**target-config** Parameters

Supported parameters: **MccMnc**, **Locale**, **Orientation**, **Device**, **ColorMode**, and **Density**.

Format: Use semicolons (;\) to separate different parameter configurations. Use square brackets ([]) to encapsulate values for a parameter and use commas (,) to separate values.

**MccMnc** matching rule: The MCC must always be used for matching. If an MNC is not included, a match is found as long as the MCC is the same. If an MNC is included, a match is found when both the MCC and MNC are the same.

**Locale** matching rules:

1. The language must always be used for matching.

2. If a script (text) is not included, a match is found as long as the language is the same. If a script is included, a match is found when both the language and script are the same.

3. If a country/region is not included, a match is found as long as the language is the same. If a country/region is included, a match is found when both the language and country/region are the same.

Example: Locale[zh_CN,en_US];Device[phone]. This configuration uses the languages zh_CN and en_US and the device phone as filter criteria. A match is found only when the resources use zh_CN and en_US as the languages and apply to phones. Other parameters (such as **MccMnc** and **Orientation**) are not configured and any values of them can match.

### Subcommands

| Command | Description|
| ---  | ---- |
| dump | Dumps the resource content in the HAP file in JSON format.|

**dump**

```
restool dump [-h] [config] filePath
```

Parameters

| Parameter| Default Value Allowed| Argument Carried| Description|
| --- | --------- | ---------- |------- |
| -h  | Yes| No| Help information.|
| config | Yes| No| Qualifiers of resources in the HAP file.|

Example:

```sh
# Dump all resource information in the HAP file.
restool dump entry.hap
# Dump qualifiers of resources in the HAP file.
restool dump config entry.hap
```

## Example

An example **entry** directory structure is as follows:
```
entry/src/main
|    |----resource
|    |    |----base
|    |    |    |----element
|    |    |    |----media
|    |    |    |----profile
|    |    |----rawfile
|    |    |----resfile
|    |----config.json/module.json
```

### Compiling Resources

There are three resource compilation modes: full resource compilation, incremental resource compilation, and overlay resource compilation. Incremental compilation is available only in preview mode and is used to preview the component effect in the development phase. Full compilation is used to build resource files of a project. Overlay compilation allows resources in a project to be added to existing Hap template resources and compiled resource items to be reused.

1. To compile all resources, run the following command:

```
restool -i entry/src/main -j entry/src/main/module.json -p com.ohos.demo -o out -r out/ResourceTable.txt -f
```

2. To compile incremental resources, perform the following steps:

Step 1: Generate the resource middleware.

```
restool -x entry/src/main/resource -o out
```
Step 2: Compile the resource middleware.

```
restool -i out1 -i out2 -o out -p com.ohos.demo -r out/ResourceTable.txt -j entry/src/main/module.json -f -z
```

3. To compile overlay resources, run the following command:

```
# hapResource is the path of the decompressed HAP file.
restool -i entry/src/main -i hapResource -j entry/src/main/module.json -p com.ohos.demo -o out -r out/ResourceTable.txt -f
```

### Fixing the Resource ID

To fix the resource ID, perform the following steps:

Step 1: Create the **id_defined.json** file. There are two ways to create the file.

+ Run the following command to generate the file:

```
restool -i entry/src/main -j entry/src/main/module.json -p com.ohos.demo -o out -r out/ResourceTable.txt --ids out -f
```

+ Customize the **id_defined.json** file with the following content:

```
{
    "record" :
    [
        {
            "id": "0x01000000", // A fixed ID for the resource.
            "name": "app_name", // Resource name.
            "type": "string" // Resource type.
        }
    ]
}
```

Step 2: Fix the resource ID. There are two ways to fix the resource ID.

+ Run the following command to fix the resource ID:
```
restool -i entry/src/main -j entry/src/main/module.json -p com.ohos.demo -o out1 -r out1/ResourceTable.txt --defined-ids out/id_defined.json -f
```

+ Place the customized **id_defined.json** file in the **resource/base/element/** directory and then run the following command to fix the resource ID:
```
restool -i entry/src/main -j entry/src/main/module.json -p com.ohos.demo -o out1 -r out1/ResourceTable.txt  -f
```
