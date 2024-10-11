# logback单元测试用例

该测试用例基于OpenHarmony系统下，进行单元测试

单元测试用例覆盖情况

|                                        接口名                                         |是否通过	|备注|
|:----------------------------------------------------------------------------------:|:---:|:---:|
|  LoggerFactory.getLogger(context: Context, name: string, config?: Configuration)   |    pass        |       |
|     LoggerFactory.getILoggerFactory(context: Context, config?: Configuration)      |pass   |        |
|                         MDC.put(key: string, val: string)                          |pass   |        |
|                                 trace(msg: string)                                 |pass   |        |
|                                 debug(msg: string)                                 |pass   |        |
|                                 info(msg: string)                                  |pass   |        |
|                                 warn(msg: string)                                  |pass  |     |
|                                 error(msg: string)                                 |   pass  |          |
|                                 fatal(msg: string)                                 | pass |  |
|                               setLevel(level: Level)                               | pass  |       |
|                                setTag(tag: string)                                 |  pass |          |
|                                     getLevel()                                     |  pass |          |
|                                      getTag()                                      | pass  |          |