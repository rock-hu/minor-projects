import {userAuth} from '@kit.UserAuthenticationKit';
import {promptAction} from '@kit.ArkUI';
import IfaaModel from '../model/IfaaModel'


const ifaaAuthType = "fp";

function show(message: string)
{
  promptAction.showToast({
    message: message,
    duration: 2000
  });
}

@Entry
@Component
struct Index {
  build() {
    Row() {
      Column() {
        Button("Enable")
          .margin(20)
          .width(200)
          .onClick(async ()=>{
            try{
              await IfaaModel.register(ifaaAuthType);
              show("IIFAA register success");
            } catch (err) {
              show(err.message);
            }
          })

        Button("Authenticate")
          .margin(20)
          .width(200)
          .onClick(async ()=> {
            let ifaaChallenge:Uint8Array = IfaaModel.getChallenge();
            // Start biometric authentication. (This step is required in the IIFAA process but is not an IFAA capability.)
            let authParam : userAuth.AuthParam = {
              challenge: ifaaChallenge,
              authType: [userAuth.UserAuthType.FINGERPRINT],
              authTrustLevel: userAuth.AuthTrustLevel.ATL4,
            };
            let userAuthInstance = userAuth.getUserAuthInstance(authParam,  {title: ' '});
            userAuthInstance.on('result', {
              async onResult (result) {
                let authToken = result.token;
                try {
                  // After biometric features are successfully authenticated, start IFAA authentication.
                  await IfaaModel.auth(ifaaAuthType, authToken);
                  show("IIFAA auth success");
                } catch (err) {
                  show("IIFAA auth failed");
                }
              }
            });
            userAuthInstance.start();
          })

        Button("Disable")
          .margin(20)
          .width(200)
          .onClick(async ()=>{
            try{
              await IfaaModel.deregister(ifaaAuthType);
              show("deregister success");
            } catch (err) {
              show("IIFAA deregister failed");
            }
          })
      }.width('100%')
    }.height('100%')
  }
}
