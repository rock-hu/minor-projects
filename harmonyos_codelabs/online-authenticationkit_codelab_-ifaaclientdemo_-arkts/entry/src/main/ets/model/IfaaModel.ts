import {ifaa} from '@kit.OnlineAuthenticationKit'

/* sample data */
const tlvRegisterReqFace = "Registration packet (face) signed and delivered by the IIFAA central server.";
const tlvRegisterReqFp = "Registration packet (fingerprint) signed and delivered by the IIFAA central server.";
const tlvAuthReqFace = "Authentication packet (face) signed and delivered by the IIFAA central server.";
const tlvAuthReqFp = "Authentication packet (fingerprint) signed and delivered by the IIFAA central server.";
const tlvDeregisterReqFace = "Deregistration packet (face) signed and delivered by the IIFAA central server.";
const tlvDeregisterReqFp = "Deregistration packet (fingerprint) signed and delivered by the IIFAA central server.";
const queryReqFace = "Face token delivered by the IIFAA central server.";
const queryReqFp = "Fingerprint token delivered by the IIFAA central server.";

type DataPair = Record<string, [string, string, string, string]>;
const sampleData: DataPair = {
  "face": [tlvRegisterReqFace, tlvAuthReqFace, tlvDeregisterReqFace, queryReqFace],
  "fp": [tlvRegisterReqFp, tlvAuthReqFp, tlvDeregisterReqFp, queryReqFp],
};

export class IfaaModel {
  public register(dataType: string): Promise<Uint8Array|null> {
    // Check whether the IIFAA password-free authentication capability is enabled.
    if (sampleData[dataType] === null) {
      console.error("invalid parameters!")
      throw new Error("invalid parameters");
    }
    let data = sampleData[dataType];
    let registerRes = ifaa.queryStatusSync(this.stringToUint8Array(data[3]));
    if (registerRes === true) {
      throw new Error("iifaa already registered");
    }

    // Replace the data in the following code with the data signed and delivered by the IIFAA central server.
    return ifaa.register(this.hexToBytes(data[0]));
  }

  public auth(dataType: string, authToken: Uint8Array): Promise<Uint8Array|null> {
    if (authToken === null) {
      console.error("invalid parameters!")
      throw new Error("invalid parameters!");
    }
    let data = sampleData[dataType];
    // Replace the data in the following code with the data signed and delivered by the IIFAA central server.
    return ifaa.auth(authToken, this.hexToBytes(data[1]));
  }

  public deregister(dataType: string): Promise<void> {
    if (sampleData[dataType] === null) {
      console.error("invalid parameters!")
      throw new TypeError("Error message");
    }
    let data = sampleData[dataType];
    // Replace the data in the following code with the data signed and delivered by the IIFAA central server.
    return ifaa.deregister(this.hexToBytes(data[2]));
  }

  public getChallenge(): Uint8Array {
    return ifaa.preAuthSync();
  }

  private stringToUint8Array(str:string): Uint8Array {
    var arr = [];
    var ss = "";
    console.info("IIFAA: " + str.length);
    for (var i = 0, j = str.length + 1; i < j; ++i) {
      arr.push(str.charCodeAt(i));
      ss += str.charCodeAt(i);
    }
    var tmpArray = new Uint8Array(arr);
    console.info("IIFAA: value: " + ss);
    return tmpArray;
  }

  private hexToBytes(hex: string): Uint8Array {
    let bytes:number[] = [];
    for (let c = 0; c < hex.length; c += 2)
      bytes.push(parseInt(hex.substring(c, c+2), 16));
    return new Uint8Array(bytes);
  }

}
let ifaaWrapper = new IfaaModel();
export default ifaaWrapper as IfaaModel;