# 入门<!--application-getting-started-->
<!--Del-->
- 快速入门<!--quick-start-->
  - [开发准备](start-overview.md)
  - [构建第一个ArkTS应用（Stage模型）](start-with-ets-stage.md)
<!--DelEnd-->
- 开发基础知识<!--development-fundamentals-->
  - [应用程序包术语](application-package-glossary.md)
  - 应用程序包基础知识<!--application-package-fundamentals-->
    - [应用程序包概述](application-package-overview.md)
    - 应用程序包结构<!--application-package-structure-->
      - [Stage模型应用程序包结构](application-package-structure-stage.md)
      - [FA模型应用程序包结构](application-package-structure-fa.md)
    - 应用程序包开发与使用<!--application-package-dev-->
      - [HAP](hap-package.md)
      - [HAR](har-package.md)
      - [HSP](in-app-hsp.md)
    - [应用程序包安装卸载与更新](application-package-install-uninstall.md)
  - 应用配置文件（Stage模型）<!--application-configuration-file-stage-->
    - [应用配置文件概述（Stage模型）](application-configuration-file-overview-stage.md)
    - [app.json5配置文件](app-configuration-file.md)
    - [module.json5配置文件](module-configuration-file.md)
  - 应用配置文件（FA模型）<!--application-configuration-file-fa-->
    - [应用配置文件概述（FA模型）](application-configuration-file-overview-fa.md)
    - [app对象内部结构](app-structure.md)
    - [deviceConfig内部结构](deviceconfig-structure.md)
    - [module对象内部结构](module-structure.md)
  - 典型场景的开发指导<!--application-typical-scenarios-->
    - [集成态HSP](integrated-hsp.md)
    - [HAR转HSP指导](har-to-hsp.md)
    - [HSP转HAR指导](hsp-to-har.md)
    - [创建应用静态快捷方式](typical-scenario-configuration.md)
    - [创建应用分身](app-clone.md)
    - [创建应用多实例](multiInstance.md)
    - [配置分层图标](layered-image.md)
- [资源分类与访问](resource-categories-and-access.md)
- 学习ArkTS语言<!--learning-arkts-->
  - [初识ArkTS语言](arkts-get-started.md)
  - [ArkTS语言介绍](introduction-to-arkts.md)
  - [ArkTS语言规范](arkts-coding-style-guide.md)
  - 从TypeScript到ArkTS的适配指导<!--typescript-to-arkts-migration-->
    - [ArkTS语法适配背景](arkts-migration-background.md)
    - [从TypeScript到ArkTS的适配规则](typescript-to-arkts-migration-guide.md)
    - [适配指导案例](arkts-more-cases.md)
  - [ArkTS高性能编程实践](arkts-high-performance-programming.md)
  - UI范式基本语法<!--arkts-ui-paradigm-basic-syntax-->
    - [基本语法概述](arkts-basic-syntax-overview.md)
    - [声明式UI描述](arkts-declarative-ui-description.md)
    - 自定义组件<!--arkts-custom-components-->
      - [创建自定义组件](arkts-create-custom-components.md)
      - [页面和自定义组件生命周期](arkts-page-custom-components-lifecycle.md)
      - [自定义组件的自定义布局](arkts-page-custom-components-layout.md)
      - [自定义组件成员属性访问限定符使用限制](arkts-custom-components-access-restrictions.md)
    - [\@Builder装饰器：自定义构建函数](arkts-builder.md)
    - [\@LocalBuilder装饰器：维持组件父子关系](arkts-localBuilder.md)
    - [\@BuilderParam装饰器：引用\@Builder函数](arkts-builderparam.md)
    - [wrapBuilder：封装全局@Builder](arkts-wrapBuilder.md)
    - [\@Styles装饰器：定义组件重用样式](arkts-style.md)
    - [\@Extend装饰器：定义扩展组件样式](arkts-extend.md)
    - [stateStyles：多态样式](arkts-statestyles.md)
    - [@AnimatableExtend装饰器：定义可动画属性](arkts-animatable-extend.md)
    - [@Require装饰器：校验构造传参](arkts-require.md)
    - [@Reusable装饰器：组件复用](arkts-reusable.md)
  - 状态管理<!--arkts-state-management-->
    - [状态管理概述](arkts-state-management-overview.md)
    - 状态管理（V1）<!--arkts-state-management-v1-->
      - 管理组件拥有的状态<!--arkts-component-state-management-->
        - [\@State装饰器：组件内状态](arkts-state.md)
        - [\@Prop装饰器：父子单向同步](arkts-prop.md)
        - [\@Link装饰器：父子双向同步](arkts-link.md)
        - [\@Provide装饰器和\@Consume装饰器：与后代组件双向同步](arkts-provide-and-consume.md)
        - [\@Observed装饰器和\@ObjectLink装饰器：嵌套类对象属性变化](arkts-observed-and-objectlink.md)
      - 管理应用拥有的状态<!--arkts-application-state-management-->
        - [管理应用拥有的状态概述](arkts-application-state-management-overview.md)
        - [LocalStorage：页面级UI状态存储](arkts-localstorage.md)
        - [AppStorage：应用全局的UI状态存储](arkts-appstorage.md)
        - [PersistentStorage：持久化存储UI状态](arkts-persiststorage.md)
        - [Environment：设备环境查询](arkts-environment.md)
      - 其他状态管理<!--arkts-other-state-mgmt-functions-->
        - [其他状态管理概述](arkts-other-state-mgmt-functions-overview.md)
        - [\@Watch装饰器：状态变量更改通知](arkts-watch.md)
        - [$$语法：内置组件双向同步](arkts-two-way-sync.md)
        - [\@Track装饰器：class对象属性级更新](arkts-track.md)
        - [自定义组件冻结功能](arkts-custom-components-freeze.md)
      - [MVVM模式](arkts-mvvm.md)
      - [状态管理优秀实践](arkts-state-management-best-practices.md)
      - [状态管理合理使用开发指导](properly-use-state-management-to-develope.md)
    - 状态管理（V2）<!--arkts-state-management-v2-->
      - V2所属装饰器<!--arkts-v2-decorators-->
        - [\@ObservedV2装饰器和\@Trace装饰器：类属性变化观测](arkts-new-observedV2-and-trace.md)
        - [\@ComponentV2装饰器：自定义组件](arkts-new-componentV2.md)
        - [\@Local装饰器：组件内部状态](arkts-new-local.md)
        - [\@Param装饰器：组件外部输入](arkts-new-param.md)
        - [\@Once装饰器：初始化同步一次](arkts-new-once.md)
        - [\@Event装饰器：组件输出](arkts-new-event.md)
        - [\@Provider和\@Consumer装饰器：跨组件层级双向同步](arkts-new-Provider-and-Consumer.md)
        - [\@Monitor装饰器：状态变量修改监听](arkts-new-monitor.md)
        - [\@Computed装饰器：计算属性](arkts-new-Computed.md)
        - [\@Type装饰器: 标记类属性的类型](arkts-new-type.md)
        - [\@ReusableV2装饰器：组件复用](arkts-new-reusableV2.md)
      - 其他状态管理<!--arkts-new-other-state-mgmt-->
        - [AppStorageV2: 应用全局UI状态存储](arkts-new-appstoragev2.md)
        - [PersistenceV2: 持久化存储UI状态](arkts-new-persistencev2.md)
        - [!!语法：双向绑定](arkts-new-binding.md)
        - [自定义组件冻结功能](arkts-custom-components-freezeV2.md)
        - [Repeat：可复用的循环渲染](arkts-new-rendering-control-repeat.md)
        - [getTarget接口：获取状态管理框架代理前的原始对象](arkts-new-getTarget.md)
        - [makeObserved接口：将非观察数据变为可观察数据](arkts-new-makeObserved.md)
      - [MVVM模式（状态管理V2）](arkts-mvvm-V2.md)
    - V1V2混用和迁移指导<!--v1v2-migration-->
      - [自定义组件混用场景指导](arkts-custom-component-mixed-scenarios.md)
      - [状态管理V1V2混用文档](arkts-v1-v2-mixusage.md)
      - [V1->V2迁移指导](arkts-v1-v2-migration.md)
  - 渲染控制<!--arkts-rendering-control-->
      - [渲染控制概述](arkts-rendering-control-overview.md)
      - [if/else：条件渲染](arkts-rendering-control-ifelse.md)
      - [ForEach：循环渲染](arkts-rendering-control-foreach.md)
      - [LazyForEach：数据懒加载](arkts-rendering-control-lazyforeach.md)
      - [ContentSlot：混合开发](arkts-rendering-control-contentslot.md)