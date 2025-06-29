/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import { BreakpointConstants, CommonConstants } from '@ohos/commons';
import { BreakpointType } from '@ohos/commons';
import { HomeConstants } from '../constants/HomeConstants';
import { Banner, BannerViewModel } from '../viewmodel/BannerViewModel';
import { getTabIndex } from './CommonView';

class BannerDataSource implements IDataSource {
  private bannerImgList: Banner[] = [];

  constructor(bannerImgList: Banner[]) {
    this.bannerImgList = bannerImgList;
  }

  totalCount(): number {
    return this.bannerImgList.length;
  }

  getData(index: number): Banner {
    return this.bannerImgList[index];
  }

  registerDataChangeListener(listener: DataChangeListener): void {
  }

  unregisterDataChangeListener() {
  }
}

@Component
export struct BannerView {
  @StorageLink('currentWidthBreakpoint') currentWidthBreakpoint: string = BreakpointConstants.BREAKPOINT_LG;
  @StorageLink('windowWidth') windowWidth: number = 0;
  @StorageLink('currentTopIndex') currentTopIndex: number = 0;
  private bannerImgList: Banner[] = new BannerViewModel().getBannerList();
  private bannerDataSource: BannerDataSource = new BannerDataSource([]);

  aboutToAppear(): void {
    this.bannerDataSource = new BannerDataSource(this.bannerImgList);
  }

  build() {
    Column() {
      // Banner layout.
      Row() {
        // [Start banner_view]
        Swiper() {
          LazyForEach(this.bannerDataSource, (item: Banner, index: number) => {
            // [StartExclude banner_view]
            Stack() {
              Image(this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM ? item.getBannerImg().getImgSrcSm() :
                item.getBannerImg().getImgSrc())
                .objectFit(ImageFit.Fill)
                .borderRadius($r('app.float.banner_img_border_radius'))
                .height(CommonConstants.FULL_PERCENT)
                .width(CommonConstants.FULL_PERCENT)
                .focusable(true)
                .zIndex(1)

              Column() {
                Text(item.getDescription().getIsLeftCenter().getValue(this.currentWidthBreakpoint) ?
                  `${item.getDescription().getDescription()[0]}${item.getDescription().getDescription()[1]}` :
                  item.getDescription().getDescription()[0])
                  .fontSize(item.getDescription().getFontSize().getValue(this.currentWidthBreakpoint)[0])
                  .fontWeight(item.getDescription().getFontWeight()[0])
                  .lineHeight(item.getDescription().getLineHeight().getValue(this.currentWidthBreakpoint)[0])
                  .fontColor(Color.White)
                  .letterSpacing(HomeConstants.BANNER_TEXT_LETTER_SPACES[2])
                  .maxLines(1)

                Text(item.getDescription().getDescription()[1])
                  .fontSize(item.getDescription().getFontSize().getValue(this.currentWidthBreakpoint)[0])
                  .fontWeight(item.getDescription().getFontWeight()[0])
                  .lineHeight(item.getDescription().getLineHeight().getValue(this.currentWidthBreakpoint)[0])
                  .fontColor(Color.White)
                  .letterSpacing(HomeConstants.BANNER_TEXT_LETTER_SPACES[2])
                  .maxLines(1)
                  .visibility(item.getDescription().getIsLeftCenter().getValue(this.currentWidthBreakpoint) ?
                  Visibility.None : Visibility.Visible)

                Text(item.getDescription().getDescription()[2])
                  .fontSize(item.getDescription().getFontSize().getValue(this.currentWidthBreakpoint)[1])
                  .fontWeight(item.getDescription().getFontWeight()[1])
                  .lineHeight(item.getDescription().getLineHeight().getValue(this.currentWidthBreakpoint)[1])
                  .fontColor(Color.White)
                  .margin({
                    top: item.getDescription().getTopPosition().getValue(this.currentWidthBreakpoint)[1]
                  })
                  .letterSpacing(HomeConstants.BANNER_TEXT_LETTER_SPACES[1])
                  .maxLines(1)
              }
              .padding({
                left: !item.getDescription().getIsLeftCenter().getValue(this.currentWidthBreakpoint) ?
                  item.getDescription().getLeftPosition().getValue(this.currentWidthBreakpoint)[0] : '0',
                top: !item.getDescription().getIsTopCenter().getValue(this.currentWidthBreakpoint) ?
                  item.getDescription().getTopPosition().getValue(this.currentWidthBreakpoint)[0] : '0'
              })
              .alignItems(item.getDescription().getIsLeftCenter().getValue(this.currentWidthBreakpoint) ?
                HorizontalAlign.Center : HorizontalAlign.Start)
              .justifyContent(!item.getDescription().getIsTopCenter().getValue(this.currentWidthBreakpoint) ?
                FlexAlign.Start : FlexAlign.Center)
              .height(CommonConstants.FULL_PERCENT)
              .width(CommonConstants.FULL_PERCENT)
              .zIndex(2)
            }
            // [EndExclude banner_view]
            .height(item.getBannerImg().getHeight().getValue(this.currentWidthBreakpoint))
            .width(CommonConstants.FULL_PERCENT)
            // The width and height vary with the container assembly and the aspect ratio remains unchanged.
            .aspectRatio(new BreakpointType(HomeConstants.BANNER_RATIOS[0], HomeConstants.BANNER_RATIOS[1],
              HomeConstants.BANNER_RATIOS[2]).getValue(this.currentWidthBreakpoint))
            .groupDefaultFocus(index === 0 ? true : false)
            .focusable(true)
            .padding({
              top: $r('app.float.banner_margin'),
              bottom: $r('app.float.banner_margin'),
              left: this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM ? $r('app.float.banner_padding_sm') : 0,
              right: this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM ? $r('app.float.banner_padding_sm') : 0
            })
          }, (item: Banner, index: number) => index + JSON.stringify(item))
        }
        .displayCount(this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM ? 1 : HomeConstants.TWO)
        // Interval between banner images
        .itemSpace(HomeConstants.SWIPER_ITEM_SPACE)
        .prevMargin(new BreakpointType($r('app.float.swiper_prev_next_margin_sm'),
          $r('app.float.swiper_prev_next_margin_md'), $r('app.float.swiper_prev_next_margin_lg'))
          .getValue(this.currentWidthBreakpoint))
        .nextMargin(new BreakpointType($r('app.float.swiper_prev_next_margin_sm'),
          $r('app.float.swiper_prev_next_margin_md'), $r('app.float.swiper_prev_next_margin_lg'))
          .getValue(this.currentWidthBreakpoint))
        // [End banner_view]
        // Setting the navigation point Style of the swiper.
        .indicator(this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM ? Indicator.dot()
          .itemWidth($r('app.float.swiper_item_size'))
          .itemHeight($r('app.float.swiper_item_size'))
          .selectedItemWidth($r('app.float.swiper_selected_item_width'))
          .selectedItemHeight($r('app.float.swiper_item_size'))
          .color($r('app.color.swiper_indicator'))
          .bottom($r('app.float.indicator_bottom'))
          .selectedColor(Color.White) : false
        )
        .loop(true)
        .width(CommonConstants.FULL_PERCENT)
        .visibility((this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_LG) &&
          (this.currentTopIndex === 1) ? Visibility.None : Visibility.Visible)
        .effectMode(EdgeEffect.None)
        // The sizes of the front and rear banners on the MD and LG devices are different.
        .index(2)
        .tabIndex(getTabIndex(HomeConstants.DIRECTION_LIST[2]))

        // Switch the tab to switch the banner display effect.
        // [Start banner_view_row]
        Row({ space: HomeConstants.BANNER_ROW_SPACE }) {
          BannerText({
            banner: this.bannerImgList[2],
            index: 0
          })
            .layoutWeight(CommonConstants.FOUR)
            .height(CommonConstants.FULL_PERCENT)

          BannerText({
            banner: this.bannerImgList[1],
            index: 1
          })
            .layoutWeight(CommonConstants.FOUR)
            .height(CommonConstants.FULL_PERCENT)

          Column() {
            BannerText({
              banner: this.bannerImgList[0],
              index: 2
            })
              .margin({ bottom: $r('app.float.new_banner_3_margin') })
              .layoutWeight(1)

            BannerText({
              banner: this.bannerImgList[4],
              index: 3
            })
              .margin({ top: $r('app.float.new_banner_3_margin') })
              .layoutWeight(1)
          }
          .layoutWeight(CommonConstants.THREE)
        }
        .height(this.getBannerNewHeight(this.windowWidth))
        .width(CommonConstants.FULL_PERCENT)
        .visibility((this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_LG) &&
          (this.currentTopIndex === 1) ? Visibility.Visible : Visibility.None)
        .padding({
          left: $r('app.float.banner_padding_sm'),
          right: $r('app.float.banner_padding_sm')
        })
        // [End banner_view_row]
      }
      .width(CommonConstants.FULL_PERCENT)
      .visibility(this.currentTopIndex === 2 ? Visibility.None : Visibility.Visible)

      Row() {
        // Immersive layout.
        Column() {
          Text(this.bannerImgList[2].getDescription().getIsLeftCenter().getValue(this.currentWidthBreakpoint) ?
            `${this.bannerImgList[2].getDescription().getDescription()[0]}${this.bannerImgList[2].getDescription()
              .getDescription()[1]}` : this.bannerImgList[2].getDescription().getDescription()[0])
            .fontSize(new BreakpointType($r('app.float.immersive_text_1_font_sm'),
              $r('app.float.immersive_text_1_font_md'), $r('app.float.immersive_text_1_font_lg'))
              .getValue(this.currentWidthBreakpoint))
            .fontWeight(this.bannerImgList[2].getDescription().getFontWeight()[0])
            .lineHeight(new BreakpointType($r('app.float.immersive_text_1_line_sm'),
              $r('app.float.immersive_text_1_line_md'), $r('app.float.immersive_text_1_line_lg'))
              .getValue(this.currentWidthBreakpoint))
            .fontColor(Color.White)
            .letterSpacing(HomeConstants.BANNER_TEXT_LETTER_SPACES[2])
            .maxLines(1)

          Text(this.bannerImgList[2].getDescription().getDescription()[1])
            .fontSize(new BreakpointType($r('app.float.immersive_text_1_font_sm'),
              $r('app.float.immersive_text_1_font_md'), $r('app.float.immersive_text_1_font_lg'))
              .getValue(this.currentWidthBreakpoint))
            .fontWeight(this.bannerImgList[2].getDescription().getFontWeight()[0])
            .lineHeight(new BreakpointType($r('app.float.immersive_text_1_line_sm'),
              $r('app.float.immersive_text_1_line_md'), $r('app.float.immersive_text_1_line_lg'))
              .getValue(this.currentWidthBreakpoint))
            .fontColor(Color.White)
            .letterSpacing(HomeConstants.BANNER_TEXT_LETTER_SPACES[2])
            .maxLines(1)
            .visibility(this.bannerImgList[2].getDescription().getIsLeftCenter().getValue(this.currentWidthBreakpoint) ?
            Visibility.None : Visibility.Visible)

          Text(this.bannerImgList[2].getDescription().getDescription()[2])
            .fontSize(new BreakpointType($r('app.float.immersive_text_2_font_sm'),
              $r('app.float.immersive_text_2_font_md'), $r('app.float.immersive_text_2_font_lg'))
              .getValue(this.currentWidthBreakpoint))
            .fontWeight(this.bannerImgList[2].getDescription().getFontWeight()[1])
            .lineHeight(new BreakpointType($r('app.float.immersive_text_2_line_sm'),
              $r('app.float.immersive_text_2_line_md'), $r('app.float.immersive_text_2_line_lg'))
              .getValue(this.currentWidthBreakpoint))
            .fontColor(Color.White)
            .margin({
              top: this.bannerImgList[2].getDescription().getTopPosition().getValue(this.currentWidthBreakpoint)[1]
            })
            .letterSpacing(HomeConstants.BANNER_TEXT_LETTER_SPACES[1])
            .maxLines(1)
        }
        .padding({
          left: new BreakpointType($r('app.float.immersive_col_padding_left_sm'),
            $r('app.float.immersive_col_padding_left_md'), $r('app.float.immersive_col_padding_left_lg'))
            .getValue(this.currentWidthBreakpoint),
          top: new BreakpointType($r('app.float.immersive_col_padding_top_sm'),
            $r('app.float.immersive_col_padding_top_md'), $r('app.float.immersive_col_padding_top_lg'))
            .getValue(this.currentWidthBreakpoint)
        })
        .height(CommonConstants.FULL_PERCENT)
        .width(CommonConstants.FULL_PERCENT)
        .alignItems(new BreakpointType(true, false, false).getValue(this.currentWidthBreakpoint) ?
        HorizontalAlign.Center : HorizontalAlign.Start)
        .justifyContent(!this.bannerImgList[2].getDescription().getIsTopCenter().getValue(this.currentWidthBreakpoint) ?
        FlexAlign.Start : FlexAlign.Center)
      }
      .margin({
        top: $r('app.float.immersive_col_margin')
      })
      .padding({
        left: $r('app.float.home_content_padding'),
        right: $r('app.float.home_content_padding')
      })
      .height(new BreakpointType($r('app.float.immersive_banner_height_sm'), $r('app.float.immersive_banner_height_md'),
        $r('app.float.immersive_banner_height_lg')).getValue(this.currentWidthBreakpoint))
      .width(CommonConstants.FULL_PERCENT)
      .visibility(this.currentTopIndex === 2 ? Visibility.Visible : Visibility.None)
    }
    .width(CommonConstants.FULL_PERCENT)
    .padding({
      left: this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_LG && this.currentTopIndex === 2 ?
      $r('app.float.side_bar_width') : 0
    })
  }

  getBannerNewHeight(windowWidth: number): string {
    // Obtain the window width and subtract the blank parts on both sides.
    let result: number = this.getUIContext().px2vp(windowWidth) - new BreakpointType(HomeConstants.VIDEO_GRID_MARGIN[0],
      HomeConstants.VIDEO_GRID_MARGIN[1], HomeConstants.VIDEO_GRID_MARGIN[2]).getValue(this.currentWidthBreakpoint) -
    HomeConstants.LG_SIDEBAR_WIDTH;
    // Calculate the width of a single image based on the percent.
    result = (result - HomeConstants.VIDEO_GRID_ITEM_SPACE * HomeConstants.TWO) * CommonConstants.FOUR /
    HomeConstants.ELEVEN;
    // Calculate the total height of the grid layout.
    return result / HomeConstants.NEW_BANNER_ASPECT_RATIO + HomeConstants.HEIGHT_UNIT;
  }
}

@Component
struct BannerText {
  public banner: Banner = new BannerViewModel().getBannerList()[0];
  public currentWidthBreakpoint: string = BreakpointConstants.BREAKPOINT_LG;
  public index: number = 0;

  build() {
    Stack() {
      Image(((this.index === 0) || (this.index === 1)) ? this.banner.getBannerImg().getImgSrcSm() :
        this.banner.getBannerImg().getImgSrc())
        .width(CommonConstants.FULL_PERCENT)
        .height(CommonConstants.FULL_PERCENT)
        .objectFit(ImageFit.Fill)
        .borderRadius($r('app.float.banner_img_border_radius'))

      Column() {
        Text(this.banner.getDescription().getIsLeftCenter().getValue(this.currentWidthBreakpoint) ?
          `${this.banner.getDescription().getDescription()[0]}${this.banner.getDescription().getDescription()[1]}` :
          this.banner.getDescription().getDescription()[0])
          .fontSize(this.banner.getDescription().getFontSize().getValue(this.currentWidthBreakpoint)[0])
          .fontWeight(this.banner.getDescription().getFontWeight()[0])
          .lineHeight(this.banner.getDescription().getLineHeight().getValue(this.currentWidthBreakpoint)[0])
          .fontColor(Color.White)
          .letterSpacing(HomeConstants.BANNER_TEXT_LETTER_SPACES[2])
          .maxLines(1)
        Text(this.banner.getDescription().getDescription()[1])
          .fontSize(this.banner.getDescription().getFontSize().getValue(this.currentWidthBreakpoint)[0])
          .fontWeight(this.banner.getDescription().getFontWeight()[0])
          .lineHeight(this.banner.getDescription().getLineHeight().getValue(this.currentWidthBreakpoint)[0])
          .fontColor(Color.White)
          .letterSpacing(HomeConstants.BANNER_TEXT_LETTER_SPACES[2])
          .maxLines(1)
          .visibility(this.banner.getDescription().getIsLeftCenter().getValue(this.currentWidthBreakpoint) ?
            Visibility.None : Visibility.Visible)

        Text(this.banner.getDescription().getDescription()[2])
          .fontSize(this.banner.getDescription().getFontSize().getValue(this.currentWidthBreakpoint)[1])
          .fontWeight(this.banner.getDescription().getFontWeight()[1])
          .lineHeight(this.banner.getDescription().getLineHeight().getValue(this.currentWidthBreakpoint)[1])
          .fontColor(Color.White)
          .margin({ top: this.banner.getDescription().getTopPosition().getValue(this.currentWidthBreakpoint)[1] })
          .letterSpacing(HomeConstants.BANNER_TEXT_LETTER_SPACES[1])
          .maxLines(1)
      }
      .padding({
        left: !this.banner.getDescription().getIsLeftCenter().getValue(this.currentWidthBreakpoint) ?
          this.banner.getDescription().getLeftPosition().getValue(this.currentWidthBreakpoint)[0] : '0',
        top: !this.banner.getDescription().getIsTopCenter().getValue(this.currentWidthBreakpoint) ?
          this.banner.getDescription().getTopPosition().getValue(this.currentWidthBreakpoint)[0] : '0'
      })
      .alignItems(this.banner.getDescription().getIsLeftCenter().getValue(this.currentWidthBreakpoint) ?
        HorizontalAlign.Center : HorizontalAlign.Start)
      .justifyContent(!this.banner.getDescription().getIsTopCenter().getValue(this.currentWidthBreakpoint) ?
        FlexAlign.Start : FlexAlign.Center)
      .width(CommonConstants.FULL_PERCENT)
      .height(CommonConstants.FULL_PERCENT)
    }
  }
}