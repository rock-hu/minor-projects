import * as React from 'react';
import {
  Keyboard,
  ScrollView,
  Text,
  TextInput,
  TouchableOpacity,
  View,
} from 'react-native';

export function KeyboardDismissExample() {
  return (
    <View style={{flex: 1}}>
      <TouchableOpacity
        onPress={() => Keyboard.dismiss()}
        style={{
          width: '100%',
          height: 48,
          backgroundColor: 'lightblue',
          justifyContent: 'center',
          alignItems: 'center',
        }}>
        <Text>Dismiss Keyboard</Text>
      </TouchableOpacity>
      <ScrollView style={{flex: 1}} keyboardShouldPersistTaps={'always'}>
        <TextInput style={{padding: 16, backgroundColor: 'lightgray'}} />
      </ScrollView>
    </View>
  );
}
