# @arkts.utils (ArkTS工具库)

本模块提供了各种ArkTS实用工具函数。

> **说明：**
>
> 本模块首批接口从API version 12开始支持。后续版本的新增接口，采用上角标单独标记接口的起始版本。
>
> 此模块仅支持在ArkTS文件（文件后缀为.ets）中导入使用。

## 导入模块

```ts
import { ArkTSUtils } from '@kit.ArkTS'
```

## ArkTSUtils.locks

为了解决多并发实例间的数据竞争问题，ArkTS语言基础库引入了异步锁能力。为了开发者的开发效率，AsyncLock对象支持跨并发实例引用传递。

由于ArkTS语言支持异步操作，阻塞锁容易产生死锁问题，因此我们在ArkTS中仅支持异步锁（非阻塞式锁）。

使用异步锁的方法需要标记为async，调用方需要await修饰调用，才能保证时序正确。因此会导致外层调用函数全部标记成async。

### AsyncLockCallback

type AsyncLockCallback\<T> = () => T | Promise\<T>

这是一个补充类型别名，表示[lockAsync](#lockasync)函数所有重载中的回调。

**原子化服务API**：从API version 12 开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

### AsyncLock

实现异步锁功能的类，允许在锁下执行异步操作。

#### 属性

**原子化服务API**：从API version 12 开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

| 名称 | 类型   | 可读 | 可写 | 说明       |
| ---- | ------ | ---- | ---- | ---------- |
| name | string | 是   | 否   | 锁的名称。 |

**示例：**

```ts
// 示例一：
@Sendable
class A {
  count_: number = 0;
  async getCount(): Promise<number> {
    let lock: ArkTSUtils.locks.AsyncLock = ArkTSUtils.locks.AsyncLock.request("lock_1");
    return lock.lockAsync(() => {
      return this.count_;
    })
  }
  async setCount(count: number) {
    let lock: ArkTSUtils.locks.AsyncLock = ArkTSUtils.locks.AsyncLock.request("lock_1");
    await lock.lockAsync(() => {
      this.count_ = count;
    })
  }
}

// 示例二：
@Sendable
class A {
  count_: number = 0;
  lock_: ArkTSUtils.locks.AsyncLock = new ArkTSUtils.locks.AsyncLock();
  async getCount(): Promise<number> {
    return this.lock_.lockAsync(() => {
      return this.count_;
    })
  }
  async setCount(count: number) {
    await this.lock_.lockAsync(() => {
      this.count_ = count;
    })
  }
}

@Concurrent
async function foo(a: A) {
  await a.setCount(10)
}
```

#### constructor

constructor()

默认构造函数。创建一个异步锁。

**原子化服务API**：从API version 12 开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**返回值：**

| 类型                    | 说明               |
| ----------------------- | ------------------ |
| [AsyncLock](#asynclock) | 创建的异步锁实例。 |

**示例：**

```ts
let lock = new ArkTSUtils.locks.AsyncLock();
```

#### request

static request(name: string): AsyncLock

使用指定的名称查找或创建（如果未找到）异步锁实例。

**原子化服务API**：从API version 12 开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 名称 | 类型   | 必填 | 说明                             |
| ---- | ------ | ---- | -------------------------------- |
| name | string | 是   | 按指定名称查找或创建异步锁实例。 |

**返回值：**

| 类型                    | 说明                             |
| ----------------------- | -------------------------------- |
| [AsyncLock](#asynclock) | 返回查找到或创建后的异步锁实例。 |

**示例：**

```ts
let lockName = 'isAvailableLock';
let lock = ArkTSUtils.locks.AsyncLock.request(lockName);
```

#### query

static query(name: string): AsyncLockState

查询指定异步锁的信息。

**原子化服务API**：从API version 12 开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 名称 | 类型   | 必填 | 说明       |
| ---- | ------ | ---- | ---------- |
| name | string | 是   | 锁的名称。 |

**返回值：**

| 类型                              | 说明                               |
| --------------------------------- | ---------------------------------- |
| [AsyncLockState](#asynclockstate) | 一个包含状态描述的异步锁状态实例。 |

**错误码：**

以下错误码的详细介绍请参见[语言基础类库错误码](errorcode-utils.md)。

| 错误码ID | 错误信息      |
| -------- | ------------- |
| 10200030 | The lock does not exist. |

**示例：**

```ts
// 你已经在别的地方创建了一个锁。
// let lock = ArkTSUtils.locks.AsyncLock.request("queryTestLock");
let state = ArkTSUtils.locks.AsyncLock.query('queryTestLock');
if (!state) {
    throw new Error('测试失败：期望有效的状态，但得到的是 ' + state);
}
let pending: ArkTSUtils.locks.AsyncLockInfo[] = state.pending;
let held: ArkTSUtils.locks.AsyncLockInfo[] = state.held;
```

#### queryAll

static queryAll(): AsyncLockState[]

查询所有现有锁的信息。

**原子化服务API**：从API version 12 开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**返回值：**

| 类型                                | 说明                             |
| ----------------------------------- | -------------------------------- |
| [AsyncLockState](#asynclockstate)[] | 包含锁状态信息的异步锁状态数组。 |

**示例：**

```ts
let states: ArkTSUtils.locks.AsyncLockState[] = ArkTSUtils.locks.AsyncLock.queryAll();
if (states.length == 0) {
    throw new Error('测试失败：期望至少有1个状态，但得到的是 ' + states.length);
}
```

#### lockAsync

lockAsync\<T>(callback: AsyncLockCallback\<T>): Promise\<T>

在获取的锁下独占执行操作。该方法首先获取锁，然后调用回调，最后释放锁。回调在调用[lockAsync](#lockasync)的同一线程中以异步方式执行。

**原子化服务API**：从API version 12 开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 名称     | 类型                                    | 必填 | 说明                   |
| -------- | --------------------------------------- | ---- | ---------------------- |
| callback | [AsyncLockCallback](#asynclockcallback) | 是   | 获取锁后要调用的函数。 |

**返回值：**

| 类型        | 说明                        |
| ----------- | --------------------------- |
| Promise\<T> | 回调执行后将解决的Promise。 |

**错误码：**

以下错误码的详细介绍请参见[语言基础类库错误码](errorcode-utils.md)。

| 错误码ID | 错误信息      |
| -------- | ------------- |
| 10200030 | The lock does not exist. |

**示例：**

```ts
let lock = new ArkTSUtils.locks.AsyncLock();
let p1 = lock.lockAsync<void>(() => {
    // 执行某些操作
});
```

#### lockAsync

lockAsync\<T>(callback: AsyncLockCallback\<T>, mode: AsyncLockMode): Promise\<T>

在获取的锁下执行操作。该方法首先获取锁，然后调用回调，最后释放锁。回调在调用[lockAsync](#lockasync)的同一线程中以异步方式执行。

**原子化服务API**：从API version 12 开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 名称     | 类型                                    | 必填 | 说明                   |
| -------- | --------------------------------------- | ---- | ---------------------- |
| callback | [AsyncLockCallback](#asynclockcallback) | 是   | 获取锁后要调用的函数。 |
| mode     | [AsyncLockMode](#asynclockmode)         | 是   | 锁的操作模式。         |

**返回值：**

| 类型        | 说明                        |
| ----------- | --------------------------- |
| Promise\<T> | 回调执行后将解决的Promise。 |

**错误码：**

以下错误码的详细介绍请参见[语言基础类库错误码](errorcode-utils.md)。

| 错误码ID | 错误信息      |
| -------- | ------------- |
| 10200030 | The lock does not exist. |

**示例：**

```ts
let lock = new ArkTSUtils.locks.AsyncLock();
let p1 = lock.lockAsync<void>(() => {
    // 执行某些操作
}, ArkTSUtils.locks.AsyncLockMode.EXCLUSIVE);
```

#### lockAsync

lockAsync\<T, U>(callback: AsyncLockCallback\<T>, mode: AsyncLockMode, options: AsyncLockOptions\<U>): Promise\<T | U>

在获取的锁下执行操作。该方法首先获取锁，然后调用回调，最后释放锁。回调在调用[lockAsync](#lockasync)的同一线程中以异步方式执行。在[AsyncLockOptions](#asynclockoptions)中可以提供一个可选的超时值。在这种情况下，如果超时前未能获取锁，lockAsync将拒绝返回的Promise并带上一个BusinessError实例。这种情况下，错误信息将包含持有的锁和等待的锁的信息以及可能的死锁警告。

**原子化服务API**：从API version 12 开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 名称     | 类型                                      | 必填 | 说明                   |
| -------- | ----------------------------------------- | ---- | ---------------------- |
| callback | [AsyncLockCallback](#asynclockcallback)   | 是   | 获取锁后要调用的函数。 |
| mode     | [AsyncLockMode](#asynclockmode)           | 是   | 锁的操作模式。         |
| options  | [AsyncLockOptions\<U>](#asynclockoptions) | 是   | 锁的操作选项。         |

**返回值：**

| 类型             | 说明                                               |
| ---------------- | -------------------------------------------------- |
| Promise\<T \| U> | 回调执行后解决的 Promise，或者在超时情况下被拒绝。 |

**错误码：**

以下错误码的详细介绍请参见[语言基础类库错误码](errorcode-utils.md)。

| 错误码ID | 错误信息          |
| -------- | ----------------- |
| 10200030 | The lock does not exist.     |
| 10200031 | Timeout exceeded. |

**示例：**

```ts
let lock = new ArkTSUtils.locks.AsyncLock();
let options = new ArkTSUtils.locks.AsyncLockOptions<void>();
options.timeout = 1000;
let p: Promise<void> = lock.lockAsync<void, void>(
    () => {
        // 执行某些操作
    },
    ArkTSUtils.locks.AsyncLockMode.EXCLUSIVE,
    options
);
```

### AsyncLockMode

锁操作对应的模式枚举。

**原子化服务API**：从API version 12 开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

| 名称      | 值  | 说明                                                     |
| --------- | --- | -------------------------------------------------------- |
| SHARED    | 1   | 共享锁模式。如果指定了此模式，可以在任意线程同时执行。   |
| EXCLUSIVE | 2   | 独占锁模式。如果指定了此模式，仅在独占获取锁时才能执行。 |

**示例：**

```ts
let lock = new ArkTSUtils.locks.AsyncLock();
// shared0可获取锁并开始执行
lock.lockAsync(async () => {
    console.log('shared0');
    await new Promise<void>((resolve) => setTimeout(resolve, 1000));
}, ArkTSUtils.locks.AsyncLockMode.SHARED);
// shared1可获取锁并开始执行，无需等待shared0
lock.lockAsync(async () => {
    console.log('shared1');
    await new Promise<void>((resolve) => setTimeout(resolve, 1000));
}, ArkTSUtils.locks.AsyncLockMode.SHARED);
// exclusive0需等待shared0、1执行完后才可获取锁并执行
lock.lockAsync(async () => {
    console.log('exclusive0');
    await new Promise<void>((resolve) => setTimeout(resolve, 1000));
}, ArkTSUtils.locks.AsyncLockMode.EXCLUSIVE);
// shared2需等待exclusive0执行完后才可获取锁并执行
lock.lockAsync(async () => {
    console.log('shared2');
    await new Promise<void>((resolve) => setTimeout(resolve, 1000));
}, ArkTSUtils.locks.AsyncLockMode.SHARED);
// shared3需等待exclusive0执行完后才可获取锁并执行，无需等待shared2
lock.lockAsync(async () => {
    console.log('shared3');
    await new Promise<void>((resolve) => setTimeout(resolve, 1000));
}, ArkTSUtils.locks.AsyncLockMode.SHARED);
```

### AsyncLockOptions

class AsyncLockOptions\<T>

表示锁操作选项的类。

**原子化服务API**：从API version 12 开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

#### constructor

constructor()

默认构造函数。创建一个所有属性均具有默认值的异步锁配置项实例。

**原子化服务API**：从API version 12 开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**返回值：**

| 类型                                  | 说明                   |
| ------------------------------------- | ---------------------- |
| [AsyncLockOptions](#asynclockoptions) | 新的异步锁配置项实例。 |

**示例：**

```ts
let s: ArkTSUtils.locks.AbortSignal<string> = { aborted: false, reason: 'Aborted' };
let options = new ArkTSUtils.locks.AsyncLockOptions<string>();
options.isAvailable = false;
options.signal = s;
```

#### 属性

| 名称        | 类型                                  | 可读 | 可写 | 说明                                                                                                                      |
| ----------- | ------------------------------------- | ---- | ---- | ------------------------------------------------------------------------------------------------------------------------- |
| isAvailable | boolean                               | 是   | 是   | 当前锁是否可用。取值为true，则只有在尚未持有锁定请求时才会授予该锁定请求；为false则表示将等待当前锁被释放。默认为 false。 |
| signal      | [AbortSignal\<T>](#abortsignal)\|null | 是   | 是   | 用于中止异步操作的对象。如果signal.aborted为true，则锁请求将被丢弃；为null则请求正常排队运行。默认为 null。               |
| timeout     | number                                | 是   | 是   | 锁操作的超时时间（毫秒）。如果该值大于零，且运行超过该时间，[lockAsync](#lockasync)将返回被拒绝的Promise。默认为 0。      |

### AsyncLockState

用于存储特定异步锁实例上当前执行的所有锁操作的信息的类。

**原子化服务API**：从API version 12 开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

#### 属性

| 名称    | 类型                              | 可读 | 可写 | 说明             |
| ------- | --------------------------------- | ---- | ---- | ---------------- |
| held    | [AsyncLockInfo[]](#asynclockinfo) | 是   | 是   | 持有的锁信息。   |
| pending | [AsyncLockInfo[]](#asynclockinfo) | 是   | 是   | 等待中的锁信息。 |

### AsyncLockInfo

关于锁的信息。

**原子化服务API**：从API version 12 开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

#### 属性

| 名称      | 类型                            | 可读 | 可写 | 说明                                                      |
| --------- | ------------------------------- | ---- | ---- | --------------------------------------------------------- |
| name      | string                          | 是   | 是   | 锁的名称。                                                |
| mode      | [AsyncLockMode](#asynclockmode) | 是   | 是   | 锁的模式。                                                |
| contextId | number                          | 是   | 是   | [AsyncLockMode](#asynclockmode)调用者的执行上下文标识符。 |

### AbortSignal

用于中止异步操作的对象。该类的实例必须在其创建的同一线程中访问。从其他线程访问此类的字段会导致未定义的行为。

**原子化服务API**：从API version 12 开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

#### 属性

| 名称    | 类型    | 可读 | 可写 | 说明                                                             |
| ------- | ------- | ---- | ---- | ---------------------------------------------------------------- |
| aborted | boolean | 是   | 是   | 设置为true以中止操作。                                           |
| reason  | \<T>    | 是   | 是   | 中止的原因。此值将用于拒绝[lockAsync](#lockasync)返回的Promise。 |

## ArkTSUtils.ASON

为支持将JSON字符串解析成共享数据，即[Sendable支持的数据类型](../../arkts-utils/arkts-sendable.md#sendable支持的数据类型)，ArkTS语言基础库新增了ASON工具。ASON支持开发者解析JSON字符串，并生成共享数据进行跨并发域传输，同时ASON也支持将共享数据转换成JSON字符串。

### ISendable

type ISendable = lang.ISendable

ISendable是所有Sendable类型（除`null`和`undefined`）的父类型。自身没有任何必须的方法和属性。

**原子化服务API：** 从API version 12开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

| 类型 | 说明   |
| ------ | ------ |
| [lang.ISendable](js-apis-arkts-lang.md#langisendable)   | 所有Sendable类型的父类型。 |

### Transformer

type Transformer = (this: ISendable, key: string, value: ISendable | undefined | null) => ISendable | undefined | null

用于转换结果函数的类型。

**原子化服务API：** 从API version 12开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 参数名 | 类型   | 必填 | 说明            |
| ------ | ------ | ---- | --------------- |
| this   | [ISendable](#isendable) | 是 | 在解析的键值对所属的对象。|
| key  | string | 是 | 属性名。|
| value  | [ISendable](#isendable) | 是 | 在解析的键值对的值。|

**返回值：**

| 类型 | 说明 |
| -------- | -------- |
| [ISendable](#isendable) \| undefined \| null | 返回转换结果后的ISendable对象或undefined或null。|

### BigIntMode

定义处理BigInt的模式。

**原子化服务API：** 从API version 12开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

| 名称 | 值| 说明            |
| ------ | ------ | --------------- |
| DEFAULT   | 0 |不支持BigInt。|
| PARSE_AS_BIGINT   | 1 |当整数小于-(2^53-1)或大于(2^53-1)时，解析为BigInt。|
| ALWAYS_PARSE_AS_BIGINT   | 2 |所有整数都解析为BigInt。|

### ParseReturnType

定义解析结果的返回类型。

**系统能力：** SystemCapability.Utils.Lang

| 名称 | 值| 说明            |
| ------ | ------ | --------------- |
| OBJECT   | 0 |返回 SendableObject 对象。<br>**原子化服务API：** 从API version 12开始，该接口支持在原子化服务中使用。|
| MAP<sup>13+</sup>   | 1 |返回 SendableMap 对象。<br>**原子化服务API：** 从API version 13开始，该接口支持在原子化服务中使用。|

### ParseOptions

解析的选项，可定义处理BigInt的模式与解析结果的返回类型。

**原子化服务API：** 从API version 12开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

| 名称 | 类型| 必填 | 说明            |
| ------ | ------ | ---- | --------------- |
| bigIntMode   | [BigIntMode](#bigintmode) | 是 |定义处理BigInt的模式。|
| parseReturnType   | [ParseReturnType](#parsereturntype) | 是 |定义解析结果的返回类型。|

### parse

parse(text: string, reviver?: Transformer, options?: ParseOptions): ISendable | null

用于解析JSON字符串生成ISendable数据或null。

**原子化服务API：** 从API version 12开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 参数名 | 类型   | 必填 | 说明            |
| ------ | ------ | ---- | --------------- |
| text   | string | 是 | 有效的JSON字符串。|
| reviver   | [Transformer](#transformer) | 否 | 转换函数，传入该参数，可以用来修改解析生成的原始值。默认值是undefined。目前只支持传入undefined。|
| options   | [ParseOptions](#parseoptions) | 否 | 解析的配置，传入该参数，可以用来控制解析生成的结果类型。默认值是undefined。|

**返回值：**

| 类型 | 说明 |
| -------- | -------- |
| [ISendable](#isendable) \| null | 返回ISendable数据或null。当入参是null时，返回null。|

**示例：**

```ts
import { lang } from '@kit.ArkTS';
import { collections } from '@kit.ArkTS';

type ISendable = lang.ISendable;
let jsonText = '{"name": "John", "age": 30, "city": "ChongQing"}';
let obj = ArkTSUtils.ASON.parse(jsonText) as ISendable;
console.info((obj as object)?.["name"]);
// 期望输出: 'John'
console.info((obj as object)?.["age"]);
// 期望输出: 30
console.info((obj as object)?.["city"]);
// 期望输出: 'ChongQing'

let options: ArkTSUtils.ASON.ParseOptions = {
  bigIntMode: ArkTSUtils.ASON.BigIntMode.PARSE_AS_BIGINT,
  parseReturnType: ArkTSUtils.ASON.ParseReturnType.OBJECT,
}
let numberText = '{"largeNumber":112233445566778899}';
let numberObj = ArkTSUtils.ASON.parse(numberText,undefined,options) as ISendable;

console.info((numberObj as object)?.["largeNumber"]);
// 期望输出: 112233445566778899

let options2: ArkTSUtils.ASON.ParseOptions = {
    bigIntMode: ArkTSUtils.ASON.BigIntMode.PARSE_AS_BIGINT,
    parseReturnType: ArkTSUtils.ASON.ParseReturnType.MAP,
  }
let mapText = '{"largeNumber":112233445566778899}';
let map  = ArkTSUtils.ASON.parse(mapText,undefined,options2);
console.info("map is " + map);
// 期望输出: map is [object SendableMap]
console.info("largeNumber is " + (map as collections.Map<string,bigint>).get("largeNumber"));
// 期望输出: largeNumber is 112233445566778899
```

### stringify

stringify(value: ISendable | null | undefined): string

该方法将ISendable数据转换为JSON字符串。

**原子化服务API：** 从API version 12开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 参数名 | 类型 | 必填 | 说明 |
| -------- | -------- | -------- | -------- |
| value | [ISendable](#isendable) \| null \| undefined  | 是 | ISendable数据。|

**返回值：**

| 类型 | 说明 |
| -------- | -------- |
| string | 转换后的JSON字符串。|

**示例：**

```ts
import { collections } from '@kit.ArkTS';

let arr = new collections.Array(1, 2, 3);
let str = ArkTSUtils.ASON.stringify(arr);
console.info(str);
// 期望输出: '[1,2,3]'
```

### stringify<sup>16+</sup>

stringify(value: Object | null | undefined): string

该方法将ArkTS对象数据转换为JSON字符串，额外支持Map和Set相关类型。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 参数名 | 类型 | 必填 | 说明 |
| -------- | -------- | -------- | -------- |
| value | Object \| null \| undefined  | 是 | ArkTS对象数据。|

**返回值：**

| 类型 | 说明 |
| -------- | -------- |
| string | 转换后的JSON字符串。|

**示例：**

```ts
import { ArkTSUtils, collections, HashMap, HashSet } from '@kit.ArkTS';

let hashMap = new HashMap<string,string>();
hashMap.set("ha","a");
hashMap.set("sh","b");
hashMap.set("map","c");
let str1 = ArkTSUtils.ASON.stringify(hashMap);
console.info(str1);
//期望输出：'{"sh":"b","ha":"a","map":"c"}'
let hashSet = new HashSet<string>();
hashSet.add("ha");
hashSet.add("sh");
hashSet.add("set");
let str2 = ArkTSUtils.ASON.stringify(hashSet);
console.info(str2);
//期望输出：'["set","sh","ha"]'
let map = new Map<string,string>();
map.set("m","a");
map.set("a","b");
map.set("p","c");
let str3 = ArkTSUtils.ASON.stringify(map);
console.info(str3);
//期望输出：'{"m":"a","a":"b","p":"c"}'
let set = new Set<string>();
set.add("s");
set.add("e");
set.add("t");
let str4 = ArkTSUtils.ASON.stringify(set);
console.info(str4);
//期望输出：'["s","e","t"]'
let sendableMap = new collections.Map<string,string>();
sendableMap.set("send","a");
sendableMap.set("able","b");
sendableMap.set("map","c");
let str5 = ArkTSUtils.ASON.stringify(sendableMap);
console.info(str5);
//期望输出：'{"send":"a","able":"b","map":"c"}'
let sendableSet = new collections.Set<string>();
sendableSet.add("send");
sendableSet.add("able");
sendableSet.add("set");
let str6 = ArkTSUtils.ASON.stringify(sendableSet);
console.info(str6);
//期望输出：'["send","able","set"]'
```

### isSendable

isSendable(value: Object | null | undefined): boolean

该方法用于判断value是否为Sendable数据类型。

**原子化服务API：** 从API version 12开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 参数名 | 类型 | 必填 | 说明 |
| -------- | -------- | -------- | -------- |
| value | Object \| null \| undefined  | 是 | 待校验的对象。|

**返回值：**

| 类型 | 说明 |
| -------- | -------- |
| boolean | value是否为Sendable数据类型，true表示value是Sendable数据类型，否则为false。|

**示例：**

```ts
import { ArkTSUtils } from '@kit.ArkTS'

@Sendable
function sendableFunc() {
  console.info("sendableFunc")
}

if (ArkTSUtils.isSendable(sendableFunc)) {
  console.info("sendableFunc is Sendable");
} else {
  console.info("sendableFunc is not Sendable");
}
// 期望输出: 'SendableFunc is Sendable'
```

## SendableLruCache<K, V><sup>16+</sup>

SendableLruCache用于在缓存空间不够的时候，将近期最少使用的数据替换为新数据。此设计基于资源访问的考虑：近期访问的数据，可能在不久的将来会再次访问。于是最少访问的数据就是价值最小的数据，是最应该踢出缓存空间的数据。 SendableLruCache支持Sendable特性，保存Sendable对象，可以跨线程安全访问。

### 属性

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

| 名称   | 类型   | 可读 | 可写 | 说明                   |
| ------ | ------ | ---- | ---- | ---------------------- |
| length | number | 是   | 否   | 当前缓冲区中值的总数。 |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
pro.put(2, 10);
pro.put(1, 8);
let result = pro.length;
console.info('result = ' + result);
// 期望输出：result = 2
```

### constructor<sup>16+</sup>

constructor(capacity?: number)

默认构造函数用于创建一个新的SendableLruCache实例，默认容量为64。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 参数名   | 类型   | 必填 | 说明                         |
| -------- | ------ | ---- | ---------------------------- |
| capacity | number | 否   | 指示要为缓冲区自定义的容量，不传默认值为64，最大值不能超过2147483647。 |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
```

### updateCapacity<sup>16+</sup>

updateCapacity(newCapacity: number): void

将缓冲区容量更新为指定容量。当缓冲区中值的总数大于指定容量时，会执行删除操作，删除最少使用的键值对。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 参数名      | 类型   | 必填 | 说明                         |
| ----------- | ------ | ---- | ---------------------------- |
| newCapacity | number | 是   | 指示要为缓冲区自定义的容量，最大值不能超过2147483647；小于等于0时会抛出异常。|

**错误码：**

以下错误码的详细介绍请参见[通用错误码](../errorcode-universal.md)。

| 错误码ID  | 错误信息                                    |
| -------- | ------------------------------------------|
| 401      | Parameter error. Possible causes: 1.Mandatory parameters are left unspecified;2.Incorrect parameter types. |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
pro.updateCapacity(100);
```

### toString<sup>16+</sup>

toString(): string

返回对象的字符串表示形式。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**返回值：**

| 类型   | 说明                       |
| ------ | -------------------------- |
| string | 返回对象的字符串表示形式。 返回字符串格式是：SendableLruCache[ maxSize = (maxSize), hits = (hitCount), misses = (missCount), hitRate = (hitRate) ]。 (maxSize)表示缓存区最大值，(hitCount)表示查询值匹配成功的次数，(missCount)表示查询值匹配失败的次数，(hitRate)表示查询值匹配率。|

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
pro.put(2, 10);
pro.get(2);
pro.get(3);
console.info(pro.toString());
// 预期输出：SendableLruCache[ maxSize = 64, hits = 1, misses = 1, hitRate = 50% ]
// maxSize: 缓存区最大值 hits: 查询值匹配成功的次数 misses: 查询值匹配失败的次数 hitRate: 查询值匹配率
```

### getCapacity<sup>16+</sup>

getCapacity(): number

获取当前缓冲区的容量。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**返回值：**

| 类型   | 说明                   |
| ------ | ---------------------- |
| number | 返回当前缓冲区的容量。 |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
let result = pro.getCapacity();
console.info('result = ' + result);
// 预期输出：result = 64
```

### clear<sup>16+</sup>

clear(): void

从当前缓冲区清除所有键值对。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
pro.put(2, 10);
let result = pro.length;
pro.clear();
let res = pro.length;
console.info('result = ' + result);
console.info('res = ' + res);
// 预期输出：result = 1
// 预期输出：res = 0
```

### getCreateCount<sup>16+</sup>

getCreateCount(): number

获取调用createDefault接口创建对象的次数。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**返回值：**

| 类型   | 说明                |
| ------ | -------------------|
| number | 返回使用createDefault接口创建对象的次数。 |

**示例：**

```ts
@Sendable
class ChildLRUCache extends ArkTSUtils.SendableLruCache<number, number> {
  constructor() {
    super();
  }
  createDefault(key: number): number {
    return key;
  }
}

let lru = new ChildLRUCache();
lru.put(2, 10);
lru.get(3);
lru.get(5);
let res = lru.getCreateCount();
console.info('res = ' + res);
// 预期输出：res = 2
// 执行get操作时，如果键值不存在，调用createDefault接口判断返回值是否等于undefined
// 如果不等于，则需要把key和返回值作为键值对添加到cache中，并且创建次数加1
```

### getMissCount<sup>16+</sup>

getMissCount(): number

获取查询值不匹配的次数。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**返回值：**

| 类型   | 说明                     |
| ------ | ------------------------ |
| number | 返回查询值不匹配的次数。 |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
pro.put(2, 10);
pro.get(2);
let result = pro.getMissCount();
console.info('result = ' + result);
// 预期输出：result = 0
```

### getRemoveCount<sup>16+</sup>

getRemoveCount(): number

获取缓冲区键值对回收的次数，当缓冲区数超过容量限制后，最少使用的键值对会被回收。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**返回值：**

| 类型   | 说明                       |
| ------ | -------------------------- |
| number | 返回缓冲区键值对回收的次数。 |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
pro.put(2, 10);
pro.updateCapacity(2);
pro.put(50, 22);
let result = pro.getRemoveCount();
console.info('result = ' + result);
// 预期输出：result = 0
```

### getMatchCount<sup>16+</sup>

getMatchCount(): number

获取查询值匹配成功的次数。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**返回值：**

| 类型   | 说明                       |
| ------ | -------------------------- |
| number | 返回查询值匹配成功的次数。 |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
pro.put(2, 10);
pro.get(2);
let result = pro.getMatchCount();
console.info('result = ' + result);
// 预期输出：result = 1
```

### getPutCount<sup>16+</sup>

getPutCount(): number

获取将值添加到缓冲区的次数。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**返回值：**

| 类型   | 说明                         |
| ------ | ---------------------------- |
| number | 返回将值添加到缓冲区的次数。 |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
pro.put(2, 10);
let result = pro.getPutCount();
console.info('result = ' + result);
// 预期输出：result = 1
```

### isEmpty<sup>16+</sup>

isEmpty(): boolean

检查当前缓冲区是否为空。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**返回值：**

| 类型    | 说明                                     |
| ------- | ---------------------------------------- |
| boolean | 返回true表示当前缓冲区为空不包含任何键值对；返回false表示当前缓冲区不为空。 |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
pro.put(2, 10);
let result = pro.isEmpty();
console.info('result = ' + result);
// 预期输出：result = false
```

### get<sup>16+</sup>

get(key: K): V | undefined

返回键对应的值。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 参数名 | 类型 | 必填 | 说明         |
| ------ | ---- | ---- | ------------ |
| key    | K    | 是   | 要查询的键。 |

**返回值：**

| 类型                     | 说明                                                         |
| ------------------------ | ------------------------------------------------------------ |
| V \| undefined | 如果指定的键存在于缓冲区中，则返回与键关联的值；否则调用createDefault接口，如果返回值为undefined，则返回undefined，否则返回createDefault接口结果。 |

**错误码：**

以下错误码的详细介绍请参见[通用错误码](../errorcode-universal.md)。

| 错误码ID | 错误信息 |
| -------- | -------- |
| 401 | Parameter error. Possible causes: 1. Mandatory parameters are left unspecified; 2. Incorrect parameter types. |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
pro.put(2, 10);
let result  = pro.get(2);
console.info('result = ' + result);
// 预期输出：result = 10
```

### put<sup>16+</sup>

put(key: K,value: V): V

将键值对添加到缓冲区中，返回与添加的键关联的值。当缓冲区中值的总数大于容量时，会执行删除操作。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 参数名 | 类型 | 必填 | 说明                       |
| ------ | ---- | ---- | -------------------------- |
| key    | K    | 是   | 要添加的键。             |
| value  | V    | 是   | 指示与要添加的键关联的值。 |

**返回值：**

| 类型 | 说明                                                         |
| ---- | ------------------------------------------------------------ |
| V    | 返回与添加的键关联的值。如果键或值为空，则抛出错误ID为401的参数错误异常。 |

**错误码：**

以下错误码的详细介绍请参见[通用错误码](../errorcode-universal.md)。

| 错误码ID | 错误信息 |
| -------- | -------- |
| 401 | Parameter error. Possible causes: 1. Mandatory parameters are left unspecified; 2. Incorrect parameter types. |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
let result = pro.put(2, 10);
console.info('result = ' + result);
// 预期输出：result = 10
```

### values<sup>16+</sup>

values(): V[]

获取当前缓冲区中所有值从最近访问到最少访问的顺序列表，最近访问表示有最新操作的键值对。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**返回值：**

| 类型      | 说明                                                         |
| --------- | ------------------------------------------------------------ |
| V&nbsp;[] | 按从最近访问到最少访问的顺序返回当前缓冲区中所有值的列表。 |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number|string,number|string>();
pro.put(2, 10);
pro.put(2, "anhu");
pro.put("afaf", "grfb");
let result = pro.values();
console.info('result = ' + result);
// 预期输出：result = anhu,grfb
```

### keys<sup>16+</sup>

keys(): K[]

获取当前缓冲区中所有键从最近访问到最近最少访问的升序列表。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**返回值：**

| 类型      | 说明                                                         |
| --------- | ------------------------------------------------------------ |
| K&nbsp;[] | 按升序返回当前缓冲区中所有键的列表，从最近访问到最近最少访问。 |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
pro.put(2, 10);
pro.put(3, 1);
let result = pro.keys();
console.info('result = ' + result);
// 预期输出：result = 2,3
```

### remove<sup>16+</sup>

remove(key: K): V | undefined

从当前缓冲区中删除指定的键及其关联的值，返回键关联的值。如果键不存在时，则返回undefined。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 参数名 | 类型 | 必填 | 说明           |
| ------ | ---- | ---- | -------------- |
| key    | K    | 是   | 要删除的键值。 |

**返回值：**

| 类型                     | 说明                                                         |
| ------------------------ | ------------------------------------------------------------ |
| V&nbsp;\|&nbsp;undefined | 返回一个包含已删除键值对的Optional对象；如果key不存在，则返回undefined，如果key为null，则抛出异常。 |

**错误码：**

以下错误码的详细介绍请参见[通用错误码](../errorcode-universal.md)。

| 错误码ID | 错误信息 |
| -------- | -------- |
| 401 | Parameter error. Possible causes: 1. Mandatory parameters are left unspecified; 2. Incorrect parameter types. |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
pro.put(2, 10);
let result = pro.remove(20);
console.info('result = ' + result);
// 预期输出：result = undefined
```

### contains<sup>16+</sup>

contains(key: K): boolean

检查当前缓冲区是否包含指定的键，假如存在，返回true，否则返回false。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**参数：**

| 参数名 | 类型   | 必填 | 说明             |
| ------ | ------ | ---- | ---------------- |
| key    | K      | 是   | 表示要检查的键。 |

**返回值：**

| 类型    | 说明                                       |
| ------- | ------------------------------------------ |
| boolean | 如果缓冲区包含指定的键，则返回true，否则返回false。 |

**错误码：**

以下错误码的详细介绍请参见[通用错误码](../errorcode-universal.md)。

| 错误码ID | 错误信息 |
| -------- | -------- |
| 401 | Parameter error. Possible causes: 1. Mandatory parameters are left unspecified; 2. Incorrect parameter types. |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
pro.put(2, 10);
let result = pro.contains(2);
console.info('result = ' + result);
// 预期输出：result = true
```

### entries<sup>16+</sup>

entries(): IterableIterator&lt;[K,V]&gt;

允许迭代包含在这个对象中的所有键值对。

**原子化服务API：** 从API version 16开始，该接口支持在原子化服务中使用。

**系统能力：** SystemCapability.Utils.Lang

**返回值：**

| 类型        | 说明                 |
| ----------- | -------------------- |
| IterableIterator<[K,&nbsp;V]> | 返回一个可迭代数组。 |

**示例：**

```ts
let pro = new ArkTSUtils.SendableLruCache<number, number>();
pro.put(2, 10);
pro.put(3, 15);
let pair:Iterable<Object[]> = pro.entries();
let arrayValue = Array.from(pair);
for (let value of arrayValue) {
  console.info(value[0]+ ', '+ value[1]);
  // 预期输出：
  // 2, 10
  // 3, 15
}
```