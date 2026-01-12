import { ProductDetailModel } from "../model/BasicDataSource";
import { Month } from "../model/MonthDataSource";
import { ProductDetailSource } from "../model/ProductDetailSource";
import { hiTraceMeter } from "@kit.PerformanceAnalysisKit";

// [Start transition_scene_usually]
@Component
export struct TransitionScene {
  private productData: ProductDetailSource = new ProductDetailSource();

  aboutToAppear(): void {
    this.productData.getProductData();
  }

  build() {
    WaterFlow() {
      LazyForEach(this.productData, (item: ProductDetailModel) => {
        FlowItem() {
          // ...
        }
      }, (item: ProductDetailModel) => item.id.toString())
    }
    // ...
  }
}
// [End transition_scene_usually]

// [Start sliding_scene_usually]
@Reusable
@Component
export struct DateItemView {
  @State monthItem: Month = {
    month: '',
    num: 0,
    days: [],
    lunarDays: [],
    year: 0
  };
  // ...
  aboutToReuse(params: Record<string, Object>): void {
    hiTraceMeter.startTrace('reuse_' + (params.monthItem as Month).month, 1);
    this.monthItem = params.monthItem as Month;
    hiTraceMeter.finishTrace('reuse_' + (params.monthItem as Month).month, 1);
  }

  build() {
    Flex({ wrap: FlexWrap.Wrap }) {
      // ...
      ForEach(this.monthItem.days, (day: number, index: number) => {
        // ...
      }, (index: number): string => index.toString())
    }
    // ...
  }
}
// [End sliding_scene_usually]