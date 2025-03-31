// [Start 1]
import screenLock from '@ohos.screenLock';

// [StartExclude 1]
export default {
    data: {
        title: ""
    },
    onInit() {
        this.title = "Hello World";
    }
};

// [EndExclude 1]

function unlockScreen() {
    let result;
    screenLock.unlockScreen(function (err) {
        if (err) {
            result = `Failed to unlock the screen, Code: ${err.code}, ${err.message}`;
        } else {
            result = `call unlockScreen sucess`;
        }
    });

    return result;
}
// [End 1]
// [Start 2]
function isScreenLocked() {
    let isLocked = false;
    let result;
    screenLock.isScreenLocked(function (err, data) {
        if (err) {
            result = `call isScreenLocked erro ${err.message}`;
        } else {
            isLocked = data
            result = `call isScreenLocked sucess islocked: ${isLocked}`
        }
    });
    return result;
}
// [End 2]
// [Start 3]
function isSecureMode() {
    let result;
    let isSafety = false;
    screenLock.isSecureMode(function (err, data) {
        if (err) {
            result = `call isSecureMode erro ${err.message}`;
        } else {
            isSafety = data
            result = `call isSecureMode sucess isSafety ${isSafety}`;
        }
    });
    return result;
}
// [End 3]