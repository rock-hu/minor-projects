import { media } from '@kit.MediaKit';


@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  async play(){
    // [Start player]
    // 创建avPlayer实例对象
    let player = await media.createAVPlayer();
    let headers: Record<string, string> = {"User-Agent" : "User-Agent-Value"};
    let mediaSource : media.MediaSource = media.createMediaSourceWithUrl("http://xxx",  headers);
    // 将preferredBufferDuration设置为最大值20MB；preferredWidth为播放策略首选宽度；preferredHeight为播放策略首选高度；preferredHdr为播放策略true是hdr，false非hdr
    let playStrategy : media.PlaybackStrategy = {preferredWidth: 1, preferredHeight: 2, preferredBufferDuration: 20, preferredHdr: false};
    // 设置媒体来源和播放策略
    player.setMediaSource(mediaSource, playStrategy);
    // [End player]
  }
  build() {
    RelativeContainer() {
      Text(this.message)
        .id('HelloWorld')
        .fontSize($r('app.float.page_text_font_size'))
        .fontWeight(FontWeight.Bold)
        .alignRules({
          center: { anchor: '__container__', align: VerticalAlign.Center },
          middle: { anchor: '__container__', align: HorizontalAlign.Center }
        })
        .onClick(() => {
          this.message = 'Welcome';
        })
    }
    .height('100%')
    .width('100%')
  }
}