# Node-API

## 简介

Node-API是用于封装JavaScript能力为Native插件的API，独立于底层JavaScript，并作为Node.js的一部分。

## 支持的能力

Node-API可以去除底层的JavaScript引擎的差异，提供一套稳定的接口。

OpenHarmony的Node-API组件对Node-API的接口进行了重新实现，底层对接了ArkJS等引擎。当前支持Node-API标准库中的部分接口。

## 引入Node-API能力

如果开发者需要使用Node-API相关功能，首先请添加头文件：

```cpp
#include <napi/native_api.h>
```

其次在CMakeLists.txt中添加以下动态链接库：

```
libace_napi.z.so
```

## 已从Node-API组件标准库中导出的符号列表

从Node-API标准库导出的接口，其使用方法及行为基于[Node.js](https://nodejs.org/docs/latest-v8.x/api/n-api.html)，并进行了部分[能力拓展](#node-api组件扩展的符号列表)。

|符号类型|符号名|说明|起始支持API版本|
| --- | --- | --- | --- |
|FUNC|napi_module_register|napi native模块注册接口。|10|
|FUNC|napi_get_last_error_info|获取`napi_extended_error_info`结构体，其中包含最近一次出现的error信息。|10|
|FUNC|napi_throw|抛出一个js value。|10|
|FUNC|napi_throw_error|抛出一个带文本信息的js `Error`。|10|
|FUNC|napi_throw_type_error|抛出一个带文本信息的js `TypeError`。|10|
|FUNC|napi_throw_range_error|抛出一个带文本信息的js `RangeError`。|10|
|FUNC|napi_is_error|判断`napi_value`是否表示为一个error对象。|10|
|FUNC|napi_create_error|创建并获取一个带文本信息的js `Error`。|10|
|FUNC|napi_create_type_error|创建并获取一个带文本信息的js `TypeError`。|10|
|FUNC|napi_create_range_error|创建并获取一个带文本信息的js `RangeError`。|10|
|FUNC|napi_get_and_clear_last_exception|获取并清除最近一次出现的异常。|10|
|FUNC|napi_is_exception_pending|判断是否出现了异常。|10|
|FUNC|napi_fatal_error|引发致命错误以立即终止进程。|10|
|FUNC|napi_open_handle_scope|创建一个上下文环境使用。|10|
|FUNC|napi_close_handle_scope|关闭传入的上下文环境，关闭后，全部在其中声明的引用都将被关闭。|10|
|FUNC|napi_open_escapable_handle_scope|创建出一个可逃逸的handel scope，可将范围内声明的值返回到父作用域。|10|
|FUNC|napi_close_escapable_handle_scope|关闭传入的可逃逸的handel scope。|10|
|FUNC|napi_escape_handle|提升传入的js object的生命周期到其父作用域。|10|
|FUNC|napi_create_reference|为`Object`创建一个reference，以延长其生命周期。调用者需要自己管理reference生命周期。|10|
|FUNC|napi_delete_reference|删除传入的reference。|10|
|FUNC|napi_reference_ref|增加传入的reference的引用计数，并获取该计数。|10|
|FUNC|napi_reference_unref|减少传入的reference的引用计数，并获取该计数。|10|
|FUNC|napi_get_reference_value|获取与reference相关联的js `Object`。|10|
|FUNC|napi_create_array|创建并获取一个js `Array`。|10|
|FUNC|napi_create_array_with_length|创建并获取一个指定长度的js `Array`。|10|
|FUNC|napi_create_arraybuffer|创建并获取一个指定大小的js `ArrayBuffer`。|10|
|FUNC|napi_create_external|分配一个附加有外部数据的js value。|10|
|FUNC|napi_create_external_arraybuffer|分配一个附加有外部数据的js `ArrayBuffer`。|10|
|FUNC|napi_create_object|创建一个默认的js `Object`。|10|
|FUNC|napi_create_symbol|创建一个js `Symbol`。|10|
|FUNC|napi_create_typedarray|通过现有的`ArrayBuffer`创建一个js `TypeArray`。|10|
|FUNC|napi_create_dataview|通过现有的`ArrayBuffer`创建一个js `DataView`。|10|
|FUNC|napi_create_int32|通过一个C的`int32_t`数据创建js `Number`。|10|
|FUNC|napi_create_uint32|通过一个C的`uint32_t`数据创建js `Number`。|10|
|FUNC|napi_create_int64|通过一个C的`int64_t`数据创建js `Number`。|10|
|FUNC|napi_create_double|通过一个C的`double`数据创建js `Number`。|10|
|FUNC|napi_create_string_latin1|通过ISO-8859-1编码的C字符串数据创建js `String`。|10|
|FUNC|napi_create_string_utf8|通过UTF8编码的C字符串数据创建js `String`。|10|
|FUNC|napi_create_string_utf16|通过UTF16编码的C字符串数据创建js `String`。|10|
|FUNC|napi_get_array_length|获取array的length。|10|
|FUNC|napi_get_arraybuffer_info|获取`ArrayBuffer`的底层data buffer及其长度。|10|
|FUNC|napi_get_prototype|获取给定js `Object`的`prototype`。|10|
|FUNC|napi_get_typedarray_info|获取给定`TypedArray`的各种属性。|10|
|FUNC|napi_get_dataview_info|获取给定`DataView`的各种属性。|10|
|FUNC|napi_get_value_bool|获取给定js `Boolean`对应的C bool值。|10|
|FUNC|napi_get_value_double|获取给定js `Number`对应的C double值。|10|
|FUNC|napi_get_value_external|获取先前通过napi_create_external()传递的外部数据指针。|10|
|FUNC|napi_get_value_int32|获取给定js `Number`对应的C int32值。|10|
|FUNC|napi_get_value_int64|获取给定js `Number`对应的C int64值。|10|
|FUNC|napi_get_value_string_latin1|获取给定js vaule对应的ISO-8859-1编码的字符串。|10|
|FUNC|napi_get_value_string_utf8|获取给定js vaule对应的UTF8编码的字符串。|10|
|FUNC|napi_get_value_string_utf16|获取给定js vaule对应的UTF16编码的字符串。|10|
|FUNC|napi_get_value_uint32|获取给定js `Number`对应的C uint32值。|10|
|FUNC|napi_get_boolean|根据给定的C boolean值，获取js bool对象。|10|
|FUNC|napi_get_global|获取`global`对象。|10|
|FUNC|napi_get_null|获取`null`对象。|10|
|FUNC|napi_get_undefined|获取`undefined`对象。|10|
|FUNC|napi_coerce_to_bool|将给定的js value强转成js `Boolean`。|10|
|FUNC|napi_coerce_to_number|将给定的js value强转成js `Number`。|10|
|FUNC|napi_coerce_to_object|将给定的js value强转成js `Object`。|10|
|FUNC|napi_coerce_to_string|将给定的js value强转成js `String`。|10|
|FUNC|napi_typeof|获取给定js value的js type。|10|
|FUNC|napi_instanceof|判断给定object是否为给定constructor的实例。|10|
|FUNC|napi_is_array|判断给定js value是否为array。|10|
|FUNC|napi_is_arraybuffer|判断给定js value是否为`ArrayBuffer`。|10|
|FUNC|napi_is_typedarray|判断给定js value是否表示一个`TypedArray`。|10|
|FUNC|napi_is_dataview|判断给定js value是否表示一个`DataView`。|10|
|FUNC|napi_is_date|判断给定js value是否为js `Date`对象。|10|
|FUNC|napi_strict_equals|判断给定的两个js value是否严格相等。|10|
|FUNC|napi_get_property_names|以字符串数组的形式获取对象的可枚举属性的名称。|10|
|FUNC|napi_set_property|对给定`Object`设置属性。|10|
|FUNC|napi_get_property|获取给定`Object`的给定属性。|10|
|FUNC|napi_has_property|判断给定对象中是否存在给定属性。|10|
|FUNC|napi_delete_property|尝试从给定`Object`中删除给定`key`属性。|10|
|FUNC|napi_has_own_property|判断给定`Object`中是否有名为`key`的own property。|10|
|FUNC|napi_set_named_property|对给定`Object`设置一个给定名称的属性。|10|
|FUNC|napi_get_named_property|获取给定`Object`中指定名称的属性。|10|
|FUNC|napi_has_named_property|判断给定`Object`中是否有给定名称的属性。|10|
|FUNC|napi_set_element|在给定`Object`的指定索引处，设置元素。|10|
|FUNC|napi_get_element|获取给定`Object`指定索引处的元素。|10|
|FUNC|napi_has_element|若给定`Object`的指定索引处拥有属性，获取该元素。|10|
|FUNC|napi_delete_element|尝试删除给定`Object`的指定索引处的元素。|10|
|FUNC|napi_define_properties|批量的向给定`Object`中定义属性。|10|
|FUNC|napi_type_tag_object|将tag指针的值与`Object`关联。|10|
|FUNC|napi_check_object_type_tag|判断给定的tag指针是否被关联到了js `Object`上。|10|
|FUNC|napi_call_function|在Native方法中调用js function，即native call js。|10|
|FUNC|napi_create_function|创建native方法给js使用，以便于js call native。|10|
|FUNC|napi_get_cb_info|从给定的callback info中获取有关调用的详细信息，如参数和this指针。|10|
|FUNC|napi_get_new_target|获取构造函数调用的`new.target`。|10|
|FUNC|napi_new_instance|通过给定的构造函数，构建一个实例。|10|
|FUNC|napi_define_class|定义与C++类相对应的JavaScript类。|10|
|FUNC|napi_wrap|在js object上绑定一个native对象实例。|10|
|FUNC|napi_unwrap|从js object上获取先前绑定的native对象实例。|10|
|FUNC|napi_remove_wrap|从js object上获取先前绑定的native对象实例，并解除绑定。|10|
|FUNC|napi_create_async_work|创建一个异步工作对象。|10|
|FUNC|napi_delete_async_work|释放先前创建的异步工作对象。|10|
|FUNC|napi_queue_async_work|将异步工作对象加到队列，由底层去调度执行。|10|
|FUNC|napi_cancel_async_work|取消入队的异步任务。|10|
|FUNC|napi_async_init|创建一个异步资源上下文环境（不支持与async_hook相关能力）。|11|
|FUNC|napi_make_callback|在异步资源上下文环境中回调JS函数(不支持与async_hook相关能力)。|11|
|FUNC|napi_async_destroy|销毁先前创建的异步资源上下文环境（不支持与async_hook相关能力）。|11|
|FUNC|napi_open_callback_scope|创建一个回调作用域（不支持与async_hook相关能力）。|11|
|FUNC|napi_close_callback_scope|关闭先前创建的回调作用域（不支持与async_hook相关能力）。|11|
|FUNC|napi_get_node_version|获取node的版本信息。|10|
|FUNC|napi_get_version|获取Node运行时支持的最高 N-API 版本。|10|
|FUNC|napi_create_promise|创建一个延迟对象和js promise。|10|
|FUNC|napi_resolve_deferred|resolve与js promise对象关联的延迟函数。|10|
|FUNC|napi_reject_deferred|reject与js promise对象关联的延迟函数。|10|
|FUNC|napi_is_promise|判断给定js value是否为promise对象。|10|
|FUNC|napi_get_uv_event_loop|获取当前libuv loop实例。|10|
|FUNC|napi_create_threadsafe_function|创建线程安全函数。|10|
|FUNC|napi_get_threadsafe_function_context|获取线程安全函数中的context。|10|
|FUNC|napi_call_threadsafe_function|调用线程安全函数。|10|
|FUNC|napi_acquire_threadsafe_function|指示线程安全函数可以开始使用。|10|
|FUNC|napi_release_threadsafe_function|指示线程安全函数将停止使用。|10|
|FUNC|napi_ref_threadsafe_function|指示在主线程上运行的事件循环在线程安全函数被销毁之前不应退出。|10|
|FUNC|napi_unref_threadsafe_function|指示在主线程上运行的事件循环可能会在线程安全函数被销毁之前退出。|10|
|FUNC|napi_create_date|通过一个C的`double`数据创建js `Date`。|10|
|FUNC|napi_get_date_value|获取给定js `Date`对应的C double值。|10|
|FUNC|napi_create_bigint_int64|通过一个C的`int64`数据创建js `BigInt`。|10|
|FUNC|napi_create_bigint_uint64|通过一个C的`uint64`数据创建js `BigInt`。|10|
|FUNC|napi_create_bigint_words|通过一个C的`uint64`数组创建单个js `BigInt`。|10|
|FUNC|napi_get_value_bigint_int64|获取给定js `BigInt`对应的C int64值。|10|
|FUNC|napi_get_value_bigint_uint64|获取给定js `BigInt`对应的C uint64值。|10|
|FUNC|napi_get_value_bigint_words|获取给定js `BigInt`对应的信息，包括符号位、64位小端序数组和数组中的元素个数。|10|
|FUNC|napi_create_buffer|创建并获取一个指定大小的js `Buffer`。|10|
|FUNC|napi_create_buffer_copy|创建并获取一个指定大小的js `Buffer`，并以给定数据进行初始化。|10|
|FUNC|napi_create_external_buffer|创建并获取一个指定大小的js `Buffer`，并以给定数据进行初始化，该接口可为`Buffer`附带额外数据。|10|
|FUNC|napi_get_buffer_info|获取js `Buffer`底层data及其长度。|10|
|FUNC|napi_is_buffer|判断给定js value是否为`Buffer`对象。|10|
|FUNC|napi_object_freeze|冻结给定的对象。|10|
|FUNC|napi_object_seal|密封给定的对象。|10|
|FUNC|napi_get_all_property_names|获取一个数组，其中包含此对象过滤后的属性名称。|10|
|FUNC|napi_detach_arraybuffer|分离给定`ArrayBuffer`的底层数据。|10|
|FUNC|napi_is_detached_arraybuffer|判断给定的`ArrayBuffer`是否已被分离过。|10|
|FUNC|napi_run_script|将给定对象作为js代码运行。当前接口实际为空实现，可使用系统拓展接口`napi_run_script_path`接口，提升安全性。|10|
|FUNC|napi_set_instance_data|绑定与当前运行的环境相关联的数据项。|11|
|FUNC|napi_get_instance_data|检索与当前运行的环境相关联的数据项。|11|
|FUNC|napi_add_env_cleanup_hook|注册环境清理钩子函数。|11|
|FUNC|napi_remove_env_cleanup_hook|取消环境清理钩子函数。|11|
|FUNC|napi_add_async_cleanup_hook|注册清理异步钩子函数。|11|
|FUNC|napi_remove_async_cleanup_hook|取消清理异步钩子函数。|11|
|FUNC|node_api_get_module_file_name|用于获取加载项加载位置的绝对路径。|11|
|FUNC|napi_add_finalizer|当js `Object`中的对象被垃圾回收时调用注册的napi_finalize回调。|11|
|FUNC|napi_fatal_exception|向js抛出 `UncaughtException`。|12|

## 未从Node-API组件标准库中导出的符号列表

|符号类型|符号名|说明|
| --- | --- | --- |
|FUNC|napi_adjust_external_memory|调整js `Object`持有的外部内存。|

## Node-API组件扩展的符号列表

|符号类型|符号名|说明|起始支持API版本|
| --- | --- | --- | --- |
|FUNC|napi_queue_async_work_with_qos|将异步工作对象加到队列，由底层根据传入的qos优先级去调度执行。|10|
|FUNC|napi_run_script_path|运行abc文件。|10|
|FUNC|napi_load_module|将abc文件作为模块加载，返回模块的命名空间。|11|
|FUNC|napi_create_object_with_properties|使用给定的napi_property_descriptor创建js `Object`。descriptor的键名必须为 string，且不可转为number。|11|
|FUNC|napi_create_object_with_named_properties|使用给定的napi_value和键名创建js `Object`。键名必须为 string，且不可转为number。|11|
|FUNC|napi_coerce_to_native_binding_object|强制将js `Object`和Native对象绑定。|11|
|FUNC|napi_create_ark_runtime|创建基础运行时环境。|12|
|FUNC|napi_destroy_ark_runtime|销毁基础运行时环境。|12|
|FUNC|napi_run_event_loop|触发底层的事件循环。|12|
|FUNC|napi_stop_event_loop|停止底层的事件循环。|12|
|FUNC|napi_load_module_with_info|将abc文件作为模块加载，返回模块的命名空间。可在新创建的ArkTS基础运行时环境中使用。|12|
|FUNC|napi_serialize|将ArkTS对象转换为native数据。|12|
|FUNC|napi_deserialize|将native数据转为ArkTS对象。|12|
|FUNC|napi_delete_serialization_data|删除序列化数据。|12|
|FUNC|napi_call_threadsafe_function_with_priority|将指定优先级和入队方式的任务投递到ArkTS主线程。|12|
|FUNC|napi_is_sendable|判断给定JS value是否是Sendable的。|12|
|FUNC|napi_define_sendable_class|创建一个sendable类。|12|
|FUNC|napi_create_sendable_object_with_properties | 使用给定的napi_property_descriptor创建一个sendable对象。|12|
|FUNC|napi_create_sendable_array | 创建一个sendable数组。|12|
|FUNC|napi_create_sendable_array_with_length | 创建一个指定长度的sendable数组。|12|
|FUNC|napi_create_sendable_arraybuffer | 创建一个sendable ArrayBuffer。|12|
|FUNC|napi_create_sendable_typedarray | 创建一个sendable TypedArray。|12|
|FUNC|napi_wrap_sendable | 包裹一个native实例到ArkTS对象中。|12|
|FUNC|napi_wrap_sendable_with_size | 包裹一个native实例到ArkTS对象中并指定大小。|12|
|FUNC|napi_unwrap_sendable | 获取ArkTS对象包裹的native实例。|12|
|FUNC|napi_remove_wrap_sendable | 移除并获取ArkTS对象包裹的native实例。|12|

### napi_qos_t

```cpp
typedef enum {
    napi_qos_background = 0,      // 低等级，用户不可见任务，例如数据同步、备份。
    napi_qos_utility = 1,         // 中低等级，不需要立即看到响应效果的任务，例如下载或导入数据。
    napi_qos_default = 2,         // 默认
    napi_qos_user_initiated = 3,  // 高等级，用户触发并且可见进展，例如打开文档。
} napi_qos_t;
```

**描述：**
表示QoS的枚举值，QoS决定了线程调度的优先级

### napi_event_mode

```cpp
typedef enum {
    napi_event_mode_default = 0,  // 阻塞式的运行底层事件循环，直到循环中没有任何任务时退出事件循环。
    napi_event_mode_nowait = 1,   // 非阻塞式的运行底层事件循环，尝试去处理一个任务，处理完之后退出事件循环；如果事件循环中没有任务，立刻退出事件循环。
} napi_event_mode;
```

**描述：**
用于运行事件循环的事件模式。

### napi_queue_async_work_with_qos

```cpp
napi_status napi_queue_async_work_with_qos(napi_env env,
                                           napi_async_work work,
                                           napi_qos_t qos);
```

**描述：**

将异步工作对象加到队列，由底层根据传入的qos优先级去调度执行。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] work: 一个表示异步工作项的对象。这个对象通常是通过napi_create_async_work函数创建的。

- [in] qos: 决定了线程调度的优先级。

**返回：**

如果API成功，则返回napi_ok。

### napi_run_script_path

```cpp
napi_status napi_run_script_path(napi_env env,
                                 const char* abcPath,
                                 napi_value* result);
```

**描述：**

运行指定abc文件。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] abcPath: 要运行的脚本的JavaScript路径。这是一个字符串，指定了要运行的脚本文件的位置。

- [out] result: 一个指向napi_value类型的指针，用于存储运行脚本的结果。

**返回：**

如果API成功，则返回napi_ok。

### napi_load_module

```cpp
napi_status napi_load_module(napi_env env,
                             const char* path,
                             napi_value* result);
```

**描述：**

加载系统模块或开发者自定义的模块，返回模块的命名空间。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] path: 要加载的系统模块的名称或开发者自定义模块的路径。

- [out] result: 一个指向napi_value类型的指针，用于存储加载模块的结果。

**返回：**

如果API成功，则返回napi_ok。

### napi_create_object_with_properties

```cpp
napi_status napi_create_object_with_properties(napi_env env,
                                               napi_value* result,
                                               size_t property_count,
                                               const napi_property_descriptor* properties);
```

**描述：**

属性描述符napi_property_descriptor用于描述一个属性，它包括属性的名称获取和设置方法、属性特性等信息。通过传入这些描述符，可以在创建对象时就定义属性。

使用给定的napi_property_descriptor创建js Object。descriptor的键名必须为string，且不可转为number。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [out] result: 一个指向napi_value类型的指针，用于存储创建的对象。

- [in] property_count: 要添加到对象中的属性的数量。

- [in] properties: 一个指向napi_property_descriptor数组的指针，描述了要添加到对象中的属性的信息。

**返回：**

如果API成功，则返回napi_ok。

### napi_create_object_with_named_properties

```cpp
napi_status napi_create_object_with_named_properties(napi_env env,
                                                     napi_value* result,
                                                     size_t property_count,
                                                     const char** keys,
                                                     const napi_value* values);
```

**描述：**

使用给定的napi_value和键名创建js Object。键名必须为string，且不可转为number。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [out] result: 一个指向napi_value类型的指针，用于存储创建的对象。

- [in] property_count: 要添加到对象中的属性的数量。

- [in] keys: 一个指向const char*数组的指针，表示属性的名称。

- [in] values: 一个指向napi_value数组的指针，表示属性的值，与属性名称一一对应。

**返回：**

如果API成功，则返回napi_ok。

### napi_coerce_to_native_binding_object

```cpp
napi_status napi_coerce_to_native_binding_object(napi_env env,
                                                 napi_value js_object,
                                                 napi_native_binding_detach_callback detach_cb,
                                                 napi_native_binding_attach_callback attach_cb,
                                                 void* native_object,
                                                 void* hint);
```

**描述：**

用于给JS Object绑定回调和回调所需的参数，转成携带Native信息的JS Object。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] js_object: 要转换的JavaScript对象。

- [in] detach_cb: 解绑回调，一般在序列化时调用，可在对象解绑时执行一些清理操作。

- [in] attach_cb: 绑定回调，一般在序列化时调用。

- [in] native_object: 需要传递给回调的参数，不能为空。

- [in] hint: 一个指针，可以用于传递附加的信息给回调函数。

**返回：**

如果API成功，则返回napi_ok。

### napi_create_ark_runtime

```cpp
napi_status napi_create_ark_runtime(napi_env *env)
```

**描述：**

创建基础运行时环境。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

**返回：**

如果API成功，则返回napi_ok。

### napi_destroy_ark_runtime

```cpp
napi_status napi_destroy_ark_runtime(napi_env *env)
```

**描述：**

销毁基础运行时环境。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

**返回：**

如果API成功，则返回napi_ok。

### napi_run_event_loop

```cpp
napi_status napi_run_event_loop(napi_env env, napi_event_mode mode)
```

**描述：**

触发底层的事件循环。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。
- [in] mode: 用于运行事件循环的事件模式。

**返回：**

如果API成功，则返回napi_ok。

### napi_stop_event_loop

```cpp
napi_status napi_stop_event_loop(napi_env env)
```

**描述：**

停止底层的事件循环。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

**返回：**

如果API成功，则返回napi_ok。

### napi_load_module_with_info

```cpp
napi_status napi_load_module_with_info(napi_env env,
                                       const char* path,
                                       const char* module_info,
                                       napi_value* result)
```

**描述：**

将abc文件作为模块加载，返回模块的命名空间。可在新创建的ArkTS基础运行时环境中使用。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] path: 要加载的模块的路径。

- [in] module_info: 模块信息。这是一个包含模块信息字符串。模块信息可以用于指定模块的版本、作者、描述等详细信息。

- [out] result: 指向napi_value的指针，用于接收模块的结果。

**返回：**

如果API成功，则返回napi_ok。

### napi_serialize

```cpp
napi_status napi_serialize(napi_env env,
                           napi_value object,
                           napi_value transfer_list,
                           napi_value clone_list,
                           void** result)
```

**描述：**

将ArkTS对象转换为native数据。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] object: 要序列化的JavaScript对象。

- [in] transfer_list: 传输列表，包含需要在序列化过程中转移的JavaScript对象。

- [in] clone_list: 克隆列表，包含需要在序列化过程中克隆的JavaScript对象。

- [out] result: 用于接收序列化结果的指针。在调用完成后，指向实际结果的指针会存储在此位置。

**返回：**

如果API成功，则返回napi_ok。

### napi_deserialize

```cpp
napi_status napi_deserialize(napi_env env, void* buffer, napi_value* object)
```

**描述：**

将native数据转为ArkTS对象。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] buffer: 指向包含二进制数据的指针。这些二进制数据需要被反序列化为JavaScript对象。

- [out] object: 用于接收反序列化后的JavaScript对象。

**返回：**

如果API成功，则返回napi_ok。

### napi_delete_serialization_data

```cpp
napi_status napi_delete_serialization_data(napi_env env, void* buffer)
```

**描述：**

删除序列化数据。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] buffer: 指向包含序列化数据的内存缓冲区的指针。这些数据在序列化完成后不再需要，并且可以通过调用此函数来释放相应的内存。

**返回：**

如果API成功，则返回napi_ok。

### napi_call_threadsafe_function_with_priority

```cpp
napi_status napi_call_threadsafe_function_with_priority(napi_threadsafe_function func,
                                                        void *data,
                                                        napi_task_priority priority,
                                                        bool isTail)
```

**描述：**

将指定优先级和入队方式的任务投递到ArkTS主线程。

**参数：**

- [in] func: 线程安全函数对象，在创建线程安全函数时返回。

- [in] data: 传递给 JavaScript 回调函数的参数数据。

- [in] priority: 指定调用 JavaScript 回调函数的任务优先级。

- [in] isTail: 一个布尔值，指示调用是否应该排队等待在事件循环的尾部执行。如果为 true，则调用将在事件循环的尾部执行；如果为 false，则调用将立即执行，不会排队等待。

**返回：**

如果API成功，则返回napi_ok。

### napi_is_sendable

```cpp
napi_status napi_is_sendable(napi_env env, napi_value value, bool* result)
```

**描述：**

判断给定JS value是否是Sendable的。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] value: 一个napi_value类型的参数，是需要判断的值。

- [out] result: 一个bool类型的指针，用于存储判断结果。

**返回：**

如果API成功，则返回napi_ok。


### napi_define_sendable_class

```cpp
napi_status napi_define_sendable_class(napi_env env,
                                       const char* utf8name,
                                       size_t length,
                                       napi_callback constructor,
                                       void* data,
                                       size_t property_count,
                                       const napi_property_descriptor* properties,
                                       napi_value parent,
                                       napi_value* result)


```

**描述：**

创建一个sendable类。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] utf8name：一个const char*类型的参数，表示类的名称。

- [in] length：一个size_t类型的参数，表示类名称的字节数。

- [in] constructor：一个napi_callback类型的参数，表示类的构造函数。

- [in] data：[可选]一个void*类型的参数，表示构造函数的附加数据。

- [in] property_count：一个size_t类型的参数，表示类的属性数量。

- [in] properties：[可选]一个const napi_property_descriptor*类型的参数，表示类的属性描述符数组。

- [in] parent：[可选]一个napi_value类型的参数，表示父类。

- [out] result：一个napi_value类型的指针，用于存储创建的对象。

**返回：**

如果API成功，则返回napi_ok。

### napi_create_sendable_object_with_properties

```cpp
napi_status napi_create_sendable_object_with_properties(napi_env env,
                                                        size_t property_count,
                                                        const napi_property_descriptor* properties,
                                                        napi_value* result)
```

**描述：**

使用给定的napi_property_descriptor创建一个sendable对象。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] property_count：一个size_t类型的参数，表示属性数量。

- [in] properties：一个const napi_property_descriptor*类型的参数，表示属性描述符数组。

- [out] result：一个napi_value类型的指针，用于存储创建的对象。

**返回：**

如果API成功，则返回napi_ok。

### napi_create_sendable_array

```cpp
napi_status napi_create_sendable_array(napi_env env, napi_value* result)
```

**描述：**

创建一个sendable数组。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [out] result：一个napi_value类型的指针，用于存储创建的数组。

**返回：**

如果API成功，则返回napi_ok。

### napi_create_sendable_array_with_length

```cpp
napi_status napi_create_sendable_array_with_length(napi_env env, size_t length, napi_value* result)
```

**描述：**

创建一个指定长度的sendable数组。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] length：一个size_t类型的参数，表示数组的长度。

- [out] result：一个napi_value类型的指针，用于存储创建的数组。

**返回：**

如果API成功，则返回napi_ok。

### napi_create_sendable_arraybuffer

```cpp
napi_status napi_create_sendable_arraybuffer(napi_env env, size_t byte_length, void** data, napi_value* result)
```

**描述：**

创建一个sendable ArrayBuffer。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] byte_length：要创建的ArrayBuffer的大小。

- [in] data：指向底层字节缓冲区的指针。

- [out] result：一个napi_value类型的指针，用于存储创建的ArrayBuffer。

**返回：**

如果API成功，则返回napi_ok。

### napi_create_sendable_typedarray

```cpp
napi_status napi_create_sendable_typedarray(napi_env env,
                                            napi_typedarray_type type,
                                            size_t length,
                                            napi_value arraybuffer,
                                            size_t byte_offset,
                                            napi_value* result);
```

**描述：**

创建一个sendable TypedArray。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] type：TypedArray 的类型。

- [in] length：TypedArray 的长度。

- [in] arraybuffer：一个 ArrayBuffer 实例。

- [in] byte_offset：ArrayBuffer 的偏移量。

- [out] result：一个napi_value类型的指针，用于存储创建的TypedArray。

**返回：**

如果API成功，则返回napi_ok。

### napi_wrap_sendable

```cpp
napi_status napi_wrap_sendable(napi_env env,
                               napi_value js_object,
                               void* native_object,
                               napi_finalize finalize_cb,
                               void* finalize_hint);
```

**描述：**

包裹一个native实例到ArkTS对象中。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] js_object：ArkTS对象。

- [in] native_object：将被包裹在ArkTS对象中的native实例。

- [in] napi_finalize：[可选]ArkTS对象被销毁时调用的回调函数。

- [in] finalize_hint：[可选]上下文提示，会传递给回调函数。

**返回：**

如果API成功，则返回napi_ok。

### napi_wrap_sendable_with_size

```cpp
napi_status napi_wrap_sendable_with_size(napi_env env,
                                         napi_value js_object,
                                         void* native_object,
                                         napi_finalize finalize_cb,
                                         void* finalize_hint,
                                         size_t native_binding_size);
```

**描述：**

包装一个native实例到ArkTS对象中并指定大小。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] js_object：ArkTS对象。

- [in] native_object：将被包裹在ArkTS对象中的native实例。

- [in] napi_finalize：[可选]ArkTS对象被销毁时调用的回调函数。

- [in] finalize_hint：[可选]上下文提示，会传递给回调函数。

- [in] native_binding_size：[可选]绑定的native实例的大小。

**返回：**

如果API成功，则返回napi_ok。

### napi_unwrap_sendable

```cpp
napi_status napi_unwrap_sendable(napi_env env, napi_value js_object, void** result)
```

**描述：**

获取ArkTS对象包装的native实例。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] js_object：ArkTS对象。

- [out] result：包裹在ArkTS对象中的native实例。

**返回：**

如果API成功，则返回napi_ok。

### napi_remove_wrap_sendable

```cpp
napi_status napi_remove_wrap_sendable(napi_env env, napi_value js_object, void** result)
```

**描述：**

移除并获取ArkTS对象包装的native实例。

**参数：**

- [in] env: Node-API的环境对象，表示当前的执行环境。

- [in] js_object：ArkTS对象。

- [out] result：包裹在ArkTS对象中的native实例。

**返回：**

如果API成功，则返回napi_ok。

<!--no_check-->