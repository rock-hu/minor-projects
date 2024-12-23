/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_theme_bridge.h"

#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_native_common_bridge.h"
#include "bridge/declarative_frontend/engine/jsi/nativeModule/arkts_utils.h"
#include "core/components_ng/syntax/with_theme_node.h"
#include "core/components_ng/token_theme/token_theme_storage.h"

namespace OHOS::Ace::NG {
ArkUINativeModuleValue ThemeBridge::Create(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::JSValueRef::Undefined(vm));
    Local<JSValueRef> themeScopeIdArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> themeIdArg = runtimeCallInfo->GetCallArgRef(1);
    Local<JSValueRef> colorsArg = runtimeCallInfo->GetCallArgRef(2); // 2: colorsArg index
    Local<JSValueRef> colorModeArg = runtimeCallInfo->GetCallArgRef(3); // 3: colorModeArg index
    Local<JSValueRef> onThemeScopeDestroyArg = runtimeCallInfo->GetCallArgRef(4); // 4: destroy callback arg index

    // handle theme scope id argument
    if (!themeScopeIdArg->IsNumber()) {
        return panda::JSValueRef::Undefined(vm);
    }
    ArkUI_Int32 themeScopeId = static_cast<ArkUI_Int32>(themeScopeIdArg->Int32Value(vm));

    // handle theme id argument
    if (!themeIdArg->IsNumber()) {
        return panda::JSValueRef::Undefined(vm);
    }
    ArkUI_Int32 themeId = static_cast<ArkUI_Int32>(themeIdArg->Int32Value(vm));

    // handle colors argument
    if (!colorsArg->IsArray(vm)) {
        return panda::JSValueRef::Undefined(vm);
    }
    std::vector<ArkUI_Uint32> colors;
    HandleThemeColorsArg(vm, colorsArg, colors);

    // handle color mode argument
    if (!colorModeArg->IsNumber()) {
        return panda::JSValueRef::Undefined(vm);
    }
    ArkUI_Int32 colorMode = static_cast<ArkUI_Int32>(colorModeArg->Int32Value(vm));

    // handle on theme scope destroy argument
    if (!onThemeScopeDestroyArg->IsFunction(vm)) {
        return panda::JSValueRef::Undefined(vm);
    }
    auto obj = onThemeScopeDestroyArg->ToObject(vm);
    auto containerId = Container::CurrentId();
    panda::Local<panda::FunctionRef> func = obj;
    std::function<void()> onThemeScopeDestroy = [vm, func = panda::CopyableGlobal(vm, func), containerId]() {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ContainerScope scope(containerId);
        func->Call(vm, func.ToLocal(), nullptr, 0);
    };

    // execute C-API
    auto themeModifier = GetArkUINodeModifiers()->getThemeModifier();
    auto theme = themeModifier->createTheme(themeId, colors.data(), colorMode);
    CHECK_NULL_RETURN(theme, panda::NativePointerRef::New(vm, nullptr));
    ArkUINodeHandle node = themeModifier->getWithThemeNode(themeScopeId);
    if (!node) {
        node = CreateWithThemeNode(themeScopeId);
    }
    themeModifier->createThemeScope(node, theme);
    themeModifier->setOnThemeScopeDestroy(node, reinterpret_cast<void*>(&onThemeScopeDestroy));

    return panda::JSValueRef::Undefined(vm);
}

void ThemeBridge::HandleThemeColorsArg(const EcmaVM* vm, const Local<JSValueRef>& colorsArg,
    std::vector<ArkUI_Uint32>& colors)
{
    auto basisTheme = TokenThemeStorage::GetInstance()->GetDefaultTheme();
    if (!basisTheme) {
        basisTheme = TokenThemeStorage::GetInstance()->ObtainSystemTheme();
    }
    for (size_t i = 0; i < TokenColors::TOTAL_NUMBER; i++) {
        Color color;
        auto colorParams = panda::ArrayRef::GetValueAt(vm, colorsArg, i);
        if (!ArkTSUtils::ParseJsColorAlpha(vm, colorParams, color)) {
            color = basisTheme->Colors()->GetByIndex(i);
        }
        colors.push_back(static_cast<ArkUI_Uint32>(color.GetValue()));
    }
}

ArkUINodeHandle ThemeBridge::CreateWithThemeNode(ArkUI_Int32 themeScopeId)
{
    auto themeModifier = GetArkUINodeModifiers()->getThemeModifier();
    auto node = themeModifier->createWithThemeNode(themeScopeId);
    RefPtr<WithThemeNode> withThemeNode = AceType::Claim(reinterpret_cast<WithThemeNode*>(node));
    withThemeNode->DecRefCount();
    ViewStackProcessor::GetInstance()->Push(withThemeNode);
    return node;
}

ArkUINativeModuleValue ThemeBridge::Pop(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    ViewStackProcessor::GetInstance()->PopContainer();
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue ThemeBridge::SetDefaultTheme(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> colorsArg = runtimeCallInfo->GetCallArgRef(0);
    Local<JSValueRef> isDarkArg = runtimeCallInfo->GetCallArgRef(1);

    // handle colors argument
    if (!colorsArg->IsArray(vm)) {
        return panda::JSValueRef::Undefined(vm);
    }
    std::vector<ArkUI_Uint32> colors;
    auto basisTheme = TokenThemeStorage::GetInstance()->ObtainSystemTheme();
    for (size_t i = 0; i < TokenColors::TOTAL_NUMBER; i++) {
        Color color;
        auto colorParams = panda::ArrayRef::GetValueAt(vm, colorsArg, i);
        if (!ArkTSUtils::ParseJsColorAlpha(vm, colorParams, color)) {
            color = basisTheme->Colors()->GetByIndex(i);
        }
        colors.push_back(static_cast<ArkUI_Uint32>(color.GetValue()));
    }

    // handle color mode argument
    if (!isDarkArg->IsBoolean()) {
        return panda::JSValueRef::Undefined(vm);
    }
    ArkUI_Bool isDark = static_cast<ArkUI_Bool>(isDarkArg->BooleaValue(vm));

    // execute C-API
    GetArkUINodeModifiers()->getThemeModifier()->setDefaultTheme(colors.data(), isDark);
    return panda::JSValueRef::Undefined(vm);
}

ArkUINativeModuleValue ThemeBridge::RemoveFromCache(ArkUIRuntimeCallInfo* runtimeCallInfo)
{
    EcmaVM* vm = runtimeCallInfo->GetVM();
    CHECK_NULL_RETURN(vm, panda::NativePointerRef::New(vm, nullptr));
    Local<JSValueRef> themeIdArg = runtimeCallInfo->GetCallArgRef(0);

    // handle theme id argument
    if (!themeIdArg->IsNumber()) {
        return panda::JSValueRef::Undefined(vm);
    }
    ArkUI_Int32 themeId = static_cast<ArkUI_Int32>(themeIdArg->Int32Value(vm));

    // execute C-API
    GetArkUINodeModifiers()->getThemeModifier()->removeFromCache(themeId);
    return panda::JSValueRef::Undefined(vm);
}
} // namespace OHOS::Ace::NG