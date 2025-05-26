// [Start register_custom_schemes]
// 注册三方协议的配置，需要在Web内核初始化之前调用，否则会注册失败。
static napi_value RegisterCustomSchemes(napi_env env, napi_callback_info info) {
    OH_LOG_INFO(LOG_APP, "register custom schemes");
    OH_ArkWeb_RegisterCustomSchemes("custom", ARKWEB_SCHEME_OPTION_STANDARD | ARKWEB_SCHEME_OPTION_CORS_ENABLED | ARKWEB_SCHEME_OPTION_CODE_CACHE_ENABLED);
    return nullptr;
}
// [End register_custom_schemes]

// [Start rawfile_request]
// 在worker线程中读取rawfile，并通过ResourceHandler返回给Web内核。
void RawfileRequest::ReadRawfileDataOnWorkerThread() {
    OH_LOG_INFO(LOG_APP, "read rawfile in worker thread.");
    const struct UrlInfo {
        std::string resource;
        std::string mimeType;
    } urlInfos[] = {{"local.html", "text/html"},
                    {"local_script.js", "text/javascript"},
                    {"test-cc.js", "text/javascript"}
                    };


    if (!resourceManager()) {
        OH_LOG_ERROR(LOG_APP, "read rawfile error, resource manager is nullptr.");
        return;
    }


    RawFile *rawfile = OH_ResourceManager_OpenRawFile(resourceManager(), rawfilePath().c_str());
    if (!rawfile) {
        OH_ArkWebResponse_SetStatus(response(), 404);
    } else {
        OH_ArkWebResponse_SetStatus(response(), 200);
    }


    for (auto &urlInfo : urlInfos) {
        if (urlInfo.resource == rawfilePath()) {
            OH_ArkWebResponse_SetMimeType(response(), urlInfo.mimeType.c_str());
            break;
        }
    }


    if ("test-cc.js" == rawfilePath()) {
        OH_LOG_ERROR(LOG_APP, "OH_ArkWebResponse_SetHeaderByName ResponseDataID");
        OH_ArkWebResponse_SetHeaderByName(response(), "ResponseDataID", "0000000000001", true);
    }
    OH_ArkWebResponse_SetCharset(response(), "UTF-8");
}
// [End rawfile_request]