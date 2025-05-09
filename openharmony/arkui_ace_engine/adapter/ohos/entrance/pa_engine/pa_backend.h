/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_PA_BACKEND_H
#define FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_PA_BACKEND_H

#include <string>
#include <unordered_map>

#include "abs_shared_result_set.h"
#include "data_ability_predicates.h"
#include "form_provider_info.h"
#include "iremote_object.h"
#include "values_bucket.h"
#include "want.h"

#include "adapter/ohos/entrance/pa_engine/engine/common/js_backend_asset_manager.h"
#include "adapter/ohos/entrance/pa_engine/engine/common/js_backend_engine.h"
#include "base/memory/ace_type.h"
#include "base/utils/string_utils.h"
#include "core/common/backend.h"
#include "frameworks/bridge/common/manifest/manifest_parser.h"

namespace OHOS::Ace {

class PaBackend : public Backend {
    DECLARE_ACE_TYPE(PaBackend, Backend);

public:

    PaBackend() = default;
    ~PaBackend() noexcept override;

    bool Initialize(BackendType type, SrcLanguage language) override;

    void LoadEngine(const char* libName, int32_t instanceId) override;

    void UpdateState(Backend::State state) override;

    void SetJsEngine(const RefPtr<JsBackendEngine>& jsBackEngine)
    {
        jsBackendEngine_ = jsBackEngine;
    }

    void SetAssetManager(const RefPtr<AssetManager>& assetManager) override;

    void RunPa(const std::string& url, const OHOS::AAFwk::Want& want);
    void OnCreate(const OHOS::AAFwk::Want& want);
    void OnDelete(const int64_t formId);
    void OnTriggerEvent(const int64_t formId, const std::string& message);
    void OnUpdate(const int64_t formId);
    void OnCastTemptoNormal(const int64_t formId);
    void OnVisibilityChanged(const std::map<int64_t, int32_t>& formEventsMap);
    int32_t OnAcquireFormState(const OHOS::AAFwk::Want& want);
    sptr<IRemoteObject> OnConnect(const OHOS::AAFwk::Want& want);
    void OnDisConnect(const OHOS::AAFwk::Want& want);

    int32_t Insert(const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value);
    std::shared_ptr<OHOS::NativeRdb::AbsSharedResultSet> Query(const Uri& uri, const std::vector<std::string>& columns,
        const OHOS::NativeRdb::DataAbilityPredicates& predicates);
    int32_t Update(const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value,
        const OHOS::NativeRdb::DataAbilityPredicates& predicates);
    int32_t Delete(const Uri& uri, const OHOS::NativeRdb::DataAbilityPredicates& predicates);

    int32_t BatchInsert(const Uri& uri, const std::vector<OHOS::NativeRdb::ValuesBucket>& values);
    std::string GetType(const Uri& uri);
    std::vector<std::string> GetFileTypes(const Uri& uri, const std::string& mimeTypeFilter);
    int32_t OpenFile(const Uri& uri, const std::string& mode);
    int32_t OpenRawFile(const Uri& uri, const std::string& mode);
    Uri NormalizeUri(const Uri& uri);
    Uri DenormalizeUri(const Uri& uri);
    std::shared_ptr<AppExecFwk::PacMap> Call(const Uri& uri,
        const std::string& method, const std::string& arg, const AppExecFwk::PacMap& pacMap);
    bool OnShare(int64_t formId, OHOS::AAFwk::WantParams &wantParams);

    AppExecFwk::FormProviderData GetFormData() const
    {
        if (jsBackendEngine_) {
            return jsBackendEngine_->GetFormData();
        } else {
            LOGE("PA: PaBackend::jsBackendEngine_ is null");
            return AppExecFwk::FormProviderData("");
        }
    }

    void OnCommand(const OHOS::AAFwk::Want &want, int startId);

private:
    void ParseManifest();
    void LoadPa(const std::string& url, const OHOS::AAFwk::Want& want);

    BackendType type_ = BackendType::SERVICE;
    SrcLanguage language_ = SrcLanguage::ETS;

    RefPtr<Framework::ManifestParser> manifestParser_ = nullptr;
    RefPtr<AssetManager> assetManager_ = nullptr;
    RefPtr<JsBackendAssetManager> jsBackendAssetManager_ = nullptr;
    RefPtr<JsBackendEngine> jsBackendEngine_ = nullptr;

    mutable std::once_flag onceFlag_;
    std::mutex LoadPaMutex_;
    std::condition_variable condition_;
    bool isStagingPageExist_ = false;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_PA_BACKEND_H
