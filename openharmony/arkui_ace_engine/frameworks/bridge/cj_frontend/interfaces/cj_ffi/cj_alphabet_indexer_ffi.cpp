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

#include "bridge/cj_frontend/interfaces/cj_ffi/cj_alphabet_indexer_ffi.h"

#include "cj_lambda.h"
#include "core/components_ng/pattern/indexer/indexer_model.h"

using namespace OHOS::Ace;
using namespace OHOS::Ace::Framework;

namespace {
const std::vector<FontStyle> FONT_STYLES = { FontStyle::NORMAL, FontStyle::ITALIC };
const std::vector<V2::AlignStyle> ALIGN_STYLE_ = { V2::AlignStyle::LEFT, V2::AlignStyle::RIGHT };
const std::vector<NG::AlignStyle> NG_ALIGN_STYLE_ = { NG::AlignStyle::LEFT, NG::AlignStyle::RIGHT };
}; // namespace

extern "C" {
void FfiOHOSAceFrameworkAlphabetIndexerCreate(VectorStringPtr vecContent, int32_t selected)
{
    auto actualVec = reinterpret_cast<std::vector<std::string>*>(vecContent);
    IndexerModel::GetInstance()->Create(*actualVec, selected);
}
void FfiOHOSAceFrameworkAlphabetIndexerColor(int32_t color)
{
    IndexerModel::GetInstance()->SetColor(Color(color));
}
void FfiOHOSAceFrameworkAlphabetIndexerSelectedColor(int32_t color)
{
    IndexerModel::GetInstance()->SetSelectedColor(Color(color));
}
void FfiOHOSAceFrameworkAlphabetIndexerPopupColor(int32_t color)
{
    IndexerModel::GetInstance()->SetPopupColor(Color(color));
}
void FfiOHOSAceFrameworkAlphabetIndexerSelectedBackgroundColor(int32_t color)
{
    IndexerModel::GetInstance()->SetSelectedBackgroundColor(Color(color));
}
void FfiOHOSAceFrameworkAlphabetIndexerPopupBackground(int32_t color)
{
    IndexerModel::GetInstance()->SetPopupBackground(Color(color));
}
void FfiOHOSAceFrameworkAlphabetIndexerUsingPopup(bool usingPop)
{
    IndexerModel::GetInstance()->SetUsingPopup(usingPop);
}
void FfiOHOSAceFrameworkAlphabetIndexerSelectedFont(
    double size, int32_t unit, const char* weight, const char* family, int32_t style)
{
    std::optional<Dimension> fontSize = Dimension(size, DimensionUnit(unit));
    std::optional<FontWeight> fontWeight = FontWeight::REGULAR;
    std::optional<std::vector<std::string>> fontFamily = std::vector<std::string>{family};
    std::optional<FontStyle> fontStyle = static_cast<FontStyle>(style);
    IndexerModel::GetInstance()->SetSelectedFont(fontSize, fontWeight, fontFamily, fontStyle);
}
void FfiOHOSAceFrameworkAlphabetIndexerPopupFont(
    double size, int32_t unit, const char* weight, const char* family, int32_t style)
{
    std::optional<Dimension> fontSize = Dimension(size, DimensionUnit(unit));
    std::optional<FontWeight> fontWeight = FontWeight::REGULAR;
    std::optional<std::vector<std::string>> fontFamily = std::vector<std::string>{family};
    std::optional<FontStyle> fontStyle = static_cast<FontStyle>(style);
    IndexerModel::GetInstance()->SetPopupFont(fontSize, fontWeight, fontFamily, fontStyle);
}
void FfiOHOSAceFrameworkAlphabetIndexerFont(double size, int32_t unit, const char* weight,
                                            const char* family, int32_t style)
{
    std::optional<Dimension> fontSize = Dimension(size, DimensionUnit(unit));
    std::optional<FontWeight> fontWeight = FontWeight::REGULAR;
    std::optional<std::vector<std::string>> fontFamily = std::vector<std::string>{family};
    std::optional<FontStyle> fontStyle = static_cast<FontStyle>(style);
    IndexerModel::GetInstance()->SetFont(fontSize, fontWeight, fontFamily, fontStyle);
}
void FfiOHOSAceFrameworkAlphabetIndexerItemSize(double size, int32_t unit)
{
    IndexerModel::GetInstance()->SetItemSize(Dimension(size, static_cast<DimensionUnit>(unit)));
}
void FfiOHOSAceFrameworkAlphabetIndexerAlignStyle(int32_t alignStyle)
{
    if (alignStyle >= 0 && alignStyle < static_cast<int32_t>(ALIGN_STYLE_.size())) {
        IndexerModel::GetInstance()->SetAlignStyle(static_cast<int32_t>(NG_ALIGN_STYLE_[alignStyle]));
    }
}
void FfiOHOSAceFrameworkAlphabetIndexerSelected(int32_t selected)
{
    IndexerModel::GetInstance()->SetSelected(selected);
}
void FfiOHOSAceFrameworkAlphabetIndexerPopupPosition(double x, double y)
{
    IndexerModel::GetInstance()->SetPopupPositionX(Dimension(x));
    IndexerModel::GetInstance()->SetPopupPositionY(Dimension(y));
}
void FfiOHOSAceFrameworkAlphabetIndexerOnSelected(void (*callback)(int32_t idx))
{
    auto onSelected = [ffiCallback = CJLambda::Create(callback)](int64_t idx) { ffiCallback(idx); };
    IndexerModel::GetInstance()->SetOnSelected(std::move(onSelected));
}
void FfiOHOSAceFrameworkAlphabetIndexerOnRequestPopupData(VectorStringHandle (*callback)(int32_t idx))
{
    auto onRequestPopupData = [ffiCallback = CJLambda::Create(callback)](int64_t idx) {
        auto callbackRes = ffiCallback(idx);
        auto resPtr = reinterpret_cast<std::vector<std::string>*>(callbackRes);
        auto result = *resPtr;
        FFICJCommonVectorStringDelete(callbackRes);
        return result;
    };
    IndexerModel::GetInstance()->SetOnRequestPopupData(std::move(onRequestPopupData));
}
void FfiOHOSAceFrameworkAlphabetIndexerOnPopupSelected(void (*callback)(int32_t idx))
{
    auto onSelected = [ffiCallback = CJLambda::Create(callback)](int64_t idx) { ffiCallback(idx); };
    IndexerModel::GetInstance()->SetOnPopupSelected(std::move(onSelected));
}
}
