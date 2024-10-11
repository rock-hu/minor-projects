import React from 'react';
import {StyleSheet, Text, View, TouchableOpacity} from 'react-native';

export interface GoodsProps {
  name: string;
  onPress: () => void;
}

export interface ListProps {
  goods: GoodsProps[];
}

export default function HomeListPage({goods}: ListProps) {
  return (
    <View style={styles.container}>
      {goods.map((good, index) => (
        <TouchableOpacity key={index} style={styles.row} onPress={good.onPress}>
          <Text style={styles.text}>{good.name}</Text>
        </TouchableOpacity>
      ))}
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    marginTop: 5,
    flexDirection: 'column',
    borderRadius: 8,
    paddingHorizontal: 10,
  },
  row: {
    height: 100,
    alignItems: 'center',
    justifyContent: 'center',
    backgroundColor: 'white',
    borderColor: 'white',
    borderRadius: 15,
    marginTop: 10,
  },
  text: {
    fontSize: 30,
    color: '#3C3C3C',
  },
});
