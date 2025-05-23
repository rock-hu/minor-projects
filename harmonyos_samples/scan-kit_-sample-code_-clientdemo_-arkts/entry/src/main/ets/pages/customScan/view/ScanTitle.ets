import Logger from '../../../common/Logger';
import { BreakpointConstants } from '../constants/BreakpointConstants';
import { CommonConstants } from '../constants/CommonConstants';
import { BREAK_POINT_TYPE_MARGIN_VP } from '../model/BreakpointType';
import { FontSizeScale } from '../model/FontSizeScale';
import { ScanLayout } from '../model/ScanLayout';

@Extend(Text)
function scanTitle(color: ResourceColor, fontWeight: FontWeight) {
  .textAlign(TextAlign.Center)
  .fontFamily('HarmonyHeiTi')
  .fontWeight(fontWeight)
  .fontColor(color)
  .maxLines(2)
  .textOverflow({ overflow: TextOverflow.Ellipsis })
}

const TAG = 'ScanTitle';

@Component
export struct ScanTitle {
  @StorageLink('scanFontSizeScale') fontSizeScale: number = 1;
  @State scanLayout: ScanLayout = ScanLayout.getInstance();
  @Prop scanResultSize: number = 0;

  build() {
    Column() {
      Text(this.scanResultSize > 1 ? $r('app.string.Scan_barcode_tips_callback') : $r('app.string.Scan_barcode'))
        .scanTitle($r('sys.color.ohos_id_color_primary_contrary'), FontWeight.Regular)
        .fontSize(FontSizeScale.getLimitFontSize(this.fontSizeScale,
          CommonConstants.SCAN_FONT_SIZE_SCALE_LIMIT_TITLE_TEXT, $r('sys.float.Subtitle_L')))
        .width(this.scanLayout.heightBreakpoint === BreakpointConstants.BREAKPOINT_SM ||
          this.scanLayout.heightBreakpoint === BreakpointConstants.BREAKPOINT_MD ? `calc(100% - 80vp - 16vp)` : '100%')
        .constraintSize({
          maxWidth: this.scanLayout.heightBreakpoint === BreakpointConstants.BREAKPOINT_SM ||
            this.scanLayout.heightBreakpoint === BreakpointConstants.BREAKPOINT_MD ? `calc(100% - 80vp - 16vp)` : 460
        })
    }
    .onSizeChange((oldValue: SizeOptions, newValue: SizeOptions) => {
      Logger.info(TAG,
        `title height has changed, oldTitleHeight: ${oldValue.height}, newTitleHeight: ${newValue.height}`);
      let titleHeight: number = Number(newValue.height);
      if (Math.abs(this.scanLayout.titleHeight - titleHeight) > 1) {
        this.scanLayout.setTopLayout(titleHeight);
      }
    })
    .id('scan_scan_title')
    .alignItems(HorizontalAlign.Center)
    .margin({
      top: this.scanLayout.titleMarginTop
    })
    .padding({
      left: BREAK_POINT_TYPE_MARGIN_VP.getValue(this.scanLayout.widthBreakpoint),
      right: BREAK_POINT_TYPE_MARGIN_VP.getValue(this.scanLayout.widthBreakpoint)
    })
    .width('100%')
  }
}