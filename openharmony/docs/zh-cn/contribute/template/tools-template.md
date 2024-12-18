# 调试工具写作模板

## 总体写作说明
> **说明：**
> 所有的写作说明，在完成写作后，都要删除。

调试工具的介绍应着重于详尽阐述，旨在为开发者全面展示调试工具所支持的每一项调试与优化命令，确保无遗漏且避免进行简单概括。


***

# 文档标题
> *写作说明*
> 1. **文档标题**：作为文档标题，要求使用调试工具的场景化短语；同时保留工具原来的名称便于开发者理解，可以直接使用。
> 2. **标题层级**：文档标题为一级标题，使用`#`；介绍环境要求、调试工具命令列表、具体命令、工具错误码（可选）为二级标题，使用`##`。


对xxx工具进行描述。此处对该工具的定义、能力、使用场景进行描述。采用如下句式。

（工具介绍）XXX工具(**)是xxxxxx的工具。

（能力描述）为开发者提供了xxxxxxx的能力。

（使用场景的描述）用于xxxxx\xxxxxxx。

**举例**：应用程序包工具（bm工具）

应用程序包工具（Bundle Manager，简称bm）是实现应用安装、卸载、更新、查询等功能的工具，为开发者提供基本的应用安装包的调试能力。
>**说明**
>XXX工具（xx），括号中的内容为英文缩写时，需要在工具介绍中说明全称。


## 环境要求

> *写作说明*
> 1. 根据实际情况说明该工具的运行环境。

使用该工具的环境要求。


**举例**：环境要求

使用本工具前，开发者需要先获取[hdc工具](../../application-dev/dfx/hdc.md)，执行hdc shell。


## 命令列表
> *写作说明*
>1. 命令列表的获取,执行"xx -h"或"xx -help"。
>2. 将得到的返回值中每一个命令例举出来，放到表格中（help/-h 放到第一行）,剩下的命令可以按照使用频率排列。

|命令|描述|
|----|--------|
|help/-h|帮助命令，用于****。|
|...|......|


**举例：**
### bm工具命令列表
| 命令 | 描述 |
| -------- | -------- |
| help | 帮助命令，用于查询bm工具支持的命令信息。 |
| install | 安装命令，用于安装应用。 |
| uninstall | 卸载命令，用于卸载应用。 |
| dump | 查询命令，用于查询应用的相关信息。 |
| clean | 清理命令，用于清理应用的缓存和数据。此命令在root版本下可用，在user版本下打开开发者模式可用。其它情况不可用。|
| enable | 使能命令，用于使能应用，使能后应用可以继续使用。此命令在root版本下可用，在user版本下不可用。 |
|...|...|


## **命令
>*写作要求*
>1. 命令用法：必须包含"xx xxxx help/-h"的返回值中所支持的每一个参数。
>2. 命令参数列表：必须按照"xx xxxx help/-h"的返回值去介绍**命令所支持的每一个参数,并说明每一个参数是否必选以及参数适用的场景。"help/-h"在首位。
>3. 示例：示例命令要根据命令列表的参数进行组合举例（尽可能涉及到每一个）。

```bash
xx xxxx [h] [****] [****]
```
>*注:*将包含参数全部列举。

**命令参数列表**
|参数|参数说明|
|----|--------|
|-h|帮助信息|
|...|......|

示例：

```bash
# 场景描述
xx xxxx 
```
**举例：**
## 安装命令

```bash
bm install [-h] [-p filePath] [-u userId] [-r] [-w waitingTime] [-s hspDirPath]
```

  **安装命令参数列表**


| 参数 | 参数说明 | 
| -------- | -------- | 
| -h | 帮助信息。 | 
| -p | 必选参数，指定路径和多个HAP同时安装。 | 
| -u | 可选参数，给指定用户安装一个HAP。默认安装到当前所有用户上。| 
| -r | 可选参数，覆盖安装一个HAP。默认值为覆盖安装。 | 
| -s | 根据场景判断，安装应用间HSP时为必选参数，其他场景为可选参数。安装应用间共享库， 每个路径目录下只能存在一个同包名的HSP。 |
| -w | 可选参数，安装HAP时指定bm工具等待时间，最小的等待时长为5s，最大的等待时长为600s,&nbsp;默认缺省为5s。 | 




示例：
```bash
# 安装一个hap
bm install -p /data/app/ohos.app.hap
# 覆盖安装一个hap
bm install -p /data/app/ohos.app.hap -r
# 安装一个应用间共享库
bm install -s xxx.hsp
# 同时安装使用方应用和其依赖的应用间共享库
bm install -p aaa.hap -s xxx.hsp yyy.hsp
# 指定用户安装一个hap,等待时间为10s
bm install -p /data/app/ohos.app.hap -u 100 -w 10s 
```

**注意**
> 帮助命令没有参数列表。
<!--no_check-->