/**
 * 最佳实践: 视频场景图层低功耗建议
 */
import { webview } from '@kit.ArkWeb';

@Entry
@Component
struct VideoSceneLayers {
  build() {
    Column(){
      // [Start Video_scene_layers]
      XComponent({ id: 'XComponentId', type: 'surface', libraryname: 'entry'})
        .width(360)
        .height(360)
      // [End Video_scene_layers]
    }
  }
}