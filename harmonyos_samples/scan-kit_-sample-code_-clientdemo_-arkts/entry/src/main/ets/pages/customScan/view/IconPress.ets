import { accessibility } from '@kit.AccessibilityKit';
import { BreakpointConstants } from '../constants/BreakpointConstants';
import { CommonConstants } from '../constants/CommonConstants';
import { FontSizeScale } from '../model/FontSizeScale';
import { ScanLayout } from '../model/ScanLayout';

@Extend(Column)
function iconContainerMaskLayer() {
  .width(48)
  .height(48)
  .borderRadius(48)
  .clip(true)
}

@Extend(Column)
function iconContainer() {
  .width(48)
  .height(48)
  .borderRadius(48)
  .clip(true)
  .alignItems(HorizontalAlign.Center)
  .justifyContent(FlexAlign.Center)
  .backgroundColor($r('sys.color.ohos_id_color_button_normal_transparent'))
}

@Extend(SymbolGlyph)
function icon() {
  .fontColor([$r('sys.color.font_on_primary')])
  .fontWeight(FontWeight.Regular)
  .fontSize('24vp')
}

@Extend(Text)
function iconText() {
  .fontColor($r('sys.color.font_on_primary'))
  .textAlign(TextAlign.Center)
  .fontFamily('HarmonyHeiTi')
  .maxLines(2)
  .textOverflow({ overflow: TextOverflow.Ellipsis })
}

const BREAKPOINT_SM: string = BreakpointConstants.BREAKPOINT_SM;
const BREAKPOINT_MD: string = BreakpointConstants.BREAKPOINT_MD;
const BREAKPOINT_LG: string = BreakpointConstants.BREAKPOINT_LG;
const BREAKPOINT_XL: string = BreakpointConstants.BREAKPOINT_XL;
const ICON_MARGIN_BOTTOM_WITHOUT_TEXT: number = 12;
const ICON_MARGIN_BOTTOM_WITH_TEXT: number = 0;
const TEXT_MARGIN_BOTTOM: number = 2;
const TEXT_MARGIN_TOP: number = 6; // 4 + 2

@Component
export struct IconPress {
  @State scanLayout: ScanLayout = ScanLayout.getInstance();
  @StorageLink('scanFontSizeScale') fontSizeScale: number = 1;
  @Prop url: Resource;
  @Prop text: Resource;
  @Prop iconId: string;
  public imageClick: () => void = () => {
  }

  @Builder
  IconBuilder() {
    Column() {
      Column() {
        SymbolGlyph(this.url)
          .icon()
      }
      .iconContainer()
    }.stateStyles({
      pressed: {
        .iconContainerMaskLayer()
        .backgroundColor($r('sys.color.ohos_id_color_click_effect_transparent'))
      },
      normal: {
        .iconContainerMaskLayer()
        .backgroundColor('#00000000')
      }
    })
    .margin({
      bottom: this.scanLayout.widthBreakpoint === BREAKPOINT_SM &&
        (this.scanLayout.heightBreakpoint === BREAKPOINT_SM ||
          this.scanLayout.heightBreakpoint === BREAKPOINT_MD) ? ICON_MARGIN_BOTTOM_WITHOUT_TEXT :
        ICON_MARGIN_BOTTOM_WITH_TEXT,
    })
    .onClick(() => {
      this.imageClick();
    })
  }

  @Builder
  textBuilder() {
    Text(this.text).iconText()
      .fontSize(FontSizeScale.getLimitFontSize(this.fontSizeScale,
        CommonConstants.SCAN_FONT_SIZE_SCALE_LIMIT_SYMBOL_TEXT, $r('sys.float.Body_S')))
      .margin({
        bottom: TEXT_MARGIN_BOTTOM,
        top: TEXT_MARGIN_TOP
      })
  }

  build() {
    Column() {
      this.IconBuilder()
      if ((this.scanLayout.widthBreakpoint === BREAKPOINT_SM &&
        (this.scanLayout.heightBreakpoint === BREAKPOINT_LG || this.scanLayout.heightBreakpoint === BREAKPOINT_XL)) ||
        this.scanLayout.widthBreakpoint !== BREAKPOINT_SM) {
        this.textBuilder()
      }
    }
    .id(this.iconId)
    .justifyContent(this.scanLayout.widthBreakpoint === BREAKPOINT_SM ? FlexAlign.End : FlexAlign.Center)
    .height(this.scanLayout.widthBreakpoint === BREAKPOINT_SM ? '100%' : '')
    .accessibilityGroup(true)
    .onClick(() => {
      if(accessibility.isOpenAccessibilitySync()){
        this.imageClick();
      }
    })
  }
}
