# C-API组件混合方案的使用

> 需要注意的是，在C-API中可以使用的ArkTS组件，当前仅支持叶子节点，如`Image`，不支持容器节点，如`Stack`。

## C-API中渲染ArkTS组件

1. 实现自定义组件，并在业务代码中使用。

2. 创建一个自定义的构建函数，使用`@Builder`进行装饰。在构建函数中设计工厂方法，用于根据`ComponentName`构建对应的组件。参数是`ComponentBuilderContext`，记录了组件构建时需要用到的信息，如`ComponentName`、`Tag`等。需要注意的是，工厂方法需要被一个`Stack`组件包裹，该`Stack`组件需要设置`position`属性为`(0,0)`，用于保证组件显示的位置正确。

    ```TypeScript
    @Builder
    export function buildCustomComponent(ctx: ComponentBuilderContext) {
    Stack() {
      if (ctx.componentName === MarqueeView.NAME) {
        MarqueeView({
            ctx: ctx.rnComponentContext,
            tag: ctx.tag
        })
      }
    }
    .position({ x: 0, y: 0 })
    }
    ```

3. 封装构建函数为`wrapBuilder`，并作为参数传递给`RNApp`或是`RNComponentContext`，`RNComponentContext`是`RNSurface`的必要参数。

    ```TypeScript
    const wrappedCustomRNComponentBuilder = wrapBuilder(buildCustomRNComponent);
    ···
    // 作为RNApp的参数
    RNApp({
      ···
      wrappedCustomRNComponentBuilder: wrappedCustomRNComponentBuilder,
      ···
    })
    
    // 作为RNComponentContext的参数
    new RNComponentContext(
      RNOHContext.fromCoreContext(rnohCoreContext!, rnInstance),
      wrappedCustomRNComponentBuilder,
      wrapBuilder(buildRNComponentForTag),
      new Map(),
    )
    ```

4. 在创建`RNApp`或`RNInstance`的时候，将组件的name放到数组中，并作为`arkTsComponentNames`的参数传入：
   
   ```TypeScript
   const arkTsComponentNames = [MarqueeView.NAME];

   // 作为RNApp的参数
   RNApp({
     rnInstanceConfig: {
       ···
       arkTsComponentNames: arkTsComponentNames
     },
     ···
   })

   // 创建RNInstance时传入
   await rnohCoreContext.createAndRegisterRNInstance({
     ···
     arkTsComponentNames: arkTsComponentNames
   });
   ```

5. 使用`RNApp`或者`RNSurface`启动React Native for OpenHarmony。

## C-API组件中使用ArkTS组件

当前RNOH框架支持C-API组件挂载一个ArkTs组件

1. 通过`createArkTSComponent`创建ArkTs组件。

   `createArkTSComponent`接口需要传递三个参数：

   - `tag`： 组件的tag
   - `componentHandle`：component句柄（`facebook::react::ComponentHandle`）
   - `componentName`：组件名，需要与入口文件的`buildCustomRNComponent`函数中定义的自定义ArkTS组件名相对应

   ```cpp
   auto componentInstanceFactory = rnInstance->getComponentInstanceFactory();
   auto newChildComponentInstance = componentInstanceFactory->createArkTSComponent(tag, componentHandle, componentName);
   ```

2. 然后调用组件的`onChildInserted`方法插入。

   ```cpp
   if (newChildComponentInstance != nullptr) {
     this->onChildInserted(newChildComponentInstance, index);
   }
   ```
