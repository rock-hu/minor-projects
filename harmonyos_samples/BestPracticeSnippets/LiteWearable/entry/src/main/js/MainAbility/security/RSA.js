// [Start rsa_generate]
// [Start rsa_decrypt]
import huks from '@ohos.security.huks';

// [Start rsa_encrypt]
// 别名，用于区分生成的KEY
const RSA_KEY_ALIAS = 'RSAKeyAlias';
// 自定义密钥长度必须在1024 -2048之间，且是8的倍数
const HUKS_RSA_KEY_SIZE_1024 = 1024;
// [StartExclude rsa_generate]
// 明文，加密前数据
let plainText = 'RSASSAdffssghCBC5612345612345192';
// 明文，加密前数据的长度
let plainTextLen = 32;
// 密文，存放加密后数据
let cipherText = '';
// 操作句柄
let handle;

// [EndExclude rsa_generate]
// [StartExclude rsa_encrypt]
// [StartExclude rsa_decrypt]
function getRSAGenProperties() {
    let properties = new Array();
    let index = 0;
    // 算法
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
        value: huks.HuksKeyAlg.HUKS_ALG_RSA
    };
    // 密钥长度
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_KEY_SIZE,
        value: HUKS_RSA_KEY_SIZE_1024
    };
    // 密钥用途
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_PURPOSE,
        value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT
    };
    return properties;
}

function generateRSAKey() {
    let huksInfo;
    let options = { properties: getRSAGenProperties() };
    huks.generateKeyItem(HUKS_RSA_KEY_SIZE_1024, options, (err, data) => {
        if (err) {
            huksInfo = 'generateRSAKey return code:' + err.code + ' ： ' + err.message;
        } else {
            huksInfo = 'The key has been generated:' + JSON.stringify(data);
        }
    });
    return huksInfo;
}

// [End rsa_generate]
// [EndExclude rsa_encrypt]
function getRSAEncryptProperties() {
    let properties = new Array();
    let index = 0;
    // 算法
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
        value: huks.HuksKeyAlg.HUKS_ALG_RSA
    };
    // 密钥长度
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_KEY_SIZE,
        value: HUKS_RSA_KEY_SIZE_1024
    };

    // 密钥用途
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_PURPOSE,
        value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT
    };
    //  密钥 PADDING方式
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_PADDING,
        value: huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5
    };
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_DIGEST,
        value: huks.HuksKeyDigest.HUKS_DIGEST_SHA256
    }
    return properties;
}

function encryptProcess() {
    let ret = true;
    let huksInfo;
    let initOptions = {
        properties: getRSAEncryptProperties(),
        inData: new Uint8Array()
    }
    let updateOptions = {
        properties: getRSAEncryptProperties(),
        inData: stringToUint8Array(plainText.substring(0, plainTextLen / 2))
    }
    let finishOptions = {
        properties: getRSAEncryptProperties(),
        inData: stringToUint8Array(plainText.substring(plainTextLen / 2, plainTextLen))
    }
    huks.initSession(RSA_KEY_ALIAS, initOptions, (initErr, initData) => {
        if (initErr) {
            huksInfo = 'encryptProcess initSession return code:' + initErr.code + ' ： ' + initErr.message;
            ret = false;
            huks.abortSession(initData.handle, initOptions, (abortErr, abortData) => {
                if (abortErr) {
                    huksInfo =
                        'encryptProcess init abortSession return code:' + abortErr.code + ' ： ' + abortErr.message;
                }
            });
        } else {
            handle = initData.handle;
        }
    });
    if (!ret) {
        return huksInfo;
    }

    huks.updateSession(handle, updateOptions, (updateErr, updateData) => {
        if (updateErr) {
            huksInfo = 'encryptProcess updateSession return code:' + updateErr.code + ' ： ' + updateErr.message;
            ret = false;
            huks.abortSession(handle, updateOptions, (abortErr, abortData) => {
                if (abortErr) {
                    huksInfo = 'encryptProcess updateSession abortSession return code:' + abortErr.code + ' ： ' +
                    abortErr.message;
                }
            });
        }
    });

    if (!ret) {
        return huksInfo;
    }

    huks.finishSession(handle, finishOptions, (finishErr, finishData) => {
        if (finishErr) {
            ret = false;
            huksInfo = 'encryptProcess finishSession return code:' + finishErr.code + ' ： ' + finishErr.message;
            huks.abortSession(handle, finishOptions, (abortErr, abortData) => {
                if (abortErr) {
                    huksInfo =
                        'encryptProcess finish  abortSession return code:' + abortErr.code + ' ： ' + abortErr.message;
                }
            });
        } else {
            // 密文接收
            cipherText = uint8ArrayToString(finishData.outData);
            huksInfo = cipherText;
        }
    });
    return huksInfo;
}

function uint8ArrayToString(fileData) {
    let dataString = '';
    for (let i = 0; i < fileData.length; i++) {
        dataString += String.fromCharCode(fileData[i]);
    }
    return dataString;
}

function stringToUint8Array(str) {
    let arr = [];
    for (let i = 0, j = str.length; i < j; ++i) {
        arr.push(str.charCodeAt(i));
    }
    return new Uint8Array(arr);
}

// [End rsa_encrypt]
// [EndExclude rsa_decrypt]
function getRSADecryptProperties() {
    let properties = new Array();
    let index = 0;
    // 算法
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
        value: huks.HuksKeyAlg.HUKS_ALG_RSA
    };
    // 密钥长度
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_KEY_SIZE,
        value: HUKS_RSA_KEY_SIZE_1024
    };

    // 密钥用途
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_PURPOSE,
        value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT
    };

    // 密钥 PADDING方式
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_PADDING,
        value: huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5
    };

    // 摘要算法
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_DIGEST,
        value: huks.HuksKeyDigest.HUKS_DIGEST_SHA256
    }

    return properties;
}

function decryptProcess() {
    // 加密密文长度
    let len = HUKS_RSA_KEY_SIZE_1024 / 8;
    let ret = true;
    let outPlainText;
    let huksInfo;
    let initOptions = {
        properties: getRSADecryptProperties(),
        inData: new Uint8Array()
    }
    let updateOptions = {
        properties: getRSADecryptProperties(),
        inData: stringToUint8Array(cipherText.substring(0, len / 2))
    }
    let finishOptions = {
        properties: getRSADecryptProperties(),
        inData: stringToUint8Array(cipherText.substring(len / 2, len))
    }
    huks.initSession(RSA_KEY_ALIAS, initOptions, (initErr, initData) => {
        if (initErr) {
            huksInfo = 'decryptProcess initSession return code:' + initErr.code + ' ： ' + initErr.message;
            ret = false;
            huks.abortSession(initData.handle, initOptions, (abortErr, abortData) => {
                if (abortErr) {
                    huksInfo =
                        'decryptProcess init abortSession return code:' + abortErr.code + ' ： ' + abortErr.message;
                }
            });
        } else {
            handle = initData.handle;
        }
    });
    if (!ret) {
        return huksInfo;
    }
    huks.updateSession(handle, updateOptions, (updateErr, updateData) => {
        if (updateErr) {
            huksInfo = 'decryptProcess updateSession return code:' + updateErr.code + ' ： ' + updateErr.message;
            ret = false;
            huks.abortSession(handle, updateOptions, (abortErr, abortData) => {
                if (abortErr) {
                    huksInfo = 'decryptProcess updateSession abortSession return code:' + abortErr.code + ' ： ' +
                    abortErr.message;
                }
            });
        }
    });

    if (!ret) {
        return huksInfo;
    }
    huks.finishSession(handle, finishOptions, (finishErr, finishData) => {
        if (finishErr) {
            ret = false;
            huksInfo = 'decryptProcess finishSession return code:' + finishErr.code + ' ： ' + finishErr.message;
            huks.abortSession(handle, finishOptions, (abortErr, abortData) => {
                if (abortErr) {
                    huksInfo =
                        'decryptProcess finish  abortSession return code:' + abortErr.code + ' ： ' + abortErr.message;
                }
            });
        } else {
            // 明文接收
            outPlainText = uint8ArrayToString(finishData.outData);
        }
    });
    if (!ret) {
        return huksInfo;
    } else {
        huksInfo = 'Success:' + outPlainText;
    }
    return huksInfo;
}

// [End rsa_decrypt]