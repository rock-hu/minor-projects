## v2.0.2

- 修复LoggerContext错误引用问题
- 
## v2.0.1

- 适配DevEco Studio 版本： 5.0.3.401，OpenHarmony SDK:API12 (5.0.0.27)
  
## v2.0.0

- 适配DevEco Studio 版本： 4.1 Canary(4.1.3.317), OpenHarmony SDK:API11 (4.1.0.36)
- 修复bug
  1. 日志不输出异常修改
  2. socket日志没有发出异常修改
  3. smtp邮件发送异常处理

### 1.0.2

- 适配DevEco Studio 版本：3.1 Beta1（3.1.0.200），OpenHarmony SDK:API9（3.2.10.6）

### 1.0.1

1. api8升级到api9
2. 支持DBAppender保存日志
3. 支持RollingFileAppender过滤日志
4. 支持SiftingAppender过滤日志
5. 支持SMTPAppender保存日志
6. 支持SocketAppender保存日志

### 1.0.0

1. 支持全局配置Logger配置文件
2. 无配置默认日志打印
3. 依据Level级别打印日志
4. ConsoleAppender将日志输出到控制台
5. FileAppender将日志写入到文件
6. 使用LevelFilter过滤打印日志
7. 使用ThresholdFilter过滤打印日志
8. Encoder对日志输出内容进行格式化