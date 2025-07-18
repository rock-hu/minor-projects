// [Start quick_start]
import { MyDataSource } from '../model/MyDataSource'
import { image } from '@kit.ImageKit';
// [StartExclude flow_item]
@Reusable
@Component
struct ReusableItem {
  @State item: number = 0;

  aboutToReuse(params: Record<string, number>): void {
    this.item = params.item;
    console.log('Reuse item: ' + this.item);
  }

  build() {
    Column() {
      Text('N ' + this.item)
        .fontSize(12)
        .height('16vp')
      Image($rawfile(`sections/${this.item % 4}.jpg`))
        .objectFit(ImageFit.Fill)
        .width('100%')
        .layoutWeight(1)
    }
  }
}
// [EndExclude flow_item]
@Entry
@Component
struct ZoomChangeColumnPage {
  // [StartExclude variable_declare]
  @State minSize: number = 80
  @State maxSize: number = 210;
  @State fontSize: number = 24;
  @State colors: number[] = [0xFFC0CB, 0xDA70D6, 0x6B8E23, 0x6A5ACD, 0x00FFFF, 0x00FF7E];
  @State sections: WaterFlowSections = new WaterFlowSections();
  private data: MyDataSource = new MyDataSource();
  private itemWidthArray: number[] = [];
  private itemHeightArray: number[] = [];
  // [EndExclude variable_declare]
  // 1、Variables required for scaling operations.
  @State itemScale: number = 1;
  @State imageScale: number = 1;
  @State itemOpacity: number = 1;
  @State gestureEnd: boolean = false;
  @State pixelMap: image.PixelMap | undefined = undefined;
  @State columns: number = 4;
  private pinchTime: number = 0;
  private columnChanged: boolean = false;
  private oldColumn: number = this.columns;
  // [StartExclude section_init]
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

  aboutToAppear(): void {
    for(let i = 0; i < 100; i++) {
      this.data.pushData(i);
    }
    let lastCount = AppStorage.get<number>('columnsCount');
    if (typeof lastCount != 'undefined') {
      this.columns = lastCount;
    }
    this.setItemSizeArray();
  }
  // [EndExclude section_init]
  // 7、Adjust the number of columns according to the zoom ratio.
  changeColumns(scale: number) {
    this.oldColumn = this.columns;
    if (scale > (this.columns / (this.columns - 0.5))) {
      this.columns--;
      this.columnChanged = true;
    } else if (scale < 1 && this.columns < 4) {
      this.columns++;
      this.columnChanged = true;
    }
    this.columns = Math.min(4, Math.max(1, this.columns));
  }

  build() {
    Column({ space: 2 }) {
      // [StartExclude title]
      Row() {
        Text('双指缩放改变列数')
          .width('100%')
          .height('5%')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .margin({ top: 10, left: 16 })
      }
      // [EndExclude title]
      Stack() {
        // 2、Display the current screen's snapshot when zooming.
        Image(this.pixelMap)
          .width('100%')
          .height('100%')
          .scale({
            x: this.imageScale,
            y: this.imageScale,
            centerX: 0,
            centerY: 0
          })
        WaterFlow() {
          // [StartExclude water_flow_item]
          LazyForEach(this.data, (item: number) => {
            FlowItem() {
              ReusableItem({ item: item })
            }
            .width('100%')
            .aspectRatio(this.itemHeightArray[item % 100] / this.itemWidthArray[item % 100])
            .backgroundColor(this.colors[item % 5])
          }, (item: number) => item.toString())
          // [EndExclude water_flow_item]
        }
        .id('waterflow')
        // 3、Dynamic adjustment of column numbers.
        .columnsTemplate('1fr '.repeat(this.columns))
        .backgroundColor(0xFAEEE0)
        .width('100%')
        .height('100%')
        .layoutWeight(1)
        // 4、WaterFlow's scaling information.
        .opacity(this.itemOpacity)
        .scale({
          x: this.itemScale,
          y: this.itemScale,
          centerX: 0,
          centerY: 0
        })
        .priorityGesture(
          PinchGesture()
            .onActionStart((event: GestureEvent) => {
              // 5、Initialise scaling parameters.
              this.gestureEnd = false;
              this.pinchTime = event.timestamp;
              this.columnChanged = false;
              this.getUIContext().getComponentSnapshot().get('waterflow', (error: Error, pixelMap: image.PixelMap) => {
                if (error) {
                  console.info('error: ' + error.message);
                  return;
                }
                this.pixelMap = pixelMap;
              })
            })
            .onActionUpdate((event: GestureEvent) => {
              // 6、Calculate the zoom ratio based on the finger swipe event and dynamically change the number of columns.
              // 6.1、Intercept duplicate gestures.
              if (event.timestamp - this.pinchTime < 10000000) {
                return;
              }
              this.pinchTime = event.timestamp;
              // 6.2、Calculate the scaling ratio.
              let maxScale = this.oldColumn / (this.oldColumn - 1);
              this.itemScale = event.scale > maxScale ? maxScale : event.scale;
              this.imageScale = event.scale > maxScale ? maxScale : event.scale;
              this.itemOpacity = (this.itemScale > 1) ? (this.itemScale - 1) : (1 - this.itemScale);
              this.itemOpacity *= 3;
              // 6.3、Dynamically adjust the number of columns.
              if (!this.columnChanged) {
                this.changeColumns(event.scale);
              }
              if (this.columnChanged) {
                this.itemScale = this.imageScale * this.columns / this.oldColumn;
                if (event.scale < 1) {
                  this.itemScale = this.itemScale > 1 ? this.itemScale : 1;
                } else {
                  this.itemScale = this.itemScale < 1 ? this.itemScale : 1;
                }
              }
            })
            .onActionEnd((event: GestureEvent) => {
              // 8、Reset the relevant scaling parameters and execute the scaling animation.
              this.gestureEnd = true;
              this.getUIContext().animateTo({ duration: 300 }, () => {
                this.itemScale = 1;
                this.itemOpacity = 1;
              });
              AppStorage.setOrCreate<number>('columnsCount', this.columns);
            })
        )
      }
      .width('100%')
      .height('100%')
    }
  }
}
// [End quick_start]