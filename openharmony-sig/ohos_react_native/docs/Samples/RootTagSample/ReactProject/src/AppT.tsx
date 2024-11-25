
import React, { useContext, useState } from 'react';
import { Text, View, DeviceEventEmitter, RootTagContext } from 'react-native';

const App = () => {
  const rootTag = useContext(RootTagContext);
  const [msg, setMsg] = useState('')
  DeviceEventEmitter.addListener('clickEvent', e => {
    setMsg(`The message was ${e.params.rootTag === rootTag ? '' : 'not'} sent to me.`)
    const timer = setTimeout(() => {
      setMsg('')
      clearTimeout(timer)
    }, 1000)
  })
  return (
    <View style={{flex: 1, backgroundColor: 'green', alignItems: 'center', justifyContent: 'center'}}>
      <Text style={{fontSize: 20, color: 'white'}}>Here is the flight ticket page.</Text>
      <Text style={{fontSize: 20, color: 'white'}}>{msg}</Text>
    </View>
  );
};

export default App;
