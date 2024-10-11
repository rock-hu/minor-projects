# \@Type装饰器：标记类属性的类型

为了实现序列化类时不丢失属性的复杂类型，开发者可以使用\@Type装饰器装饰类属性。

>**说明：**
>
>\@Type从API version 12开始支持。
>
>当前状态管理（V2试用版）仍在逐步开发中，相关功能尚未成熟，建议开发者尝鲜试用。


## 概述

\@Type标记类属性，使得类属性序列化时不丢失类型信息，便于类的反序列化。


## 装饰器说明

| \@Type装饰器 | 说明 |
| ------------------- | ------------------------------------------------------------ |
| 装饰器参数 | type：类型。 |
| 可装饰的类型 | Object class以及Array、Date、Map、Set等内嵌类型。 |


## 使用限制

1、只能用在class内中；

2、不支持collections.Set、collections.Map等类型；

3、不支持非buildin类型，如PixelMap、NativePointer、ArrayList等Native类型；

4、不支持简单类型，如string、number、boolean等。

## 使用场景

### 持久化数据

数据页面
```ts
import { Type } from '@kit.ArkUI';

// 数据中心
@ObservedV2
class SampleChild {
  @Trace p1: number = 0;
  p2: number = 10;
}

@ObservedV2
export class Sample {
  // 对于复杂对象需要@Type修饰，确保序列化成功
  @Type(SampleChild)
  @Trace f: SampleChild = new SampleChild();
}
```

页面
```ts
import { PersistenceV2 } from '@kit.ArkUI';
import { Sample } from '../Sample';

@Entry
@ComponentV2
struct Page {
  prop: Sample = PersistenceV2.connect(Sample, () => new Sample())!;

  build() {
    Column() {
      Text(`Page1 add 1 to prop.p1: ${this.prop.f.p1}`)
        .fontSize(30)
        .onClick(() => {
          this.prop.f.p1++;
        })
    }
  }
}
```
