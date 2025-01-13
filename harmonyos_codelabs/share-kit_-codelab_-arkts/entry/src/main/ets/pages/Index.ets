import { common } from '@kit.AbilityKit';
import { fileIo as fs } from '@kit.CoreFileKit';
import { SymbolGlyphModifier } from '@kit.ArkUI';
import Constants from '../utils/Constants';
import AccessModel from '../components/AccessModel';
import BasicUse from '../components/BasicUse';
import FunctionExample from '../components/FunctionExample';
import TypicalScenarios from '../components/TypicalScenarios';
import KnockShare from '../components/KnockShare';
import Logger from '../utils/Logger';

let logger = Logger.getLogger('[EntryIndex]');

// 获取共享的storage实例
// Obtaining a Shared Storage Instance
let storage = LocalStorage.getShared();

@Entry(storage)
@Component
struct Index {
  @State navigationIndex: string = Constants.NAVIGATION_MODEL;

  async aboutToAppear(): Promise<void> {
    const context = getContext(this) as common.UIAbilityContext;
    const filesDir = context.filesDir;
    [
      [context.resourceManager.getMediaContentSync($r('app.media.exampleImage')), '/exampleImage.jpg'],
      [context.resourceManager.getMediaContentSync($r('app.media.exampleGif')), '/exampleGif.gif'],
      [context.resourceManager.getMediaContentSync($r('app.media.exampleVideo')), '/exampleVideo.mp4'],
      [context.resourceManager.getMediaContentSync($r('app.media.startIcon')), '/icon.jpg'],
      [context.resourceManager.getMediaContentSync($r('app.media.exampleKnock1')), '/exampleKnock1.jpg'],
      [context.resourceManager.getMediaContentSync($r('app.media.exampleKnock2')), '/exampleKnock2.png'],
      [context.resourceManager.getMediaContentSync($r('app.media.exampleKnock3')), '/exampleKnock3.jpg'],
    ].forEach(item => {
      let file = fs.openSync(filesDir + item[1], fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
      let writeLen = fs.writeSync(file.fd, (item[0] as Uint8Array).buffer);
      logger.info("write data to file succeed and size is:" + writeLen);
      fs.closeSync(file);
    })
  }

  onPageShow(): void {
    logger.info('onPageHide invoked.');
    let context = getContext(this);
    context.eventHub.emit('onFocus');
  }

  onPageHide(): void {
    logger.info('onPageHide invoked.');
    let context = getContext(this);
    context.eventHub.emit('onBackGround');
  }

  @Builder Title() {
    Text($r('app.string.huawei_share_title'))
      .fontSize(24)
      .textAlign(TextAlign.Center)
      .margin({ top: 20, left: 20 })
  }

  build() {
    Navigation() {
      if (this.navigationIndex === Constants.NAVIGATION_MODEL) {
        AccessModel();
      }
      if (this.navigationIndex === Constants.NAVIGATION_BASE) {
        BasicUse();
      }
      if (this.navigationIndex === Constants.NAVIGATION_FUCTION) {
        FunctionExample();
      }
      if (this.navigationIndex === Constants.NAVIGATION_UTD) {
        TypicalScenarios();
      }
      if (this.navigationIndex === Constants.NAVIGATION_KNOCK) {
        KnockShare();
      }
    }
    .title({
      builder: this.Title(),
      height: TitleHeight.MainOnly
    })
    .mode(NavigationMode.Stack)
    .toolbarConfiguration([
      {
        value: $r("app.string.navigation_toolbar_model"),
        status: this.navigationIndex === Constants.NAVIGATION_MODEL ? ToolbarItemStatus.ACTIVE : ToolbarItemStatus.NORMAL,
        symbolIcon: new SymbolGlyphModifier($r("sys.symbol.house")),
        activeSymbolIcon: new SymbolGlyphModifier($r("sys.symbol.house_fill")),
        action: () => { this.navigationIndex = Constants.NAVIGATION_MODEL },
      },
      {
        value: $r("app.string.navigation_toolbar_base"),
        status: this.navigationIndex === Constants.NAVIGATION_BASE ? ToolbarItemStatus.ACTIVE : ToolbarItemStatus.NORMAL,
        symbolIcon: new SymbolGlyphModifier($r("sys.symbol.square_grid_2x2")),
        activeSymbolIcon: new SymbolGlyphModifier($r("sys.symbol.square_fill_grid_2x2")),
        action: () => { this.navigationIndex = Constants.NAVIGATION_BASE },
      },
      {
        value: $r("app.string.navigation_toolbar_function"),
        status: this.navigationIndex === Constants.NAVIGATION_FUCTION ? ToolbarItemStatus.ACTIVE : ToolbarItemStatus.NORMAL,
        symbolIcon: new SymbolGlyphModifier($r("sys.symbol.doc_plaintext_and_pencil")),
        activeSymbolIcon: new SymbolGlyphModifier($r("sys.symbol.doc_plaintext_and_pencil_fill")),
        action: () => { this.navigationIndex = Constants.NAVIGATION_FUCTION },
      },
      {
        value: $r("app.string.navigation_toolbar_utd"),
        status: this.navigationIndex === Constants.NAVIGATION_UTD ? ToolbarItemStatus.ACTIVE : ToolbarItemStatus.NORMAL,
        symbolIcon: new SymbolGlyphModifier($r("sys.symbol.rectangle_on_rectangle")),
        activeSymbolIcon: new SymbolGlyphModifier($r("sys.symbol.rectangle_on_rectangle_fill")),
        action: () => { this.navigationIndex = Constants.NAVIGATION_UTD },
      },
      {
        value: $r("app.string.navigation_toolbar_knock"),
        status: this.navigationIndex === Constants.NAVIGATION_KNOCK ? ToolbarItemStatus.ACTIVE : ToolbarItemStatus.NORMAL,
        symbolIcon: new SymbolGlyphModifier($r("sys.symbol.person_2")),
        activeSymbolIcon: new SymbolGlyphModifier($r("sys.symbol.person_2_fill")),
        action: () => { this.navigationIndex = Constants.NAVIGATION_KNOCK },
      },
    ])
  }
}