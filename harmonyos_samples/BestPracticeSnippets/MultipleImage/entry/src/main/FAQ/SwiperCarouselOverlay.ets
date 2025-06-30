/**
 * 常见问题:实现类似轮播图的叠加效果
 */

import { Size } from '@kit.ArkUI';

const SIZE_ATTR_ARR: readonly Size[] =
  [{ width: 270, height: 240 }, { width: 290, height: 220 }, { width: 310, height: 200 }]; // 卡片尺寸类型
const SIZE_DIFF_ARR: Size = { width: -20, height: 20 }; // 前后卡片尺寸差值
const OPACITY_ATTR_ARR: readonly number[] = [1.0, 0.6, 0.3, 0]; // 透明度分界
const OPACITY_DIFF_ARR: readonly number[] = [0.4, 0.3, 0.3]; // 透明度分界间差值
const DATA_LENGTH = 6; // 数据长度
const ON_SHOW_PAGE = 3; // 显示卡片的数目
const OFFSET_DISTANCE_4_FADE_THREHOLD = 200; // 单向滑动卡片淡出淡入动效完成的拖动距离：vp单位
const OFFSET_DISTANCE_4_SWICH_THREHOLD = 150; // 滑动卡片触发切换的距离阈值：vp单位
const SWITCH_PAGE_VELOCITY_THREHOLD = 2000; // 滑动卡片触发切换的速度阈值

enum PageIndex {
  FIRSTPAGE = 0,
  SCENDPAGE = 1,
  THRIDPAGE = 2,
  FOURTHPAGE = 3
}

let timerId: number = 0;

@Component
struct SwiperExample {
  currentIndexArray: Array<number> = []; // 记录当前显示的卡片在数组中的索引值
  // 各个卡片的参数值，包括宽高尺寸，可视状态，z序，透明度。偏移量，图像尺寸
  @State sizeArray: Array<Size> = [];
  @State visibleArray: Array<Visibility> = [];
  @State zIndexArray: Array<number> = [];
  @State opacityArray: Array<number> = [];
  @State offsetXArray: Array<number> = [];
  @State scaleArray: Array<ScaleOptions> = [];
  @Prop @Watch('onAnimationEnable') isEnableAnimate: boolean = false;
  isAppear: boolean = false;
  isStart: boolean = false;
  private data: string[] = []; // 卡片数据数组
  // 滑动手势参数
  private panOption: PanGestureOptions = new PanGestureOptions({ direction: PanDirection.Left | PanDirection.Right });

  aboutToAppear(): void {
    for (let i = 0; i < DATA_LENGTH; i++) {
      this.data.push(`this is ${i}`);
      if (i < ON_SHOW_PAGE) { //前三个卡片的状态
        this.sizeArray.push({ width: SIZE_ATTR_ARR[i].width, height: SIZE_ATTR_ARR[i].height }); // 前三个卡片，三种尺寸
        this.visibleArray.push(Visibility.Visible); // 前三个卡片均可视
        this.zIndexArray.push(ON_SHOW_PAGE - i); // 索引越小，z序越大，叠层
        this.opacityArray.push(OPACITY_ATTR_ARR[i]); // 前三个卡片透明度
        this.currentIndexArray.push(i); // 当前展示的卡片在数组中的索引
      } else {
        this.sizeArray.push({ width: SIZE_ATTR_ARR[2].width, height: SIZE_ATTR_ARR[2].height }); // 剩下的卡片的尺寸都统一
        this.visibleArray.push(Visibility.None); // 剩下的卡片均不可见
        this.zIndexArray.push(0); // 剩下的卡片均在最小的序列上
        this.opacityArray.push(OPACITY_ATTR_ARR[3]); // 剩下的卡片均完全透明
      }
      this.scaleArray.push({
        // 每个卡片的图形尺寸均一样
        x: 1,
        y: 1,
        centerX: '50%',
        centerY: '50%'
      });
      this.opacityArray.push(0);
    }
    this.currentIndexArray.push(3);
  }

  onAnimationEnable(propName: string) {
    if (!this.isEnableAnimate && this.isStart) {
      clearInterval(timerId); // 停止循环播放
      this.isStart = false;
    }
  }

  changeSubPageWhenUpdate(dataIndex: number, PageIndex: number, distanceScl: number, isNeedToChangeSize: boolean,
    isNeedToChangeOpac: boolean) {
    if (dataIndex < 0 || dataIndex >= DATA_LENGTH || PageIndex < 1 || PageIndex >= (ON_SHOW_PAGE + 1)) {
      return;
    }
    if (isNeedToChangeSize) {
      this.sizeArray[dataIndex].width = SIZE_ATTR_ARR[PageIndex].width + SIZE_DIFF_ARR.width * (1.0 - distanceScl);
      this.sizeArray[dataIndex].height = SIZE_ATTR_ARR[PageIndex].height + SIZE_DIFF_ARR.height * (1.0 - distanceScl);
    }
    if (isNeedToChangeOpac) {
      this.opacityArray[dataIndex] = OPACITY_ATTR_ARR[PageIndex] + OPACITY_DIFF_ARR[PageIndex -1] * (1.0 - distanceScl);
    }
  }

  changePageWhenEnd(firPageOffsetX: number, isSwitch: boolean, rightOrleft: boolean) {
    this.offsetXArray[this.currentIndexArray[PageIndex.FIRSTPAGE]] =
      (isSwitch && rightOrleft) ? firPageOffsetX : -firPageOffsetX;
    this.opacityArray[this.currentIndexArray[PageIndex.FIRSTPAGE]] = isSwitch ? 0 : 1;
    this.scaleArray[this.currentIndexArray[PageIndex.FIRSTPAGE]].x =
      this.scaleArray[this.currentIndexArray[PageIndex.FIRSTPAGE]].y = isSwitch ? 0 : 1;
    this.opacityArray[this.currentIndexArray[PageIndex.SCENDPAGE]] = OPACITY_ATTR_ARR[isSwitch ? 0 : 1];
    this.sizeArray[this.currentIndexArray[PageIndex.SCENDPAGE]].width = SIZE_ATTR_ARR[isSwitch ? 0 :1].width;
    this.sizeArray[this.currentIndexArray[PageIndex.SCENDPAGE]].height = SIZE_ATTR_ARR[isSwitch ? 0 :1].height;

    this.opacityArray[this.currentIndexArray[PageIndex.THRIDPAGE]] = OPACITY_ATTR_ARR[isSwitch ? 1 : 2];
    this.sizeArray[this.currentIndexArray[PageIndex.THRIDPAGE]].width = SIZE_ATTR_ARR[isSwitch ? 1 : 2].width;
    this.sizeArray[this.currentIndexArray[PageIndex.THRIDPAGE]].height = SIZE_ATTR_ARR[isSwitch ? 1 : 2].height;

    this.opacityArray[this.currentIndexArray[PageIndex.FOURTHPAGE]] = OPACITY_ATTR_ARR[isSwitch ? 2 : 3];
  }

  changePagePropertyWhenFinished() {
    this.visibleArray[this.currentIndexArray[PageIndex.FIRSTPAGE]] = Visibility.None;
    this.visibleArray[this.currentIndexArray[PageIndex.FOURTHPAGE]] = Visibility.Visible;
    this.changeSubPageWhenUpdate(this.currentIndexArray[PageIndex.FIRSTPAGE], PageIndex.THRIDPAGE, 1.0, true, false);
    this.scaleArray[this.currentIndexArray[PageIndex.FIRSTPAGE]].x =
      this.scaleArray[this.currentIndexArray[PageIndex.FIRSTPAGE]].y = 1;
    this.offsetXArray[this.currentIndexArray[PageIndex.FIRSTPAGE]] = 0;
    for (let i = 0; i < this.currentIndexArray.length; i++) {
      this.zIndexArray[this.currentIndexArray[i]] =
        (this.zIndexArray[this.currentIndexArray[i]] + 1) % this.currentIndexArray.length;
      this.currentIndexArray[i] = (this.currentIndexArray[i] + 1) % this.data.length;
      this.opacityArray[this.currentIndexArray[i]] = OPACITY_ATTR_ARR[i];
    }
  }

  setSwiperAnimation() {
    timerId = setInterval(() => { // 设置定时器
      this.getUIContext().animateTo({
        duration: 250,
        curve: Curve.Linear,
        onFinish: () => {
          this.changePagePropertyWhenFinished();
        }
      }, () => {
        this.changePageWhenEnd(OFFSET_DISTANCE_4_FADE_THREHOLD, true, true);
      })
    }, 3000);
    this.isStart = true;
  }

  build() {
    Column() {
      Text(`CurrentOffset.x: ${Math.round(this.offsetXArray[this.currentIndexArray[PageIndex.FIRSTPAGE]] * 100) /
        100.0}`)
        .fontSize((20))
      // [Start SwiperCarouselOverlay]
      Stack({ alignContent: Alignment.Start }) {
        ForEach(this.data, (item: string, index: number) => {
          Column() {
            Text(item)
              .fontSize(30)
              .fontColor(Color.Blue)
          }
          .zIndex(this.zIndexArray[index])
          .visibility(this.visibleArray[index])
          .opacity(this.opacityArray[index])
          .backgroundColor(Color.Pink)
          .width(this.sizeArray[index].width)
          .height(this.sizeArray[index].height)
          .offset({ x: this.offsetXArray[index], y: 0 })
          .scale(this.scaleArray[index])
        }, (item: string, index: number) => `${item}index`)
      }
      .onAppear(() => { // 挂载之后只调用一次
        this.isAppear = true;
      })
      .gesture(
        PanGesture(this.panOption)
          .onActionStart((event: GestureEvent) => {
            clearInterval(timerId); // 停止循环播放
            this.isStart = false;
            this.visibleArray[this.currentIndexArray[3]] = Visibility.Visible;
          })
          .onActionUpdate((event: GestureEvent) => {
            if (!event) {
              return;
            }
            let distanceScl: number = 0;
            let index0 = this.currentIndexArray[PageIndex.FIRSTPAGE];
            // 顶层卡片的动效
            this.offsetXArray[index0] = event.offsetX;
            if (this.offsetXArray[index0] < 0) { // 左
              distanceScl = this.offsetXArray[index0] > -OFFSET_DISTANCE_4_FADE_THREHOLD ?
                1.0 + this.offsetXArray[index0] / OFFSET_DISTANCE_4_FADE_THREHOLD : 0;
            } else { // 右
              distanceScl = this.offsetXArray[index0] < OFFSET_DISTANCE_4_FADE_THREHOLD ?
                1.0 - this.offsetXArray[index0] / OFFSET_DISTANCE_4_FADE_THREHOLD : 0;
            }

            // 二层卡片的动效
            let index1 = this.currentIndexArray[PageIndex.SCENDPAGE];
            this.changeSubPageWhenUpdate(index1, PageIndex.SCENDPAGE, distanceScl, true, true);

            // 三层卡片的动效
            let index2 = this.currentIndexArray[PageIndex.THRIDPAGE];
            this.changeSubPageWhenUpdate(index1, PageIndex.THRIDPAGE, distanceScl, true, true);

            // 四层卡片的动效
            let index3 = this.currentIndexArray[PageIndex.FOURTHPAGE];
            this.changeSubPageWhenUpdate(index1, PageIndex.FOURTHPAGE, distanceScl, false, true);
          })
          .onActionEnd((event: GestureEvent) => { // 抬起手指
            if (!event) {
              return;
            }
            this.getUIContext().animateTo({
              duration: 200,
              curve: Curve.Linear,
              onFinish: () => { // 动效结束之后对各个卡片做状态赋值，保证每个组件在正确的状态
                // 在触发切换页的范围内
                if (Math.abs(this.offsetXArray[this.currentIndexArray[PageIndex.FIRSTPAGE]]) <
                  OFFSET_DISTANCE_4_SWICH_THREHOLD) {
                  this.visibleArray[this.currentIndexArray[PageIndex.FIRSTPAGE]] = Visibility.Visible;
                  this.visibleArray[this.currentIndexArray[PageIndex.FOURTHPAGE]] = Visibility.None;
                } else { // 在触发切换页的范围之外，更新状态
                  this.changePagePropertyWhenFinished();
                }
              }
            }, () => {
              if (this.offsetXArray[this.currentIndexArray[PageIndex.FIRSTPAGE]] > OFFSET_DISTANCE_4_SWICH_THREHOLD ||
                event.velocityX > SWITCH_PAGE_VELOCITY_THREHOLD) { // 向右淡出页面
                this.changePageWhenEnd(OFFSET_DISTANCE_4_FADE_THREHOLD, true, true);
              } else if (this.offsetXArray[this.currentIndexArray[PageIndex.FIRSTPAGE]] <
                -OFFSET_DISTANCE_4_SWICH_THREHOLD || event.velocityX < -SWITCH_PAGE_VELOCITY_THREHOLD) { // 向左淡出页面
                this.changePageWhenEnd(OFFSET_DISTANCE_4_FADE_THREHOLD, true, false);
              } else {
                this.changePageWhenEnd(0, false, true); // 回归
              }
            })
          })
      )
      .backgroundColor(Color.Black)
      .width('100%')
      .height('100%')
      // [End SwiperCarouselOverlay]
    }
    .width('100%')
    .height('100%')
    .margin({ top: 150 })
  }
}

@Entry
@Component
struct CarouselDemo {
  @State isPageShow: boolean = true;

  onPageShow(): void {
    this.isPageShow = true;
  }

  onPageHide(): void {
    this.isPageShow = false;
  }

  build() {
    Column() {
      SwiperExample({ isEnableAnimate: this.isPageShow })
    }
  }
}