
import React, { useContext, useState } from 'react';
import { Text, View, DeviceEventEmitter, RootTagContext } from 'react-native';

const App = () => {
  const rootTag = useContext(RootTagContext)
  const [msg, setMsg] = useState('')
  DeviceEventEmitter.addListener('clickEvent', e => {
    setMsg(`The message was ${e.params.rootTag === rootTag ? '' : 'not'} sent to me.`)
    const timer = setTimeout(() => {
      setMsg('')
      clearTimeout(timer)
    }, 1000)
  })
  return (
    <View style={{flex: 1, backgroundColor: 'yellow', alignItems: 'center', justifyContent: 'center'}}>
      <Text style={{fontSize: 20, color: 'red'}}>Here is the hotel page.</Text>
      <Text style={{fontSize: 20, color: 'red'}}>{msg}</Text>
    </View>
  );
};

export default App;
