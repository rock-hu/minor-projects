/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/web/web_data_detector_adapter.h"

#include <algorithm>
#include <string>
#include <vector>

#include "adapter/ohos/entrance/ace_container.h"
#include "base/utils/utf_helper.h"
#include "base/utils/utils.h"
#include "core/common/ai/data_detector_mgr.h"
#include "core/components_ng/pattern/rich_editor_drag/preview_menu_controller.h"
#include "core/components_ng/pattern/web/web_pattern.h"
#include "core/components/text_overlay/text_overlay_theme.h"
#include "core/components/web/resource/web_delegate.h"
#include "core/pipeline_ng/pipeline_context.h"


namespace OHOS::Ace::NG {
namespace {
const std::string SEPARATE_STRING = "\n";
const size_t SEP_LENGTH = 1;
const size_t MAX_DETECT_LENGTH = (1 << 30);

const std::string JS_DATA_DETECTOR = "arkWebEntityReplacer";
const std::string JS_DATA_DETECTOR_METHOD = "handleNativeResult";
const std::string DATA_DETECTOR_PROXY = "arkWebAceEntityReplacerProxy";
const std::string PROXY_METHOD_PROCESS_REQUEST = "processRequest";
const std::string PROXY_METHOD_CLICK_ENTITY = "clickEntity";

const std::string ALL_TEXT_DETECT_TYPES = "phoneNum,url,email,location,datetime";

const std::vector<std::string> TEXT_DETECT_LIST = {
    "phoneNum" ,"url" ,"email", "location", "datetime"
};

const std::unordered_map<std::string, std::string> TEXT_DETECT_MAP_TO_HREF = {
    { "phoneNum", "tel:" }, { "url", "" },
    { "email", "mailto:" }, { "location", "geo:" },
    { "datetime", "webcal:" }
};

const std::unordered_map<TextDecoration, std::string> TEXT_DECORATION_MAP = {
    {TextDecoration::NONE, ""},
    {TextDecoration::UNDERLINE, "underline"},
    {TextDecoration::OVERLINE, "overline"},
    {TextDecoration::LINE_THROUGH, "line-through"},
    {TextDecoration::INHERIT, "inherit"},
};

const std::unordered_map<TextDecorationStyle, std::string> TEXT_DECORATION_STYLE_MAP = {
    {TextDecorationStyle::SOLID, "solid"},
    {TextDecorationStyle::DOUBLE, "double"},
    {TextDecorationStyle::DOTTED, "dotted"},
    {TextDecorationStyle::DASHED, "dashed"},
    {TextDecorationStyle::WAVY, "wavy"},
    {TextDecorationStyle::INITIAL, "initial"},
    {TextDecorationStyle::INHERIT, "inherit"},
};

constexpr char COPY[] = "copy";
constexpr char SELECT_TEXT[] = "selectText";

constexpr Dimension PREVIEW_MENU_MARGIN_LEFT = 16.0_vp;
constexpr Dimension PREVIEW_MENU_MARGIN_RIGHT = 16.0_vp;
constexpr Dimension MENU_WIDTH = 224.0_vp;
} // namespace

WebDataDetectorAdapter::WebDataDetectorAdapter(const WeakPtr<Pattern>& pattern, size_t cacheSize)
{
    pattern_ = pattern;
    if (cacheSize > 0) {
        resultCache_ = AceType::MakeRefPtr<WebDataDetectorCache<std::string, DataDetectorResult>>(cacheSize);
    }
    TextDetectConfig defaultConfig;
    defaultConfig.types = ALL_TEXT_DETECT_TYPES;
    SetDataDetectorConfig(defaultConfig);
}

void WebDataDetectorAdapter::SetDataDetectorEnable(bool enable)
{
    TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::SetDataDetectorEnable: %{public}d", enable);
    newConfig_.enable = enable;
}

void WebDataDetectorAdapter::SetDataDetectorConfig(const TextDetectConfig& config)
{
    TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::UpdateDataDetectorConfig");
    newConfig_.types = config.types.empty() ? ALL_TEXT_DETECT_TYPES : config.types;
    newConfig_.color = config.entityColor.ToString();
    TAG_LOGD(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::UpdateDataDetectorConfig dataDetectType_ : %{public}s",
        newConfig_.types.c_str());

    auto itType = TEXT_DECORATION_MAP.find(config.entityDecorationType);
    auto itStyle = TEXT_DECORATION_STYLE_MAP.find(config.entityDecorationStyle);

    newConfig_.textDecorationStyle = config.entityDecorationColor.ToString() + " " +
                                     (itType != TEXT_DECORATION_MAP.end() ? itType->second : "") + " " +
                                     (itStyle != TEXT_DECORATION_STYLE_MAP.end() ? itStyle->second : "");

    TAG_LOGD(AceLogTag::ACE_WEB,
        "WebDataDetectorAdapter::UpdateDataDetectorConfig dataDetectTextDecorationStyle_ : %{public}s",
        newConfig_.textDecorationStyle.c_str());
    newConfig_.enablePreview = config.enablePreviewMenu;
    TAG_LOGD(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::UpdateDataDetectorConfig enablePreview : %{public}d",
        newConfig_.enablePreview);
}

void WebDataDetectorAdapter::Init()
{
    if (!IsAISupported()) {
        return;
    }
    bool flag = config_.enable != newConfig_.enable || !hasInit_;
    config_ = newConfig_;
    TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::Init config changed = %{public}d, enable = %{public}d", flag,
        config_.enable);
    ResetContextMap();
    if (!flag) {
        return;
    }
    if (config_.enable) {
        InitJSProxy();
        InitAIMenu();
    } else {
        ReleaseJSProxy();
    }
    SetNWebConfig();
    hasInit_ = true;
}

void WebDataDetectorAdapter::InitJSProxy()
{
    if (initDataDetectorProxy_) {
        return;
    }
    auto pattern = DynamicCast<WebPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(pattern);
    auto webId = pattern->GetWebId();
    TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::InitJSProxy WebId: %{public}d", webId);
    if (webId < 0) {
        return;
    }
    auto instanceId = Container::CurrentIdSafely();
    std::vector<std::string> methods = { PROXY_METHOD_PROCESS_REQUEST, PROXY_METHOD_CLICK_ENTITY };
    std::vector<std::function<void(const std::vector<std::string>&)>> funcs = {
        [weak = AceType::WeakClaim(this), instanceId](const std::vector<std::string>& param) {
            ContainerScope scope(instanceId);
            auto adapter = weak.Upgrade();
            if (adapter && param.size() > 0) {
                adapter->ProcessRequest(param[0]);
            }
        },
        [weak = AceType::WeakClaim(this), instanceId](const std::vector<std::string>& param) {
            ContainerScope scope(instanceId);
            auto adapter = weak.Upgrade();
            if (adapter && param.size() > 0) {
                adapter->ProcessClick(param[0]);
            }
        }
    };
    auto delegate = pattern->delegate_;
    CHECK_NULL_VOID(delegate);
    delegate->RegisterNativeJavaScriptProxy(DATA_DETECTOR_PROXY, methods, funcs, false, "", hasInit_);
    initDataDetectorProxy_ = true;
}

void WebDataDetectorAdapter::ReleaseJSProxy()
{
    if (!initDataDetectorProxy_) {
        return;
    }
    auto pattern = DynamicCast<WebPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(pattern);
    auto webId = pattern->GetWebId();
    TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::ReleaseJSProxy WebId: %{public}d", webId);
    if (webId < 0) {
        return;
    }
    auto delegate = pattern->delegate_;
    CHECK_NULL_VOID(delegate);
    delegate->UnRegisterNativeArkJSFunction(DATA_DETECTOR_PROXY); // with stability problems
    initDataDetectorProxy_ = false;
}

void WebDataDetectorAdapter::SetNWebConfig()
{
    auto pattern = DynamicCast<WebPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(pattern);
    auto webId = pattern->GetWebId();
    TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::SetNWebConfig WebId: %{public}d", webId);
    if (webId < 0) {
        return;
    }
    auto delegate = pattern->delegate_;
    CHECK_NULL_VOID(delegate);
    delegate->SetDataDetectorEnable(config_.enable);
}

void WebDataDetectorAdapter::ProcessRequest(const std::string& jsonStr)
{
    TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::ProcessRequest start");
    if (!GetDataDetectorEnable()) {
        return;
    }
    auto requestJson = JsonUtil::ParseJsonString(jsonStr);
    if (!requestJson || !requestJson->IsObject()) {
        return;
    }
    auto requestId = requestJson->GetString("requestId");
    if (requestId.empty()) {
        return;
    }
    TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::ProcessRequest requestId: %{public}s", requestId.c_str());
    DataDetectorRequestData requestData;
    requestData.requestId = requestId;
    auto nodesValue = requestJson->GetValue("nodes");
    if (nodesValue && nodesValue->IsArray()) {
        int32_t arraySize = nodesValue->GetArraySize();
        for (int32_t i = 0; i < arraySize; ++i) {
            auto nodeValue = nodesValue->GetArrayItem(i);
            if (!nodeValue || !nodeValue->IsObject()) {
                continue;
            }
            NodeData nodeData;
            nodeData.path = nodeValue->GetString("path");
            nodeData.text = nodeValue->GetString("text");
            requestData.nodes.emplace_back(nodeData);
        }
    }
    TAG_LOGD(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::ProcessRequest request node size: %{public}zu",
        requestData.nodes.size());
    if (requestData.nodes.empty()) {
        TAG_LOGE(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::ProcessRequest request node empty");
        return;
    }
    SetRequestContext(requestId, std::move(requestData));
    AIPostTask(
        [weak = AceType::WeakClaim(this), requestId]() {
            auto adapter = weak.Upgrade();
            CHECK_NULL_VOID(adapter);
            adapter->SendRequestToAI(requestId);
        },
        TaskExecutor::TaskType::UI, "SendRequestToAI");
}

std::string WebDataDetectorAdapter::PrepareDetectText(const std::string& requestId)
{
    TAG_LOGD(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::PrepareContext");
    auto requestContext = GetRequestContext(requestId);
    CHECK_NULL_RETURN(requestContext, "");
    size_t index = 0;
    std::vector<size_t> detectIds;
    std::string detectText = "";
    // u16 pair list
    std::vector<std::pair<size_t, size_t> > detectOffsets;
    size_t maxDetectLength = 0;
    std::vector<DataDetectorResult> matches(requestContext->nodes.size());
    for (const auto& node : requestContext->nodes) {
        DataDetectorResult result;
        if (resultCache_ && resultCache_->Get(node.text, result)) {
            matches[index] = result; // overwrite
        } else {
            detectIds.emplace_back(index);
            detectText += node.text + SEPARATE_STRING;
            auto wText = UtfUtils::Str8DebugToStr16(node.text);
            detectOffsets.emplace_back(maxDetectLength, maxDetectLength + wText.size());
            maxDetectLength += wText.size() + SEP_LENGTH;
        }
        ++index;
    }
    requestContext->matches = std::move(matches);
    requestContext->detectIds = std::move(detectIds);
    requestContext->detectOffsets = std::move(detectOffsets);
    return detectText;
}

void WebDataDetectorAdapter::SendRequestToAI(const std::string& requestId)
{
    TAG_LOGD(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::SendRequestToAI");
    // ui thread

    auto instanceId = Container::CurrentIdSafely();
    auto resultFunc = [weak = AceType::WeakClaim(this), requestId, instanceId](const TextDataDetectResult result) {
        // background thread
        ContainerScope scope(instanceId);
        auto adapter = weak.Upgrade();
        CHECK_NULL_VOID(adapter);
        adapter->HandleResultFromAI(requestId, result);
    };

    std::string detectText = PrepareDetectText(requestId);
    if (detectText.empty()) {
        TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::SendRequestToAI detectText empty");
        AIPostTask(
            [resultFunc]() {
                TextDataDetectResult emptyResult;
                emptyResult.entity = "{}";
                resultFunc(emptyResult);
            },
            TaskExecutor::TaskType::BACKGROUND, "NoDataDetect");
        return;
    }

    TextDataDetectInfo info { detectText, config_.types };
    AIPostTask(
        [info, resultFunc]() {
            TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::SendRequestToAI start AI detect, length: %{public}zu",
                info.text.size());
            DataDetectorMgr::GetInstance().DataDetect(info, resultFunc);
        },
        TaskExecutor::TaskType::BACKGROUND, "DataDetect");
}

void WebDataDetectorAdapter::ParseAIResultByType(std::shared_ptr<DataDetectorRequestData>& requestContext,
    const std::string& detectType, const std::unique_ptr<JsonValue>& jsonValue)
{
    CHECK_NULL_VOID(requestContext);
    int32_t arraySize = jsonValue->GetArraySize();
    for (int32_t i = 0; i < arraySize; ++i) {
        auto item = jsonValue->GetArrayItem(i);
        if (!item || !item->IsObject()) {
            continue;
        }
        auto charOffset = item->GetInt("charOffset");
        if (charOffset < 0) {
            TAG_LOGE(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::ParseAIResultByType charOffset invalid");
            continue;
        }
        EntityMatch mat;
        mat.start = static_cast<size_t>(charOffset); // u16 offset
        mat.clean = item->GetString("oriText"); // u8 string
        mat.end = mat.start + UtfUtils::Str8DebugToStr16(mat.clean).length(); // u16 offset not include
        mat.entityType = detectType;
        mat.params = ParseExtraParams(detectType, item);
        int pos = MatchInOffsets(mat, requestContext->detectOffsets);
        if (pos < 0) {
            continue;
        }
        size_t id = requestContext->detectIds[pos];
        if (id >= requestContext->matches.size()) {
            continue;
        }
        requestContext->matches[id].emplace_back(std::move(mat));
    }
}

void WebDataDetectorAdapter::HandleResultFromAI(const std::string& requestId, const TextDataDetectResult& result)
{
    TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::HandleResultFromAI receive AI result");
    // background thread
    auto entityJson = JsonUtil::ParseJsonString(result.entity);
    if (!entityJson || !entityJson->IsObject()) {
        TAG_LOGE(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::HandleResultFromAI entityJson invalid");
        entityJson = JsonUtil::ParseJsonString("{}");
    }
    auto requestContext = GetRequestContext(requestId);
    CHECK_NULL_VOID(requestContext);
    for (const auto& detectType : TEXT_DETECT_LIST) {
        if (auto jsonValue = entityJson->GetValue(detectType)) {
            if (!jsonValue->IsArray()) {
                continue;
            }
            ParseAIResultByType(requestContext, detectType, jsonValue);
        }
    }
    for (auto id : requestContext->detectIds) {
        if (id >= requestContext->matches.size()) {
            continue;
        }
        auto& nodeResult = requestContext->matches[id]; // overwrite
        std::sort(nodeResult.begin(), nodeResult.end(), [](const EntityMatch& a, const EntityMatch& b) {
            return a.start < b.start;
        });
        if (resultCache_) {
            resultCache_->Put(requestContext->nodes[id].text, nodeResult);
        }
    }
    auto resultStr = GetResultJsonString(requestId);
    RemoveRequestContext(requestId);
    AIPostTask(
        [weak = AceType::WeakClaim(this), resultStr]() {
            auto adapter = weak.Upgrade();
            CHECK_NULL_VOID(adapter);
            adapter->SendResultToJS(resultStr);
        },
        TaskExecutor::TaskType::UI, "SendResultToJS");
}

void WebDataDetectorAdapter::SendResultToJS(const std::string& resultStr)
{
    auto jsCode = JS_DATA_DETECTOR + "." + JS_DATA_DETECTOR_METHOD + "('" + resultStr + "')";
    auto pattern = DynamicCast<WebPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(pattern);
    pattern->RunJavascriptAsync(jsCode,  [](std::string result) {});
}

std::map<std::string, std::string> WebDataDetectorAdapter::AttrsToParams(const std::unique_ptr<JsonValue>& attrs)
{
    if (!attrs || !attrs->IsObject()) {
        return {};
    }
    std::map<std::string, std::string> params;
    for (auto key : extraParamKeys_) {
        auto value = attrs->GetString(key);
        if (!value.empty()) {
            params[key] = value;
        }
    }
    return params;
}

std::map<std::string, std::string> WebDataDetectorAdapter::ParseExtraParams(
    const std::string& detectType, const std::unique_ptr<JsonValue>& item)
{
    if (!item || !item->IsObject()) {
        return {};
    }
    auto type = ConvertTypeFromString(detectType);
    if (type == TextDataDetectType::INVALID) {
        return {};
    }
    std::map<std::string, std::string> params;
    switch (type) {
        case TextDataDetectType::DATE_TIME: {
            auto startTimestamp = item->GetInt64("startTimestamp", -1);
            if (startTimestamp != -1) {
                params["startTimestamp"] = std::to_string(startTimestamp);
            }
            break;
        }
        default:
            break;
    }
    return params;
}

int32_t WebDataDetectorAdapter::MatchInOffsets(
    EntityMatch& match, const std::vector<std::pair<size_t, size_t>>& detectOffsets)
{
    auto iter = std::upper_bound(
        detectOffsets.begin(), detectOffsets.end(), std::make_pair(match.start, MAX_DETECT_LENGTH));
    if (iter == detectOffsets.begin()) {
        return -1;
    }
    --iter;
    if (match.start >= iter->second || match.end <= iter->first || match.end > iter->second) {
        return -1;
    }
    match.start -= iter->first;
    match.end -= iter->first;
    return std::distance(detectOffsets.begin(), iter);
}

std::string WebDataDetectorAdapter::GetResultJsonString(const std::string& requestId)
{
    auto requestContext = GetRequestContext(requestId);
    CHECK_NULL_RETURN(requestContext, "");
    const auto& matches = requestContext->matches;

    auto resultJson = JsonUtil::Create(true);
    resultJson->Put("requestId", requestId.c_str());

    auto styleJson = JsonUtil::Create(false);
    styleJson->Put("color", config_.color.c_str());
    styleJson->Put("textDecoration", config_.textDecorationStyle.c_str());
    resultJson->PutRef("style", std::move(styleJson));

    auto matchesJson = JsonUtil::CreateArray(false);
    int32_t nodeIndex = 0;
    for (const auto& nodeMatch : matches) {
        auto nodeMatchJson = JsonUtil::Create(false);
        nodeMatchJson->Put("nodeIndex", nodeIndex++);
        auto matchArray = JsonUtil::CreateArray(false);
        for (const auto& match : nodeMatch) {
            auto matchJson = JsonUtil::Create(false);
            matchJson->Put("start", match.start);
            matchJson->Put("end", match.end);
            matchJson->Put("hrefType", TEXT_DETECT_MAP_TO_HREF.at(match.entityType).c_str());
            matchJson->Put("clean", match.clean.c_str());

            auto attrsJson = JsonUtil::Create(false);
            attrsJson->Put("OhosArkWebType", match.entityType.c_str());
            for (auto [k, v] : match.params) {
                attrsJson->Put(k.c_str(), v.c_str());
                extraParamKeys_.insert(k);
            }

            matchJson->PutRef("attrs", std::move(attrsJson));
            matchArray->PutRef(std::move(matchJson));
        }
        if (matchArray->GetArraySize() > 0) {
            nodeMatchJson->PutRef("entities", std::move(matchArray));
            matchesJson->PutRef(std::move(nodeMatchJson));
        }
    }
    resultJson->PutRef("matches", std::move(matchesJson));
    std::string resultStr = resultJson->ToString();
    return resultStr;
}

void WebDataDetectorAdapter::ProcessClick(const std::string& jsonStr)
{
    if (!GetDataDetectorEnable()) {
        return;
    }
    auto instanceId = Container::CurrentIdSafelyWithCheck();
    ContainerScope scope(instanceId);
    auto msg = JsonUtil::ParseJsonString(jsonStr);
    if (!msg || !msg->IsObject()) {
        return;
    }
    auto content = msg->GetString("content");
    auto outerHTML = msg->GetString("outerHTML");
    auto entityType = msg->GetString("entityType");
    if (auto touchTest = msg->GetBool("touchTest")) {
        SetPreviewMenuAttr(ConvertTypeFromString(entityType), content, AttrsToParams(msg->GetObject("attrs")));
        return;
    }
    SetPreviewMenuAttr();
    auto rectJson = msg->GetObject("rect");
    if (!rectJson || !rectJson->IsObject()) {
        return;
    }

    auto left = rectJson->GetDouble("left");
    auto top = rectJson->GetDouble("top");
    auto right = rectJson->GetDouble("right");
    auto bottom = rectJson->GetDouble("bottom");

    TAG_LOGI(AceLogTag::ACE_WEB,
        "WebDataDetectorAdapter::ProcessClick left= %{public}f, top= %{public}f, right= %{public}f, bottom= %{public}f",
        left, top, right, bottom);

    auto aiRect = CalcAIMenuRect(left, top, right, bottom);

    AIMenuInfo info {
        entityType,
        content,
        outerHTML,
        aiRect,
    };
    AIPostTask(
        [weak = AceType::WeakClaim(this), info, instanceId]() {
            ContainerScope scope(instanceId);
            auto adapter = weak.Upgrade();
            CHECK_NULL_VOID(adapter);
            if (adapter->ShowAIMenu(info)) {
                adapter->CloseOtherMenu();
            }
        },
        TaskExecutor::TaskType::UI, "ShowAIEntityMenuAsync");
}

void WebDataDetectorAdapter::InitAIMenu()
{
    if (initAIMenu_) {
        return;
    }
    GetAIMenu();
    MenuParam menuParam;
    menuParam.type = MenuType::CONTEXT_MENU;
    menuParam.contextMenuRegisterType = ContextMenuRegisterType::CUSTOM_TYPE;
    menuParam.previewMode = MenuPreviewMode::CUSTOM;
    PaddingProperty paddings;
    paddings.start = CalcLength(PREVIEW_MENU_MARGIN_LEFT);
    paddings.end = CalcLength(PREVIEW_MENU_MARGIN_RIGHT);
    menuParam.layoutRegionMargin = paddings;
    menuParam.disappearScaleToTarget = true;
    menuParam.isShow = true;

    auto param = std::make_shared<WebPreviewSelectionMenuParam>(
        WebElementType::AILINK, ResponseType::LONG_PRESS, nullptr, nullptr, menuParam);
    auto pattern = DynamicCast<WebPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(pattern);
    pattern->SetPreviewSelectionMenu(param);
    initAIMenu_ = true;
}

bool WebDataDetectorAdapter::IsAISupported()
{
    if (aiSupportStatus_ == AISupportStatus::UNKNOWN) {
        aiSupportStatus_ = DataDetectorMgr::GetInstance().IsDataDetectorSupported() ? AISupportStatus::SUPPORTED
                                                                                    : AISupportStatus::UNSUPPORTED;
    }
    return aiSupportStatus_ == AISupportStatus::SUPPORTED;
}

void WebDataDetectorAdapter::GetAIMenu()
{
    AIPostTask(
        [weak = AceType::WeakClaim(this)] () {
            auto adapter = weak.Upgrade();
            CHECK_NULL_VOID(adapter);
            TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::GetAIMenu getting menu from ai_engine");
            DataDetectorMgr::GetInstance().GetAIEntityMenu(adapter->textDetectResult_);
        },
        TaskExecutor::TaskType::UI, "GetAIMenu");
}

RectF WebDataDetectorAdapter::CalcAIMenuRect(double left, double top, double right, double bottom)
{
    auto pattern = DynamicCast<WebPattern>(pattern_.Upgrade());
    CHECK_NULL_RETURN(pattern, RectF());
    auto comX = pattern->GetHost()->GetTransformRelativeOffset().GetX();
    auto comY = pattern->GetHost()->GetTransformRelativeOffset().GetY();
    left = std::max(left + comX, 0.0);
    top = std::max(top + comY, 0.0);
    right = std::max(right + comX, 0.0);
    bottom = std::max(bottom + comY, 0.0);
    auto rect = RectF(left, top, right - left, bottom - top);
    return rect;
}

bool WebDataDetectorAdapter::GetAIMenuOptions(
    const AIMenuInfo& info, std::vector<std::pair<std::string, std::function<void()>>>& menuOptions)
{
    if (textDetectResult_.menuOptionAndAction.empty()) {
        GetAIMenu();
        return false;
    }
    auto instanceId = Container::CurrentIdSafely();
    menuOptions.clear();
    auto menuOptionAndAction = textDetectResult_.menuOptionAndAction[info.entityType];
    if (menuOptionAndAction.empty()) {
        TAG_LOGE(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::GetAIMenuOptions menuOption empty");
        return false;
    }
    TAG_LOGD(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::GetAIMenuOptions option size: %{public}zu",
        menuOptionAndAction.size());

    for (auto menuOption : menuOptionAndAction) {
        menuOptions.emplace_back(
            std::make_pair(menuOption.first, [weak = AceType::WeakClaim(this), info, menuOption, instanceId]() {
                ContainerScope scope(instanceId);
                auto adapter = weak.Upgrade();
                CHECK_NULL_VOID(adapter);
                adapter->OnClickAIMenuOption(info, menuOption);
            }));
    }
    return true;
}

bool WebDataDetectorAdapter::ShowAIMenu(const AIMenuInfo& info)
{
    std::vector<std::pair<std::string, std::function<void()>>> menuOptions;
    if (!GetAIMenuOptions(info, menuOptions)) {
        return false;
    }
    auto pipeline = PipelineContext::GetCurrentContextSafely();
    CHECK_NULL_RETURN(pipeline, false);
    auto overlayManager = pipeline->GetOverlayManager();
    CHECK_NULL_RETURN(overlayManager, false);
    auto pattern = DynamicCast<WebPattern>(pattern_.Upgrade());
    CHECK_NULL_RETURN(pattern, false);
    auto targetNode = pattern->GetHost();
    CHECK_NULL_RETURN(targetNode, false);
    return overlayManager->ShowAIEntityMenu(menuOptions, info.rect, targetNode);
}

void WebDataDetectorAdapter::OnClickAIMenuOption(
    const AIMenuInfo& info, const std::pair<std::string, FuncVariant>& menuOption)
{
    TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::OnClickAIMenuOption click menuOption: %{public}s",
        menuOption.first.c_str());
    CloseAIMenu();

    auto container = Container::Current();
    CHECK_NULL_VOID(container);
    auto ace_container = AceType::DynamicCast<Platform::AceContainer>(container);
    CHECK_NULL_VOID(ace_container);
    auto containerId = container->GetInstanceId();
    auto token = ace_container->GetToken();
    auto bundleName = container->GetBundleName();

    TAG_LOGD(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::OnClickAIMenuOption prepare context");
    auto visitor = [&](auto&& func) {
        if (!func) {
            return;
        }
        using T = std::decay_t<decltype(func)>;
        if constexpr (std::is_same_v<T, std::function<std::string()>>) { // copy or selectText
            auto action = func();
            OnClickMenuItem(action, info);
        } else if constexpr (std::is_same_v<T,
                                 std::function<void(sptr<IRemoteObject>, std::string)>>) { // phone, email, url
            func(token, info.content);
        } else if constexpr (std::is_same_v<T, std::function<void(int32_t, std::string)>>) { // location
            func(containerId, info.content);
        } else if constexpr (std::is_same_v<T, std::function<void(int32_t, std::string, std::string, int32_t,
                                                   std::string)>>) { // datetime
            func(containerId, info.content, bundleName, 0, info.content);
        }
    };
    std::visit(visitor, menuOption.second);
}

void WebDataDetectorAdapter::OnClickMenuItem(const std::string& action, const AIMenuInfo& info)
{
    auto pattern = DynamicCast<WebPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(pattern);
    TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::OnClickMenuItem action: %{public}s", action.c_str());
    auto delegate = pattern->delegate_;
    CHECK_NULL_VOID(delegate);
    if (action == SELECT_TEXT) {
        delegate->OnDataDetectorSelectText();
    } else if (action == COPY) {
        std::vector<std::string> recordMix { info.content, info.outerHTML };
        delegate->OnDataDetectorCopy(recordMix);
    }
}

void WebDataDetectorAdapter::OnClickAISelectMenuOption(TextDataDetectType type, const std::string& content)
{
    TAG_LOGI(
        AceLogTag::ACE_WEB, "WebDataDetectorAdapter::OnClickAISelectMenuOption %{public}d", static_cast<int32_t>(type));
    if (!GetDataDetectorEnable()) {
        return;
    }
    if (textDetectResult_.menuOptionAndAction.empty()) {
        GetAIMenu();
        return;
    }
    int32_t index = static_cast<int32_t>(type);
    if (index < 0 || index >= static_cast<int32_t>(TEXT_DETECT_LIST.size())) {
        return;
    }
    auto entityType = TEXT_DETECT_LIST[index];
    auto menuOptionAndAction = textDetectResult_.menuOptionAndAction[entityType];
    if (menuOptionAndAction.empty()) {
        TAG_LOGE(AceLogTag::ACE_WEB, "OnClickAISelectMenu menuOption empty");
        return;
    }
    auto menuOption = menuOptionAndAction[0];
    AIMenuInfo info;
    info.entityType = entityType;
    info.content = content;
    OnClickAIMenuOption(info, menuOption);
}

void WebDataDetectorAdapter::DetectSelectedText(const std::string& detectText)
{
    if (!GetDataDetectorEnable()) {
        return;
    }
    TAG_LOGD(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::DetectSelectedText");
    if (detectText.size() > MAX_SELECTED_TEXT_SIZE) {
        return;
    }

    // ui thread
    auto instanceId = Container::CurrentIdSafely();
    auto resultFunc = [weak = AceType::WeakClaim(this), instanceId](const TextDataDetectResult result) {
        // background thread
        ContainerScope scope(instanceId);
        auto adapter = weak.Upgrade();
        CHECK_NULL_VOID(adapter);
        adapter->OnDetectSelectedTextDone(result);
    };
    TextDataDetectInfo info { detectText, config_.types };
    AIPostTask(
        [info, resultFunc]() {
            TAG_LOGI(AceLogTag::ACE_WEB,
                "WebDataDetectorAdapter::DetectSelectedText start AI detect, length: %{public}zu", info.text.size());
            DataDetectorMgr::GetInstance().DataDetect(info, resultFunc);
        },
        TaskExecutor::TaskType::BACKGROUND, "DataDetect");
}

DataDetectorResult WebDataDetectorAdapter::ParseAIResultJson(std::unique_ptr<JsonValue>& entityJson)
{
    DataDetectorResult result;
    if (!entityJson || !entityJson->IsObject()) {
        return result;
    }
    for (const auto& detectType : TEXT_DETECT_LIST) {
        auto jsonValue = entityJson->GetValue(detectType);
        if (!jsonValue || !jsonValue->IsArray()) {
            continue;
        }
        int32_t arraySize = jsonValue->GetArraySize();
        for (int32_t i = 0; i < arraySize; ++i) {
            auto item = jsonValue->GetArrayItem(i);
            if (!item || !item->IsObject()) {
                continue;
            }
            auto charOffset = item->GetInt("charOffset");
            if (charOffset < 0) {
                TAG_LOGE(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::ParseAIResultJson charOffset invalid");
                continue;
            }
            EntityMatch mat;
            mat.start = static_cast<size_t>(charOffset); // u16 offset
            mat.clean = item->GetString("oriText"); // u8 string
            mat.end = mat.start + UtfUtils::Str8DebugToStr16(mat.clean).length(); // u16 offset not include
            mat.entityType = detectType;
            result.emplace_back(std::move(mat));
        }
    }
    std::sort(
        result.begin(), result.end(), [](const EntityMatch& a, const EntityMatch& b) { return a.start < b.start; });
    return result;
}

void WebDataDetectorAdapter::OnDetectSelectedTextDone(const TextDataDetectResult& result)
{
    TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::OnDetectSelectedTextDone receive AI result");
    auto entityJson = JsonUtil::ParseJsonString(result.entity);
    if (!entityJson || !entityJson->IsObject()) {
        TAG_LOGE(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::OnDetectSelectedTextDone entityJson invalid");
        return;
    }
    auto ret = ParseAIResultJson(entityJson);
    if (ret.size() != 1) {
        return;
    }
    AIPostTask(
        [weak = AceType::WeakClaim(this), ret]() {
            auto adapter = weak.Upgrade();
            CHECK_NULL_VOID(adapter);
            adapter->UpdateAISelectMenu(ret[0].entityType, ret[0].clean);
        },
        TaskExecutor::TaskType::UI, "UpdateAISelectMenu");
}

void WebDataDetectorAdapter::UpdateAISelectMenu(const std::string& entityType, const std::string& content)
{
    TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::UpdateAISelectMenu type: %{public}s", entityType.c_str());
    TextDataDetectType type = ConvertTypeFromString(entityType);
    if (type == TextDataDetectType::INVALID) {
        return;
    }
    auto pattern = DynamicCast<WebPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(pattern);
    auto selectOverlay = pattern->webSelectOverlay_;
    CHECK_NULL_VOID(selectOverlay);
    if (selectOverlay->IsShowHandle()) {
        selectOverlay->UpdateAISelectMenu(type, content);
    }
}

std::string WebDataDetectorAdapter::UrlDecode(const std::string& str)
{
    std::string decoded;
    decoded.reserve(str.size());

    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '+') {
            decoded += ' ';
        } else if (str[i] == '%' && i + 2 < str.size()) {
            if (std::isxdigit(str[i + 1]) && std::isxdigit(str[i + 2])) {
                std::istringstream hexStream(str.substr(i + 1, 2));
                int charCode;
                hexStream >> std::hex >> charCode;

                decoded += static_cast<char>(charCode);
                i += 2;
            } else {
                decoded += str[i];
            }
        } else {
            decoded += str[i];
        }
    }
    return decoded;
}

std::string WebDataDetectorAdapter::ReplaceARGBToRGBA(const std::string& text)
{
    const std::regex argbRegex("#([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})\\b");
    std::string result = std::regex_replace(text, argbRegex, "#$2$3$4$1");
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool WebDataDetectorAdapter::SetPreviewMenuLink(const std::string& link)
{
    if (!GetDataDetectorEnable()) {
        return false;
    }
    auto linkType = TextDataDetectType::URL;
    auto linkContent = link;
    for (size_t i = 0; i < TEXT_DETECT_LIST.size(); ++i) {
        if (static_cast<int32_t>(i) == static_cast<int32_t>(TextDataDetectType::URL)) {
            continue;
        }
        auto entityType = TEXT_DETECT_LIST[i];
        auto prefix = TEXT_DETECT_MAP_TO_HREF.at(entityType);
        if (link.compare(0, prefix.length(), prefix) == 0) {
            linkType = static_cast<TextDataDetectType>(i);
            linkContent = link.substr(prefix.length());
            break;
        }
    }
    TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::SetPreviewMenuLink type: %{public}d",
        static_cast<int32_t>(linkType));
    if (linkType == previewMenuType_) {
        return true;
    }
    SetPreviewMenuAttr(linkType, UrlDecode(linkContent));
    return true;
}

bool WebDataDetectorAdapter::GetPreviewMenuBuilder(
    std::function<void()>& menuBuilder, std::function<void()>& previewBuilder)
{
    if (!GetDataDetectorEnable() || previewMenuType_ == TextDataDetectType::INVALID) {
        return false;
    }
    AIMenuInfo info;
    info.entityType = TEXT_DETECT_LIST[static_cast<size_t>(previewMenuType_)];
    info.content = previewMenuContent_;
    info.outerHTML = GetLinkOuterHTML(info.entityType, info.content);
    auto menuNode = GetPreviewMenuNode(info);
    CHECK_NULL_RETURN(menuNode, false);
    TAG_LOGI(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::GetPreviewMenuBuilder success");
    menuBuilder = [menuNode, instanceId = Container::CurrentIdSafely()]() {
        ContainerScope scope(instanceId);
        std::optional<CalcLength> width = CalcLength(MENU_WIDTH);
        auto layoutProperty = menuNode->GetLayoutProperty();
        CHECK_NULL_VOID(layoutProperty);
        layoutProperty->UpdateUserDefinedIdealSize(CalcSize(width, std::nullopt));
        ViewStackProcessor::GetInstance()->Push(menuNode);
    };
    previewBuilder = [menuType = previewMenuType_, menuContent = previewMenuContent_,
                         menuExtraParams = previewMenuExtraParams_, instanceId = Container::CurrentIdSafely()]() {
        ContainerScope scope(instanceId);
        // arkui create preview menu static func
        PreviewMenuController::CreatePreviewMenu(menuType, menuContent, nullptr);
    };
    SetPreviewMenuAttr();
    return true;
}

std::string WebDataDetectorAdapter::GetLinkOuterHTML(const std::string& entityType, const std::string& content)
{
    return R"(<a href=")" + entityType + ":" + content + R"(" style="color: )" + ReplaceARGBToRGBA(config_.color) +
           R"(; text-decoration: )" + ReplaceARGBToRGBA(config_.textDecorationStyle) + R"(;">)" + content + R"(</a>)";
}

std::function<void()> WebDataDetectorAdapter::GetPreviewMenuOptionCallback(
    TextDataDetectType type, const std::string& content)
{
    return [type, content, instanceId = Container::CurrentIdSafelyWithCheck()] () {
        ContainerScope scope(instanceId);
        auto pipeline = PipelineContext::GetCurrentContextSafelyWithCheck();
        CHECK_NULL_VOID(pipeline);
        auto fontManager = pipeline->GetFontManager();
        CHECK_NULL_VOID(fontManager);
        fontManager->OnPreviewMenuOptionClick(type, content);
    };
}
RefPtr<FrameNode> WebDataDetectorAdapter::GetPreviewMenuNode(const AIMenuInfo& info)
{
    std::vector<std::pair<std::string, std::function<void()>>> menuOptions;
    if (!GetAIMenuOptions(info, menuOptions)) {
        return nullptr;
    }
    auto pipeline = PipelineContext::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_RETURN(pipeline, nullptr);
    auto overlayManager = pipeline->GetOverlayManager();
    CHECK_NULL_RETURN(overlayManager, nullptr);

    if (auto theme = pipeline->GetTheme<TextOverlayTheme>()) {
        auto name = theme->GetAiMenuPreviewOptionName(previewMenuType_);
        if (!menuOptions.empty() && !name.empty()) {
            auto& option = menuOptions.front();
            option.first = name;
            option.second = GetPreviewMenuOptionCallback(previewMenuType_, previewMenuContent_);
        }
    }
    return overlayManager->BuildAIEntityMenu(menuOptions);
}

void WebDataDetectorAdapter::SetPreviewMenuAttr(
    TextDataDetectType type, const std::string& content, const std::map<std::string, std::string>& params)
{
    previewMenuType_ = type;
    previewMenuContent_ = (type != TextDataDetectType::INVALID) ? content : "";
    previewMenuExtraParams_ = (type != TextDataDetectType::INVALID) ? params : std::map<std::string, std::string>();
}

TextDataDetectType WebDataDetectorAdapter::ConvertTypeFromString(const std::string& type)
{
    auto it = std::find(TEXT_DETECT_LIST.begin(), TEXT_DETECT_LIST.end(), type);
    if (it == TEXT_DETECT_LIST.end()) {
        return TextDataDetectType::INVALID;
    }
    return static_cast<TextDataDetectType>(std::distance(TEXT_DETECT_LIST.begin(), it));
}

void WebDataDetectorAdapter::CloseAIMenu()
{
    auto pipeline = PipelineContext::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto pattern = DynamicCast<WebPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(pattern);
    auto targetNode = pattern->GetHost();
    CHECK_NULL_VOID(targetNode);
    // Close Menu
    auto overlayManager = pipeline->GetOverlayManager();
    CHECK_NULL_VOID(overlayManager);
    overlayManager->CloseAIEntityMenu(targetNode->GetId());
}

void WebDataDetectorAdapter::CloseOtherMenu()
{
    auto pattern = DynamicCast<WebPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(pattern);
    pattern->CloseSelectOverlay();
    pattern->DestroyAnalyzerOverlay();
    auto delegate = pattern->delegate_;
    CHECK_NULL_VOID(delegate);
    delegate->OnContextMenuHide("");
}

void WebDataDetectorAdapter::AIPostTask(
    const std::function<void()>& task, TaskExecutor::TaskType taskType, const std::string& name, uint32_t delay)
{
    TAG_LOGD(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::AIPostTask start");
    auto pipeline = PipelineContext::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto taskExecutor = pipeline->GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    bool result = taskExecutor->PostDelayedTask(task, taskType, delay, name);
    TAG_LOGD(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::AIPostTask end, result: %{public}d", result);
}

bool WebDataDetectorAdapter::SetRequestContext(const std::string& requestId, DataDetectorRequestData&& requestData)
{
    std::lock_guard<std::mutex> lock(contextMutex_);
    TAG_LOGD(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::SetRequestContext requestId: %{public}s", requestId.c_str());
    contextMap_[requestId] = std::make_shared<DataDetectorRequestData>(std::move(requestData));
    return true;
}

bool WebDataDetectorAdapter::RemoveRequestContext(const std::string& requestId)
{
    std::lock_guard<std::mutex> lock(contextMutex_);
    TAG_LOGD(
        AceLogTag::ACE_WEB, "WebDataDetectorAdapter::RemoveRequestContext requestId: %{public}s", requestId.c_str());
    size_t count = contextMap_.erase(requestId);
    if (!count) {
        TAG_LOGE(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::RemoveRequestContext requestId: %{public}s not found",
            requestId.c_str());
    }
    return count > 0;
}

std::shared_ptr<DataDetectorRequestData> WebDataDetectorAdapter::GetRequestContext(const std::string& requestId)
{
    std::lock_guard<std::mutex> lock(contextMutex_);
    auto it = contextMap_.find(requestId);
    if (it != contextMap_.end()) {
        TAG_LOGD(
            AceLogTag::ACE_WEB, "WebDataDetectorAdapter::GetRequestContext requestId: %{public}s", requestId.c_str());
        return it->second;
    }
    return nullptr;
}

void WebDataDetectorAdapter::ResetContextMap()
{
    std::lock_guard<std::mutex> lock(contextMutex_);
    TAG_LOGD(AceLogTag::ACE_WEB, "WebDataDetectorAdapter::ResetContextMap");
    contextMap_.clear();
}

} // namespace OHOS::Ace::NG
