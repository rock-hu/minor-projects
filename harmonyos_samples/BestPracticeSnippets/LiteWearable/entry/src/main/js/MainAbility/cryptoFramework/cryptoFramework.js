// [Start 1]
import cryptoFramework from '@ohos.security.cryptoFramework';
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
function stringToUint8Array(str) {
    let arr = [];
    for (let i = 0, j = str.length; i < j; ++i) {
        arr.push(str.charCodeAt(i));
    }
    let tmpUint8Array = new Uint8Array(arr);
    return tmpUint8Array;
}

function doMd() {
    let mdAlgName = 'SHA256'; // 摘要算法名
    let message = 'mdTestMessage'; // 待摘要的数据
    let handle;
    let mdResult;
    let mdLen;
    //指定摘要算法SHA256，生成摘要操作实例
    try {
        handle = cryptoFramework.createMd(mdAlgName);
    } catch (error) {
        console.error(`createMd error, code: ${error.code}, msg: ${error.message}`);
    }
    try {
        // 数据量较少时，可以只做一次update，将数据全部传入，接口未对入参长度做限制
        handle?.updateSync({ data: stringToUint8Array(message) });
    } catch (error) {
        console.error(`updateSync error, code:+${error.code}, msg: ${error.message}`);
    }
    // 获取摘要计算结果。
    try {
        mdResult = handle?.digest();
    } catch (error) {
        console.error(`digest error, code: ${error.code}, msg: ${error.message}`);
    }
    console.info('Md result:' + mdResult?.data);
    // 获取摘要计算长度，单位为字节
    try {
        mdLen = handle?.getMdLength();
    } catch (error) {
        console.error(`getMdLength error, code: ${error.code}, msg: ${error.message}`);
    }
    console.info(`md len: ${mdLen}`);
}
// [End 1]
// [Start 2]
function doRand() {
    let rand;
    let ret = true;
    let randData
    // 生成随机数操作实例
    try {
        rand = cryptoFramework.createRandom();
    } catch (error) {
        ret = false;
        console.error(`createRandom error, code:+${error.code}, msg: ${error.message}`);
    }
    let len = 24; // Generate a 24-byte random number.
    // (可选)调用Random.setSeed，为随机数生成池设置种子
    let seed = new Uint8Array([1, 2, 3]);
    try {
        rand?.setSeed({ data: seed });
    } catch (error) {
        ret = false;
        console.error(`setSeed error, code:+${error.code}, msg: ${error.message}`);
    }

    try {
        //生成安全随机数
        randData = rand?.generateRandomSync(len);
    } catch (error) {
        ret = false;
        console.error(`generateRandomSync error, code:+${error.code}, msg: ${error.message}`);
    }
    if (ret) {
        return randData?.data;
    } else {
        console.error(`doRand error`);
        return 'doRand error';
    }
}
// [End 2]