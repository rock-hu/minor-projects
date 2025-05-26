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

/**
 * 最佳实践: 使用drawing自绘制能力提升性能
 */

import { FrameNode, NodeController, RenderNode } from '@kit.ArkUI';
import { resourceManager } from '@kit.LocalizationKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { Context } from '@kit.AbilityKit';
import { image } from '@kit.ImageKit';
import testNapi from 'libndkDrawing.so';

enum DrawType { NONE, PATH, TEXT, IMAGE };

const uiContext: UIContext | undefined = AppStorage.get('uiContext');

// [Start my_render_node]
// entry\src\main\ets\pages\Index.ets
// Define a RenderNode self-drawing RenderNode MyRenderNode, so as to draw with the interface of Native
class MyRenderNode extends RenderNode {
  private drawType: DrawType = DrawType.NONE;
  private pMap: image.PixelMap | undefined = undefined; // 背景图

  draw(context: DrawContext): void {
    // Call the Native ondraw interface on the native side to draw, and pass in the background image this.pMap and the graphic drawing context as parameters
    testNapi.nativeOnDraw(666, context, uiContext?.vp2px(this.size.width), uiContext?.vp2px(this.size.height), this.drawType, this.pMap);
  }

  // Set the drawing type
  resetType(type: DrawType): void {
    this.drawType = type;
  }

  // Set the background picture
  setPixelMap(p: PixelMap): void {
    this.pMap = p;
  }
}
// [End my_render_node]

// [Start my_node_controller]
// entry\src\main\ets\pages\Index.ets
// Create a MyRenderNode object
const newNode = new MyRenderNode();
// Defines the size and location of the newNode
newNode.frame = {
  x: 0,
  y: 0,
  width: 980,
  height: 1280
};

// Mount the MyRenderNode object node on the NodeContainer
class MyNodeController extends NodeController {
  private rootNode: FrameNode | null = null;

  makeNode(uiContext: UIContext): FrameNode | null {
    this.rootNode = new FrameNode(uiContext);
    if (this.rootNode === null) {
      return null;
    }
    const renderNode = this.rootNode.getRenderNode();
    if (renderNode !== null) {
      renderNode.appendChild(newNode);
    }
    return this.rootNode;
  }
}
// [End my_node_controller]

// [Start page_component]
// entry\src\main\ets\pages\Index.ets
@Entry
@Component
struct Index {
  private myNodeController: MyNodeController = new MyNodeController();

  aboutToAppear(): void {
    const context: Context = this.getUIContext().getHostContext()!;
    const resourceMgr: resourceManager.ResourceManager = context.resourceManager;
    resourceMgr.getRawFileContent('drawImage.jpg').then((fileData: Uint8Array) => {
      console.info('success in getRawFileContent');
      const buffer = fileData.buffer.slice(0);
      const imageSource: image.ImageSource = image.createImageSource(buffer);
      imageSource.createPixelMap().then((pMap: image.PixelMap) => {
        // Self-drawing rendering node background map
        newNode.setPixelMap(pMap);
      }).catch((err: BusinessError) => {
        console.error(`fail to create PixelMap, err: ${err}`);
      }).catch((err: BusinessError) => {
        console.error(`fail to getRawFileContent, err: ${err}`);
      })
    })
  }

  build() {
    Stack() {
      // Mount the self-drawn rendering node to NodeContainer
      NodeContainer(this.myNodeController)
        .height('100%')
      Row() {
        Button('Begin Draw')
          .width('100%')
          .height(40)
          .onClick(() => {
            newNode.resetType(DrawType.IMAGE);
            newNode.invalidate();
          })
      }
      .padding({
        right: 16,
        bottom: 16,
        left: 16
      })
    }
    .alignContent(Alignment.Bottom)
    .width('100%')
    .height('100%')
  }
}
// [End page_component]
