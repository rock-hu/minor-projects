# ArkTS与CPP之间相互通信

## ArkTS侧给CPP侧发消息

### ArkTS侧发送message
在ArkTS侧，可以通过获取到`rnInstance`实例，并调用`rnInstance.postMessageToCpp(name: string, payload: any)`向CPP侧发送消息。该方法存在两个参数：
- name：message的名字，用于在广播的时候区分不同的message
- payload：需要发送的message内容
```TypeScript
this.ctx.rnInstance.postMessageToCpp("SAMPLE_MESSAGE", payload);
```

当需要从ArkTS侧发送消息到`Component`或`TurboModule`的时候，可以使用下面的方法将消息发送到CPP侧：

### CPP侧接收message

- 方式一：使用onMessageReceived接收消息

    1. 组件的`ComponentInstance`需要继承`ArkTSMessageHub::Observer`用于接收消息：
        ```CPP
        class xxxComponentInstance
        : public CppComponentInstance<facebook::react::xxxShadowNode>,
          public ArkTSMessageHub::Observer
        ```
    2. 在构造函数中构造`ArkTSMessageHub::Observer`：
        ```CPP
        xxxComponentInstance::xxxComponentInstance(Context context)
            : CppComponentInstance(std::move(context)),
              ArkTSMessageHub::Observer(m_deps->arkTSMessageHub) {
                ···
        }
        ```
    3. 在`ComponentInstance`中实现`onMessageReceived`方法，用于接收消息，接收到消息后，根据消息名称，确认是否需要进行处理：
        ```CPP
        void xxxViewComponentInstance::onMessageReceived(
            ArkTSMessage const& message) {
          if (message.name == "SAMPLE_MESSAGE") {
            ···
          }
        }
        ```
- 方式二：使用handleArkTSMessage接收消息

  1. 在`Package`中创建并实现`ArkTSMessageHandler`类，并实现`handleArkTSMessage`方法，方法中实现对`message`的处理：
      ```CPP
      class SampleArkTSMessageHandler : public ArkTSMessageHandler {
        void handleArkTSMessage(const Context& ctx) override {
          ···
        };
      };
      ```
  2. 重写`Package`对象的`createArkTSMessageHandlers`方法，返回`ArkTSMessageHandler`类：
      ```CPP
      std::vector<ArkTSMessageHandler::Shared> SamplePackage::createArkTSMessageHandlers() {
        return {std::make_shared<SampleArkTSMessageHandler>()};
      }
      ```

## CPP侧给ArkTS侧发消息

### 使用postMessageToArkTS发送消息

1. CPP侧发送消息  
在CPP侧通过`rnInstance`对象上得方法`postMessageToArkTS`发送事件到ArkTS侧。该方法有两个参数：
    - name：string，是发送的message的名字
    - payload：folly::dynamic，是发送的信息数据
    ```CPP
    m_deps->rnInstance.lock()->postMessageToArkTS(messageName, messagePayload);
    ```

2. ArkTS侧接收消息  
在ArkTS侧可以通过`rnInstance`上得`cppEventEmitter`订阅CPP侧传过来的消息，并进行处理：
    ```TypeScript
    const unsubscribe = rnInstance.cppEventEmitter.subscribe("SAMPLE_MESSAGE", (value: object) => {
      ···
      unsubscribe();
    })
    ```

### 借用TurboModule发消息（不推荐使用）

1. 在ArkTS侧创建`TurboModule`，并实现对应的接口。
2. CPP侧获取`TurboModule`对象：
在CPP侧可以通过`rnInstance`对象上得方法`getTurboModule`获取到`TurboModule`对象，并转换为`ArkTSTurboModule`：
    ```CPP
    auto TurboModule = m_deps->rnInstance.lock()->getTurboModule<xxxTurboModule>("moduleName");
    auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::ArkTSTurboModule>(turboModule);
    ```
3. 处理参数，并调用ArkTS侧的对应方法：
    ```CPP
    folly::dynamic payload = folly::dynamic::object;
    arkTsTurboMOudle->callSync(functionName, payload);
    ```
