/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import dataRdb from '@ohos.data.relationalStore'
import { Context } from '@ohos.abilityAccessCtrl';
import { LogBackEntity } from './LogBackEntity';

export class RdbModel {
    private rdbStore: dataRdb.RdbStore = null
    private tableName: string = ''
    private sqlCreateTable: string = ''
    private columns: Array<string> = []
    private storeConfig: dataRdb.StoreConfig
    private context: Context

    constructor(context: Context, storeName: string, tableName: string, sqlCreateTable: string, columns: Array<string>) {
        this.tableName = tableName
        this.sqlCreateTable = sqlCreateTable
        this.columns = columns
        this.storeConfig = { name: storeName ,securityLevel:dataRdb.SecurityLevel.S1}
        this.context = context
    }
    // file path /data/app/el2/100/database/cn.openharmony.logback/entry/rdb/logback_rdb.db
    getRdbStore(callback) {
        if (this.rdbStore != null || !!!this.context) {
            callback()
            return;
        }
        dataRdb.getRdbStore(this.context, this.storeConfig, (err, rdb) => {
            this.rdbStore = rdb
            this.rdbStore.executeSql(this.sqlCreateTable, null, function (err) {
                if (err) {
                    console.info("ExecuteSql failed, err: " + err)
                    return
                }
                callback()
            })
        })
    }

    insertData(entity: LogBackEntity) {
        const valueBucket = {
            'level': entity.level,
            'msg': entity.msg,
        }
        if (this.context)
        dataRdb.getRdbStore(this.context, this.storeConfig, (err, rdb) => {
            if (err) {
                console.info("Get RdbStore failed, err: " + err)
                return;
            }
            rdb.insert(this.tableName, valueBucket, (err, ret) => {
                if (err) {
                    console.info("Insert is failed, err: " + err);
                    return;
                }
            })
        })
    }

    query(predicates: dataRdb.RdbPredicates) {
        let self = this
        if (this.context)
        dataRdb.getRdbStore(this.context, this.storeConfig, (err, rdb) => {
            rdb.query(predicates, this.columns, (err, resultSet) => {
                if (err) {
                    console.info("Query failed, err: " + err)
                    return;
                }
                self.getListFromResultSet(resultSet)
            })
        })
    }

    getListFromResultSet(resultSet: dataRdb.ResultSet) {
        let array = []
        for (let i = 0;i < resultSet.rowCount; i++) {
            let entity = new LogBackEntity()
            entity.id = resultSet.getDouble(resultSet.getColumnIndex('id'))
            entity.level = resultSet.getString(resultSet.getColumnIndex('level'))
            entity.msg = resultSet.getString(resultSet.getColumnIndex('msg'))
            array.push(entity)
            resultSet.goToNextRow()
        }
        return array
    }
}