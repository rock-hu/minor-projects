import { webview } from '@kit.ArkWeb';

@Component
struct Index {
  @State message: string = 'Hello World';

  // [Start test_code_build]
  build() {
    StackA() {
      ComponentB() {
        ComponentC()
      }

      ComponentD() {
        ComponentE()
      }
    }
  }
  // [End test_code_build]
}

@Component
struct StackA {
  controller: webview.WebviewController = new webview.WebviewController();

  build() {
    // [Start web_nested_scroll]
    Web(
      // [StartExclude web_nested_scroll]
      { src: 'https://developer.huawei.com/consumer/cn/', controller: this.controller }
      // [EndExclude web_nested_scroll]
    )
      .nestedScroll({
        scrollForward: NestedScrollMode.PARENT_FIRST,
        scrollBackward: NestedScrollMode.SELF_FIRST
      })
    // [End web_nested_scroll]
  }
}

@Component
struct ComponentB {
  build() {

  }
}

@Component
struct ComponentC {
  build() {

  }
}

@Component
struct ComponentD {
  build() {

  }
}

@Component
struct ComponentE {
  build() {

  }
}