/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
import { ChartPixelMap } from '@ohos/mpchart';

import ImageSaver from './ImageSaver';
import resourceManager from '@ohos.resourceManager';
import image from '@ohos.multimedia.image';

export default class Utils {
  public static saveImage(filename: string, context: CanvasRenderingContext2D | null) {
    const imageSaver = new ImageSaver();
    imageSaver.saveImage(filename, context);
  }

  public static async getPixelMap(id:number,resourceMgr:resourceManager.ResourceManager): Promise<ChartPixelMap>{
    let charPixelMap: ChartPixelMap = new ChartPixelMap();
    let fileData = resourceMgr.getMediaContentSync(id)
    const buffer:ArrayBuffer =fileData.buffer.slice(0,fileData.buffer.byteLength);
    const imageSource = image.createImageSource(buffer)
    let pixelMap=await imageSource.createPixelMap()
    charPixelMap.setIcon(pixelMap)
    charPixelMap.setWidth(20)
    charPixelMap.setHeight(20);
    return charPixelMap
  }

}