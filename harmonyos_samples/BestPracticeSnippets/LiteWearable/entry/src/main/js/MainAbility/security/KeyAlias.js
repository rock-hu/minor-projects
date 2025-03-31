// [Start 1]
// [Start 2]
import huks from '@ohos.security.huks';

// [StartExclude 2]
// 密钥材料
let plainTextKey = new Uint8Array([
    0x1d, 0x2c, 0x3a, 0x4b, 0x5e, 0x6f, 0x7d, 0x8a, 0x9c, 0xab, 0xbc, 0xcd, 0xde, 0xef, 0xf1, 0x23
]);
// [EndExclude 2]
// 确定密钥别名
const KEY_ALIAS = 'keyAlias';

// [StartExclude 2]
// 封装密钥属性集和密钥材料
function getImportKeyProperties() {
    let properties = new Array();
    let index = 0;
    // 算法
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
        value: huks.HuksKeyAlg.HUKS_ALG_AES
    };
    // 秘钥长度 （128/192/256）
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_KEY_SIZE,
        value: huks.HuksKeySize.HUKS_AES_KEY_SIZE_128
    };
    // 秘钥用途,生成秘钥时使用可以限制秘钥的使用权限 （AES一般用于加密、解密）
    properties[index++] = {
        tag: huks.HuksTag.HUKS_TAG_PURPOSE,
        value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT
    };
    return properties
}

// 明文导入密钥
function importKey() {
    let huksInfo;
    let ret = true;
    let options = {
        properties: getImportKeyProperties(),
        inData: plainTextKey
    }
    huks.importKeyItem(KEY_ALIAS, options, function (initErr, initData) {
        if (initErr) {
            ret = false;
            huksInfo = "import key:" + initErr.code + " ： " + initErr.message;
            huks.abortSession(initData.handle, options, function (abortErr, abortData) {
                if (abortErr) {
                    huksInfo = "import key init abortSession return code:" + abortErr.code + " ： " + abortErr.message;
                }
            });
        } else {
            huksInfo = uint8ArrayToString(initData.outData);
        }
    });

    if (ret == false) {
        return huksInfo + "导入失败";
    }

    return huksInfo + "导入成功";
}
// [End 1]
// [EndExclude 2]
function exportKeyProcess() {
    let huksInfo;
    let emptyOptions = {
        properties: []
    }

    huks.exportKeyItem(KEY_ALIAS, emptyOptions, function (err, data) {
        if (err) {
            huksInfo = "exportKeyItem error return code:" + err.code + " ： " + err.message;
        } else {
            huksInfo = uint8ArrayToString(data.outData);
        }
    })

    return huksInfo;
}
// [End 2]

function uint8ArrayToString(fileData) {
    let dataString = '';
    for (let i = 0; i < fileData.length; i++) {
        dataString += String.fromCharCode(fileData[i]);
    }

    return dataString;
}