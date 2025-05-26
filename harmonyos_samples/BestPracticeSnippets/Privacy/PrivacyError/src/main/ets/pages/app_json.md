/**
* 最佳实践：应用安全编码实践
* 场景三十一：建议正确设置发布版本应用调试属性
  */
// [Start app_json]
// app.json5   
{  
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
  "debug": false,  
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
// [End app_json]