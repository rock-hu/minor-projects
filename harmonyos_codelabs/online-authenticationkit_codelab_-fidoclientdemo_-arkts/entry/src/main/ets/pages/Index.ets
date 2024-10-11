import { common } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { promptAction } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';
import { fido } from '@kit.OnlineAuthenticationKit';
import { ConnectService } from '../ConnectService';
import { Util } from '../util/Util';
import { FidoReg } from '../bean/FidoReg';

@Entry
@Component
struct Index {
  con = new ConnectService();
  uiContext: common.UIAbilityContext = getContext(this) as common.UIAbilityContext;

  build() {
    Row() {
      Column() {
        Button('Discover')
          .margin(20)
          .width(200)
          .onClick(async () => {
            try {
              let discoverData: fido.DiscoveryData = await fido.discover(this.uiContext);
              hilog.info(0x0000, 'TAG', `FidoTest-Index------discover: ${JSON.stringify(discoverData)}`);
              try {
                promptAction.showToast({
                  message: JSON.stringify(discoverData),
                  duration: 2000
                });
              } catch (error) {
                let message = (error as BusinessError).message;
                let code = (error as BusinessError).code;
                hilog.error(0x0000, 'TAG', `showToast args error code is ${code}, message is ${message}`);
              }
            } catch (error) {
              hilog.error(0x0000, 'TAG', `FidoTest-Index------discover: ${error}`);
              try {
                promptAction.showToast({
                  message: '' + error,
                  duration: 2000
                });
              } catch (error) {
                let message = (error as BusinessError).message;
                let code = (error as BusinessError).code;
                hilog.error(0x0000, 'TAG', `showToast args error code is ${code}, message is ${message}`);
              }
            }
          })

        Button('Enable')
          .margin(20)
          .width(200)
          .onClick(async () => {
            // Access the FIDO server and obtain the policy check packet.
            hilog.info(0x0000, 'TAG', 'FidoTest-Index------start');
            let message: string = await this.con.selectType('UAF_REG');
            let uafMessage: fido.UAFMessage = {
              uafProtocolMessage: message,
              additionalData: ''
            };
            // (Optional) Pass transmission channel parameters.
            let channelBinding: fido.ChannelBinding = {};
            try {
              hilog.info(0x0000, 'TAG', 'FidoTest-Index------call processUAFOperation');
              let messageResp: fido.UAFMessage = await fido.processUAFOperation(this.uiContext, uafMessage, channelBinding);
              let uafProtocolMessage: string = messageResp.uafProtocolMessage;
              hilog.info(0x0000, 'TAG', `FidoTest-Index------uafProtocolMessage: ${uafProtocolMessage}`);
              let par = Util.SpP('regResp', uafProtocolMessage);
              let regResp: string = await Util.connectFidoServer1(ConnectService.url + 'fidoreg', par);
              let isReg = regResp.includes('fidoreg');
              if (isReg) {
                let regPar: FidoReg = JSON.parse(regResp);
                let fidoreg = ConnectService.base64.decodeSync(regPar.fidoreg);
                if (fidoreg.length > 0) {
                  let uafMess = Util.uint8ArrayToString(fidoreg);
                  let uafMessage: fido.UAFMessage = {
                    uafProtocolMessage: uafMess,
                    additionalData: ''
                  };
                  try {
                    // Call notifyUAFResult to send the registration result notification.
                    fido.notifyUAFResult(this.uiContext, uafMessage).then(() => {
                      hilog.info(0x0000, 'TAG', 'FidoTest-Index------notify:success');
                    });
                  } catch (error) {
                    hilog.error(0x0000, 'TAG', `notify: ${error}`);
                  }

                  try {
                    promptAction.showToast({
                      message: 'UAF_REG:success',
                      duration: 2000
                    });
                  } catch (error) {
                    let message = (error as BusinessError).message;
                    let code = (error as BusinessError).code;
                    hilog.error(0x0000, 'TAG', `showToast args error code is ${code}, message is ${message}`);
                  }
                  hilog.info(0x0000, 'TAG', 'UAF_REG:success');
                }
              }
            } catch (error) {
              try {
                promptAction.showToast({
                  message: '' + error,
                  duration: 2000
                });
              } catch (error) {
                let message = (error as BusinessError).message;
                let code = (error as BusinessError).code;
                hilog.error(0x0000, 'TAG', `showToast args error code is ${code}, message is ${message}`);
              }
              hilog.error(0x0000, 'TAG', `processUAFOperation args error code is ${error.code}, message is ${error.message}`);
            }
          });

        Button('Authenticate')
          .margin(20)
          .width(200)
          .onClick(async () => {
            let message: string = await this.con.selectType('UAF_AUTH');
            let uafMessage: fido.UAFMessage = {
              uafProtocolMessage: message,
              additionalData: ''
            };
            let channelBinding: fido.ChannelBinding = {};
            try {
              let messageResp: fido.UAFMessage = await fido.processUAFOperation(this.uiContext, uafMessage, channelBinding);
              let uafProtocolMessage: string = messageResp.uafProtocolMessage;
              hilog.info(0x0000, 'TAG', `FidoTest-Index------uafProtocolMessage: ${uafProtocolMessage}`);
              let par = Util.SpP('loginAuthResp', uafProtocolMessage);
              let authResp: string = await Util.connectFidoServer1(ConnectService.url + 'fidoauth', par);
              let isAuth = authResp.includes('userinfo');
              if (isAuth) {
                try {
                  promptAction.showToast({
                    message: 'UAF_AUTH:success',
                    duration: 2000
                  });
                } catch (error) {
                  let message = (error as BusinessError).message;
                  let code = (error as BusinessError).code;
                  hilog.error(0x0000, 'TAG', `showToast args error code is ${code}, message is ${message}`);
                }
                hilog.info(0x0000, 'TAG', 'UAF_AUTH:success');
              }
            } catch (error) {
              try {
                promptAction.showToast({
                  message: '' + error,
                  duration: 2000
                });
              } catch (error) {
                let message = (error as BusinessError).message;
                let code = (error as BusinessError).code;
                hilog.error(0x0000, 'TAG', `showToast args error code is ${code}, message is ${message}`);
              }
              hilog.error(0x0000, 'TAG', `processUAFOperation args error code is ${error.code}, message is ${error.message}`);
            }
          });

        Button('Disable')
          .margin(20)
          .width(200)
          .onClick(async () => {
            let message: string = await this.con.selectType('UAF_DEREG');
            let uafMessage: fido.UAFMessage = {
              uafProtocolMessage: message,
              additionalData: ''
            };
            let channelBinding: fido.ChannelBinding = {};
            try {
              let messageResp: fido.UAFMessage = await fido.processUAFOperation(this.uiContext, uafMessage, channelBinding);
              let uafProtocolMessage: string = messageResp.uafProtocolMessage;
              hilog.info(0x0000, 'TAG', `FidoTest-Index------uafProtocolMessage: ${uafProtocolMessage}`);
              try {
                promptAction.showToast({
                  message: 'UAF_DEREG:success',
                  duration: 2000
                });
              } catch (error) {
                let message = (error as BusinessError).message;
                let code = (error as BusinessError).code;
                hilog.error(0x0000, 'TAG', `showToast args error code is ${code}, message is ${message}`);
              }
            } catch (error) {
              try {
                promptAction.showToast({
                  message: '' + error,
                  duration: 2000
                });
              } catch (error) {
                let message = (error as BusinessError).message;
                let code = (error as BusinessError).code;
                hilog.error(0x0000, 'TAG', `showToast args error code is ${code}, message is ${message}`);
              }
              hilog.error(0x0000, 'TAG', `processUAFOperation args error code is ${error.code}, message is ${error.message}`);
            }
          });

        Button('Check policy')
          .margin(20)
          .width(200)
          .onClick(async () => {
            let message: string = await this.con.selectType('UAF_AUTH');
            let uafMessage: fido.UAFMessage = {
              uafProtocolMessage: message,
              additionalData: ''
            };
            try {
              await fido.checkPolicy(this.uiContext, uafMessage).then(() => {
                hilog.info(0x0000, 'TAG', 'FidoTest-Index------check policy-success');
                try {
                  promptAction.showToast({
                    message: 'Enabled.',
                    duration: 2000
                  });
                } catch (error) {
                  let message = (error as BusinessError).message;
                  let code = (error as BusinessError).code;
                  hilog.error(0x0000, 'TAG', `showToast args error code is ${code}, message is ${message}`);
                }
                ;
              })
            } catch (error) {
              try {
                promptAction.showToast({
                  message: '' + error,
                  duration: 2000
                });
              } catch (error) {
                let message = (error as BusinessError).message;
                let code = (error as BusinessError).code;
                hilog.error(0x0000, 'TAG', `showToast args error code is ${code}, message is ${message}`);
              }
              hilog.error(0x0000, 'TAG', `checkPolicy args error code is ${error.code}, message is ${error.message}`);
            }
          });
      }
      .width('100%')
    }
    .height('100%')
  }
}