/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
// [Start LightArt]
// LightArt.ets
import { lang } from '@kit.ArkTS';

/*
 * The data to be shared is encapsulated into four Sendable classes: LightArtDataModel,
 * LightArtDataDocument, LightArtViewModel, and LightArtDataComponent.
 * UI data is encapsulated into the UI class.
 * The data of the Sendable class is generated in a child thread.
 */
@Sendable
class LightArtDataModel {}

@Sendable
class LightArtDataDocument {}

@Sendable
class LightArtViewModel {}

class UI {}

interface ISerializableType<T> extends lang.ISendable {}

@Sendable
class LightArtDataComponent {
  public dataModel?: LightArtDataModel;
  public document?: LightArtDataDocument;
  public parent?: LightArtDataComponent;
  public components?: LightArtDataComponent;
  // [StartExclude LightArt]
  // [EndExclude LightArt]
  public fromJson(jsonObj: object) {
    // [StartExclude LightArt]
    // [EndExclude LightArt]
  }

  public mergeFrom(jsonObj: object) {
    // [StartExclude LightArt]
    // [EndExclude LightArt]
  }
}

/*
 * Assemble the Sendable class and the UI data class into LightArtUIComponent.
 * LightArtUIComponent assembles the data based on the data returned by the child thread.
 */
class LightArtUIComponent {
  public data?: LightArtDataComponent;
  public model?: LightArtViewModel;
  public parent?: LightArtUIComponent;
  public components?: LightArtUIComponent;
  // [StartExclude LightArt]
  // [EndExclude LightArt]
  public uiData?: UI;
}

// Fill in the data to LightArtDataComponent.
@Sendable
export class LightArtDataComponentType implements ISerializableType<LightArtDataComponent> {
  public fromJson(jsonObj: object): LightArtDataComponent {
    let ans = new LightArtDataComponent();
    ans.mergeFrom(jsonObj);
    return ans;
  }  
  public static instance: LightArtDataComponentType = new LightArtDataComponentType();
}
// [End LightArt]