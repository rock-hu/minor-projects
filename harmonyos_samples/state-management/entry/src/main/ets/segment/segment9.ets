export class LearningResource {
  id: string = "";
  collectionCount: number = 0;
}

export class CommonConstants {
  static readonly SPACE_12: number = 12;
}

interface GeneratedTypeLiteralInterface_1 {
  swiperData: LearningResource[];
}

@Component
struct HotFeedsView {
  @Prop hotFeedList: never[] = [];
  @Prop showMore: () => void = () => {
  };
  @Prop handleClick: (item: LearningResource) => void = () => {
  };

  build() {
    Column()
  }
}

@Component
struct TechArticlesView {
  @Prop articlesDataSource: never[] = [];
  @Prop handleClick: (item: LearningResource) => void = () => {
  };

  build() {
    Column()
  }
}

// [Start discover_view]
@Component
export struct DiscoverView {
  // [StartExclude Case6]
  private jumpList(): void {
  }
  @State hotFeedList: never[] = [];
  @State articlesDataSource: never[] = [];
  @State discoverModel: GeneratedTypeLiteralInterface_1 = { swiperData: [] };
  // [EndExclude Case6]
  // 1.Getting shared state related to jump logic
  @Consume('appPathStack') appPathStack: NavPathStack;
  @Consume('discoverPathStack') discoverPathStack: NavPathStack;
  @StorageProp('currentBreakpoint') currentBreakpoint: string = 'BreakpointTypeEnum.MD';
  // 5.Centralize the jump processing logic of the 3 components in the parent component
  jumpDetail(item: LearningResource): void {
    if (this.currentBreakpoint === 'BreakpointTypeEnum.LG') {
      this.discoverPathStack.pushPathByName('articleDetail', item);
    } else {
      this.appPathStack.pushPathByName('articleDetail', item);
    }
  }
  build() {
    // [StartExclude Case6]
    Column() {
      List() {
        ListItem() {
          BannerView({
            swiperData: this.discoverModel.swiperData,
            // 2.The top rotator component passes in the parent component's logic handler function
            handleClick: (item: LearningResource) => this.jumpDetail(item)
          })
        }
        // [EndExclude Case6]
        ListItem() {
          Column({ space: CommonConstants.SPACE_12 }) {
            HotFeedsView({
              hotFeedList: this.hotFeedList,
              showMore: () => this.jumpList(),
              // 3.Information list component passes in the logic handler function of the parent component
              handleClick: (item: LearningResource) => this.jumpDetail(item)
            })
            TechArticlesView({
              articlesDataSource: this.articlesDataSource,
              // 4.Logic handler function passed to parent component by technical article component
              handleClick: (item: LearningResource) => this.jumpDetail(item)
            })
          }
        }
      }
    }
    // [End Case6]
  }
}

// [Start Case6]
@Component
struct BannerView {
  @Prop swiperData: LearningResource[] = [];
  private swiperController: SwiperController = new SwiperController();
  // 6.The rotatoire component receives the logic handler passed to it by the parent component.
  handleClick: (item: LearningResource) => void = () => {
  };
  build() {
    Swiper(this.swiperController) {
      ForEach(this.swiperData, (item: LearningResource) => {
        Row() {
          Image($r('app.media.icon'))
            .width(55)
            .height(55)
            .borderRadius($r('app.float.component_radius'))
              //  7.When clicking on an image, call the received function to process the logic
            .onClick(() => this.handleClick(item))
        }
        // ...
      }, (item: LearningResource) => item.id)
    }
    // ...
  }
}
// [End discover_view]