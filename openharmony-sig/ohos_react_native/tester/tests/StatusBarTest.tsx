import React, {useEffect, useState} from 'react';
import {
  StyleSheet,
  StatusBar,
  Text,
  View,
  StatusBarStyle,
  StatusBarProps,
} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {Button, TestCase} from '../components';

export function StatusBarTest() {
  return (
    <TestSuite name="Status bar">
      <StatusBarView animated />
    </TestSuite>
  );
}
const STYLES = ['default', 'dark-content', 'light-content'] as const;
const BACKGROUND_COLORS = ['#FF000088', '#00FF0088'];

function StatusBarView({animated}: {animated?: boolean}) {
  const [hidden, setHidden] = useState(false);
  const [backgroundColor, setBackgroundColor] = useState(BACKGROUND_COLORS[0]);
  const [statusBarStyle, setStatusBarStyle] = useState<StatusBarStyle>(
    STYLES[0],
  );
  const [translucent, setTranslucent] = useState(false);
  const [greenProps, setGreenProps] = useState<StatusBarProps>({});

  const changeStatusBarStyle = () => {
    const styleId = STYLES.indexOf(statusBarStyle) + 1;
    if (styleId === STYLES.length) {
      setStatusBarStyle(STYLES[0]);
    } else {
      setStatusBarStyle(STYLES[styleId]);
    }
  };
  const changeBackgroundColor = () => {
    setBackgroundColor(prevColor => {
      const newColorId = BACKGROUND_COLORS.indexOf(prevColor) + 1;
      return BACKGROUND_COLORS[
        newColorId === BACKGROUND_COLORS.length ? 0 : newColorId
      ];
    });
  };

  useEffect(() => {
    return () => {
      setStatusBarStyle('dark-content');
    };
  }, []);

  return (
    <View>
      <StatusBar
        animated={animated}
        backgroundColor={backgroundColor} // 'rgba(200, 0, 0, 0.5)'
        barStyle={statusBarStyle}
        hidden={hidden}
        translucent={translucent}
      />
      <TestCase.Example itShould="toggle status bar visibility">
        <View
          style={styles.button}
          onTouchEnd={() => {
            setHidden(!hidden);
          }}>
          <Text style={styles.buttonText}>{hidden ? 'hidden' : 'visible'}</Text>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="toggle status bar background color(red/green), with alpha 88">
        <View
          style={styles.button}
          onTouchEnd={() => {
            changeBackgroundColor();
          }}>
          <Text style={styles.buttonText}>{backgroundColor}</Text>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="toggle status bar style (light-content(default)/dark-content) ">
        <View
          style={styles.button}
          onTouchEnd={() => {
            changeStatusBarStyle();
          }}>
          <Text style={styles.buttonText}>{statusBarStyle}</Text>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="toggle status bar translucent">
        <View
          style={styles.button}
          onTouchEnd={() => {
            setTranslucent(!translucent);
          }}>
          <Text style={styles.buttonText}>
            {translucent ? 'translucent' : 'non-translucent'}
          </Text>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="set status bar color to blue (StatusBar.setBackgroundColor)">
        <View
          style={styles.button}
          onTouchEnd={() => {
            StatusBar.setBackgroundColor('#0000FF88');
          }}>
          <Text style={styles.buttonText}>Set blue</Text>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="set status bar dark style (light-content(default)/dark-content) (StatusBar.setBarStyle) ">
        <View
          style={styles.button}
          onTouchEnd={() => {
            StatusBar.setBarStyle('dark-content');
          }}>
          <Text style={styles.buttonText}>Set dark-content</Text>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="set status bar translucent (StatusBar.setTranslucent)">
        <View
          style={styles.button}
          onTouchEnd={() => {
            StatusBar.setTranslucent(true);
          }}>
          <Text style={styles.buttonText}>Set translucent</Text>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="set status bar color to purple with animation (StatusBar.setBackgroundColor)">
        <View
          style={styles.button}
          onTouchEnd={() => {
            StatusBar.setBackgroundColor('#FF00FF88', true);
          }}>
          <Text style={styles.buttonText}>Set purple</Text>
        </View>
      </TestCase.Example>
      <TestCase.Example
        skip="This test is skipped because the API is not available in the current version of OpenHarmony"
        itShould="set status bar dark style with animation (light-content(default)/dark-content) (StatusBar.setBarStyle) ">
        <View
          style={styles.button}
          onTouchEnd={() => {
            StatusBar.setBarStyle('dark-content', true);
          }}>
          <Text style={styles.buttonText}>Set dark-content</Text>
        </View>
      </TestCase.Example>
      <TestCase.Example
        skip="This test is skipped because the API is not available in the current version of OpenHarmony"
        itShould="set status bar color to hidden with animation (StatusBar.setHidden)">
        <View
          style={styles.button}
          onTouchEnd={() => {
            StatusBar.setHidden(true, 'slide');
          }}>
          <Text style={styles.buttonText}>Set hidden</Text>
        </View>
      </TestCase.Example>
      <TestCase.Logical
        itShould="return correct status bar height for device"
        fn={({expect}) => {
          expect(StatusBar.currentHeight).to.be.closeTo(38.8, 0.1);
        }}
      />
      <TestCase.Example itShould="set status bar color to green on push and return to previous style on pop">
        <Button
          label="push green"
          onPress={() => {
            const res = StatusBar.pushStackEntry({
              backgroundColor: 'green',
            });
            setGreenProps(res);
          }}
        />
        <Button
          label="pop green"
          onPress={() => {
            StatusBar.popStackEntry(greenProps);
          }}
        />
      </TestCase.Example>
    </View>
  );
}

const styles = StyleSheet.create({
  button: {
    width: 160,
    height: 40,
    backgroundColor: 'red',
    paddingHorizontal: 16,
  },
  buttonText: {
    width: '100%',
    height: '100%',
    fontWeight: 'bold',
    color: 'white',
  },
});
