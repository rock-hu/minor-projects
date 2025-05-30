# Quick Start<!--application-getting-started-->
<!--Del-->
- Getting Started<!--quick-start-->
  - [Before You Start](start-overview.md)
  - [Building the First ArkTS Application in Stage Model](start-with-ets-stage.md)
<!--DelEnd-->
- Development Fundamentals<!--development-fundamentals-->
  - Application Package Fundamentals<!--application-package-fundamentals-->
    - [Application Package Overview](application-package-overview.md)
    - Application Package Structure<!--application-package-structure-->
      - [Application Package Structure in Stage Model](application-package-structure-stage.md)
      - [Application Package Structure in FA Model](application-package-structure-fa.md)
    - Application Package Development and Usage<!--application-package-dev-->
      - [HAP](hap-package.md)
      - [HAR](har-package.md)
      - [HSP](in-app-hsp.md)
    - [Application Package Installation, Uninstall, and Update](application-package-install-uninstall.md)
  - Application Configuration Files in Stage Model<!--application-configuration-file-stage-->
    - [Overview of Application Configuration Files in Stage Model](application-configuration-file-overview-stage.md)
    - [app.json5 Configuration File](app-configuration-file.md)
    - [module.json5 Configuration File](module-configuration-file.md)
  - Application Configuration Files in FA Model<!--application-configuration-file-fa-->
    - [Overview of Application Configuration Files in FA Model](application-configuration-file-overview-fa.md)
    - [Structure of the app Tag](app-structure.md)
    - [Structure of the deviceConfig Tag](deviceconfig-structure.md)
    - [Structure of the module Tag](module-structure.md)
  - Typical Scenarios Development<!--application-typical-scenarios-->
    - [Integrated HSP](integrated-hsp.md)
    - [Converting HAR to HSP](har-to-hsp.md)
    - [Converting HSP to HAR](hsp-to-har.md)
    - [Creating a Static Shortcut of the Application](typical-scenario-configuration.md)
    - [Creating an Application Clone](app-clone.md)
    - [Creating an Application Multi-Instance](multiInstance.md)
    - [Configuring Layered Icons](layered-image.md)
- [Resource Categories and Access](resource-categories-and-access.md)
- Learning ArkTS<!--learning-arkts-->
  - [Getting Started with ArkTS](arkts-get-started.md)
  - [Introduction to ArkTS](introduction-to-arkts.md)
  - [ArkTS Coding Style Guide](arkts-coding-style-guide.md)
  - TypeScript to ArkTS Migration Guide<!--typescript-to-arkts-migration-->
    - [ArkTS Migration Background](arkts-migration-background.md)
    - [TypeScript to ArkTS Cookbook](typescript-to-arkts-migration-guide.md)
    - [Adaptation Cases](arkts-more-cases.md)
  - [ArkTS Performant Programming Practices](arkts-high-performance-programming.md)
  - Basic Syntax of UI Paradigms<!--arkts-ui-paradigm-basic-syntax-->
    - [Basic Syntax Overview](arkts-basic-syntax-overview.md)
    - [Declarative UI Description](arkts-declarative-ui-description.md)
    - Custom Components<!--arkts-custom-components-->
      - [Creating a Custom Component](arkts-create-custom-components.md)
      - [Page and Custom Component Lifecycle](arkts-page-custom-components-lifecycle.md)
      - [Custom Component Layout](arkts-page-custom-components-layout.md)
      - [Constraints on Access Modifiers of Custom Component Member Variables](arkts-custom-components-access-restrictions.md)
    - [\@Builder Decorator: Custom Builder Function](arkts-builder.md)
    - [\@LocalBuilder Decorator: Maintaining the Parent-Child Relationship Between Component and State Management](arkts-localBuilder.md)
    - [\@BuilderParam Decorator: Referencing the \@Builder Function](arkts-builderparam.md)
    - [wrapBuilder: Encapsulating a Global @Builder](arkts-wrapBuilder.md)
    - [\@Styles Decorator: Definition of Reusable Styles](arkts-style.md)
    - [\@Extend Decorator: Extension of Built-in Components](arkts-extend.md)
    - [stateStyles: Polymorphic Style](arkts-statestyles.md)
    - [\@AnimatableExtend Decorator: Definition of Animatable Attributes](arkts-animatable-extend.md)
    - [\@Require Decorator: Validating Constructor Input Parameters](arkts-require.md)
    - [\@Reusable Decorator: Reusing Components](arkts-reusable.md)
  - State Management<!--arkts-state-management-->
    - [State Management Overview](arkts-state-management-overview.md)
    - State Management (V1)<!--arkts-state-management-v1-->
      - Component State Management<!--arkts-component-state-management-->
        - [\@State Decorator: State Owned by Component](arkts-state.md)
        - [\@Prop Decorator: One-Way Synchronization from the Parent Component to Child Components](arkts-prop.md)
        - [\@Link Decorator: Two-Way Synchronization Between Parent and Child Components](arkts-link.md)
        - [\@Provide and \@Consume Decorators: Two-Way Synchronization with Descendant Components](arkts-provide-and-consume.md)
        - [\@Observed and \@ObjectLink Decorators: Observing Property Changes in Nested Class Objects](arkts-observed-and-objectlink.md)
      - Application State Management<!--arkts-application-state-management-->
        - [Application State Management Overview](arkts-application-state-management-overview.md)
        - [LocalStorage: Storing UI State](arkts-localstorage.md)
        - [AppStorage: Storing Application-wide UI State](arkts-appstorage.md)
        - [PersistentStorage: Persisting Application State](arkts-persiststorage.md)
        - [Environment: Device Environment Query](arkts-environment.md)
      - Other State Management Features<!--arkts-other-state-mgmt-functions-->
        - [Overview of Other State Management Features](arkts-other-state-mgmt-functions-overview.md)
        - [\@Watch Decorator: Getting Notified of State Variable Changes](arkts-watch.md)
        - [$$ Syntax: Two-Way Synchronization of Built-in Components](arkts-two-way-sync.md)
        - [\@Track Decorator: Class Object Property-level Update](arkts-track.md)
        - [Freezing a Custom Component](arkts-custom-components-freeze.md)
      - [MVVM](arkts-mvvm.md)
      - [Best Practices for State Management](arkts-state-management-best-practices.md)
      - [Proper Use of State Management](properly-use-state-management-to-develope.md)
    - State Management (V2)<!--arkts-state-management-v2-->
      - V2 Exclusive Decorators<!--arkts-v2-decorators-->
        - [\@ObservedV2 and \@Trace Decorators: Observing Class Property Changes](arkts-new-observedV2-and-trace.md)
        - [\@ComponentV2 Decorator: Custom Component](arkts-new-componentV2.md)
        - [\@Local Decorator: Representing the Internal State of Components](arkts-new-local.md)
        - [\@Param Decorator: Inputting External Parameters to Components](arkts-new-param.md)
        - [\@Once: Implementing Initialization Once](arkts-new-once.md)
        - [\@Event Decorator: Component Output](arkts-new-event.md)
        - [\@Provider and \@Consumer Decorators: Synchronizing Across Component Levels in a Two-Way Manner](arkts-new-Provider-and-Consumer.md)
        - [\@Monitor Decorator: Listening for Value Changes of the State Variables](arkts-new-monitor.md)
        - [\@Computed Decorator: Computed Property](arkts-new-Computed.md)
        - [\@Type Decorator: Marking the Types of the Class Property](arkts-new-type.md)
        - [\@ReusableV2 Decorator: Reusing Components](arkts-new-reusableV2.md)
      - Other State Management Features<!--arkts-new-other-state-mgmt-->
        - [AppStorageV2: Storing Application-wide UI State](arkts-new-appstoragev2.md)
        - [PersistenceV2: Persisting Application State](arkts-new-persistencev2.md)
        - [!! Syntax: Two-Way Binding](arkts-new-binding.md)
        - [Freezing a Custom Component](arkts-custom-components-freezeV2.md)
        - [Repeat: Reusable Repeated Rendering](arkts-new-rendering-control-repeat.md)
        - [getTarget API: Obtaining Original Objects](arkts-new-getTarget.md)
        - [makeObserved API: Changing Unobservable Data to Observable Data](arkts-new-makeObserved.md)
      - [MVVM (V2)](arkts-mvvm-V2.md)
    - Mixed Use and Migration Guide for V1 and V2<!--v1v2-migration-->
      - [Mixing Use of Custom Components](arkts-custom-component-mixed-scenarios.md)
      - [Mixing Use of State Management V1 and V2](arkts-v1-v2-mixusage.md)
      - [Migrating Applications from V1 to V2](arkts-v1-v2-migration.md)
  - Rendering Control<!--arkts-rendering-control-->
      - [Rendering Control Overview](arkts-rendering-control-overview.md)
      - [if/else: Conditional Rendering](arkts-rendering-control-ifelse.md)
      - [ForEach: Rendering Repeated Content](arkts-rendering-control-foreach.md)
      - [LazyForEach: Lazy Data Loading](arkts-rendering-control-lazyforeach.md)
      - [ContentSlot: Representing a Placeholder in Hybrid Development](arkts-rendering-control-contentslot.md)

