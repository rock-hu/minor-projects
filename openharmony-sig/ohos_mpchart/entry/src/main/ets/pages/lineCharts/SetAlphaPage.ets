import {
  JArrayList,
  EntryOhos,
  ILineDataSet,
  LineData,
  LineChart,
  LineChartModel,
  Mode,
  LineDataSet,
  XAxisPosition,
} from '@ohos/mpchart';
import AlphaDataRender from './AlphaDataRender';

@Entry
@Component
struct SetAlphaPage {
  private model: LineChartModel = new LineChartModel();

  aboutToAppear() {
    let values: JArrayList<EntryOhos> = new JArrayList<EntryOhos>();
    for (let i = 1; i <= 20; i++) {
      values.add(new EntryOhos(i, Math.random() * 100));
    }
    let dataSet = new LineDataSet(values, 'DataSet');
    dataSet.setMode(Mode.CUBIC_BEZIER);
    dataSet.setDrawCircles(false);
    let dataSetList: JArrayList<ILineDataSet> = new JArrayList<ILineDataSet>();
    dataSetList.add(dataSet);

    let lineData: LineData = new LineData(dataSetList);
    this.model?.setData(lineData);
    this.model.getAxisLeft()?.setAxisLineWidth(2);
    this.model.getXAxis()?.setPosition(XAxisPosition.BOTTOM);
    this.model.getAxisRight()?.setEnabled(false);
    this.model.getDescription()?.setEnabled(false);
    this.model.setRenderer(new AlphaDataRender(this.model, this.model.getAnimator()!, this.model.getViewPortHandler()))

  }

  build() {
    Column() {
      LineChart({ model: this.model })
        .width('100%')
        .height('50%')
        .backgroundColor(Color.White)
    }
  }
}