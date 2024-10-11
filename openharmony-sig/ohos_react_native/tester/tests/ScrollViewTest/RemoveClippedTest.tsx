import {TestSuite} from '@rnoh/testerino';
import {Button, TestCase} from '../../components';
import {useState, useMemo, useRef} from 'react';
import {ScrollView, Text, View} from 'react-native';

export default function RemoveClippedTest() {
  const [insert, setInsert] = useState(false);
  const [changeSize, setChangeSize] = useState(false);

  const scrollViewRef = useRef<ScrollView>(null);
  const scrollToY = (y: number) => {
    if (scrollViewRef.current) {
      scrollViewRef.current.scrollTo({y, animated: true});
    }
  };

  const rows = useMemo(() => {
    const temp_rows = [];
    for (let i = 0; i < 10; i++) {
      temp_rows.push(i);
    }
    return temp_rows;
  }, []);
  return (
    <TestSuite name="RemoveClipped">
      <TestCase.Example itShould="Render ScrollView with removed clipped subviews">
        <Button onPress={() => setInsert(prev => !prev)} label="Insert" />
        <Button
          onPress={() => setChangeSize(prev => !prev)}
          label="Change Size"
        />
        <Button onPress={() => scrollToY(500)} label="Scroll To 500" />
        <Button onPress={() => scrollToY(100)} label="Scroll To 100" />

        <ScrollView
          ref={scrollViewRef}
          removeClippedSubviews
          style={{height: 200, backgroundColor: 'white'}}>
          {insert && (
            <View style={{width: 100, height: 100, backgroundColor: 'green'}} />
          )}
          <View
            style={{
              width: 100,
              height: changeSize ? 200 : 100,
              backgroundColor: 'red',
            }}
          />
          <View
            style={[
              {
                flexDirection: 'row',
              },
            ]}>
            <View style={{width: 100, height: 100, backgroundColor: 'red'}} />
            <View style={{width: 100, height: 100, backgroundColor: 'blue'}} />
            <View style={{width: 100, height: 100, backgroundColor: 'green'}} />
          </View>
          {rows.map(e => (
            <Text key={e}>
              {e} Lorem ipsum dolor sit amet, consectetur adipiscibhjbng elit.
            </Text>
          ))}
          <View style={{width: 100, height: 100, backgroundColor: 'red'}} />
          <View style={{width: 100, height: 100, backgroundColor: 'blue'}} />
          <View style={{width: 100, height: 100, backgroundColor: 'green'}} />
        </ScrollView>
      </TestCase.Example>
    </TestSuite>
  );
}
