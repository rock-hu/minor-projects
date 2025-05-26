/**
 * 最佳实践：应用安全编码实践
 * 场景二十八：避免直接使用不可信数据来拼接SQL语句
 */

// [Start untrusted_data_to_sql]
import { BusinessError } from '@kit.BasicServicesKit';

import { relationalStore } from '@kit.ArkData'

// [StartExclude untrusted_data_to_sql]
const uiContext: UIContext | undefined = AppStorage.get('uiContext');
let context = uiContext!.getHostContext()!;
// [EndExclude untrusted_data_to_sql]

// The sql parameters are from external input
function exesql(sql: string) {
  const STORE_CONFIG: relationalStore.StoreConfig = {
    name: "RdbTest.db",
    securityLevel: relationalStore.SecurityLevel.S1
  };
  let store: relationalStore.RdbStore | undefined = undefined;
  relationalStore.getRdbStore(context, STORE_CONFIG, (err: BusinessError, rdbStore: relationalStore.RdbStore) => {
    store = rdbStore;
    if (err) {
      console.error(`Get RdbStore failed, code is ${err.code},message is ${err.message}`);
      return;
    }
    console.info('Get RdbStore successfully.');
  })
  // The external input is used to directly concatenate the SQL statement, and the verification is not performed
  let SQL_DELETE_TABLE = "DELETE FROM test WHERE name = " + sql;
  if (store != undefined) {
    (store as relationalStore.RdbStore).executeSql(SQL_DELETE_TABLE, (err) => {
      if (err) {
        console.error(`ExecuteSql failed, code is ${err.code},message is ${err.message}`);
        return;
      }
      console.info('Delete table done.');
    })
  }
}
// [End untrusted_data_to_sql]

/**
 * 最佳实践：应用安全编码实践
 * 场景二十九：避免使用未校验的外部数据拼接文件路径
 */

// [Start untrusted_data_to_file_name]
// The fileName is from external
function readFile(fileName: string) {
  let filePath = "/data/storage/";
  if (fileName.indexOf("..") === -1) { // Anti-splicing verification
    let fileAllPath: string = filePath + fileName
    // Service processing
  } else {
    console.info("be attacked")
    return
  }
}
// [End untrusted_data_to_file_name]