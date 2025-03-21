# ohresmgr.h


## 概述

提供资源管理native侧获取资源的能力。

**系统能力：** SystemCapability.Global.ResourceManager

**库：** libohresmgr.so

**起始版本：** 12

**相关模块：**[Resourcemanager](resourcemanager.md)


## 汇总


### 函数

| 名称 | 描述 | 
| -------- | -------- |
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetMediaBase64](resourcemanager.md#oh_resourcemanager_getmediabase64) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, char \*\*resultValue, uint64_t \*resultLen, uint32_t density=0) | 通过指定资源ID，获取屏幕密度对应的media资源的Base64码。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetMediaBase64Data](resourcemanager.md#oh_resourcemanager_getmediabase64data) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, char \*\*resultValue, uint64_t \*resultLen, uint32_t density) | 通过指定资源ID，获取屏幕密度对应的media资源的Base64码。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetMediaBase64ByName](resourcemanager.md#oh_resourcemanager_getmediabase64byname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, char \*\*resultValue, uint64_t \*resultLen, uint32_t density=0) | 通过指定资源名称，获取屏幕密度对应的media资源的Base64码。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetMediaBase64DataByName](resourcemanager.md#oh_resourcemanager_getmediabase64databyname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, char \*\*resultValue, uint64_t \*resultLen, uint32_t density) | 通过指定资源名称，获取屏幕密度对应的media资源的Base64码。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetMedia](resourcemanager.md#oh_resourcemanager_getmedia) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, uint8_t \*\*resultValue, uint64_t \*resultLen, uint32_t density=0) | 通过指定资源ID，获取屏幕密度对应的media资源的内容。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetMediaData](resourcemanager.md#oh_resourcemanager_getmediadata) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, uint8_t \*\*resultValue, uint64_t \*resultLen, uint32_t density) | 通过指定资源ID，获取屏幕密度对应的media资源的内容。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetMediaByName](resourcemanager.md#oh_resourcemanager_getmediabyname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, uint8_t \*\*resultValue, uint64_t \*resultLen, uint32_t density=0) | 通过指定资源名称，获取屏幕密度对应的media资源的内容。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetMediaDataByName](resourcemanager.md#oh_resourcemanager_getmediadatabyname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, uint8_t \*\*resultValue, uint64_t \*resultLen, uint32_t density) | 通过指定资源名称，获取屏幕密度对应的media资源的内容。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetDrawableDescriptor](resourcemanager.md#oh_resourcemanager_getdrawabledescriptor) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, ArkUI_DrawableDescriptor \*\*drawableDescriptor, uint32_t density=0, uint32_t type=0) | 通过指定资源Id，获取屏幕密度对应的图标资源的DrawableDescriptor。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetDrawableDescriptorData](resourcemanager.md#oh_resourcemanager_getdrawabledescriptordata) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, ArkUI_DrawableDescriptor \*\*drawableDescriptor, uint32_t density, uint32_t type) | 通过指定资源Id，获取屏幕密度对应的图标资源的DrawableDescriptor。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetDrawableDescriptorByName](resourcemanager.md#oh_resourcemanager_getdrawabledescriptorbyname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, ArkUI_DrawableDescriptor \*\*drawableDescriptor, uint32_t density=0, uint32_t type=0) | 通过指定资源名称，获取屏幕密度对应的图标资源的DrawableDescriptor。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetDrawableDescriptorDataByName](resourcemanager.md#oh_resourcemanager_getdrawabledescriptordatabyname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, ArkUI_DrawableDescriptor \*\*drawableDescriptor, uint32_t density, uint32_t type) | 通过指定资源名称，获取屏幕密度对应的图标资源的DrawableDescriptor。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetSymbol](resourcemanager.md#oh_resourcemanager_getsymbol) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, uint32_t \*resultValue) | 通过指定资源ID，获取对应的symbol资源。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetSymbolByName](resourcemanager.md#oh_resourcemanager_getsymbolbyname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, uint32_t \*resultValue) | 通过指定资源名称，获取对应的symbol资源。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetLocales](resourcemanager.md#oh_resourcemanager_getlocales) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, char \*\*\*resultValue, uint32_t \*resultLen, bool includeSystem=false) | 获取语言列表。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetLocalesData](resourcemanager.md#oh_resourcemanager_getlocalesdata) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, char \*\*\*resultValue, uint32_t \*resultLen, bool includeSystem) | 获取语言列表。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetConfiguration](resourcemanager.md#oh_resourcemanager_getconfiguration) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, [ResourceManager_Configuration](_resource_manager___configuration.md) \*configuration) | 获取设备配置。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_ReleaseConfiguration](resourcemanager.md#oh_resourcemanager_releaseconfiguration) ([ResourceManager_Configuration](_resource_manager___configuration.md) \*configuration) | 释放OH_ResourceManager_GetConfiguration()方法申请的内存。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetString](resourcemanager.md#oh_resourcemanager_getstring) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, char \*\*resultValue,...) | 通过指定资源ID，获取对应的string资源。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetStringByName](resourcemanager.md#oh_resourcemanager_getstringbyname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, char \*\*resultValue,...) | 通过指定资源名称，获取对应的string资源。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetStringArray](resourcemanager.md#oh_resourcemanager_getstringarray) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, char \*\*\*resultValue, uint32_t \*resultLen) | 通过指定资源ID，获取字符串数组。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetStringArrayByName](resourcemanager.md#oh_resourcemanager_getstringarraybyname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, char \*\*\*resultValue, uint32_t \*resultLen) | 通过指定资源名称，获取字符串数组。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_ReleaseStringArray](resourcemanager.md#oh_resourcemanager_releasestringarray) (char \*\*\*resValue, uint32_t len) | 释放字符串数组内存。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetPluralString](resourcemanager.md#oh_resourcemanager_getpluralstring) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, uint32_t num, char \*\*resultValue) | 通过指定资源ID，获取对应的单复数字符串。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetPluralStringByName](resourcemanager.md#oh_resourcemanager_getpluralstringbyname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, uint32_t num, char \*\*resultValue) | 通过指定资源名称，获取对应的单复数字符串。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetIntPluralString](resourcemanager.md#oh_resourcemanager_getintpluralstring) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, uint32_t num, char \*\*resultValue,...) | 通过指定资源ID，获取对应的单复数字符串。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetDoublePluralString](resourcemanager.md#oh_resourcemanager_getdoublepluralstring) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, double num, char \*\*resultValue,...) | 通过指定资源ID，获取对应的单复数字符串。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetIntPluralStringByName](resourcemanager.md#oh_resourcemanager_getintpluralstringbyname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, uint32_t num, char \*\*resultValue,...) | 通过指定资源名称，获取对应的单复数字符串。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetDoublePluralStringByName](resourcemanager.md#oh_resourcemanager_getdoublepluralstringbyname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, double num, char \*\*resultValue,...) | 通过指定资源名称，获取对应的单复数字符串。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetColor](resourcemanager.md#oh_resourcemanager_getcolor) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, uint32_t \*resultValue) | 通过指定资源ID，获取对应的颜色值。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetColorByName](resourcemanager.md#oh_resourcemanager_getcolorbyname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, uint32_t \*resultValue) | 通过指定资源ID，获取对应的颜色值。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetInt](resourcemanager.md#oh_resourcemanager_getint) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, int \*resultValue) | 通过指定资源ID，获取对应的int值。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetIntByName](resourcemanager.md#oh_resourcemanager_getintbyname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, int \*resultValue) | 通过指定资源名称，获取对应的int值。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetFloat](resourcemanager.md#oh_resourcemanager_getfloat) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, float \*resultValue) | 通过指定资源ID，获取对应的float值。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetFloatByName](resourcemanager.md#oh_resourcemanager_getfloatbyname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, float \*resultValue) | 通过指定资源名称，获取对应的float值。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetBool](resourcemanager.md#oh_resourcemanager_getbool) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, uint32_t resId, bool \*resultValue) | 通过指定资源ID，获取对应的bool值。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_GetBoolByName](resourcemanager.md#oh_resourcemanager_getboolbyname) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*resName, bool \*resultValue) | 通过指定资源名称，获取对应的bool值。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_AddResource](resourcemanager.md#oh_resourcemanager_addresource) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*path) | 在应用程序运行时添加overlay资源。  | 
| [ResourceManager_ErrorCode](resourcemanager.md#resourcemanager_errorcode) [OH_ResourceManager_RemoveResource](resourcemanager.md#oh_resourcemanager_removeresource) (const [NativeResourceManager](rawfile.md#nativeresourcemanager) \*mgr, const char \*path) | 在应用程序运行时删除overlay资源。  | 
