/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 */

import { formInfo } from "@kit.FormKit"
import { formBindingData } from '@kit.FormKit';
import { Want } from '@kit.AbilityKit';
import { PushExtensionAbility } from '@kit.PushKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

export default class FormAbility extends PushExtensionAbility {
  onAddForm(want: Want): formBindingData.FormBindingData {
    // get formId
    const formId = want.parameters![formInfo.FormParam.IDENTITY_KEY] as string;
    hilog.info(0x0000, 'PushKitDemo', 'Get formId successfully: %{public}s', JSON.stringify(formId));

    // define widget card field
    const bindingData = formBindingData.createFormBindingData({
      formId,
      text: 'default',
      image: '',
    });

    // define proxy field
    bindingData.proxies = [
      {
        key: 'text', subscriberId: formId
      },
      {
        key: 'image', subscriberId: formId
      },
    ];
    return bindingData;
  }

  onCastToNormalForm(formId: string) {
    // Called when the form provider is notified that a temporary form is successfully
    // converted to a normal form.
  }

  onUpdateForm(formId: string) {
    // Called to notify the form provider to update a specified form.
  }

  onChangeFormVisibility(newStatus: Record<string, number>) {
    // Called when the form provider receives form events from the system.
  }

  onFormEvent(formId: string, message: string) {
    // Called when a specified message event defined by the form provider is triggered.
  }

  onRemoveForm(formId: string) {
    // Called to notify the form provider that a specified form has been destroyed.
  }

  onAcquireFormState(want: Want) {
    // Called to return a {@link FormState} object.
    return formInfo.FormState.READY;
  }

};


