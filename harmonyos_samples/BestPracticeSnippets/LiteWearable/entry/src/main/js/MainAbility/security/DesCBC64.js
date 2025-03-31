// [Start 1]
// [Start 2]
import huks from '@ohos.security.huks';

const KEY_ALIAS = 'DesCBC64KeyAlias';
// [StartExclude 2]
function isKeyItemExist() {
    let huksInfo;
    let emptyOptions = {
        properties: []
    };

    huks.isKeyItemExist(KEY_ALIAS, emptyOptions, function (err, data) {
        if (data) {
            huksInfo = "The key:" + KEY_ALIAS + " exists";
        } else {
            huksInfo = "The key doesn't exist errcode:" + err.code + " ： " + err.message;
        }
    });

    return huksInfo;
}
// [End 1]
// [EndExclude 2]
function deleteKeyProcess() {
    let huksInfo;
    let emptyOptions = {
        properties: []
    };
    huks.deleteKeyItem(KEY_ALIAS, emptyOptions, function (err, data) {
        if (err) {
            huksInfo = "deleteKeyItem error return code:" + err.code + " ： " + err.message;
        } else {
            huksInfo = "The key has been deleted";
        }
    })
    return huksInfo;
}
// [End 2]