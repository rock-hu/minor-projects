# 🚨 **重要提示 | IMPORTANT**
>
> **⚠️ 此代码仓已归档。新地址请访问 [homecheck](https://gitcode.com/openharmony-sig/homecheck)。| ⚠️ This repository has been archived. For the new address, please visit [homecheck](https://gitcode.com/openharmony-sig/homecheck).**
>
---
>
# homecheck

## 项目简介

该项目（homecheck）专为提升代码质量而设计，能高效识别代码缺陷并提出方案；其核心功能是对应用工程项目执行静态代码分析，评估代码在安全性、性能等方面上的表现，精准定位问题及其在代码中的位置。

## 目录

## 目录

```
homecheck/src
├── checkers             # 项目检测规则功能代码
├── common               # 项目通用代码
├── config               # 项目配置
├── object               # 项目中使用到的实体类
├── processFunction      # 项目运行过程中每一步抽象成的方法
├── startFunction        # 使用processFunction中函数组合成的项目主体流程
├── utils                # 项目工具类
├── test                 # 测试目录
└── Index.ts             # 项目入口
```

## 项目主体流程

1.读取配置文件

2.根据配置文件参数，使用**ArkAnalyzer**项目构建**sence**

3.根据配置文件参数，获取需要检测的文件https://gitee.com/openharmony-sig/arkanalyzer

4.前处理

5.进行检测

6.后处理

## 使用说明

本项目需要集成ArkAnalyzer项目使用，使用前必须安装[ArkAnalyzer]()，运行环境为node.js.

## QuickStart

1.下载本项目

2.进入项目根目录，打开终端

```
cmd
```

3.安装依赖库

```
npm install
```

4.修改配置

**tsconfig.json**中修改ArkAnalyzer项目路径，默认配置homecheck和ArkAnalyzer位于同一目录

```
"paths": {
      "@ArkAnalyzer/*": ["../arkanalyzer/*"]
}
```

**config/Config-xxx.json**中修改需要检测的项目路径，xxx标示后缀名，默认为Community，即**默认配置文件为config/Config-Community.json**

```
 "projectPath": "C:/work/TestProject",
```

5.配置环境变量（调试时需要，编译不需要）

```
  "env": {
    "BUILDER": "Community"
  }
```

6.启动项目（注意修改文件路径）

设置配置文件路径configPath

```
C:\my\node\node.exe -r ts-node/register -r tsconfig-paths/register C:\work\homecheck\src\Index.ts  --configPath=./src/config/Config-Community.json
```

## 注意事项

### 新增检测规则checker

- 规则名大驼峰命名，并必须以Check结尾，例如ArrayDefinitionCheck

- 自定义checker需要继承BaseChecker，并实现check方法，具体检测逻辑在check方法中

- 自定义checker需要使用export default修饰

- checker根据**分类**位于checkers目录下的不同子目录

- 创建checker后可在**config/RuleSet.json**中配置到不同分类的集合，格式'@' + **分类** + '/' + checker名称 + ':' + 等级 ；其中checker名称与创建的checker文件名一致，但是为全小写，使用'_'分割，结尾check可省略，例如：

  ```
  "@performance/number-init-check:1"
  ```

- 如需单独使用checker，可以在配置文件**config/Config-Community.json**中配置，例如：

  ```
  "rules": ["@performance/array-definition-check:1"]
  ```

  config说明

## 配置说明

### 1.config/Config-xxx.sample.json

实际配置文件的示例，包含对各个属性的说明

### 2.config/Config-xxx.json

实际生效的配置文件，即启动命令中传入的配置文件参数，各属性作用可参考Config-xxx.sample中的说明

注意，所有自定义配置文件必须位于config目录下

### 3.config/log4js.json

```json
{
  "appenders": {
    "default": {
      "type": "fileSync",
      "filename": "./out/log.log",//日志输出文件
      "maxLogSize": 5000000,
      "backups": 5,
      "compress": true,
      "encoding": "utf-8",
      "layout": {
        "type": "pattern",
        "pattern": "[%d] [%p] [%z] [tag] - %m"//日志格式：[%d]--时间 [%p]--日志级别 [%z]--进程ID [tag]--自定义标签 - %m--日志消息
      }
    },
    "console": {
      "type": "console",
      "layout": {
        "type": "pattern",
        "pattern": "[%d] [%p] [%z] [tag] - %m"
      }
    }
  },
  "categories": {
    "default": {
      "appenders": ["default", "console"],//生效的appender，default为打印到日志文件，console为打印到控制台
      "level": "info",
      "enableCallStack": true
    }
  }
}
```

### 4.config/RuleSet.json

配置规则集，完成配置后，在Config-xxx.json中的ruleSet属性配置对应的规则集后，集合中所有规则都将生效。

## 打包

windows系统下可依次使用以下命令打包

```
webpack --mode=production
```



```bash
./node_modules/.bin/ncc build ./dist/index.js -o ./out -m
```

非windows系统下可使用以下命令打包

```
npm run release
```

## 相关仓

[ArkAnalyzer](https://gitee.com/openharmony-sig/arkanalyzer)

