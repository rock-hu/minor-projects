# 规格类FAQ

### React Native 升级到 0.72.5 版本

请参考[RN升级变更整理](../RN升级变更整理.md)，在此文档中，梳理了 React Native 0.59 版本到 0.72.5 版本的主要修改点，您可以根据此文档修改您的代码，并升级 React Native 的版本。
### 开启Fabric以后重要变更

#### UIManager.measure 的实现不正确，获取到的布局数据与 iOS 不一致？

- **A**：`UIManager.measure`在Fabric开启时是不支持的，请使用`onLayout`或者`ref.current.measure`代替。

### 主题字体未生效/fontFamily未生效

- 主题字体优先于fontFamily生效。
- 主题字体的变更需要重启应用后才能生效。

### RN Date.parse()时间转换问题
RN本身并不直接规定日期字符串的格式，而是依赖于JavaScript的Date对象。JS支持以下三种日期格式：
- Date.parse("2024-11-20 14:00:00");//ISO 8601 格式
- Date.parse('Mon, 20 Mar 2023 14:00:00 GMT');//RFC 2822 / IETF 标准

`2024/10/24`是非标准的日期格式，可以考虑依赖[三方库Moment](https://gitee.com/react-native-oh-library/usage-docs/blob/master/zh-cn/moment.md)进行处理。比如：
- const date = moment('2024/10/24', 'YYYY/MM/DD').format('YYYY-MM-DD');

### RN setNativeProps只能生效一次问题
在[React Native启用Fabric渲染器](https://reactnative.cn/docs/fabric-renderer)之后，在使用`setNativeProps`时，只会在应用打开之后生效一次，剩余时间`setNativeProps`无法使用。该问题为[RN框架社区已知问题](https://github.com/facebook/react-native/issues/34391)，且[`setNativeProps`在0.72.5版本已废弃](https://github.com/Expensify/App/issues/26989)。可使用`setState`来进行状态设置。例如，在输入框内进行内容清空时，可以用`setState`来实现：

```javascript
import React, { useState } from 'react';
import {
  View,
  TextInput,
  Button,
  StyleSheet,
} from 'react-native';

const App = () => {
  const [text, setText] = useState(''); // 保存 TextInput 的值

  const clearText = () => {
    setText(''); // 清空输入框
  };

  return (
    <View style={styles.container}>
      <TextInput
        style={styles.input}
        value={text}
        onChangeText={setText} // 更新状态
        placeholder="输入一些内容"
      />
      <Button title="清空" onPress={clearText} />
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    padding: 16,
  },
  input: {
    width: '100%',
    height: 40,
    borderColor: '#ccc',
    borderWidth: 1,
    borderRadius: 4,
    marginBottom: 16,
    paddingHorizontal: 8,
  },
});

export default App;
```
