class BasicDataSource implements IDataSource {
  private listeners: DataChangeListener[] = [];
  private originDataArray: TextClass[] = [];

  public totalCount(): number {
    return 0;
  }

  public getData(index: number): TextClass {
    return this.originDataArray[index];
  }

  registerDataChangeListener(listener: DataChangeListener): void {
    if (this.listeners.indexOf(listener) < 0) {
      console.info('add listener');
      this.listeners.push(listener);
    }
  }

  unregisterDataChangeListener(listener: DataChangeListener): void {
    const pos = this.listeners.indexOf(listener);
    if (pos >= 0) {
      console.info('remove listener');
      this.listeners.splice(pos, 1);
    }
  }

  notifyDataReload(): void {
    this.listeners.forEach(listener => {
      listener.onDataReloaded();
    })
  }

  notifyDataAdd(index: number): void {
    this.listeners.forEach(listener => {
      listener.onDataAdd(index);
    })
  }

  notifyDataChange(index: number): void {
    this.listeners.forEach(listener => {
      listener.onDataChange(index);
    })
  }

  notifyDataDelete(index: number): void {
    this.listeners.forEach(listener => {
      listener.onDataDelete(index);
    })
  }

  notifyDataMove(from: number, to: number): void {
    this.listeners.forEach(listener => {
      listener.onDataMove(from, to);
    })
  }
}

class MyDataSource extends BasicDataSource {
  private dataArray: TextClass[] = [];

  public totalCount(): number {
    return this.dataArray.length;
  }

  public getData(index: number): TextClass {
    return this.dataArray[index];
  }

  // [Start Add_delete data]
  public addData(index: number, data: TextClass): void {
    this.dataArray.splice(index, 0, data);
    this.notifyDataAdd(index);
  }

  public pushData(data: TextClass): void {
    this.dataArray.push(data);
    this.notifyDataAdd(this.dataArray.length - 1);
  }

  // [Start Add_delete data]
}

class TextClass {
  message: string = '';

  constructor(message: string) {
    this.message = message;
  }
}

@Entry
@Component
struct MaintainVisibleAreaContent {
  private data: MyDataSource = new MyDataSource();
  @StorageProp('topRectHeight') topRectHeight: number = 0;

  aboutToAppear() {
    for (let i = 0; i < 20; i++) {
      this.data.pushData(new TextClass(`Hello ${i}`));
    }
  }

  build() {
    Stack() {
      // [Start MaintainVisibleAreaContent]
      List({ space: 3 }) {
        LazyForEach(this.data, (item: TextClass) => {
          ListItem() {
            Row() {
              Text(item.message).fontSize(20)
            }
            .height(50)
            .margin({ left: 10, right: 10 })
          }
        }, (item: TextClass) => JSON.stringify(item))
      }
      .width('100%')
      .height('100%')
      .scrollBar(BarState.Off)
      .maintainVisibleContentPosition(true)

      // [End MaintainVisibleAreaContent]

      Button('Insert Data')
        .onClick(() => {
          this.data.addData(0, new TextClass('Hello -1'));
          this.data.addData(0, new TextClass('Hello -2'));
          this.data.addData(0, new TextClass('Hello -3'));
        })
    }
    .padding({ top: this.getUIContext().px2vp(this.topRectHeight) })
  }
}