import {StyleSheet, Text, View} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import React from 'react';
import {Button, TestCase} from '../components';
import {useEnvironment} from '../contexts';
// @ts-ignore
import resolveAssetSource from 'react-native/Libraries/Image/resolveAssetSource';

const RCTNetworking =
  require('react-native/Libraries/Network/RCTNetworking').default;

const FILE_URI = '/data/storage/el2/base/files/testFile.txt';
const SVG_TEXT = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="-11.5 -10.23174 23 20.46348">
  <title>React Logo</title>
  <circle cx="0" cy="0" r="2.05" fill="#61dafb"/>
  <g stroke="#61dafb" stroke-width="1" fill="none">
    <ellipse rx="11" ry="4.2"/>
    <ellipse rx="11" ry="4.2" transform="rotate(60)"/>
    <ellipse rx="11" ry="4.2" transform="rotate(120)"/>
  </g>
</svg>
`;

export const NetworkingTest = () => {
  const canFetch = async (url: string) => {
    try {
      const response = await fetch(url);
      await response.json();
      return true;
    } catch (error) {
      return false;
    }
  };

  return (
    <TestSuite name="Networking">
      <TestSuite name="Fetch API">
        <TestCase.Logical
          itShould="download data"
          fn={async ({expect}) => {
            const response = await fetch('https://reactnative.dev/movies.json');
            const result = await response.text();
            const parsed = JSON.parse(result);
            expect(parsed.title).to.be.eq('The Basics - Networking');
          }}
        />
        <TestCase.Logical
          itShould="download and parse JSON data"
          fn={async ({expect}) => {
            const response = await fetch('https://reactnative.dev/movies.json');
            const result = await response.json();
            expect(result.title).to.be.eq('The Basics - Networking');
          }}
        />
        <TestCase.Logical
          itShould="download data to an ArrayBuffer"
          fn={async ({expect}) => {
            const response = await fetch('https://reactnative.dev/movies.json');
            const result = await response.arrayBuffer();
            expect(result.byteLength).to.be.eq(458);
          }}
        />
        <TestCase.Logical
          itShould="download data to a blob"
          fn={async ({expect}) => {
            const response = await fetch('https://reactnative.dev/movies.json');
            const result = await response.blob();
            expect(result.size).to.be.eq(458);
          }}
        />
        <TestCase.Logical
          itShould="fetch local assets correctly (test needs to be run without metro)"
          fn={async ({expect}) => {
            const uri = require('../assets/react-icon.svg');
            const source = resolveAssetSource(uri);
            const response = await fetch(source.uri);
            const result = await response.text();
            expect(result).to.be.eq(SVG_TEXT);
          }}
        />
        <TestCase.Logical
          itShould="fetch local assets correctly (test needs to be run without metro)"
          fn={async ({expect}) => {
            const uri = require('../assets/react-icon.svg');
            const source = resolveAssetSource(uri);
            const response = await fetch(source.uri);
            const result = await response.text();
            expect(result).to.be.eq(SVG_TEXT);
          }}
        />
        <TestCase.Logical
          itShould="correctly send requests with FormData"
          fn={async ({expect}) => {
            const formData = new FormData();
            formData.append('name', 'test');
            formData.append('surname', 'test2');
            formData.append('boolean', true);
            formData.append('number', 72);
            const response = await fetch('https://httpbin.org/post', {
              method: 'POST',
              body: formData,
            });
            const result = await response.json();
            expect(result.form.name).to.be.eq('test');
            expect(result.form.boolean).to.be.eq('true');
            expect(result.form.number).to.be.eq('72');
            expect(result.form.surname).to.be.eq('test2');
          }}
        />
        <TestCase.Logical
          itShould="correctly send a text file in FormData"
          fn={async ({expect}) => {
            try {
              let formData = new FormData();
              formData.append('file', {
                uri: FILE_URI,
                type: 'text/plain',
                name: 'testFile.txt',
              });
              formData.append('text', 'test text field');
              let response = await fetch('https://httpbin.org/post', {
                method: 'POST',
                body: formData,
                headers: {
                  'Content-Type': 'multipart/form-data',
                },
              });
              let responseJson = await response.json();
              expect(responseJson.form.text).to.be.eq('test text field');
              expect(responseJson.files.file).to.exist;
            } catch (error) {
              console.error(
                'To run this test, you need to create a file at the correct path on the phone. You can run this command in the hdc shell: echo "Test file content" >> /data/app/el2/100/base/com.rnoh.tester/files/testFile.txt',
              );
              throw error;
            }
          }}
        />
        <TestCase.Logical
          itShould="correctly read response headers"
          fn={async ({expect}) => {
            const response = await fetch(
              'https://httpbin.org/response-headers?first-header=first&second-header=second',
            );
            const firstHeader = response.headers.get('first-header');
            const secondHeader = response.headers.get('second-header');
            expect(firstHeader).to.be.eq('first');
            expect(secondHeader).to.be.eq('second');
          }}
        />
        <TestCase.Logical
          itShould="fail on bad url"
          fn={async ({expect}) => {
            const received = await canFetch(
              'https://reactnative.dev/bad_url.json',
            );
            expect(received).to.be.false;
          }}
        />
      </TestSuite>
      <TestSuite name="XmlHttpRequest">
        <TestCase.Logical
          itShould="verify if correct data is received in onprogress event"
          fn={async ({expect}) => {
            // emojis use 4 bytes so theres a higher chance of them getting cut when loading incrementally
            const emoji = '😀';
            const longStr = emoji.repeat(10000); //the value needs to be high enough so that several onprogress events are triggered
            let responseText = '';

            const xhr = new XMLHttpRequest();
            xhr.open('POST', 'https://postman-echo.com/post', true);
            xhr.setRequestHeader('Content-Type', 'text/plain; charset=UTF-8');

            await new Promise<void>((resolve, reject) => {
              xhr.onprogress = function (this, ev) {
                try {
                  expect(ev.total).to.not.be.null.and.not.be.undefined;
                  expect(ev.loaded).to.not.be.null.and.not.be.undefined;
                } catch (error) {
                  reject(error);
                }
                responseText = xhr.responseText;
              };

              xhr.onload = function () {
                const response = JSON.parse(responseText);
                try {
                  expect(response.data).to.be.eq(longStr);
                  resolve();
                } catch (error) {
                  reject(error);
                }
              };

              xhr.onerror = function () {
                reject('Request failed');
              };

              xhr.send(longStr);
            });
          }}
        />
        <TestCase.Logical
          itShould="verify if correct data is received in onupload progress event"
          fn={async ({expect}) => {
            let longStr = 'a'.repeat(100000); //the value needs to be high enough so that several events are triggered
            const xhr = new XMLHttpRequest();
            xhr.open('POST', 'https://postman-echo.com/post', true);
            xhr.setRequestHeader('Content-Type', 'text/plain; charset=UTF-8');
            await new Promise<void>((resolve, reject) => {
              xhr.upload.onprogress = function (event) {
                try {
                  expect(event.total).to.equal(longStr.length);
                  expect(event.loaded).to.lessThanOrEqual(longStr.length);
                } catch (error) {
                  reject(error);
                }
              };

              xhr.onloadend = function () {
                resolve();
              };

              xhr.onerror = function () {
                throw reject('Upload failed');
              };
              xhr.send(longStr);
            });
          }}
        />
        <TestCase.Logical
          itShould="verify that correct events are emitted"
          fn={async ({expect}) => {
            let longStr = 'a'.repeat(100000); //the value needs to be high enough so that several events are triggered
            const xhr = new XMLHttpRequest();
            xhr.open('POST', 'https://postman-echo.com/post', true);
            xhr.setRequestHeader('Content-Type', 'text/plain; charset=UTF-8');
            const events = await new Promise<string[]>((resolve, reject) => {
              const allEvents: string[] = [];
              xhr.upload.onprogress = function () {
                allEvents.push('upload progress');
              };
              xhr.onprogress = function () {
                allEvents.push('progress');
              };
              xhr.onload = function () {
                allEvents.push('load');
              };
              xhr.onloadend = function () {
                allEvents.push('loadend');

                resolve(allEvents);
              };
              xhr.onreadystatechange = function () {
                allEvents.push('readystatechange');
              };

              xhr.onerror = function () {
                throw reject('Upload failed');
              };
              xhr.send(longStr);
            });
            expect(events).to.contain('upload progress');
            expect(events).to.contain('progress');
            expect(events).to.contain('load');
            expect(events).to.contain('loadend');
          }}
        />
        <TestCase.Logical
          itShould="verify that the abort event gets triggered"
          fn={async ({expect}) => {
            const event = await new Promise<ProgressEvent<EventTarget>>(
              (resolve, reject) => {
                const xhr = new XMLHttpRequest();
                xhr.open('POST', 'invalidurlr.jpg', true);
                xhr.onabort = function (abortEvent) {
                  resolve(abortEvent);
                };
                xhr.onerror = function () {
                  reject('Request should have been aborted');
                };
                xhr.onload = function () {
                  reject('Request should have been aborted');
                };
                xhr.send();
                xhr.abort();
              },
            );
            expect(event.type).to.equal('abort');
          }}
        />
        <TestCase.Logical
          itShould="verify that the timeout event gets triggered"
          fn={async ({expect}) => {
            const event = await new Promise<ProgressEvent<EventTarget>>(
              (resolve, reject) => {
                const xhr = new XMLHttpRequest();
                xhr.open('POST', 'https://postman-echo.com/delay/10', true);
                xhr.timeout = 1;
                xhr.ontimeout = function (timeoutEvent) {
                  resolve(timeoutEvent);
                };
                xhr.onerror = function () {
                  reject('Request should have timed out');
                };
                xhr.onload = function () {
                  reject('Request should have timed out');
                };
                xhr.send();
              },
            );
            expect(event.type).to.equal('timeout');
          }}
        />
        <TestCase.Logical
          itShould="verify that cookies are saved and sent correctly (withCredentials)"
          fn={async ({expect}) => {
            await sendCookieRequest(
              'https://httpbin.org/cookies/set?theme=light',
              true,
            );
            await sendCookieRequest(
              'https://httpbin.org/cookies/set?theme=dark&secondattribute=test',
              true,
            );

            const thirdCookies = (await sendCookieRequest(
              'https://httpbin.org/cookies',
              true,
            )) as Record<string, unknown>;
            expect(thirdCookies.theme).to.equal('dark');
            expect(thirdCookies.secondattribute).to.equal('test');

            const fourthCookies = await sendCookieRequest(
              'https://httpbin.org/cookies',
              false,
            );
            expect(fourthCookies).to.be.empty;
          }}
        />
        <TestCase.Manual
          initialState={{cookies: {}}}
          arrange={({setState}) => {
            const requestSetCookies = async () => {
              const xhr = new XMLHttpRequest();
              xhr.open(
                'GET',
                'https://httpbin.org/cookies/set?testcookie=storedcookie',
                true,
              );
              xhr.send();
            };

            const getCookies = async () => {
              const xhr = new XMLHttpRequest();
              xhr.open('GET', 'https://httpbin.org/cookies', true);
              xhr.onloadend = function () {
                const response = JSON.parse(xhr.responseText);
                setState({cookies: response.cookies});
              };
              xhr.send();
            };
            const clearCookies = async () => {
              RCTNetworking.clearCookies(() => {});
            };

            return (
              <>
                <View>
                  <Text style={{fontSize: 16, marginBottom: 10}}>
                    <Text style={{fontSize: 16, marginBottom: 10}}>
                      1. Press "Clear cookies".{'\n'}
                      2. Press "Set cookies".{'\n'}
                      4. Close and reopen the app.{'\n'}
                      5. Press "Get cookies".
                    </Text>
                  </Text>
                  <Button onPress={requestSetCookies} label="Set cookies" />
                  <Button onPress={getCookies} label="Get cookies" />
                  <Button onPress={clearCookies} label="Clear cookies" />
                </View>
              </>
            );
          }}
          assert={({state, expect}) => {
            expect(
              (state.cookies as {testcookie?: string}).testcookie,
            ).to.equal('storedcookie');
          }}
          modal
          itShould="store cookies in a persistant way"
        />
      </TestSuite>
      <TestSuite name="WebSocket">
        <TestCase.Example itShould="connect to websockets">
          <WebSocketEcho />
        </TestCase.Example>
        <TestCase.Example
          modal
          itShould="send and receive arraybuffer through websocket and display 'Hello World from WebSocket!'">
          <WebSocketSendingAndReceivingArrayBuffer />
        </TestCase.Example>
        <TestCase.Example
          modal
          itShould="send and receive blob through websocket and display blob size">
          <WebSocketSendingAndReceivingBlob />
        </TestCase.Example>
      </TestSuite>
    </TestSuite>
  );
};

function arrayBufferToStr(buf: ArrayBuffer | ArrayBufferLike) {
  return String.fromCharCode(...new Uint8Array(buf));
}

function stringToArrayBuffer(str: string) {
  const buffer = new ArrayBuffer(str.length);
  const bufferView = new Uint8Array(buffer);
  for (let i = 0; i < str.length; i++) {
    bufferView[i] = str.charCodeAt(i);
  }
  return buffer;
}

const WebSocketResultBlock = ({
  status,
  data,
}: {
  status: string;
  data: undefined | string;
}) => {
  return (
    <View>
      <View style={{gap: 8, flexDirection: 'row'}}>
        <Text style={{width: 50}}>Status: </Text>
        <Text>{status}</Text>
      </View>
      <View style={{gap: 8, flexDirection: 'row'}}>
        <Text style={{width: 50}}>Data: </Text>
        <Text style={styles.movieDetails}>
          {data ? data : 'Nothing to show!'}
        </Text>
      </View>
    </View>
  );
};

const WebSocketEcho = () => {
  const [status, setStatus] = React.useState('Loading...');
  const [data, setData] = React.useState<string>();

  const runWebSockSession = () => {
    // connect to Postman's echo server
    const ws = new WebSocket('wss://ws.postman-echo.com/raw');

    ws.onopen = () => {
      setStatus('Connected');
      ws.send('something');
    };

    ws.onmessage = e => {
      setData(JSON.stringify(e));
      setTimeout(() => {
        setStatus('Closing...');
        ws.close();
      }, 3000);
    };

    ws.onerror = e => {
      console.error(e.message);
      setStatus(`Error ${e.message}`);
    };

    ws.onclose = e => {
      console.log(e.code, e.reason);
      setStatus(`Closed ${e.code} ${e.reason}`);
    };
  };

  React.useEffect(() => {
    runWebSockSession();
  }, []);

  return <WebSocketResultBlock status={status} data={data} />;
};

function WebSocketSendingAndReceivingArrayBuffer() {
  const [status, setStatus] = React.useState('Loading...');
  const [data, setData] = React.useState<string>();

  const runWebSocketSession = () => {
    const ws = new WebSocket('wss://echo.websocket.org/');

    // @ts-ignore
    ws.binaryType = 'arraybuffer';

    ws.onopen = () => {
      setStatus('Connected');
      const text = 'Hello World - arraybuffer!';
      const buffer = stringToArrayBuffer(text);
      ws.send(buffer);
    };

    ws.onmessage = event => {
      if (event.data instanceof ArrayBuffer) {
        setData(arrayBufferToStr(event.data));
        ws.close();
      }
    };

    ws.onerror = error => {
      setStatus(`Error ${error.message}`);
    };

    ws.onclose = event => {
      setStatus(`Closed ${event.code} ${event.reason}`);
    };
  };

  React.useEffect(() => {
    runWebSocketSession();
  }, []);

  return <WebSocketResultBlock status={status} data={data} />;
}

const sendCookieRequest = (
  url: string,
  withCredentials: boolean,
): Promise<{cookies: Record<string, unknown>}> => {
  return new Promise((resolve, reject) => {
    const xhr = new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.withCredentials = withCredentials;
    xhr.onloadend = () => {
      try {
        const response = JSON.parse(xhr.responseText);
        resolve(response.cookies);
      } catch (error) {
        reject(error);
      }
    };
    xhr.send();
  });
};

function WebSocketSendingAndReceivingBlob() {
  const [status, setStatus] = React.useState('Loading...');
  const [data, setData] = React.useState<string>();

  const runWebSocketSession = () => {
    const ws = new WebSocket('wss://echo.websocket.org/');

    // @ts-ignore
    ws.binaryType = 'blob';

    ws.onopen = () => {
      setStatus('Connected');
      const blobString = 'Hello World - blob!';
      const blob = new Blob([blobString], {
        type: 'text/plain',
        lastModified: Date.now(),
      });
      ws.send(blob);
    };

    ws.onmessage = event => {
      if (event.data instanceof Blob) {
        setData(`Size of the blob: ${event.data?.size}`);
        ws.close();
      }
    };

    ws.onerror = error => {
      setStatus(`Error ${error.message}`);
    };

    ws.onclose = event => {
      setStatus(`Closed ${event.code} ${event.reason}`);
    };
  };

  React.useEffect(() => {
    runWebSocketSession();
  }, []);

  return <WebSocketResultBlock status={status} data={data} />;
}

const styles = StyleSheet.create({
  movieDetails: {
    height: 20,
    width: '100%',
  },
  loadingText: {
    fontSize: 20,
    height: 40,
    width: '100%',
  },
});
