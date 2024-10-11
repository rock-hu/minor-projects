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

#include "bridge/declarative_frontend/ark_theme/theme_apply/js_with_theme.h"

#include "base/memory/referenced.h"
#include "bridge/declarative_frontend/ark_theme/theme_apply/js_theme.h"

namespace OHOS::Ace::Framework {

std::map<int32_t, JSTheme> JSThemeScope::jsThemes = {};

void JSWithTheme::JSBind(BindingTarget globalObj)
{
    JSClass<JSWithTheme>::Declare("WithTheme");
    JSClass<JSWithTheme>::StaticMethod("sendThemeToNative", &JSWithTheme::SendThemeToNative, MethodOptions::NONE);
    JSClass<JSWithTheme>::StaticMethod("removeThemeInNative", &JSWithTheme::RemoveThemeInNative, MethodOptions::NONE);
    JSClass<JSWithTheme>::InheritAndBind<JSViewAbstract>(globalObj);
}

void JSWithTheme::RemoveThemeInNative(const JSCallbackInfo& info)
{
    auto jsThemeScopeId = info[0];
    if (!jsThemeScopeId->IsNumber()) {
        return;
    }
    auto themeScopeId = jsThemeScopeId->ToNumber<int32_t>();
    JSThemeScope::jsThemes.erase(themeScopeId);
}

void JSWithTheme::SendThemeToNative(const JSCallbackInfo& info)
{
    auto jsColors = info[0];
    if (!jsColors->IsArray()) {
        return;
    }
    auto jsThemeScopeId = info[1];
    if (!jsThemeScopeId->IsNumber()) {
        return;
    }

    auto jsColorsArray = JSRef<JSArray>::Cast(jsColors);
    auto themeScopeId = jsThemeScopeId->ToNumber<int32_t>();

    auto colors = JSThemeColors();
    colors.SetColors(jsColorsArray);

    JSThemeScope::jsThemes[themeScopeId].SetColors(colors);
    // keep info about WithTheme containers usage
    if (themeScopeId > 0) {
        JSThemeScope::jsThemeScopeEnabled = true;
    }
}

} // namespace OHOS::Ace::Framework