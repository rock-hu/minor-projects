// [Start quick_start]
import { hilog } from '@kit.PerformanceAnalysisKit';
import MediaItem, { ItemType } from '../model/MediaItem';
import { StickyWaterFlowDataSource } from '../model/StickyWaterFlowDataSource';

const TAG = 'StickyPage';

// [StartExclude item_layout]
@Component
struct FlowVideoItem {
  @Prop item: MediaItem;
  controller: VideoController = new VideoController();

  aboutToReuse(params: Record<string, MediaItem>) {
    this.item = params.item as MediaItem;
  }

  build() {
    if (this.item.type === ItemType.VIDEO) {
      Stack({ alignContent: Alignment.BottomStart }) {
        Video({ src: this.item.videoUri, previewUri: this.item.videoCover, controller: this.controller })
          .controls(false)
          .muted(true)
          .loop(true)
          .borderRadius(8)
          .onVisibleAreaChange([0.0, 1.0], (isVisible: boolean, currentRatio: number) => {
            if (isVisible && currentRatio >= 1.0) {
              this.controller.start();
            }
            if (!isVisible || currentRatio < 1.0) {
              this.controller.pause();
            }
          })
        Text('NO. ' + (this.item.id + 1))
          .fontSize(12)
          .fontColor(Color.White)
          .margin({
            left: 8,
            bottom: 4
          })
      }
    } else {
      RelativeContainer() {
        Image(this.item.imageUri)
          .objectFit(ImageFit.Cover)
          .width('100%')
          .layoutWeight(1)
          .borderRadius(8)
          .alignRules({
            top: { anchor: '__container__', align: VerticalAlign.Top },
            bottom: { anchor: '__container__', align: VerticalAlign.Bottom },
            left: { anchor: '__container__', align: HorizontalAlign.Start },
            right: { anchor: '__container__', align: HorizontalAlign.End }
          })
          .id('image')

        Stack() {}
        .linearGradient({
          angle: 0,
          colors: [['#80000000', 0.0],
            ['#03000000', 1.0]]
        })
        .width('100%')
        .height(32)
        .borderRadius(8)
        .hitTestBehavior(HitTestMode.None)
        .alignRules({
          bottom: { anchor: '__container__', align: VerticalAlign.Bottom },
          left: { anchor: '__container__', align: HorizontalAlign.Start },
          right: { anchor: '__container__', align: HorizontalAlign.End }
        })
        .id('mask')

        Text('NO. ' + (this.item.id + 1))
          .fontSize(12)
          .fontColor(Color.White)
          .alignRules({
            bottom: { anchor: '__container__', align: VerticalAlign.Bottom },
            left: { anchor: '__container__', align: HorizontalAlign.Start }
          })
          .margin({
            left: 8,
            bottom: 4
          })
          .id('text')
      }
      .width('100%')
      .borderRadius(8)
      .backgroundColor(Color.Gray)
    }
  }
}
// [EndExclude item_layout]
@Component
export struct StickyPage {
  // [StartExclude variable_declare]
  @State arr: number[] = [1, 2, 3, 4, 5, 6];
  @State colors: number[] = [0xFFC0CB, 0xDA70D6, 0x6B8E23, 0x6A5ACD, 0x00FFFF, 0x00FF7F];
  @State minSize: number = 80;
  @State maxSize: number = 210;
  @State sections: WaterFlowSections = new WaterFlowSections();
  scroller: Scroller = new Scroller();
  dataSource: StickyWaterFlowDataSource = new StickyWaterFlowDataSource();
  private itemWidthArray: number[] = [];
  private itemHeightArray: number[] = [];
  // [EndExclude variable_declare]
  //1、定义吸顶布局高度
  @State scrollOffset: number = 0;
  private stick_item_height: number = 100;
  oneColumnSection: SectionOptions = {
    itemsCount: 3,
    crossCount: 1,
    columnsGap: 5,
    rowsGap: 10,
    margin: {
      top: 8,
      left: 0,
      bottom: 8,
      right: 0
    },
    onGetItemMainSizeByIndex: (index: number) => {
      if (index === 1) {
        //2、分组中设置吸顶布局高度
        return this.stick_item_height;
      } else {
        return 200;
      }
    }
  };
  twoColumnSection: SectionOptions = {
    itemsCount: 2,
    crossCount: 2,
    onGetItemMainSizeByIndex: (index: number) => {
      return 250;
    }
  };
  // [StartExclude section_init]
  getResourceStringArray(resource: Resource): Array<string> {
    let result: Array<string> = new Array<string>();
    try {
      result = this.getUIContext().getHostContext()?.resourceManager.getStringArrayValueSync(resource.id) as string[];
    } catch (e) {
      hilog.error(0x0000, TAG, `[getResourceString]getStringSync failed, error:${JSON.stringify(e)}.`);
    }
    return result;
  }

  getSize() {
    let ret = Math.floor(Math.random() * this.maxSize);
    return (ret > this.minSize ? ret : this.minSize);
  }

  setItemSizeArray() {
    for (let i = 0; i < 100; i++) {
      this.itemWidthArray.push(this.getSize());
      this.itemHeightArray.push(this.getSize());
    }
  }

  aboutToAppear() {
    this.setItemSizeArray();
    let sectionOptions: SectionOptions[] = [];
    let count = 0;
    let oneOrTwo = 0;
    while (count < 100) {
      if (oneOrTwo++ % 2 === 0) {
        sectionOptions.push(this.oneColumnSection);
        count += this.oneColumnSection.itemsCount;
      } else {
        sectionOptions.push(this.twoColumnSection);
        count += this.twoColumnSection.itemsCount;
      }
    }
    this.sections.splice(-1, 0, sectionOptions);
  }
  // [EndExclude section_init]
  build() {
    Stack({ alignContent: Alignment.TopStart }) {
      WaterFlow({ scroller: this.scroller, sections: this.sections }) {
        LazyForEach(this.dataSource, (item: MediaItem) => {
          FlowItem() {
            //3、为吸顶部分预留位置，吸顶位置在第二个元素位置，所以id为1
            if (item.id != 1) {
              FlowVideoItem({ item: item })
            }
          }
          .width('100%')
          .height(this.itemHeightArray[item.id % 100])
          .backgroundColor(Color.White)
        }, (item: MediaItem) => item.id.toString())
      }
      .columnsTemplate('1fr 1fr')
      .onWillScroll((offset: number) => {
        //4、监听瀑布流滚动事件
        this.scrollOffset = this.scroller.currentOffset().yOffset + offset;
      })

      Stack() {
        // [StartExclude stick_content]
        Column() {
          Scroll() {
            Row({ space: 0 }) {
              ForEach(this.getResourceStringArray($r('app.strarray.tab_titles')), (item: string) => {
                Row() {
                  Text(item)
                    .constraintSize({ minWidth: 56 })
                    .textAlign(TextAlign.Center)
                    .fontSize(14)
                    .fontColor(Color.Black)
                    .padding({ left: 16, right: 16 })
                    .height('100%')
                    .backgroundColor('#12000000')
                    .borderRadius(21)
                }
                .padding({
                  top: 0,
                  bottom: 0,
                  left: 0,
                  right: 8
                })
                .justifyContent(FlexAlign.Center)
                .height(36)
                .align(Alignment.Center)
              }, (item: string) => item.toString())
            }
          }
          .width('100%')
          .scrollBar(BarState.Off)
          .scrollable(ScrollDirection.Horizontal)
          .backgroundColor(Color.White)
          .padding({ top: 8 })

          Row() {
            Select([{ value: '距离' },
              { value: '<1km' },
              { value: '<5km' },
              { value: '>5km' }])
              .selected(1)
              .value('距离')
              .font({
                size: 16,
                family: 'serif',
                style: FontStyle.Normal
              })
              .fontColor(Color.Black)
              .selectedOptionFont({
                size: 16,
                family: 'serif',
                style: FontStyle.Normal
              })
              .optionFont({
                size: 16,
                family: 'serif',
                style: FontStyle.Normal
              })
              .backgroundColor('#12000000')
            Select([{ value: '分类' },
              { value: '餐饮' },
              { value: '休闲' },
              { value: '娱乐' }])
              .selected(1)
              .value('分类')
              .font({
                size: 16,
                family: 'serif',
                style: FontStyle.Normal
              })
              .fontColor(Color.Black)
              .selectedOptionFont({
                size: 16,
                family: 'serif',
                style: FontStyle.Normal
              })
              .optionFont({
                size: 16,
                family: 'serif',
                style: FontStyle.Normal
              })
              .backgroundColor('#12000000')
              .margin({ left: 8 })
            Select([{ value: '排序' },
              { value: '1' },
              { value: '2' },
              { value: '3' }])
              .selected(1)
              .value('排序')
              .font({
                size: 16,
                family: 'serif',
                style: FontStyle.Normal
              })
              .fontColor(Color.Black)
              .selectedOptionFont({
                size: 16,
                family: 'serif',
                style: FontStyle.Normal
              })
              .optionFont({
                size: 16,
                family: 'serif',
                style: FontStyle.Normal
              })
              .backgroundColor('#12000000')
              .margin({ left: 8 })

          }
          .backgroundColor(Color.White)
          .width('100%')
          .padding({ top: 8, bottom: 8 })
        }.alignItems(HorizontalAlign.Start)
        // [EndExclude stick_content]
      }
      .width('100%')
      .height(this.stick_item_height)
      .hitTestBehavior(HitTestMode.Transparent)
      //5、根据瀑布流滑动偏移量动态调整吸顶位置
      .position({ x: 0, y: this.scrollOffset >= 220 ? 0 : 220 - this.scrollOffset })
    }
  }
}
// [StartExclude homepage]
@Entry
@Component
struct StickOnTopPage {
  @State currentIndex: number = 0;
  @State changeValue: string = '';
  @State tabsIndex: number = 0;
  tabsController: TabsController = new TabsController();
  controller: SearchController = new SearchController();

  @Builder
  tabBuilder(index: number, name: string | Resource) {
    Row() {
      Text(name)
        .constraintSize({ minWidth: 56 })
        .textAlign(TextAlign.Center)
        .fontSize(14)
        .fontColor(this.tabsIndex === index ? Color.White : Color.Black)
        .height('100%')
        .padding({ left: 16, right: 16 })
        .backgroundColor(this.tabsIndex === index ? '#0A59F7' : '#12000000')
        .borderRadius(21)

    }
    .padding({
      top: 10,
      bottom: 10,
      left: 0,
      right: 8
    })
    .justifyContent(FlexAlign.Center)
    .height(56)
    .align(Alignment.Center)
    .onClick(() => {
      this.tabsIndex = index;
      this.tabsController.changeIndex(index);
    })
  }

  build() {
    Column() {
      Row() {
        Image($r('app.media.ic_public_arrow_left'))
          .width(40)
          .height(40)
          .padding(8)
          .borderRadius(21)
          .backgroundColor('#10000000')
          .onClick(() => {
            this.getUIContext().getRouter().back();
          })
        Search({ value: this.changeValue, placeholder: '', controller: this.controller })
          .searchButton('search')
          .layoutWeight(1)
          .margin({ left: 8 })
          .backgroundColor('#10000000')
      }
      .margin({
        left: 16,
        right: 16,
        top: 8,
        bottom: 8,
      })

      Tabs({ index: this.currentIndex, controller: this.tabsController }) {
        TabContent() {
          StickyPage()
        }
        .tabBar(this.tabBuilder(0, '综合'))

        TabContent() {
          StickyPage()
        }
        .tabBar(this.tabBuilder(1, '商户'))

        TabContent() {
          StickyPage()
        }
        .tabBar(this.tabBuilder(2, '笔记'))

        TabContent() {
          StickyPage()
        }
        .tabBar(this.tabBuilder(3, '团购'))

        TabContent() {
          StickyPage()
        }
        .tabBar(this.tabBuilder(4, '用户'))

        TabContent() {
          StickyPage()
        }
        .tabBar(this.tabBuilder(5, '其他'))
      }
      .padding({
        left: 16,
        right: 16
      })
      .scrollable(true)
      .barMode(BarMode.Scrollable)
      .onChange((index: number) => {
        this.tabsIndex = index;
      })
    }
  }
}
// [EndExclude homepage]
// [End quick_start]