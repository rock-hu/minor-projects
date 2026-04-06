/*
 *  Copyright (c) 2025 Huawei Device Co., Ltd.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

import { common } from '@kit.AbilityKit';
import { relationalStore } from '@kit.ArkData';
import { distributedDeviceManager } from '@kit.DistributedServiceKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import CommonConstants from '../common/CommonConstants';
import { AccountData } from '../viewmodel/BillViewModel';

// Relational data management category.
export class RdbManager {
  private rdbStore: relationalStore.RdbStore | null = null;
  context: common.UIAbilityContext | null = null;
  localDeviceNetworkId: string = '';

  constructor(context: common.UIAbilityContext) {
    this.context = context;
  }

  // Set distributed properties for data tables.
  async setDistributedTables(context: Context) {
    try {
      this.rdbStore = await relationalStore.getRdbStore(context, CommonConstants.STORE_CONFIG);
      await this.rdbStore.executeSql(CommonConstants.ACCOUNT_TABLE.sqlCreate);
      // Set the created table as a distributed table.
      await this.rdbStore.setDistributedTables([CommonConstants.ACCOUNT_TABLE.tableName]);
    } catch (error) {
      hilog.error(0x0000, 'RdbManager', `have error .Code:${error.code}, message: ${error.message}`);
    }
  }

  // Subscribe to data change messages from other devices within the network.
  subscribeDataChange() {
    if (this.rdbStore) {
      try {
        // Call the distributed data subscription interface to register observers for the database.
        // When data changes occur in a distributed database, a callback will be called.
        this.rdbStore.on('dataChange', relationalStore.SubscribeType.SUBSCRIBE_TYPE_REMOTE, async (devices) => {
          hilog.info(0x0000, 'hilog', CommonConstants.RDB_TAG, 'dataChange devices ' + devices);
          for (let i = 0; i < devices.length; i++) {
            let device = devices[i];
            if (!this.rdbStore) {
              return;
            }
            hilog.info(0x0000, 'hilog', CommonConstants.RDB_TAG, `The data of device:${device} has been changed.`);
            const result = await this.remoteQuery(device);
            this.context?.eventHub.emit('dataChange', JSON.stringify(result));
          }
        });
      } catch (err) {
        hilog.info(0x0000, 'hilog', CommonConstants.RDB_TAG,
          `Failed to register observer. Code:${err.code},message:${err.message}`);
      }
    }
  }

  // Get local device ID.
  getLocalDeviceNetworkId() {
    try {
      // create deviceManager.
      const deviceManager = distributedDeviceManager.createDeviceManager('com.example.crossDeviceDataSynchronization');
      // DeviceIds is obtained by calling the getAvailableDeviceListSync method by the DeviceManager.
      if (deviceManager != null) {
        this.localDeviceNetworkId = deviceManager.getLocalDeviceNetworkId();
        hilog.info(0x0000, 'hilog', CommonConstants.RDB_TAG,
          'local device networkId: ' + JSON.stringify(this.localDeviceNetworkId));
      }
    } catch (err) {
      hilog.error(0x0000, 'hilog', CommonConstants.RDB_TAG,
        `Failed to insert data. Code:${err.code}, message:${err.message}`);
    }
  }

  // Query the list of devices within the network.
  getDeviceList(): string[] {
    let deviceManager: distributedDeviceManager.DeviceManager;
    let deviceIds: string[] = [];
    try {
      // create deviceManager.
      deviceManager = distributedDeviceManager.createDeviceManager('com.example.crossDeviceDataSynchronization');
      // DeviceIds is obtained by calling the getAvailableDeviceListSync method by the DeviceManager.
      if (deviceManager != null) {
        let devices = deviceManager.getAvailableDeviceListSync();
        hilog.info(0x0000, 'hilog', CommonConstants.RDB_TAG, 'devices size ' + devices.length);
        for (let i = 0; i < devices.length; i++) {
          deviceIds[i] = devices[i].networkId as string;
          hilog.info(0x0000, 'hilog', CommonConstants.RDB_TAG, 'devices size ' + deviceIds[i]);
        }
      }
    } catch (err) {
      hilog.error(0x0000, 'hilog', CommonConstants.RDB_TAG,
        `Failed to insert data. Code:${err.code}, message:${err.message}`);
    }
    return deviceIds;
  }

  /**
   * Add new data
   * @param data add data
   * @param isSync isSync
   * @returns
   */
  async insertData(data: AccountData, isSync: boolean = true) {
    const insertData = this.generateBucket(data);
    let rowId: number = -1;
    if (this.rdbStore) {
      try {
        rowId = await this.rdbStore.insert(CommonConstants.ACCOUNT_TABLE.tableName, insertData);
        if (isSync) {
          await this.syncData();
        }
        hilog.info(0x0000, 'hilog', CommonConstants.RDB_TAG, `Succeeded in inserting data. rowId:${rowId}`);
      } catch (error) {
        hilog.error(0x0000, 'hilog', CommonConstants.RDB_TAG,
          `Failed to insert data. Code:${error.code}, message:${error.message}`);
      }
    }
    return rowId;
  }

  /**
   * Delete data
   * @param data Delete data
   * @param isSync isSync
   * @returns
   */
  async deleteData(data: AccountData, isSync: boolean = true) {
    let rows: number = 0;
    if (this.rdbStore) {
      try {
        let predicates = new relationalStore.RdbPredicates(CommonConstants.ACCOUNT_TABLE.tableName);
        predicates.equalTo('id', data.id);
        rows = await this.rdbStore.delete(predicates);
        if (isSync) {
          await this.syncData();
        }
        hilog.info(0x0000, 'hilog', CommonConstants.RDB_TAG, `Succeeded in delete data. rows:${rows}`);
      } catch (error) {
        hilog.error(0x0000, 'hilog', CommonConstants.RDB_TAG,
          `Failed to delete data. Code:${error.code}, message:${error.message}`);
      }
    }
    return rows;
  }

  /**
   * Update Data
   * @param data update data
   * @param isSync isSync
   * @returns
   */
  async updateData(data: AccountData, isSync: boolean = true) {
    const valueBucket: relationalStore.ValuesBucket = this.generateBucket(data);
    let rows = 0;
    if (this.rdbStore) {
      try {
        let predicates = new relationalStore.RdbPredicates(CommonConstants.ACCOUNT_TABLE.tableName);
        predicates.equalTo('id', data.id);
        rows = await this.rdbStore.update(valueBucket, predicates);
        if (isSync) {
          await this.syncData();
        }
        hilog.info(0x0000, 'hilog', CommonConstants.RDB_TAG, `Succeeded in update data. rows:${rows}`);
      } catch (error) {
        hilog.error(0x0000, 'hilog', CommonConstants.RDB_TAG,
          `Failed to update data. Code:${error.code}, message:${error.message}`);
      }
    }
    return rows;
  }

  /**
   * query data
   * @param searchValue query text
   * @param isAll Do you want to query all data
   * @returns
   */
  async query(searchValue: string, isAll: boolean = true) {
    let result: AccountData[] = [];
    if (this.rdbStore) {
      try {
        let predicates = new relationalStore.RdbPredicates(CommonConstants.ACCOUNT_TABLE.tableName);
        if (!isAll) {
          predicates.equalTo('typeText', searchValue);
        }
        const resultSet: relationalStore.ResultSet =
          await this.rdbStore.query(predicates, CommonConstants.ACCOUNT_TABLE.columns);
        result = this.formatData(resultSet);
        hilog.info(0x0000, 'hilog', CommonConstants.RDB_TAG, `Succeeded in query data. resultSet:${resultSet}`);
        if (result.length > 0) {
          resultSet.close();
        }
      } catch (error) {
        hilog.error(0x0000, 'hilog', CommonConstants.RDB_TAG,
          `Failed to query data. Code:${error.code}, message:${error.message}`);
      }
    }
    return result;
  }

  // Synchronize data.
  async syncData() {
    if (this.rdbStore) {
      const deviceIds: string[] = this.getDeviceList();
      if (deviceIds.length === 0) {
        return;
      }
      // Construct predicate objects for synchronizing distributed tables.
      const predicates = new relationalStore.RdbPredicates(CommonConstants.ACCOUNT_TABLE.tableName);
      // Specify the list of devices to synchronize.
      predicates.inDevices(deviceIds);
      try {
        // Call the interface for synchronizing data to push the current device data changes to other devices in the network.
        const result = await this.rdbStore.sync(relationalStore.SyncMode.SYNC_MODE_PUSH, predicates);
        // Obtain synchronization results.
        for (let i = 0; i < result.length; i++) {
          const deviceId = result[i][0];
          const syncResult = result[i][1];
          if (syncResult === 0) {
            hilog.info(0x0000, 'hilog', CommonConstants.RDB_TAG, `device:${deviceId} sync success`);
          } else {
            hilog.error(0x0000, 'hilog', CommonConstants.RDB_TAG,
              `device:${deviceId} sync failed, status:${syncResult}`);
          }
        }
      } catch (err) {
        hilog.error(0x0000, 'hilog', CommonConstants.RDB_TAG,
          'Push data failed, code: ' + err.code + ', message: ' + err.message);
      }
    }
  }

  /**
   * Query remote device data within the network
   * @param device device id
   * @returns
   */
  public async remoteQuery(device: string) {
    let list: AccountData[] = [];
    if (this.rdbStore) {
      // Query the list of devices within the network.
      // Construct predicate objects for synchronizing distributed tables.
      const predicates = new relationalStore.RdbPredicates(CommonConstants.ACCOUNT_TABLE.tableName);
      try {
        // Query the distributed tables on devices within the network.
        const resultSet = await this.rdbStore.remoteQuery(device, CommonConstants.ACCOUNT_TABLE.tableName, predicates,
          CommonConstants.ACCOUNT_TABLE.columns);
        list = this.formatData(resultSet);
        if (list.length > 0) {
          resultSet.close();
        }
        hilog.info(0x0000, 'hilog', CommonConstants.RDB_TAG, 'Remote query success, row cout: ' + resultSet.rowCount);
        hilog.info(0x0000, 'hilog', CommonConstants.RDB_TAG,
          `ResultSet column names: ${resultSet.columnNames}, column count: ${resultSet.columnCount}`);
      } catch (err) {
        hilog.error(0x0000, 'hilog', CommonConstants.RDB_TAG,
          'Remote query failed, code: ' + err.code + ', message: ' + err.message);
      }
    }
    return list;
  }

  // Format database data.
  formatData(resultSet: relationalStore.ResultSet) {
    let list: AccountData[] = [];
    let count: number = resultSet.rowCount;
    if (count !== 0 && typeof count !== 'string') {
      try {
        resultSet.goToFirstRow();
        for (let i = 0; i < count; i++) {
          let tmp: AccountData = {
            id: '',
            accountType: 0,
            typeText: '',
            amount: 0
          };
          tmp.id = resultSet.getString(resultSet.getColumnIndex('id'));
          tmp.accountType = resultSet.getDouble(resultSet.getColumnIndex('accountType'));
          tmp.typeText = resultSet.getString(resultSet.getColumnIndex('typeText'));
          tmp.amount = resultSet.getDouble(resultSet.getColumnIndex('amount'));
          list[i] = tmp;
          resultSet.goToNextRow();
        }
      } catch (error) {
        hilog.error(0x0000, 'RdbManager', `have error .Code:${error.code},message: ${error.message}`);
      }
    }
    return list;
  }

  /**
   * Process data into ValuesBucket type
   * @param account
   * @returns
   */
  generateBucket(account: AccountData): relationalStore.ValuesBucket {
    let obj: relationalStore.ValuesBucket = {};
    obj.id = account.id;
    obj.accountType = account.accountType;
    obj.typeText = account.typeText;
    obj.amount = account.amount;
    return obj;
  }

  /**
   * Update local database
   * @param oldList
   * @param newList
   */
  updateLocalDataBase(oldList: Array<AccountData>, newList: Array<AccountData>) {
    const list: Array<AccountData> = newList.concat(oldList);
    const idList = new Set(list.map((item: AccountData) => item.id));
    idList.forEach((id: string) => {
      const isFindInOldList = oldList.find((item: AccountData) => id === item.id);
      const isFindInNewList = newList.find((item: AccountData) => id === item.id);
      if (!isFindInOldList) {
        const item: AccountData = list.find((item: AccountData) => id === item.id) || {} as AccountData;
        this.insertData(item, false);
      } else if (!isFindInNewList) {
        const item: AccountData = list.find((item: AccountData) => id === item.id) || {} as AccountData;
        this.deleteData(item, false);
      } else {
        const item: AccountData = newList.find((item: AccountData) => id === item.id) || {} as AccountData;
        this.updateData(item, false);
      }
    });
  }
}