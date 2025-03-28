/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "ui_service_mgr_stub.h"

#include "ipc_skeleton.h"
#include "tokenid_kit.h"
#include "ui_service_mgr_xcollie.h"

namespace OHOS::Ace {
constexpr uint32_t UI_MGR_SERVICE_TIMEOUT = 5;

UIServiceMgrStub::UIServiceMgrStub()
{
    requestFuncMap_[REGISTER_CALLBACK] = &UIServiceMgrStub::RegisterCallBackInner;
    requestFuncMap_[UNREGISTER_CALLBACK] = &UIServiceMgrStub::UnregisterCallBackInner;
    requestFuncMap_[PUSH] = &UIServiceMgrStub::PushInner;
    requestFuncMap_[REQUEST] = &UIServiceMgrStub::RequestInner;
    requestFuncMap_[RETURN_REQUEST] = &UIServiceMgrStub::ReturnRequestInner;
}

UIServiceMgrStub::~UIServiceMgrStub()
{
    requestFuncMap_.clear();
}

int32_t UIServiceMgrStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    std::u16string descriptor = UIServiceMgrStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }
    auto itFunc = requestFuncMap_.find(code);
    if (itFunc != requestFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

bool UIServiceMgrStub::IsSystemApp()
{
    uint64_t accessTokenIDEx = IPCSkeleton::GetCallingFullTokenID();
    return Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(accessTokenIDEx);
}

int32_t UIServiceMgrStub::RegisterCallBackInner(MessageParcel& data, MessageParcel& reply)
{
    UIServiceMgrXCollie uiServiceMgrXCollie("UISERVICE_REGISTER_CALLBACK", UI_MGR_SERVICE_TIMEOUT);
    if (!IsSystemApp()) {
        return ERR_PERMISSION_DENIED;
    }
    std::shared_ptr<AAFwk::Want> want(data.ReadParcelable<AAFwk::Want>());
    if (want == nullptr) {
        return ERR_INVALID_VALUE;
    }

    auto object = data.ReadRemoteObject();
    if (object == nullptr) {
        return ERR_INVALID_VALUE;
    }

    auto uiService = iface_cast<IUIService>(object);
    int32_t result = RegisterCallBack(*want, uiService);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int32_t UIServiceMgrStub::UnregisterCallBackInner(MessageParcel& data, MessageParcel& reply)
{
    UIServiceMgrXCollie uiServiceMgrXCollie("UISERVICE_UNREGISTER_CALLBACK", UI_MGR_SERVICE_TIMEOUT);
    if (!IsSystemApp()) {
        return ERR_PERMISSION_DENIED;
    }
    std::shared_ptr<AAFwk::Want> want(data.ReadParcelable<AAFwk::Want>());
    if (want == nullptr) {
        return ERR_INVALID_VALUE;
    }
    int32_t result = UnregisterCallBack(*want);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int32_t UIServiceMgrStub::PushInner(MessageParcel& data, MessageParcel& reply)
{
    UIServiceMgrXCollie uiServiceMgrXCollie("UISERVICE_PUSH", UI_MGR_SERVICE_TIMEOUT);
    if (!IsSystemApp()) {
        return ERR_PERMISSION_DENIED;
    }
    std::shared_ptr<AAFwk::Want> want(data.ReadParcelable<AAFwk::Want>());
    if (want == nullptr) {
        return ERR_INVALID_VALUE;
    }
    const std::string& name = data.ReadString();
    const std::string& jsonPath = data.ReadString();
    const std::string& dataStr = data.ReadString();
    const std::string& extraData = data.ReadString();

    int32_t result = Push(*want, name, jsonPath, dataStr, extraData);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int32_t UIServiceMgrStub::RequestInner(MessageParcel& data, MessageParcel& reply)
{
    UIServiceMgrXCollie uiServiceMgrXCollie("UISERVICE_REQUEST", UI_MGR_SERVICE_TIMEOUT);
    if (!IsSystemApp()) {
        return ERR_PERMISSION_DENIED;
    }
    std::shared_ptr<AAFwk::Want> want(data.ReadParcelable<AAFwk::Want>());
    if (want == nullptr) {
        return ERR_INVALID_VALUE;
    }
    const std::string& name = data.ReadString();
    const std::string& dataStr = data.ReadString();
    int32_t result = Request(*want, name, dataStr);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int32_t UIServiceMgrStub::ReturnRequestInner(MessageParcel& data, MessageParcel& reply)
{
    UIServiceMgrXCollie uiServiceMgrXCollie("UISERVICE_RETURN_REQUEST", UI_MGR_SERVICE_TIMEOUT);
    if (!IsSystemApp()) {
        return ERR_PERMISSION_DENIED;
    }
    std::shared_ptr<AAFwk::Want> want(data.ReadParcelable<AAFwk::Want>());
    if (want == nullptr) {
        return ERR_INVALID_VALUE;
    }
    const std::string& source = data.ReadString();
    const std::string& dataStr = data.ReadString();
    const std::string& extraData = data.ReadString();
    int32_t result = ReturnRequest(*want, source, dataStr, extraData);
    reply.WriteInt32(result);
    return NO_ERROR;
}
}
