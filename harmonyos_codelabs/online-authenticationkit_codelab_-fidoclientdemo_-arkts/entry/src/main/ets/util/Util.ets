import { http } from '@kit.NetworkKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

interface Par {
  userName: string;
  downloadCer: string;
  userVerify: string;
  authType: string;
  deviceID: string;
  type: string;
}

interface Sp {
  type: string;
  userName: string;
  resp: string;
}

export class Util {
  public static fidoParameter(type: string, userAuthType: String) {
    let name: string = 'testName';
    let downloadCer = 'false';
    // Biometric authentication type. Fingerprint: 2; Face: 16
    let userVerify = userAuthType;
    let authType = 'auth';
    let deviceID = 'mate30test';
    let parameter: string = 'userName=' + name + '&type=' + type + '&downloadCer=' + downloadCer + '&userVerify=' + userVerify + '&authType=' + authType + '&deviceID=' + deviceID;
    return parameter;
  }

  public static connectFidoServer(url: string, str: string): Promise<string> {
    return new Promise((resolve) => {
      if (url !== null && url.length > 0) {
        let httpRequest = http.createHttp();
        httpRequest.request(
          url,
          {
            // Optional. The default value is http.RequestMethod.GET.
            method: http.RequestMethod.POST,
            // You can add header fields based on service requirements.
            header: {
              'Content-Type': 'application/x-www-form-urlencoded'
            },
            // Use this field to transfer information in a POST request.
            extraData: str,
            // Optional. The default value is 1.
            priority: 1,
            // Optional. The default value is 60000, in ms.
            connectTimeout: 60000,
            // Optional. The default value is 60000, in ms.
            readTimeout: 60000,
          },
          (err, data) => {
            hilog.info(0x0000, 'TAG', `test... ${JSON.stringify(err)}`);
            if (!err) {
              hilog.info(0x0000, 'TAG', `test... ${JSON.stringify(data)}`);
              let result = data.result.toString();
              httpRequest.destroy();
              resolve(result);
            }
          }
        );
      }
    });
  }

  public static SpP(spType: string, resp: string) {
    let fidoP: Sp = {
      type: spType,
      userName: 'testName',
      resp: resp
    }
    return fidoP
  }

  public static connectFidoServer1(url: string, str: Sp): Promise<string> {
    return new Promise((resolve) => {
      if (url !== null && url.length > 0) {
        let httpRequest = http.createHttp();
        httpRequest.request(
          url,
          {
            method: http.RequestMethod.POST, // Optional. The default value is http.RequestMethod.GET.
            // You can add header fields based on service requirements.
            header: {
              'Content-Type': 'application/json'
            },
            // Use this field to transfer information in a POST request.
            extraData: str,
            priority: 1, // Optional. The default value is 1.
            connectTimeout: 60000, // Optional. The default value is 60000, in ms.
            readTimeout: 60000, // Optional. The default value is 60000, in ms.
          },
          (err, data) => {
            hilog.info(0x0000, 'TAG', `test... ${JSON.stringify(err)}`);
            if (!err) {
              hilog.info(0x0000, 'TAG', `test... ${JSON.stringify(data)}`);
              let result = data.result.toString();
              httpRequest.destroy();
              resolve(result);
            }
          }
        );
      }
    });
  }

  // Convert byte streams into strings in plaintext.
  public static uint8ArrayToString(array: Uint8Array) {
    let arrayString = '';
    for (let i = 0; i < array.length; i++) {
      arrayString += String.fromCharCode(array[i]);
    }
    return arrayString;
  }
}
