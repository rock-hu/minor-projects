### 一、内容说明（相关的Issue）



### 二、建议测试周期和提测地址  
  建议测试完成时间：xxxx.xx.xx  
  投产上线时间：xxxx.xx.xx  
  提测地址：CI环境/压测环境  
  测试账号：  

### 三、变更内容
  * 3.1 关联PR列表

  * 3.2 数据库和部署说明  
    1. 常规更新 
    2. 重启unicorn
    3. 重启sidekiq
    4. 迁移任务：是否有迁移任务，没有写 "无"
    5. rake脚本：`bundle exec xxx RAILS_ENV = production`；没有写 "无"

  * 3.4 技术优化内容（做了什么，变更了什么）
    - 重构了 xxxx 代码
    - xxxx 算法优化


  * 3.5 废弃通知（什么字段、方法弃用？）



  * 3.6  后向不兼容变更（是否有无法向后兼容的变更？）


  
### 四、研发自测点（自测哪些？冒烟用例全部自测？）
  自测测试结论：


### 五、测试关注点（需要提醒QA重点关注的、可能会忽略的地方）
  检查点：

| 需求名称 | 是否影响xx公共模块 | 是否需要xx功能 | 需求升级是否依赖别的子产品 |
|------|------------|----------|---------------|
| xxx  | 否          | 需要       | 不需要           |
|      |            |          |               |

  接口测试：

  性能测试：

  并发测试：


