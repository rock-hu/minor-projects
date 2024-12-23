import {View, Button, Modal, Text, StyleSheet} from 'react-native';
import React, {useState, useEffect} from 'react';

export const ModalCrashExample = () => {
  const [isShow, setIsShow] = useState(true);

  useEffect(() => {
    const aTimer = setInterval(() => {
      setIsShow(a => !a);
    }, 200);
    return () => clearInterval(aTimer);
  }, []);
  return (
    <>
      <Text style={styles.TextHeight}>测试demo</Text>
      <Button title="TEST" />
      {isShow && <TextA />}
      {!isShow && <TextB />}
    </>
  );
};

const styles = StyleSheet.create({
  TextHeight: {
    marginTop: 100,
  },
  modelStyleA: {
    height: 50,
    width: 100,
    marginTop: 100,
    backgroundColor: 'pink',
  },
  modelStyleB: {
    height: 50,
    width: 100,
    marginTop: 100,
    backgroundColor: 'blue',
  },
});

const TextA = (props: any) => {
  return (
    <View>
      <Modal visible style={styles.modelStyleA}>
        <Text style={styles.modelStyleA}>ModalA</Text>
      </Modal>
    </View>
  );
};

const TextB = (props: any) => {
  return (
    <>
      <Modal visible style={styles.modelStyleB}>
        <Text style={styles.modelStyleB}>ModalB</Text>
      </Modal>
    </>
  );
};
