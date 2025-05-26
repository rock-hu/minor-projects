/**
* 最佳实践：应用安全编码实践
* 场景三十：建议正确设置发布版本应用调试属性错误示例
  */
// [Start app_json_error]
// app.json5   
{  
    "app": {  
    "bundleName": "com.application.music",  
    "vendor": "application",  
    "versionCode": 1,  
    "versionName": "1.0",  
    "minCompatibleVersionCode": 1,  
    "minAPIVersion": 7,  
    "targetAPIVersion": 8,  
    "apiReleaseType": "Release",  
    "debug": true, // debug字段设置
    "icon": "$media:app_icon",  
    "label": "$string:app_name",  
    "description": "$string:description_application",  
    "distributedNotificationEnabled": true,  
    "entityType": "game",  
    "car": {  
    "apiCompatibleVersion": 8  
     }  
    }  
}
// [End app_json_error]