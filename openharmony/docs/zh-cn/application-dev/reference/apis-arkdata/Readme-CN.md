# ArkData（方舟数据管理）

- ArkTS API<!--arkdata-arkts--> 
  - [@ohos.data.commonType (数据通用类型)](js-apis-data-commonType.md)
  - [@ohos.data.dataAbility (DataAbility谓词)](js-apis-data-ability.md)
  - [@ohos.data.dataSharePredicates (数据共享谓词)](js-apis-data-dataSharePredicates.md)
  - [@ohos.data.distributedDataObject (分布式数据对象)](js-apis-data-distributedobject.md)
  - [@ohos.data.distributedKVStore (分布式键值数据库)](js-apis-distributedKVStore.md)
  - [@ohos.data.preferences (用户首选项)](js-apis-data-preferences.md)
  - [@ohos.data.sendablePreferences (共享用户首选项)](js-apis-data-sendablePreferences.md)
  - @ohos.data.relationalStore (关系型数据库)<!--js-apis-data-relationalstore-->
    - [模块描述](arkts-apis-data-relationalStore.md)
    - [Functions](arkts-apis-data-relationalStore-f.md)
    - [Interface (RdbStore)](arkts-apis-data-relationalStore-RdbStore.md)
    - [Interface (ResultSet)](arkts-apis-data-relationalStore-ResultSet.md)
    - [Interface (Transaction)](arkts-apis-data-relationalStore-Transaction.md)
    - [Interfaces (其他)](arkts-apis-data-relationalStore-i.md)
    - [Class (RdbPredicates)](arkts-apis-data-relationalStore-RdbPredicates.md)
    - [Enums](arkts-apis-data-relationalStore-e.md)
    - [Types](arkts-apis-data-relationalStore-t.md)
  - [@ohos.data.sendableRelationalStore (共享关系型数据库)](js-apis-data-sendableRelationalStore.md)
  - [@ohos.data.unifiedDataChannel (标准化数据通路)](js-apis-data-unifiedDataChannel.md)
  - [@ohos.data.uniformDataStruct (标准化数据结构)](js-apis-data-uniformDataStruct.md)
  - [@ohos.data.uniformTypeDescriptor (标准化数据定义与描述)](js-apis-data-uniformTypeDescriptor.md)
  - [@ohos.data.ValuesBucket (数据集)](js-apis-data-valuesBucket.md)
  - [@ohos.data.intelligence (智慧数据平台)](js-apis-data-intelligence.md)
  <!--Del-->
  - [@ohos.application.DataShareExtensionAbility (数据共享扩展能力)(系统接口)](js-apis-application-dataShareExtensionAbility-sys.md)
  <!--DelEnd-->
  - [@ohos.data.cloudData (端云服务)](js-apis-data-cloudData.md)
  <!--Del-->
  - [@ohos.data.cloudData (端云服务)(系统接口)](js-apis-data-cloudData-sys.md)
  - [@ohos.data.cloudExtension (端云共享Extension)(系统接口)](js-apis-data-cloudExtension-sys.md)
  - [@ohos.data.dataShare (数据共享)(系统接口)](js-apis-data-dataShare-sys.md)
  - [@ohos.data.dataSharePredicates (数据共享谓词)(系统接口)](js-apis-data-dataSharePredicates-sys.md)
  - [@ohos.data.dataShareResultSet (数据共享结果集)(系统接口)](js-apis-data-DataShareResultSet-sys.md)
  - [@ohos.data.distributedKVStore (分布式键值数据库)(系统接口)](js-apis-distributedKVStore-sys.md)
  - [@ohos.data.relationalStore (关系型数据库)(系统接口)](js-apis-data-relationalStore-sys.md)
  <!--DelEnd-->
  - 已停止维护的接口<!--arkdata-arkts-dep-->
    - [@ohos.data.distributedData (分布式数据管理)](js-apis-distributed-data.md)
    - [@ohos.data.rdb (关系型数据库)](js-apis-data-rdb.md)
    - [@ohos.data.storage (轻量级存储)](js-apis-data-storage.md)
    - [@system.storage数据存储](js-apis-system-storage.md)
    - data/rdb
      - [resultSet结果集](js-apis-data-resultset.md)
- ArkTS 组件<!--arkdata-comp-->
  - [@ohos.data.UdmfComponents (内容卡片)](js-apis-data-UdmfComponents.md)
- C API<!--arkdata-c-->
  - 模块<!--arkdata-module-->
    - [Preferences](capi-preferences.md)
    - [RDB](capi-rdb.md)
    - [UDMF](capi-udmf.md)
  - 头文件<!--arkdata-headerfile-->
    - [oh_preferences.h](capi-oh-preferences-h.md)
    - [oh_preferences_err_code.h](capi-oh-preferences-err-code-h.md)
    - [oh_preferences_option.h](capi-oh-preferences-option-h.md)
    - [oh_preferences_value.h](capi-oh-preferences-value-h.md)
    - [data_asset.h](capi-data-asset-h.md)
    - [oh_cursor.h](capi-oh-cursor-h.md)
    - [oh_data_value.h](capi-oh-data-value-h.md)
    - [oh_data_values.h](capi-oh-data-values-h.md)
    - [oh_data_values_buckets.h](capi-oh-data-values-buckets-h.md)
    - [oh_predicates.h](capi-oh-predicates-h.md)
    - [oh_rdb_crypto_param.h](capi-oh-rdb-crypto-param-h.md)
    - [oh_rdb_transaction.h](capi-oh-rdb-transaction-h.md)
    - [oh_rdb_types.h](capi-oh-rdb-types-h.md)
    - [oh_value_object.h](capi-oh-value-object-h.md)
    - [oh_values_bucket.h](capi-oh-values-bucket-h.md)
    - [relational_store.h](capi-relational-store-h.md)
    - [relational_store_error_code.h](capi-relational-store-error-code-h.md)
    - [udmf.h](capi-udmf-h.md)
    - [udmf_err_code.h](capi-udmf-err-code-h.md)
    - [udmf_meta.h](capi-udmf-meta-h.md)
    - [uds.h](capi-uds-h.md)
    - [utd.h](capi-utd-h.md)
  - 结构体<!--arkdata-struct-->
    - [OH_Cursor](capi-oh-cursor.md)
    - [OH_Predicates](capi-oh-predicates.md)
    - [OH_VObject](capi-oh-vobject.md)
    - [OH_VBucket](capi-oh-vbucket.md)
    - [OH_Rdb_Config](capi-oh-rdb-config.md)
    - [OH_Rdb_Store](capi-oh-rdb-store.md)
    - [Rdb_DistributedConfig](capi-rdb-distributedconfig.md)
    - [Rdb_KeyInfo](capi-rdb-keyinfo.md)
    - [Rdb_KeyData](capi-rdb-keydata.md)
    - [Rdb_ChangeInfo](capi-rdb-changeinfo.md)
    - [Rdb_SubscribeCallback](capi-rdb-subscribecallback.md)
    - [Rdb_DataObserver](capi-rdb-dataobserver.md)
    - [Rdb_Statistic](capi-rdb-statistic.md)
    - [Rdb_TableDetails](capi-rdb-tabledetails.md)
    - [Rdb_ProgressDetails](capi-rdb-progressdetails.md)
    - [Rdb_ProgressObserver](capi-rdb-progressobserver.md)
    - [OH_Preferences](capi-oh-preferences.md)
    - [OH_PreferencesOption](capi-oh-preferencesoption.md)
    - [OH_PreferencesPair](capi-oh-preferencespair.md)
    - [OH_PreferencesValue](capi-oh-preferencesvalue.md)
    - [Data_Asset](capi-data-asset.md)
    - [OH_Data_Value](capi-oh-data-value.md)
    - [OH_Data_Values](capi-oh-data-values.md)
    - [OH_Data_VBuckets](capi-oh-data-vbuckets.md)
    - [OH_Rdb_CryptoParam](capi-oh-rdb-cryptoparam.md)
    - [OH_RDB_TransOptions](capi-oh-rdb-transoptions.md)
    - [OH_Rdb_Transaction](capi-oh-rdb-transaction.md)
    - [OH_Rdb_ConfigV2](capi-oh-rdb-configv2.md)
    - [OH_UdmfData](capi-oh-udmfdata.md)
    - [OH_UdmfRecord](capi-oh-udmfrecord.md)
    - [OH_UdmfRecordProvider](capi-oh-udmfrecordprovider.md)
    - [OH_UdmfProperty](capi-oh-udmfproperty.md)
    - [OH_Udmf_ProgressInfo](capi-oh-udmf-progressinfo.md)
    - [OH_UdmfGetDataParams](capi-oh-udmfgetdataparams.md)
    - [OH_UdmfOptions](capi-oh-udmfoptions.md)
    - [OH_UdsPlainText](capi-oh-udsplaintext.md)
    - [OH_UdsHyperlink](capi-oh-udshyperlink.md)
    - [OH_UdsHtml](capi-oh-udshtml.md)
    - [OH_UdsAppItem](capi-oh-udsappitem.md)
    - [OH_UdsFileUri](capi-oh-udsfileuri.md)
    - [OH_UdsPixelMap](capi-oh-udspixelmap.md)
    - [OH_UdsArrayBuffer](capi-oh-udsarraybuffer.md)
    - [OH_UdsContentForm](capi-oh-udscontentform.md)
    - [OH_Utd](capi-oh-utd.md)
- 错误码<!--arkdata-arkts-errcode-->
  - [关系型数据库错误码](errorcode-data-rdb.md)
  - [数据共享错误码](errorcode-datashare.md)
  - [分布式数据对象错误码](errorcode-distributed-dataObject.md)
  - [分布式键值数据库错误码](errorcode-distributedKVStore.md)
  - [用户首选项错误码](errorcode-preferences.md)
  - [统一数据管理框架错误码](errorcode-udmf.md)
  - [智慧数据平台错误码](errorcode-intelligence.md)

