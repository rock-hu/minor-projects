/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {Image, ImageSourcePropType, ScrollView, Text, View} from 'react-native';
import { TestCase, TestSuite } from '@rnoh/testerino';
import React from 'react';
import {Button} from '../components';

const WRONG_IMAGE_SRC = 'not_image';
const LOCAL_IMAGE_ASSET_ID = require('../assets/pravatar-131.jpg');
const REMOTE_IMAGE_URL = 'https://i.pravatar.cc/100?img=31';
const LARGE_REMOTE_IMAGE_URL =
  'https://images.unsplash.com/photo-1556740749-887f6717d7e4';
const REMOTE_REDIRECT_IMAGE_URL = 'http://placeholder.com/350x150';
const REMOTE_GIF_URL =
  'https://upload.wikimedia.org/wikipedia/commons/2/2c/Rotating_earth_%28large%29.gif';
const DATA_URI =
  'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAUAAAAFCAYAAACNbyblAAAAHElEQVQI12P4//8/w38GIAXDIBKE0DHxgljNBAAO9TXL0Y4OHwAAAABJRU5ErkJggg==';

export const ImageTest = () => {
  return (
    <TestSuite name="Image">
      <TestCase itShould="support loading local images">
        <Image
          style={{borderRadius: 8, borderWidth: 1}}
          source={LOCAL_IMAGE_ASSET_ID}
        />
      </TestCase>
      <ImageExampleCase
        itShould="support loading remote images"
        source={{uri: REMOTE_IMAGE_URL}}
      />
      <ImageExampleCase
        itShould="support loading remote images (with http redirect)"
        source={{uri: REMOTE_REDIRECT_IMAGE_URL}}
      />
      <ImageExampleCase
        itShould="support loading large remote images (over 5mb)"
        source={{uri: LARGE_REMOTE_IMAGE_URL}}
      />
      <ImageExampleCase
        itShould="support loading image data uris"
        source={{uri: DATA_URI}}
      />
      <ImageExampleCase
        itShould="support loading remote animated gifs"
        source={{uri: REMOTE_GIF_URL}}
      />
      <TestCase itShould="display alt when the image doesn't load">
        <View>
          <Image
            source={require('../assets/fonts/Pacifico-Regular.ttf')}
            alt="This image could not be loaded!"
          />
        </View>
      </TestCase>
      <TestCase
        itShould="retrieve remote image size"
        fn={({expect}) => {
          return new Promise((resolve, reject) => {
            Image.getSize(
              REMOTE_IMAGE_URL,
              (width, height) => {
                expect(width).to.be.eq(100);
                expect(height).to.be.eq(100);
                resolve();
              },
              e => {
                reject(e);
              },
            );
          });
        }}
      />
      <TestCase
        itShould="retrieve local image size"
        fn={({expect}) => {
          const resolvedAsset = Image.resolveAssetSource(LOCAL_IMAGE_ASSET_ID);
          expect(resolvedAsset.width).to.be.eq(150);
          expect(resolvedAsset.height).to.be.eq(150);
        }}
      />
      <TestCase
        itShould="prefetch image"
        fn={async ({expect}) => {
          let ex: any;
          try {
            await Image.prefetch(WRONG_IMAGE_SRC);
          } catch (e) {
            ex = e;
          }
          expect(ex).to.be.not.undefined;
          expect(await Image.prefetch(REMOTE_IMAGE_URL)).to.be.true;
          expect(await Image.prefetch(REMOTE_IMAGE_URL)).to.be.true;
        }}
      />
      <TestCase
        itShould="query cache"
        fn={async ({expect}) => {
          await Image.prefetch(REMOTE_IMAGE_URL);
          expect(Image.queryCache).not.to.be.undefined;
          const result = await Image.queryCache?.([
            REMOTE_IMAGE_URL,
            WRONG_IMAGE_SRC,
          ]);
          expect(result).to.be.not.undefined;
          expect(result?.[REMOTE_IMAGE_URL]).to.be.not.undefined;
          expect(result?.[REMOTE_IMAGE_URL]).to.be.eq('disk');
          expect(result?.[WRONG_IMAGE_SRC]).to.be.undefined;
        }}
      />
      <TestCase
        skip // https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/246
        itShould="render circular image on a red rectangle (overlayColor)">
        <Image
          source={LOCAL_IMAGE_ASSET_ID}
          style={{overlayColor: 'red', borderRadius: Number.MAX_SAFE_INTEGER}}
        />
      </TestCase>
      <TestCase
        itShould="call onLoadStart"
        initialState={'not called'}
        arrange={({setState}) => {
          return (
            <Image
              source={LOCAL_IMAGE_ASSET_ID}
              onLoadStart={() => setState('called')}
            />
          );
        }}
        assert={({expect, state}) => {
          expect(state).to.be.eq('called');
        }}
      />
      <TestCase
        itShould="call onLoad"
        initialState={{}}
        arrange={({setState, state}) => {
          return (
            <View>
              <Text>{JSON.stringify(state)}</Text>
              <Image
                source={LOCAL_IMAGE_ASSET_ID}
                onLoad={event => {
                  setState(event.nativeEvent.source);
                }}
              />
            </View>
          );
        }}
        assert={({expect, state}) => {
          expect(state).to.contain.all.keys('width', 'height', 'uri');
        }}
      />
      <TestCase
        itShould="call onError (local)"
        initialState={null}
        arrange={({setState, state}) => {
          return (
            <View>
              <Text>{JSON.stringify(state)}</Text>
              <Image
                source={require('../assets/fonts/Pacifico-Regular.ttf')}
                onError={event => {
                  setState(event.nativeEvent.error);
                }}
              />
            </View>
          );
        }}
        assert={({expect, state}) => {
          expect(state).to.be.not.null;
        }}
      />
      <TestCase
        itShould="call onError (remote)"
        initialState={null}
        arrange={({setState, state}) => {
          return (
            <View>
              <Text>{JSON.stringify(state)}</Text>
              <Image
                source={{uri: 'https://www.google.com/image'}}
                onError={event => {
                  setState(event.nativeEvent.error);
                }}
              />
            </View>
          );
        }}
        assert={({expect, state}) => {
          expect(state).to.be.not.null;
        }}
      />
      <TestSuite
        name="resizeMode" // https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/245
      >
        <TestCase itShould="render small image in the center (center)">
          <Image
            style={{width: '100%', height: 100}}
            source={LOCAL_IMAGE_ASSET_ID}
            resizeMode="center"
          />
        </TestCase>
        <TestCase itShould="render image touching top and bottom edges in the center (contain)">
          <Image
            style={{width: '100%', height: 100}}
            source={LOCAL_IMAGE_ASSET_ID}
            resizeMode="contain"
          />
        </TestCase>
        <TestCase itShould="fully cover test case area while preserving aspect ratio (cover)">
          <Image
            style={{width: '100%', height: 100}}
            source={LOCAL_IMAGE_ASSET_ID}
            resizeMode="cover"
          />
        </TestCase>
        <TestCase itShould="cover test case area by repeating image (repeat)">
          <Image
            style={{width: '100%', height: 100}}
            source={LOCAL_IMAGE_ASSET_ID}
            resizeMode="repeat"
          />
        </TestCase>
        <TestCase itShould="cover test case area by stretching (stretch)">
          <Image
            style={{width: '100%', height: 100}}
            source={LOCAL_IMAGE_ASSET_ID}
            resizeMode="stretch"
          />
        </TestCase>
      </TestSuite>
      <TestSuite name="blurRadius">
        <TestCase itShould="blur images with various blur radius">
          <View style={{flexDirection: 'row', flexWrap: 'wrap'}}>
            <Image
              style={{width: 64, height: 64, margin: 4}}
              source={LOCAL_IMAGE_ASSET_ID}
              blurRadius={0}
            />
            <Image
              style={{width: 64, height: 64, margin: 4}}
              source={LOCAL_IMAGE_ASSET_ID}
              blurRadius={5}
            />
            <Image
              style={{width: 64, height: 64, margin: 4}}
              source={LOCAL_IMAGE_ASSET_ID}
              blurRadius={10}
            />
            <Image
              style={{width: 64, height: 64, margin: 4}}
              source={LOCAL_IMAGE_ASSET_ID}
              blurRadius={15}
            />
            <Image
              style={{width: 64, height: 64, margin: 4}}
              source={LOCAL_IMAGE_ASSET_ID}
              blurRadius={20}
            />
            <Image
              style={{width: 64, height: 64, margin: 4}}
              source={LOCAL_IMAGE_ASSET_ID}
              blurRadius={25}
            />
          </View>
        </TestCase>
      </TestSuite>
      <TestCase itShould="replace opaque pixels with the green color (tintColor)">
        <View
          style={{
            flex: 1,
            flexDirection: 'row',
            justifyContent: 'space-around',
          }}>
          <Image
            source={require('../assets/expo.png')}
            style={{
              width: 100,
              height: 100,
            }}
          />
          <Image
            source={require('../assets/expo.png')}
            style={{
              width: 100,
              height: 100,
              tintColor: 'green',
            }}
          />
        </View>
      </TestCase>
      <TestCase modal itShould="stop displaying on press">
        <SwitchSourceTest />
      </TestCase>
      <TestCase itShould="render top image in a bit lower quality (difference barely visible)">
        <Image
          style={{width: 200, height: 200}}
          source={require('../assets/noise.png')}
          resizeMethod="resize"
          resizeMode="stretch"
        />
        <View style={{height: 10}} />
        <Image
          style={{width: 200, height: 200}}
          source={require('../assets/noise.png')}
          resizeMethod="scale"
          resizeMode="stretch"
        />
      </TestCase>
      <TestCase
        modal
        skip // https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/483
        itShould="fade images with varying durations">
        <View style={{flexDirection: 'row', gap: 24}}>
          <View style={{width: 100}}>
            <Image
              // HACK: ?v=Date.now() is used to prevent caching
              // - cached images are not fading/faded in
              source={{uri: REMOTE_IMAGE_URL + '?v=' + Date.now()}}
              style={{width: 100, height: 100, borderRadius: 8}}
              fadeDuration={0}
            />
            <Text>This image will fade in over the time of 0s.</Text>
          </View>
          <View style={{width: 100}}>
            <Image
              source={{uri: REMOTE_IMAGE_URL + '?v=' + Date.now()}}
              style={{width: 100, height: 100, borderRadius: 8}}
              fadeDuration={1500}
            />
            <Text>This image will fade in over the time of 1.5s.</Text>
          </View>
          <View style={{width: 100}}>
            <Image
              source={{uri: REMOTE_IMAGE_URL + '?v=' + Date.now()}}
              style={{width: 100, height: 100, borderRadius: 8}}
              fadeDuration={5000}
            />
            <Text>This image will fade in over the time of 5s.</Text>
          </View>
        </View>
        {/* To test local fadeDuration for localImages you have to disable caching */}
        <View style={{flexDirection: 'row', gap: 24}}>
          <View style={{width: 100}}>
            <Image
              source={LOCAL_IMAGE_ASSET_ID}
              style={{width: 100, height: 100, borderRadius: 8}}
              fadeDuration={0}
            />
            <Text>This image will fade in over the time of 0s.</Text>
          </View>
          <View style={{width: 100}}>
            <Image
              source={LOCAL_IMAGE_ASSET_ID}
              style={{width: 100, height: 100, borderRadius: 8}}
              fadeDuration={1500}
            />
            <Text>This image will fade in over the time of 1.5s.</Text>
          </View>
          <View style={{width: 100}}>
            <Image
              source={LOCAL_IMAGE_ASSET_ID}
              style={{width: 100, height: 100, borderRadius: 8}}
              fadeDuration={5000}
            />
            <Text>This image will fade in over the time of 5s.</Text>
          </View>
        </View>
      </TestCase>
      <TestCase
        modal
        itShould="load many large images without causing out-of-memory issues">
        <ScrollView>
          {Array.from({length: 25}, (_, index) => (
            <Image
              key={index}
              style={{width: 200, height: 200}}
              source={{uri: LARGE_REMOTE_IMAGE_URL}}
            />
          ))}
        </ScrollView>
      </TestCase>
    </TestSuite>
  );
};

const ImageExampleCase = ({
  itShould,
  source,
}: {
  itShould: string;
  source: ImageSourcePropType;
}) => (
  <TestCase itShould={itShould}>
    <Image
      style={{borderRadius: 8, borderWidth: 1, height: 150}}
      source={source}
      onError={e => console.error(e.nativeEvent.error)}
      // resizeMode="contain"
    />
  </TestCase>
);

const SwitchSourceTest = () => {
  const SOURCES = [
    REMOTE_IMAGE_URL,
    '',
    REMOTE_REDIRECT_IMAGE_URL,
    WRONG_IMAGE_SRC,
  ];

  const [idx, setIdx] = React.useState(0);

  return (
    <View>
      <View style={{flexDirection: 'row'}}>
        <Image source={{uri: SOURCES[idx]}} style={{width: 100, height: 100}} />
        <Text>{`Source: ${SOURCES[idx]}`}</Text>
      </View>
      <Button
        label="Switch Source"
        onPress={() => {
          setIdx(i => (i + 1) % SOURCES.length);
        }}
      />
    </View>
  );
};
