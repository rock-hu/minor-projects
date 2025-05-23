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

#include "core/components/option/option_element.h"

#include "core/components/select_popup/render_select_popup.h"

namespace OHOS::Ace {

void OptionElement::PerformBuild()
{
    RefPtr<OptionComponent> component = AceType::DynamicCast<OptionComponent>(component_);
    auto context = context_.Upgrade();
    if (!component || !context || !context->GetAccessibilityManager()) {
        LOGE("select: can not get component of select option in perform build.");
    } else {
        if (!component->Initialize(context->GetAccessibilityManager())) {
            LOGE("select: initialize of option failed.");
        }
        SetFocusable(component->GetFocusable());
    }
    ComponentGroupElement::PerformBuild();

    RefPtr<RenderOption> render = AceType::DynamicCast<RenderOption>(renderNode_);
    if (!render) {
        return;
    }
    WeakPtr<FocusNode> weakNode = AceType::WeakClaim(this);
    render->SetWeakNode(weakNode);
}

bool OptionElement::OnKeyEvent(const KeyEvent& keyEvent)
{
    FocusGroup::OnKeyEvent(keyEvent);
    if (!((keyEvent.code == KeyCode::KEY_BACK || keyEvent.code == KeyCode::KEY_ESCAPE) &&
        keyEvent.action == KeyAction::UP)) {
        return false;
    }

    RefPtr<RenderOption> render = AceType::DynamicCast<RenderOption>(renderNode_);
    if (!render) {
        LOGE("select: can not dynamic from render node to render select option.");
        return false;
    }
    return render->OnBack();
}

void OptionElement::OnFocus()
{
    FocusGroup::OnFocus();
    RefPtr<RenderOption> render = AceType::DynamicCast<RenderOption>(renderNode_);
    if (!render) {
        LOGE("select: can not dynamic from render node to render select option.");
        return;
    }
    render->OnFocus(true);
}

void OptionElement::OnClick()
{
    FocusGroup::OnClick();
    RefPtr<RenderOption> render = AceType::DynamicCast<RenderOption>(renderNode_);
    if (!render) {
        LOGE("select: can not dynamic from render node to render select option.");
        return;
    }
    render->OnClick(true);
}

void OptionElement::OnBlur()
{
    FocusGroup::OnBlur();
    RefPtr<RenderOption> render = AceType::DynamicCast<RenderOption>(renderNode_);
    if (!render) {
        LOGE("select: can not dynamic from render node to render select option.");
        return;
    }
    render->OnFocus(false);
}

bool OptionElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    return false;
}

} // namespace OHOS::Ace
