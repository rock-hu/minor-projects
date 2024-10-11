# Logback

## Introduction

> Logback is a logging framework.

![preview.gif](preview_EN.gif)

## How to Install

```shell
ohpm install @ohos/logback
```

For details about the OpenHarmony ohpm environment configuration, see [OpenHarmony HAR](https://gitee.com/openharmony-tpc/docs/blob/master/OpenHarmony_har_usage.en.md).

## How to Use
1. To use SMTPAppender, add the following permissions to the **module.json5** file.

 ```
   "requestPermissions": [
      {
        "name": "ohos.permission.INTERNET"
      },
      {
        "name": "ohos.permission.GET_NETWORK_INFO"
      },
      {
        "name": "ohos.permission.GET_WIFI_INFO"
      }
    ]
 ```

2. To use SocketAppender, add the permission to the **module.json5** file.

 ```
   "requestPermissions": [
      {
        "name": "ohos.permission.INTERNET"
      }
    ]
 ```

3. Import dependencies.

 ```
   import  { Level, Appender, FilterEnum, Configuration, Logger, LoggerFactory }  from '@ohos/logback'
 ```

4. Add the configuration file to **onWindowStageCreate** of the MainAbility.

 ```
let configuration: Configuration = {
    appender: [{
                   name: 'ROOT',
                   className: AppenderEnum.CONSOLE,
                   encoder: {
                       pattern: {
                           msg: '%date %level %logger %msg'
                       }
                   }
               },
               {
                   name: 'LOGGER',
                   className: AppenderEnum.CONSOLE,
                   encoder: {
                       pattern: {
                           msg: '%date %level %logger %msg'
                       }
                   }
               },
               {
                   name: 'THRESHOLD',
                   className: AppenderEnum.CONSOLE,
                   encoder: {
                       pattern: {
                           msg: '%date %level %logger %msg'
                       }
                   },
                   filter: {
                       className: FilterEnum.THRESHOLD,
                       level: Level.INFO,
                   }
               },
               {
                   name: 'LEVEL',
                   className: AppenderEnum.CONSOLE,
                   encoder: {
                       pattern: {
                           msg: '%date %level %logger %msg'
                       }
                   },
                   filter: {
                       className: FilterEnum.LEVEL,
                       level: Level.WARN,
                       onMatch: Match.ACCEPT,
                       onMismatch: Match.DENY
                   }
               },
               {
                   name: 'LOGFILE',
                   className: AppenderEnum.FILE,
                   file: '111.log',
                   encoder: {
                       pattern: {
                           msg: '%level %logger %msg'
                       }
                   },
                   filter: {
                       className: FilterEnum.THRESHOLD,
                       level: Level.ERROR,
                   }
               },
               {
                   name: 'SMTP',
                   className: AppenderEnum.SMTP,
                   fromMail: "xxx1@xxx.com," // Sender email address
                   toMail: "xxx2@xxx.com," // Recipient email address
                   subject: "mail subject," // Email subject
                   smtpHost: "smtp host," // Host domain name of the recipient's email server
                   smtpPort: 25, //// Port number of the recipient's email server
                   username: "xxx," // Sender email address
                   password: "xxx," // Email third-party authorization code
                   localHost: "xx.xx.xx.xx" // Device IP address
               },
               {
                   name: 'SIFT',
                   className: AppenderEnum.SIFT,
                   discriminator: {
                       Key: 'userid',
                       DefaultValue: 'unknown'
                   },
                   sift: {
                       appender: {
                           file: '${userid}.log',
                           className: AppenderEnum.FILE,
                           encoder: {
                               pattern: {
                                   msg: '%level %logger %msg'
                               }
                           },
                           filter: {
                               className: FilterEnum.THRESHOLD,
                               level: Level.INFO,
                           }
                       },
                       mdc: [
                           { key: 'userid', value: '100001' },
                           { key: 'userid', value: '100002' }
                       ]
                   }
               },
               {
                   name: 'DB',
                   className: AppenderEnum.DB,
                   storeName: 'logback_rdb.db',
                   encoder: {
                       pattern: {
                           msg: '%date %level %logger %msg'
                       }
                   },
                   filter: {
                       className: FilterEnum.THRESHOLD,
                       level: Level.INFO,
                   }
               },
               {
                   name: 'ROLLING',
                   className: AppenderEnum.ROLLING_FILE,
                   encoder: {
                       pattern: {
                           msg: '%date %level %logger %msg'
                       }
                   },
                   rollingPolicy:{
                       fileName: 'rolling.log',
                       filePattern: 'rolling-%d{yyyy-MM-dd HH:mm:ss}.log',
                       maxHistory: 5
                   }
               },
              {
                   name: 'SOCKET',
                   className: AppenderEnum.SOCKET,
                   localHost: 'xx.xx.xx.xx', // IP address of the current device
                   remoteHost: 'xx.xx.xx.xx', // IP address of the target device
                   port: xxx              // Port number of the target address
                 }],
    logger: [{
                 name: 'com.log.logger',
                 additivity: true,
                 level: Level.FATAL,
                 appenderRef: {
                     ref: 'LOGGER'
                 }
             }, {
                 name: 'com.log.threshold',
                 additivity: true,
                 level: Level.WARN,
                 appenderRef: {
                     ref: 'THRESHOLD'
                 }
             }, {
                 name: 'com.log.level',
                 additivity: true,
                 level: Level.WARN,
                 appenderRef: {
                     ref: 'LEVEL'
                 }
             },
             {
                 name: 'com.log.file',
                 additivity: true,
                 level: Level.WARN,
                 appenderRef: {
                     ref: 'LOGFILE'
                 }
             },
             {
                 name: 'com.log.smtp',
                 additivity: true,
                 level: Level.ERROR,
                 appenderRef: {
                     ref: 'SMTP'
                 }
             },
             {
                 name: 'com.log.sift',
                 additivity: true,
                 level: Level.WARN,
                 appenderRef: {
                     ref: 'SIFT'
                 }
             },
             {
                 name: 'com.log.db',
                 additivity: true,
                 level: Level.WARN,
                 appenderRef: {
                     ref: 'DB'
                 }
             },
             {
                 name: 'com.log.rolling',
                 additivity: true,
                 level: Level.WARN,
                 appenderRef: {
                     ref: 'ROLLING'
                 }
             },
             {
                name: 'com.log.socket',
                additivity: true,
                level: Level.WARN,
                appenderRef: {
                  ref: 'SOCKET'
                }
              }],
    root: {
        level: Level.ERROR,
        appenderRef: {
            ref: 'ROOT'
        }
    }
}
 ```

5. Print logs.

 ```
   let logger: Logger = LoggerFactory.getLogger(this.context, "com.log.hello")
   logger.debug("=====debug=====")
   logger.info("=====info=====")
   logger.warn("=====warn=====")
   logger.error("=====error=====")
   logger.fatal("=====fatal=====")
 ```

6. Write logs to a file.

 ```
   let logger: Logger = LoggerFactory.getLogger(this.context, "com.log.file", this.config)
   logger.info("=====info=====")
   logger.warn("=====warn=====")
   logger.error("=====error=====")
 ```

## Available APIs

1. Print log information of the debug level.
   `Logger.debug()`
2. Setting the log level.
   `Logger.setLevel(level: Level)`

## Constraints

This project has been verified in the following versions:

- DevEco Studio: 4.1 Canary (4.1.3.317)

- OpenHarmony SDK: API 11 (4.1.0.36)

## Directory Structure

````
|---- Logback
|     |---- entry  # Sample code
|     |---- library  # Logback library
|	    |----src
          |----main
              |----ets
                  |----components
                      |----appender # Appender types for saving logs
                      |----config # Log message conversion
                      |----format # Date formatting file
                      |----formatter # Log configuration file
                      |----io # File operations
                      |----log # Log output
                      |----mdc # MDC storage
                      |----rdb # Database operations
                      |----slf4j # slf4j log print class file
                      |----util # Utility class file
                      |----AppenderEnum.ts # Enum of log print types
                      |----FilterEnum.ts # Enum of log filtering types
                      |----Level.ts # Enum of log levels
                      |----Logger.ts # Log print output class
|           |---- Index.ts  # External APIs
|     |---- README.md  # Readme                   
````

## How to Contribute

If you find any problem when using the project, submit an [issue](https://gitee.com/openharmony-sig/logback/issues) or a [PR](https://gitee.com/openharmony-sig/logback/pulls).

## License

This project is licensed under [Apache License 2.0](https://gitee.com/openharmony-sig/logback/blob/master/LICENSE).
