/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "core/components_ng/base/frame_node.h"
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/utility/reverse_converter.h"
#include "arkoala_api_generated.h"

namespace OHOS::Ace::NG::Converter {
template<>
void AssignCast(std::optional<Ark_Resource_Simple>& dst, const Ark_Resource& src)
{
    ResourceConverter converter(src);
    auto resourceString = converter.ToString();
    if (resourceString) {
        Ark_Resource_Simple temp;
        temp.content = resourceString.value();
        temp.bundleName = converter.BundleName();
        temp.moduleName = converter.ModuleName();
        dst = temp;
    } else {
        LOGE("Not a string resource: %{public}s:%{public}s\n", converter.BundleName().c_str(),
            converter.ModuleName().c_str());
    }
}
template<>
void AssignCast(std::optional<Ark_Resource_Simple>& dst, const Ark_String& src)
{
    std::string str = Converter::Convert<std::string>(src);
    if (!str.empty()) {
        Ark_Resource_Simple temp;
        temp.content = str;
        dst = temp;
    }
}
} /* namespace OHOS::Ace::NG::Converter */

namespace OHOS::Ace::NG::GeneratedModifier {
namespace GlobalScope_ohos_fontAccessor {
void RegisterFontImpl(const Ark_FontOptions* options)
{
    CHECK_NULL_VOID(options);
    std::string familyName;
    std::string familySrc;
    std::string bundleName;
    std::string moduleName;
    if (auto familyNameOpt = Converter::OptConvert<Converter::FontFamilies>(options->familyName); familyNameOpt) {
        familyName = !familyNameOpt->families.empty() ? familyNameOpt->families.front() : "";
    }
    if (auto familySrcOpt = Converter::OptConvert<Converter::Ark_Resource_Simple>(options->familySrc);
        familySrcOpt) {
        familySrc = familySrcOpt->content;
        bundleName = familySrcOpt->bundleName;
        moduleName = familySrcOpt->moduleName;
    }
    auto pipeline = PipelineBase::GetCurrentContextSafely();
    if (pipeline) {
        pipeline->RegisterFont(familyName, familySrc, bundleName, moduleName);
    }
}
Array_String GetSystemFontListImpl()
{
    StringArray fontList;
    auto pipeline = PipelineBase::GetCurrentContextSafely();
    if (pipeline) {
        pipeline->GetSystemFontList(fontList);
    }
    return Converter::ArkValue<Array_String>(fontList, Converter::FC);
}
Ark_FontInfo GetFontByNameImpl(const Ark_String* fontName)
{
    FontInfo fontInfo;
    auto fontNameCasted = Converter::Convert<std::string>(*fontName);
    auto pipeline = PipelineBase::GetCurrentContextSafely();
    if (pipeline) {
        pipeline->GetSystemFont(fontNameCasted, fontInfo);
    }
    return Converter::ArkValue<Ark_FontInfo>(fontInfo, Converter::FC);
}
} // GlobalScope_ohos_fontAccessor
const GENERATED_ArkUIGlobalScope_ohos_fontAccessor* GetGlobalScope_ohos_fontAccessor()
{
    static const GENERATED_ArkUIGlobalScope_ohos_fontAccessor GlobalScope_ohos_fontAccessorImpl {
        GlobalScope_ohos_fontAccessor::RegisterFontImpl,
        GlobalScope_ohos_fontAccessor::GetSystemFontListImpl,
        GlobalScope_ohos_fontAccessor::GetFontByNameImpl,
    };
    return &GlobalScope_ohos_fontAccessorImpl;
}
}
