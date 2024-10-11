# Document update instructions

## This document is no longer maintained. Obfuscation documents will be moved to [official guide](https://developer.huawei.com/consumer/cn/doc/harmonyos-guides-V5/source-obfuscation-V5) and [doc repository](https://gitee.com/openharmony/docs/blob/master/en/application-dev/arkts-utils/source-obfuscation.md)

Note: The source obfuscation is updated synchronously with the DevEco Studio version. It is recommended to read the official guide that comes with the DevEco Studio version first. The community doc repository correspond to the latest open source code, ahead of the source obfuscation function in the released DevEco Studio.

# Arkguard

Arkguard is a javascript and typescript obfuscation tool.
For Chinese version please read [README-cn.md](README-cn.md)
(中文版说明请查看[README-cn.md](README-cn.md)).

# Usage in DevEco Studio

Arkguard has been integrated into SDK. It is convenient to use Arkguard in DevEco Studio.
In DevEco Studio, Arkguard can be enabled only in Stage Model (FA Model is not supported).
For now only name obfuscations can be used in DevEco Studio (because other obfuscation
abilities of Arkguard may hurt execution performance).
You can obfuscate the following names:

* parameter names and local variable names
* names in global scope
* property names

We enable the obfuscation of parameter names and local variable names by default. However,
global names obfuscation and property names obfuscation are disabled by default, as they may
cause runtime error if they are enabled by default.
You can enable them by [obfuscation options](#obfuscation-options).

When you create a new project, the following config will be generated in `build-profile.json5`.

```
"arkOptions": {
  "obfuscation": {
    "ruleOptions": {
      "enable": true,
      "files": ["obfuscation-rules.txt"],
    }
  }
}
```

When you create a new library, additional property `consumerFiles` will be added.

```
"arkOptions": {
  "obfuscation": {
    "ruleOptions": {
      "enable": true,
      "files": ["obfuscation-rules.txt"],
    }
    "consumerFiles": ["consumer-rules.txt"]
  }
}
```

To enable obfuscation, the following conditions should be satisfied:

* the property `ruleOptions.enable` is `true` and the property `ruleOptions.enable` of every dependent library is `true`.
* build in release mode

The files in the property `ruleOptions.files` will be applied when you build HAP or HAR.

The files in the property `consumerFiles` will be applied when you build the project or library which
depends on this library. They will also be merged into a file `obfuscation.txt` in the resulting HAR.

When you are building HAP or HAR, the final obfucation rules are combination of self's `ruleOptions.files`
property, dependent libraries' `consumerFiles` properties and dependent HAR's `obfuscation.txt`.
If you are building HAR, the content of `obfuscation.txt` is the combination of self's `consumerFiles` property,
dependent libraries' `consumerFiles` properties and dependent HAR's `obfuscation.txt`. If you are building
HAP, `obfuscation.txt` will not be generated. For more details, please jump to
"[How Arkguard merges rules](#how-arkguard-merges-rules)".

## Write rules

The files `obfuscation-rules.txt` and `consumer-rules.txt` are created by DevEco Studio automatically, but they do not
contain any rule by default. You can write rules in these files or include rules from other files, as the following
example shows.

```
"buildOption": {
  "arkOptions": {
    "obfuscation": {
      "ruleOptions": {
        "enable": true,
        "files": ["obfuscation-rules.txt", "myrules.txt"],
      }
      "consumerFiles": ["consumer-rules.txt", "my-consumer-rules.txt"]
    }
  }
}
```

In rule files, you can write [obfuscation options](#obfuscation-options) and [keep options](#keep-options).

### Obfuscation options

#### -disable-obfuscation

Specifies to disable all obfuscations. If you use this option, the resulting HAP or HAR will not be obfuscated. By default,
Arkguard only obfuscates the parameter names and local variable names by assigning random short names to them.

#### -enable-property-obfuscation

Specifies to obfuscate the property names. If you use this option, all property names will be obfuscated except the
following:

* the property names of classes or objects directly imported or exported by `import/export` will be kept. For example, the property name `data` in

    ```
    export class MyClass {
       data: string;
    }
    ```

    will not be obfuscated.
For 'indirectly export' cases such as `export MyClass` and `let a = MyClass; export {a};`, if you do not want to obfuscate
their property names, you need to use [keep options](#keep-options) to keep them. Besides, for the property names of properties of directly exported classes or objects, like `name` and `age` in the following example, if you do not want to obfuscate them, then you also need [keep options](#keep-options) to keep them.

    ```
    export class MyClass {
       person = {name: "123", age: 100};
    }
    ```

    If you want to obfuscate import/export names, please refer to the [`-enable-export-obfuscation`](#-enable-export-obfuscation) option.
* the property names defined in UI components. For example, the property names `message` and `data` in

    ```
    @Component struct MyExample {
        @State message: string = "hello";
        data: number[] = [];
        ...
    }
    ```

    will not be obfuscated.
* the property names that are specified by [keep options](#keep-options).
* the property names in system API list. System API list is a name set which is extracted from SDK automatically by default. The cache file is systemApiCache.json, and the path is build/cache/{...}/release/obfuscation in the module directory.
* in the Native API scenario, the APIs in the d.ts file of so library will not be obfuscated.
* the property names that are string literals. For example, the property names "name" and "age" in the following code will not be obfuscated.

    ```
    let person = {"name": "abc"};
    person["age"] = 22;
    ```

    If you want to obfuscate these string literal property names, you should addtionally use the option `-enable-toplevel-obfuscation`. For example,

    ```
    -enable-property-obfuscation
    -enable-string-property-obfuscation
    ```

    **Note**:  
    **1.** If there are string literal property names which contain special characters (that is, all characters except
    `a-z, A-Z, 0-9, _`, for example `let obj = {"\n": 123, "": 4, " ": 5}` then we would not suggest to enable the
    option `-enable-string-property-obfuscation`, because [keep options](#keep-options) may not allow to keep these
    names when you do not want to obfuscate them.  
    **2.** The property white list of the system API does not include the string constant in the declaration file. For example, the string `'ohos.want.action.home'` in the example is not included in the white list.

    ```
    // System Api @ohos.app.ability.wantConstant snippet:
    export enum Params {
      DLP_PARAM_SANDBOX = 'ohos.dlp.param.sandbox'
    }
    // Developer source example：
    let params = obj['ohos.want.action.home'];
    ```

    Therefore, when `-enable-string-property-obfuscation` is enabled, if you don't want to obfuscate the property like `'ohos.dlp.param.sandbox'`, which is a string constant in system api. you should keep it manually.

Specifies to obfuscate the names in the global scope. If you use this option, all global names will be obfuscated
except the following:

* the `import/export` global names.
* the global names that are not declared in the current file.
* the global names that are specified by [keep options](#keep-options).
* the global names in system API list.

#### -enable-filename-obfuscation

Specifies to obfuscate the file/folder names. If you use this option, all file/folder names will be obfuscated except the following:

* the file/folder names configured in the 'main' and 'types' fields in the oh-package.json5.
* the file/folder names configured in the 'srcEntry' field in the module.json5.
* the file/folder names that are specified by [`-keep-file-name`](#keep-options).
* non-ECMAScript module reference (ECMAScript module example: `import {foo} from './filename'`)
* non-path reference, such as json5 will not be obfuscated `import module from 'json5'`
**Note**:
**1.** Due to the system loading certain specified files during application runtime, developers need to configure the corresponding white list manually in the [` keep file name `] option to prevent specified files from being confused and causing runtime failures.
The above situations that require configure white list manually include but are not limited to the following scenarios:
(1) When the module contains Ability component, you need to configure all paths corresponding to 'srcEntry' in the 'abilities' field in `scr/main/module.json5` to the white list.
(2) When the module contains multithreading services: Worker, you need to configure all the paths in the field 'buildOption'-'sourceOption'-'workers' in `build-profiles.json5` to the white list.

#### -enable-export-obfuscation

Enable name and property name obfuscation for directly imported or exported classes or objects. If you use this option, the names of direct imports or exports in the module will be obfuscated, except in the following scenarios:

* The names and property names of classes or objects exported in remote HAR (packages with real paths in oh_modules) will not be obfuscated.
* Names and property names specified by [keep options](#keep-options) will not be obfuscated.
* Names in the system API list will not be obfuscated.

**Note**:

1. To obfuscate the property names in imported or exported classes, you need to enable both the `-enable-property-obfuscation` and `-enable-export-obfuscation` options.
2. When compiling HSP, if the `-enable-export-obfuscation` option is used, the externally exposed interfaces need to be kept in the obfuscation configuration file `obfuscation-rules.txt` in the module.
3. In the scenario where HAP/HSP/HAR depends on HSP, if the `-enable-export-obfuscation` option is used during compilation, the interface imported from HSP needs to be kept in the obfuscation configuration file `obfuscation-rules.txt` in the module.

     ```
     // Code example (entry file Index.ets in HSP):
     export { add, customApiName } from './src/main/ets/utils/Calc'
    
     // Example of keeping interface name:
     // obfuscation-rules.txt file configuration in HSP and modules that depend on this HSP:
    keep-global-name
    add
    customApiName
     ```

#### -compact

Specifies to remove unnecessary blank spaces and all line feeds. If you use this option, all code will be compressed into
one line.  
**Note**: The stack information in release mode only includes the line number of code, not the column number. Therefore, when the compact is enabled, the specific location of the source code cannot be located based on the line number of stack information.

#### -remove-log

Delete the expressions involving direct calls to console.* statements in the following scenarios:

1. Calls at the toplevel level of a file.
2. Calls within a block.
3. Calls within a module.
4. Calls within a switch statement.
and the return value of console.* should not be called

#### `-print-namecache` filepath

Specifies to print the name cache that contains the mapping from the old names to new names.  
Note: The namecache.json file will be generated every time the module is fully built, so you should save a copy each time you publish a new version.

#### `-apply-namecache` filepath

Specifies to reuse the given cache file. The old names in the cache will receive the corresponding new names specified in
the cache. Other names will receive new random short names. This option should be used in incremental obfuscation.

By default, DevEco Studio will keep and update the namecache file in the temporary cache directory and apply the cache for
incremental compilation.  
Cache directory: build/cache/{...}/release/obfuscation

#### -remove-comments

Remove all comments including single line, multi line and JsDoc comments, in a project except:

* Those names of JsDoc comments above class, function, struct, enum ... in declaration files are in `-keep-comments`.
**Note**: `-keep-comments` doesn't work for comments in generated source files, which will be deleted.

### Keep options

#### `-keep-property-name` [,identifiers,...]

Specifies property names that you want to keep. For example,

```
-keep-property-name
age
firstName
lastName
```

**Note**: This option is avaliable when `-enable-property-obfuscation` is enabled.

`-keep-comments`
To retain JsDoc comments above elements in declaration files, such as preserving the JsDoc comment above the Human class,
you can make the following configuration:

```
-keep-comments
Human
```

**Note**:

1. This option is avaliable when `-remove-comments` is enabled.
2. If the name of an element is obfuscated, the JsDoc comments
above that element cannot be kept using `-keep-comments`. For example, when you have exportClass in `-keep-comments`,
you should make sure that the following class will not be obfuscated, or the JsDoc comments above the class will still be removed:

```
/**
** @class exportClass
*/
export class exportClass {}
```

**What property names should be kept?**

For safety, we would suggest keeping all property names that are not accessed through dot syntax.

Example:

```
var obj = {x0: 0, x1: 0, x2: 0};
for (var i = 0; i <= 2; i++) {
  console.log(obj['x' + i]);  // x0, x1, x2 should be kept
}

Object.defineProperty(obj, 'y', {});  // y should be kept
console.log(obj.y);

obj.s = 0;
let key = 's';
console.log(obj[key]);        // s should be kept

obj.u = 0;
console.log(obj.u);           // u can be safely obfuscated

obj.t = 0;
console.log(obj['t']);        // t and 't' can be safely obfuscated when `-enable-string-property-obfuscation`, but we suggest keeping t

obj['v'] = 0;
console.log(obj['v']);        // 'v' can be safely obfuscated when `-enable-string-property-obfuscation`, but we suggest keeping v
```

In the native API scenario, if in the ets/ts/js file you want to use APIs that are not declared in d.ts file, you need to keep these APIs.

#### `-keep-global-name` [,identifiers,...]

Specifies names that you want to keep in the global scope. For example,

```
-keep-global-name
Person
printPersonName
```

**What global names should be kept?**

It is known that in javascript the variables in the global scope are properties of `globalThis`. So if in your code
you access a global variable as a property, then the global name should be kept.

Example:

```
var a = 0;
console.log(globalThis.a);  // a should be kept

function foo(){}
globalThis.foo();           // foo should be kept

var c = 0;
console.log(c);             // c can be safely obfuscated

function bar(){}
bar();                      // bar can be safely obfuscated

class MyClass {}
let d = new MyClass();      // MyClass can be safely obfuscated
```

#### `-keep-file-name` [,identifiers,...]

Specify the name of files/folders to keep (no need to write the file suffix). for example,

```
-keep-file-name
index
entry
```

**What file names should be kept?**

```
const module1 = require('./file1')   // ARKTs doesn't support CommonJS, this path reference should be kept.
const moduleName = './file2'
const module2 = import(moduleName)   // dynamic reference cannot identify whether moduleName is a path and should be retained.
```

#### `-keep-dts` filepath

Specifies to keep names in the given `.d.ts` file. Here filepath can be also a directory. If so, then the names in all
`d.ts` files under the given directory will be kept.
If your are building HAR with this option, then the kept names will be merged into the resulting `obfuscation.txt`.

#### `-keep` path

Names(such as variable names, class names, property names, etc.) in the specified path are not obfuscated. This path can be a file or a folder. If it is a folder, the files in the folder and the files in subfolders will not be obfuscated.  
The path only supports relative paths, `./` and `../` are relative to the directory where the obfuscation configuration file is located.

```
-keep
./src/main/ets/fileName.ts  // The names in fileName.ts are not obfusated.
../folder                   // The names of files and subfolders in the folder directory are not obfusated.
../oh_modules/json5         // The names of all files in the referenced library json5 are not obfusated.
```

Note: This option does not affect the function of file name obfuscation `-enable-filename-obfuscation`

### Keep options support wildcards

#### Wildcards for name categories

The following options support configuring wildcards for name categories:<br>
`-keep-property-name`<br>
`-keep-global-name`<br>
`-keep-file-name`<br>
`-keep-comments`<br>

The usage of name categories wildcards is as follows:

| Wildcard | Meaning                              | Example                                            |
| -------- | ------------------------------------ | -------------------------------------------------- |
| ?        | Matches any single character         | "AB?" can match "ABC", etc., but cannot match "AB" |
| \*       | Matches any number of any characters | "*AB*" can match "AB", "aABb", "cAB", "ABc", etc.  |

**Examples**：

Retains all property names starting with 'a':

```
-keep-property-name
a*
```

Retains all single-character property names:

```
-keep-property-name
?
```

Retains all property names:

```
-keep-property-name
*
```

#### Wildcards for path categories

The following options support configuring wildcards for path categories:

`-keep`

The usage of path categories wildcards is as follows:

| Wildcard | Meaning                                                                                                                                             | Example                                                       |
| -------- | --------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------- |
| ?        | Matches any single character except path separator '/'                                                                                              | "../a?" can match "../ab", etc., but cannot match "../a/"     |
| \*       | Matches any number of any characters except path separator '/'                                                                                      | "../a*/c" can match "../ab/c", but cannot match "../ab/d/s/c" |
| \*\*     | Matches any number of any characters                                                                                                                | "../a**/c" can match "../ab/c", and also "../ab/d/s/c"        |
| !        | Represents negation and can only be written at the beginning of a path to exclude certain cases that already exist in the user-configured whitelist | "!../a/b/c.ets" means except "../a/b/c.ets"                   |

**Examples**：

Indicates that the c.ets files in all folders in ../a/b/ (excluding subfolders) will not be obfuscated:

```
-keep
../a/b/*/c.ets
```

Indicates that the c.ets files in all folders in ../a/b/ (including subfolders) will not be obfuscated:

```
-keep
../a/b/**/c.ets
```

Indicates that except for the c.ets file, all other files in ../a/b/ will not be obfuscated:

```
-keep
../a/b/
!../a/b/c.ets
```

Meaningless:

```
-keep
!../a/b/c.ets
```

Indicates that all files will not be obfuscated:

```
-keep
*
```

**Note**：

(1)The above options do not support configuring wildcards '*', '?', '!' for other meanings.

For example:

```
class A {
  '*'= 1
}
-keep-property-name
*
```

It becomes ineffective when you only want to retain the '\*' property.

Here, \* indicates matching any number of any characters, resulting in all property names not being obfuscated, rather than only '\*' not being obfuscated.

(2) The -keep option only allows the use of '/' as the path separator and does not support '\\' or '\\\\'.

### Comments

You can write comments in obfuscation rule file by using `#`. The line begins with `#` is treated as comment.
For example,

```
# white list for MainAbility.ets
-keep-global-name
MyComponent
GlobalFunction

-keep-property-name # white list for dynamic property names
firstName
lastName
age
```

If your are building HAR, comments will not be merged into the resulting `obfuscation.txt`.

### How Arkguard merges rules

Typically there may be serveral rule files in your project. These rule files come from:

* `ruleOptions.files` in main project (Here by main project we mean the project you are building)
* `consumerFiles` in local dependent libraries
* `obfuscate.txt` in remote dependent HARs
When building your main project, all these rules will be merged by the following strategy (in pseudo code):

```
let `listRules` be the list of all rule files that are mentioned above.
let finalRule = {
    disableObfuscation: false,
    enablePropertyObfuscation: false,
    enableToplevelObfuscation: false,
    compact: false,
    removeLog: false,
    keepPropertyName: [],
    keepGlobalName: [],
    keepDts: [],
    printNamecache: string,
    applyNamecache: string
}
for each file in `listRules`:
    for each option in file:
        switch(option) {
            case -disable-obfuscation:
                finalRule.disableObfuscation = true;
                continue;
            case -enable-property-obfuscation:
                finalRule.enablePropertyObfuscation = true;
                continue;
            case -enable-toplevel-obfuscation:
                finalRule.enableToplevelObfuscation = true;
                continue;
            case -compact:
                finalRule.compact = true;
                continue;
            case -remove-log:
                finalRule.removeLog = true;
                continue;
            case -print-namecache:
                finalRule.printNamecache = #{specified path};
                continue;
            case -apply-namecache:
                finalRule.applyNamecache = #{specified path};
                continue;
            case -keep-property-name:
                finalRule.keepPropertyName.push(#{specified names});
                continue;
            case -keep-global-name:
                finalRule.keepGlobalName.push(#{specified names});
                continue;
            case -keep-dts:
                finalRule.keepDts.push(#{specified file path});
                continue;
        }
    end-for
end-for
```

The final obfuscation rules are in the object `finalRule`.

If you are building HAR, the resulting `obfuscate.txt` are obtained by merging the rules from `consumerFiles` in main
project and local dependent libraries, and `obfuscate.txt` in remote dependent HARs. The merging strategy is the same
except:

* The `-keep-dts` option will be converted to `-keep-global-name` and `-keep-property-name` options in the resulting
`obfuscate.txt`.
* The options `-print-namecache` and `apply-namecache` will be omitted and will not appear in the resulting
`obfuscate.txt`.
