# @ohos.application.DistributedExtensionAbility (Distributed Extension)

The **DistributedExtensionAbility** module provides distributed extension capabilities and lifecycle callbacks for distributed ability creation, destruction, and connection.

> **NOTE**
>
> The initial APIs of this module are supported since API version 20. Newly added APIs will be marked with a superscript to indicate their earliest API version.
>
> The APIs of this module can be used only in the stage model.

## Modules to Import

```ts
import { DistributedExtensionAbility} from '@kit.DistributedServiceKit';
```

## Attributes

**System capability**: SystemCapability.Ability.AbilityRuntime.Core

| Name   | Type                         | Readable| Writable| Description                                                      |
| ------- | ----------------------------- | ---- | ---- | ---------------------------------------------------------- |
| context | DistributedExtensionContext() | Yes  | No  | Context of the **DistributedExtension**. This context inherits from **ExtensionContext**.|

## DistributedExtensionAbility.onCreate

onCreate(want: Want): void

Callback invoked to initialize the service logic when a **DistributedExtensionAbility** instance is created.

**System capability**: SystemCapability.Ability.AbilityRuntime.Core

**Parameters**

| Name    | Type| Mandatory                                                            | Description|
| ----------| ---- | ---------------------------------------------------------------- | ---- |
| want      | [Want](../apis-ability-kit/js-apis-app-ability-want.md) | Yes  | Want information related to the **DistributedExtensionAbility** instance, including the ability name and bundle name.|

**Example**

```ts
import { Want } from '@kit.AbilityKit';
import { DistributedExtensionAbility } from '@kit.DistributedServiceKit';

export default class DistributedExtension extends DistributedExtensionAbility {
  onCreate(want: Want) {
    console.info(`DistributedExtension Create ok`);
    console.info(`DistributedExtension on Create want: ${JSON.stringify(want)}`);
    console.info(`DistributedExtension Create end`);
  }
}
```

## DistributedExtensionAbility.onCollaborate

onCollaborate(wantParam: Record <string, Object>) : AbilityConstant.CollaborateResult

Callback invoked to return the collaboration result in multi-device collaboration scenarios.

**System capability**: SystemCapability.Ability.AbilityRuntime.AbilityCore

**Parameters**

| Name   | Type  | Mandatory| Description                                                                                                                                  |
| --------- | ------ | ---- | -------------------------------------------------------------------------------------------------------------------------------------- |
| wantParam | Record <string, Object> | Yes  | Want parameter, which supports only the key **"ohos.extra.param.key.supportCollaborateIndex"**. The key can be used to obtain the data passed by the caller and perform corresponding processing.|

**Return value**

| Type| Description|
| ---------- | ---- |
| [AbilityConstant.CollaborateResult](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-ability-kit/js-apis-app-ability-abilityConstant.md#collaborateresult) | Collaboration result, that is, whether the target application accepts the collaboration request.|

**Example**

```ts
import { abilityConnectionManager, DistributedExtensionAbility } from '@kit.DistributedServiceKit';
import { AbilityConstant } from '@kit.AbilityKit';

export default class DistributedExtension extends DistributedExtensionAbility {
  onCollaborate(wantParam: Record<string, Object>) {
    console.info(`DistributedExtension onCollabRequest Accept to the result of Ability collaborate`);
    let sessionId = -1;
    const collaborationValues = wantParam["CollaborationValues"] as abilityConnectionManager.CollaborationValues;
    if (collaborationValues == undefined) {
      return sessionId;
    }
    console.info(`onCollab, collaborationValues: ${JSON.stringify(collaborationValues)}`);
    return AbilityConstant.CollaborateResult.ACCEPT;
  }
}
```

## DistributedExtensionAbility.onDestroy

onDestroy(): void

Callback invoked to clear resources when a **ServiceExtensionAbility** instance is destroyed.

**System capability**: SystemCapability.Ability.AbilityRuntime.Core

**Example**

```ts
import { DistributedExtensionAbility } from '@kit.DistributedServiceKit';

export default class DistributedExtension extends DistributedExtensionAbility {
  onDestroy() {
    console.info('DistributedExtension onDestroy ok');
  }
}
```
