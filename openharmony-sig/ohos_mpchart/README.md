# mpchart

## Introduction
mpchart is a chart library that contains various types of custom charts, including line charts, bar charts, pie charts, candle charts, bubble charts, radar charts, and waterfall charts. These charts are mainly used to summarize service data, such as the sales data trend chart and stock price trend chart, helping you quickly implement the chart user interface (UI).

## Effects
![gif](preview/preview.gif)
## How to Install

```
 ohpm install @ohos/mpchart
```

For details, see [Installing an OpenHarmony HAR](https://gitee.com/openharmony-tpc/docs/blob/master/OpenHarmony_har_usage.md).

## How to Use

### Bar Chart

1. Initialize the bar chart data.

```
 import {
  BarChart, // Bar chart class.
  BarChartModel, // Configuration builder class of the bar chart.
  BarData, // Data packet of the bar chart.
  BarDataSet, // Dataset of the bar chart.
  BarEntry, // Data structure of the bar chart.
  ChartGesture, // Gesture event mode.
  Description, // The Description component.
  EntryOhos, // Base class of the chart data structure.
  Fill, // Builder class of the chart filling type.
  Highlight, // Highlighted data in the chart.
  IBarDataSet, // Operation class of the bar chart data set.
  JArrayList, // Utility class of the dataset.
  Legend, // The Legend component.
  LimitLabelPosition, // Enum class of the LimitLine label position.
  LimitLine, // The LimitLine component.
  MarkerView, // The Markerview component.
  OnChartGestureListener, // Gesture event listener.
  OnChartValueSelectedListener, // Value selection listener.
  XAxis, // The X-axis component.
  XAxisPosition, // Enum class of the X-axis label position.
  YAxis, // The Y-axis component.
  YAxisLabelPosition // Enum class of the Y-axis label position.
} from '@ohos/mpchart';

// Construct a listener for value selection events.
  private valueSelectedListener: OnChartValueSelectedListener = {
    onValueSelected: (e: EntryOhos, h: Highlight) => {
    // ...todoSomething
    },
    onNothingSelected: () => {
    // ...todoSomething
    }
  }
  
  // Construct a listener for gesture events.
  private chartGestureListener: OnChartGestureListener = {
    onChartGestureStart: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, lastPerformedGestureMode: ChartGesture) => {
    },
    onChartGestureEnd: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, lastPerformedGestureMode: ChartGesture) => {
    // ...todoSomething
    },
    onChartLongPressed: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
    // ...todoSomething
    },
    onChartDoubleTapped: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
    // ...todoSomething
    },
    onChartSingleTapped: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
    // ...todoSomething
    },
    onChartFling: (isTouchEvent: boolean, me1: TouchEvent | GestureEvent, me2: TouchEvent, velocityX: number, velocityY: number) => {
    // ...todoSomething
    },
    onChartScale: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, scaleX: number, scaleY: number) => {
    // ...todoSomething
    },
    onChartTranslate: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, dX: number, dY: number) => {
    // ...todoSomething
    }
  }
  
  // Initialize the chart data.
  aboutToAppear() {
  
  	// Initialize the configuration builder class of the chart. This step is mandatory.
    this.model = new BarChartModel();
    
    // Configure the chart style. The components can be configured in any sequence.
    
    // Add a value selection listener for the chart.
    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);
    // Add a gesture listener for the chart.
    this.model.setOnChartGestureListener(this.chartGestureListener);
	// Obtain the Description component and disable it. That is, the Description component is not drawn in the chart.
    let description: Description | null = this.model.getDescription()
    if (description) {
      description.setEnabled(false);
    }
	// Obtain the Legend component and disable it.
    let l: Legend | null = this.model.getLegend();
    if (l) {
      l.setEnabled(false);
    }
    // Set the maximum visible number of value labels drawn on the chart. If the data number exceeds this value, the value label of the chart is not drawn.
    this.model.setMaxVisibleValueCount(40);
	// Set whether to draw the background color of the chart. The background color is drawn in the bar chart, excluding the part outside the axis.
    this.model.setDrawGridBackground(false);
    // Set the background color of the chart. The color must meet the CanvasRenderingContext2D.fillstyle/strokestyle specifications.
    this.model.setGridBackgroundColor('#500000ff')
    // Set whether to draw the bar shadow.
    this.model.setDrawBarShadow(false);
	// Set whether to draw the value above the bar.
    this.model.setDrawValueAboveBar(false);
    // Set whether to highlight the full bar. This function can only be used in stacked bar chart.
    this.model.setHighlightFullBarEnabled(false);
	// Set LimitLine of the left Y-axis. You can set the line width, line segment style, label position, and text size.
    this.limitLine1 = new LimitLine(120, 'Upper Limit');
    this.limitLine1.setLineWidth(4);
    this.limitLine1.enableDashedLine(10, 10, 0);
    this.limitLine1.setLabelPosition(LimitLabelPosition.RIGHT_TOP);
    this.limitLine1.setTextSize(10);

    this.limitLine2 = new LimitLine(50, 'Lower Limit');
    this.limitLine2.setLineWidth(4);
    this.limitLine2.enableDashedLine(10, 10, 0);
    this.limitLine2.setLineColor(Color.Yellow);
    this.limitLine2.setLabelPosition(LimitLabelPosition.RIGHT_BOTTOM);
    this.limitLine2.setTextSize(10);

    // Set the information of the left Y-axis.
    this.leftAxis = this.model.getAxisLeft();
    if (this.leftAxis) {
      this.leftAxis.setAxisMinimum(0); // this replaces setStartAtZero(true)
      this.leftAxis.setDrawLimitLinesBehindData(false);

      // Add LimitLines to the left Y-axis.
      this.leftAxis.addLimitLine(this.limitLine1);
      this.leftAxis.addLimitLine(this.limitLine2);
    }
	// Set the information of the right Y-axis.
    this.rightAxis = this.model.getAxisRight();
    if (this.rightAxis) {
      this.rightAxis.setEnabled(false);
      this.rightAxis.setAxisMinimum(0);
    }
	// Set the information of the X-axis.
    this.xAxis = this.model.getXAxis();
    if (this.xAxis) {
      this.xAxis.setPosition(XAxisPosition.BOTTOM);
    }
	// Set the MarkerView component.
    this.normalMarker = new MarkerView();
    this.model.setMarker(this.normalMarker);
    // Set a custom MarkerView. This step is optional.
    this.stackMarker = new CustomMarkerView();
	// Generate normal data.
    this.data = this.getNormalData();
    // Bind the data to the chart configuration class.
    this.model.setData(this.data);
    // Set the maximum visible range of the X-axis. By default, all the data in X-axis is visible.
    this.model.setVisibleXRangeMaximum(20);
  }

   private getNormalData(): BarData {
    let values: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    values.add(new BarEntry(1, 73.3));
    values.add(new BarEntry(2, 5.4));

    let dataSet: BarDataSet = new BarDataSet(values, 'DataSet');
    dataSet.setHighLightColor(Color.Black);
    dataSet.setDrawIcons(false);
    // Set the color of the bar.
    dataSet.setColorByColor(Color.Pink);

    let dataSetList: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
    dataSetList.add(dataSet);

    let barData: BarData = new BarData(dataSetList);
    // Set the width of the bar.
    barData.setBarWidth(0.85);
    // Set the top radius of the bar.
    barData.setTopRadius(5);
    return barData;
  }

  private getGradientData(): BarData {
    let values: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    values.add(new BarEntry(1, 32.9));
    values.add(new BarEntry(2, 44.7));

    let dataSet: BarDataSet = new BarDataSet(values, 'DataSet');
    dataSet.setHighLightColor(Color.Black);
    dataSet.setDrawIcons(false);
    let startColor1: string = '#ffffbb33';
    let startColor2: string = '#ff33b5e5';
    let startColor3: string = '#ffffbb33';
    let startColor4: string = '#ff99cc00';
    let startColor5: string = '#ffff4444';
    let endColor1: string = '#ff0099cc';
    let endColor2: string = '#ffaa66cc';
    let endColor3: string = '#ff669900';
    let endColor4: string = '#ffcc0000';
    let endColor5: string = '#ffff8800';
    let gradientFills: JArrayList<Fill> = new JArrayList<Fill>();
    gradientFills.add(new Fill(startColor1, endColor1));
    gradientFills.add(new Fill(startColor2, endColor2));
    gradientFills.add(new Fill(startColor3, endColor3));
    gradientFills.add(new Fill(startColor4, endColor4));
    gradientFills.add(new Fill(startColor5, endColor5));
	// Set the gradient color of the bar.
    dataSet.setFills(gradientFills);

    let dataSetList: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
    dataSetList.add(dataSet);

    let barData: BarData = new BarData(dataSetList);
    barData.setBarWidth(0.85);
    return barData;
  }

  private getStackData(): BarData {
    let values: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    values.add(new BarEntry(1, [38.0, 28.0, 39.8]));
    values.add(new BarEntry(2, [18.2, 16.1, 16.1]));

    let set1: BarDataSet | null = null;
    set1 = new BarDataSet(values, "Statistics Vienna 2014");
    set1.setDrawIcons(false);
    // Set the color for the specified segments of the bar.
    set1.setColorsByArr([Color.Red, Color.Green, Color.Pink]);
    set1.setStackLabels(["Births", "Divorces", "Marriages"]);

    let dataSets: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
    dataSets.add(set1);

    let data: BarData = new BarData(dataSets);
    data.setValueTextColor(Color.White);
    return data;
  }
```

2. Add the data to the custom vertical bar chart component.

```
// Set the configuration builder class for the component. If the chart needs to be displayed during page initialization, use aboutToAppear to build the chart data.
// If the chart is triggered by event,
// you can use barData.notifyDataSetChanged(); to update data,
// use this.model.notifyDataSetChanged(); to update axis data,
// and use This.model.invalidate(); to update chart model.
BarChart({ model: this.model })
  .width('100%')
  .height('30%')
```

### Line chart

1. Initialize the line chart data.

```
import {
  JArrayList, // Utility class of the dataset.
  XAxis, // The X-axis component.
  XAxisPosition, // Enum class of the X-axis label position.
  YAxis, // The Y-axis component.
  Description, // The Description component.
  Legend, // The Legend component.
  OnChartValueSelectedListener, // Value selection listener.
  Highlight, // Highlighted data in the chart.
  EntryOhos, // Base class of the chart data structure.
  YAxisLabelPosition, // Enum class of the Y-axis label position.
  LineDataSet, // Dataset of the line chart.
  ILineDataSet, // Operation class of the line data set.
  LineData, // Data packet of the line chart.
  Mode, // Enum class of the line chart shape.
  LineChart, // Line chart class.
  LineChartModel,// Configuration builder class of the line chart.
  LimitLine, // The LimitLine component.
  LimitLabelPosition, // Enum class of the LimitLine label position.
  ChartColorStop, // Color class.
  LegendForm, // Enum class of the legend form.
} from '@ohos/mpchart';

// Construct a listener for value selection events.
  private valueSelectedListener: OnChartValueSelectedListener = {
    onValueSelected: (e: EntryOhos, h: Highlight) => {
    // ...todoSomething
    },
    onNothingSelected: () => {
    // ...todoSomething
    }
  }
  
  // Initialize the chart data.
  aboutToAppear() {
  
  	// Initialize the configuration builder class of the chart. This step is mandatory.
    this.model = new LineChartModel();
    
    // Configure the chart style. The components can be configured in any sequence.
    
    // Add a value selection listener for the chart.
    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);
	// Obtain the Description component and disable it. That is, the Description component is not drawn in the chart.
    let description: Description | null = this.model.getDescription()
    if (description) {
      description.setEnabled(false);
    }
    
	// Obtain the Legend component and set the legend form to line.
    let legend: Legend | null = this.model.getLegend();
    if (legend) {
      legend.setEnabled(true);
      // draw legend entries as lines
      legend.setForm(LegendForm.LINE);
    }
    
    // Set the maximum visible number of value labels drawn on the chart. If the data number exceeds this value, the value label of the chart is not drawn.
    this.model.setMaxVisibleValueCount(60);
    
	// Set the LimitLine component of the left Y-axis, which includes the line width, line segment style, label position, and label font size.
    this.limitLine1 = new LimitLine(120, 'Upper Limit');
    this.limitLine1.setLineWidth(4);
    // Set the dashed line style.
    this.limitLine1.enableDashedLine(10, 10, 0);
    // Set the label position.
    this.limitLine1.setLabelPosition(LimitLabelPosition.RIGHT_TOP);
    this.limitLine1.setTextSize(10);

    this.limitLine2 = new LimitLine(50, 'Lower Limit');
    this.limitLine2.setLineWidth(4);
    this.limitLine2.enableDashedLine(10, 10, 0);
    this.limitLine2.setLineColor(Color.Yellow);
    this.limitLine2.setLabelPosition(LimitLabelPosition.RIGHT_BOTTOM);
    this.limitLine2.setTextSize(10);

    // Set the information of the left Y-axis.
    this.leftAxis = this.model.getAxisLeft();
    if (this.leftAxis) {
      // Set the number of labels.
      this.leftAxis.setLabelCount(8, false);
      // Set the label position.
      this.leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART)
      // Set the distance between the highest data point and the top of the chart.
      this.leftAxis.setSpaceTop(15);
      // Set the maximum value.
      this.leftAxis.setAxisMinimum(0);
      // Set the minimum value.
      this.leftAxis.setAxisMaximum(200);
    }

	// Set the information of the right Y-axis.
    this.rightAxis = this.model.getAxisRight();
    if (this.rightAxis) {
      this.rightAxis.setLabelCount(8, false);
      this.rightAxis.setDrawGridLines(false);
      this.rightAxis.setSpaceTop(15);
      this.rightAxis.setAxisMinimum(0);
      this.rightAxis.setAxisMaximum(200);
      this.rightAxis.setEnabled(false);
    }
    
	// Set the information of the X-axis.
        this.xAxis = this.model.getXAxis();
    if (this.xAxis) {
      this.xAxis.setPosition(XAxisPosition.BOTTOM);
      this.xAxis.setDrawGridLines(false);
      this.xAxis.setGranularity(1);
      this.xAxis.setLabelCount(7);
    }
    
	// Set the MarkerView component.
    this.normalMarker = new MarkerView();
    this.model.setMarker(this.normalMarker);
    // Set a custom MarkerView. This step is optional.
    this.stackMarker = new CustomMarkerView();
    // Generate the chart data.
    let lineData: LineData = this.getLineData();
    // Bind the data to the chart configuration class.
    this.model.setData(lineData);
    // Set the maximum visible range of the X-axis. By default, all the data in X-axis is visible.
    this.model.setVisibleXRangeMaximum(20);
  }

   private getLineData(): LineData {

    let start: number = 1;
    let values: JArrayList<EntryOhos> = new JArrayList<EntryOhos>();
    for (let i = start; i < 20; i++) {
      let val = Number(Math.random() * 141);

      if (Math.random() * 100 < 25) {
        values.add(new EntryOhos(i, val));
      } else {
        values.add(new EntryOhos(i, val));
      }
    }

    this.dataSet = new LineDataSet(values, 'DataSet');
    this.dataSet.setHighLightColor(Color.Black);
    this.dataSet.setDrawIcons(false);

    this.dataSet.setMode (Mode.LINEAR); // Set the linear mode.
    this.dataSet.setDrawCircles(true); // Set whether to draw circles for the entries.
    this.dataSet.setDrawCircleHole (false); // Set whether to draw the circle hole.
    this.dataSet.setColorByColor(Color.Black); // Set the line color.

    // Set the gradient fill color.
    let gradientFillColor = new JArrayList<ChartColorStop>();
    gradientFillColor.add(["#0C0099CC", 0.2]);
    gradientFillColor.add(["#7F0099CC", 0.4]);
    gradientFillColor.add(["#0099CC", 1.0]);
    this.dataSet.setGradientFillColor(gradientFillColor);
    this.dataSet.setDrawFilled(true);


    // Set the circle color.
    this.dataSet.setCircleColor(Color.Blue); // Set the color as required.

    // Set the radius of the data point.
    this.dataSet.setCircleRadius(4); // Set the circle radius.
    this.dataSet.setCircleHoleRadius(2); // Set the radius of the circle hole.

    let dataSetList: JArrayList<ILineDataSet> = new JArrayList<ILineDataSet>();
    dataSetList.add(this.dataSet);

    let lineData: LineData = new LineData(dataSetList);
    return lineData
  }
  
```

2. Add the data to the custom line chart component.

```
// Set the configuration builder class for the component. If the chart needs to be displayed during page initialization, use aboutToAppear to build the chart data.
// If the chart is triggered by event,
// you can use lineData.notifyDataSetChanged(); to update data,
// use this.model.notifyDataSetChanged(); to update axis data,
// and use This.model.invalidate(); to update chart model.
LineChart({ model: this.model })
  .width('100%')
  .height('30%')
```

### Pie Chart

1. Initialize the pie chart data.

```
   import {
     ChartGesture,         // Chart gesture.
     ColorTemplate,        // Color template.
     EntryOhos,            // Base class of the chart data structure.
     Highlight,            // Highlighted data in the chart.
     IPieDataSet,          // Pie dataset API.
     JArrayList,           // Utility class of the data set.
     Legend,               // The Legend component.
     MPPointF,             // The MPPointF class.
     OnChartGestureListener,   // Gesture event listener.
     OnChartValueSelectedListener,  // Value selection listener.
     PieChart,             // The PieChart component.
     PieChartModel,        // The configuration builder class of the pie chart.
     PieData,              // Data packet of the pie chart.
     PieDataSet,           // Dataset of the pie chart.
     PieEntry,             // Data structure of the pie chart.
   } from '@ohos/mpchart';
  
  // Initialize the chart data.
  
  // Initialize the configuration builder class of the chart. This step is mandatory.
  private model: PieChartModel = new PieChartModel();
    
  aboutToAppear() {
    // Configure the chart style. The components can be configured in any sequence.

    let l: Legend | null = this.model.getLegend();
    if (l) {
      l.setEnabled(true);
    }
    this.model.animateX(1000);  // Animates the charts values on the X-axis and set the animation duration to 1000 ms.
    this.model.setUsePercentValues(true);  // Set whether to use the percent value.
    this.model.getDescription()? .setEnabled(false); // Set whether to enable the Description component.
    this.model.setExtraOffsets(5, 10, 5, 5);  // Set the extra offsets.

    this.model.setOnChartGestureListener(this.chartGestureListener);  // Set the listener for the gesture events.

    this.model.setDragDecelerationFrictionCoef(0.95);  // Set the drag deceleration friction coefficient.

    this.model.setCenterText("mpchart");  // Set the center text.
    this.model.setCenterTextSize(22);  // Set the size of the center text.

    this.model.setDrawHoleEnabled(true);  // Set whether to draw the center hole.
    this.model.setHoleColor(Color.White);  // Set the color of the center hole.

    this.model.setTransparentCircleColor(Color.Red);  // Set the color of the transparent circle.
    this.model.setTransparentCircleAlpha(110);  // Set the alpha of the transparent circle.

    this.model.setHoleRadius(58);  // Set the radius of the center hole.
    this.model.setTransparentCircleRadius(61);  // Set the radius of the transparent circle.

    this.model.setDrawCenterText (true); // Set whether to draw the center text.

    this.model.setRotationAngle(0); // Set the rotation angle.
    // Set whether to enable the rotation gestures on the chart.
    this.model.setRotationEnabled(true);
    this.model.setHighlightPerTapEnabled(true);  // Set whether to enable the highlight effect.

    this.setData (4, 10); // Set the chart data.
  }

   // Initialize the pie chart data.
  private setData(count: number, range: number): void {
    let entries: JArrayList<PieEntry> = new JArrayList<PieEntry>();

    // NOTE: The order of the entries when being added to the entries array determines their position around the center of
    // the chart.
    for (let i = 0; i < count; i++) {
      entries.add(new PieEntry(((Math.random() * range) + range / 5),
        this.parties[i % this.parties.length], undefined, undefined));
    }

    // Create a pie data set object and set the names of data item and data set.
    let dataSet: PieDataSet = new PieDataSet(entries, "Election Results");
    // Set whether to draw icons for the data items.
    dataSet.setDrawIcons(false);
    // Set the gap between data items.
    dataSet.setSliceSpace(1);
    // Set the offset of the icons.
    dataSet.setIconsOffset(new MPPointF(0, 40));
    // Set the offset of the selected data item.
    dataSet.setSelectionShift(5);
    // Set the text color of the data item.
    dataSet.setValueTextColor(Color.White);


    // add a lot of colors
    let colors: JArrayList<number> = new JArrayList();
    for (let index = 0; index < ColorTemplate.VORDIPLOM_COLORS.length; index++) {
      colors.add(ColorTemplate.VORDIPLOM_COLORS[index]);
    }

    for (let index = 0; index < ColorTemplate.JOYFUL_COLORS.length; index++) {
      colors.add(ColorTemplate.JOYFUL_COLORS[index]);
    }

    for (let index = 0; index < ColorTemplate.COLORFUL_COLORS.length; index++) {
      colors.add(ColorTemplate.COLORFUL_COLORS[index]);
    }
    for (let index = 0; index < ColorTemplate.LIBERTY_COLORS.length; index++) {
      colors.add(ColorTemplate.LIBERTY_COLORS[index]);
    }
    for (let index = 0; index < ColorTemplate.PASTEL_COLORS.length; index++) {
      colors.add(ColorTemplate.PASTEL_COLORS[index]);
    }
    colors.add(ColorTemplate.getHoloBlue());
    dataSet.setColorsByList(colors);
    // Generate the chart data.
    let data: PieData = new PieData(dataSet);
    // Bind the data to the chart configuration class.
    this.model.setData(data);
  }
  
```

2. Add data to the custom pie chart component.

```
// Set the configuration builder class for the component. If the chart needs to be displayed during page initialization, use aboutToAppear to build the chart data.
// If the chart is triggered by event,
// use dataSet.notifyDataSetChanged(); to update data,
// use this.model.notifyDataSetChanged(); to update axis data,
// and use this.model.invalidate(); to update drawing.
PieChart({ model: this.model })
  .width('100%')
  .height('70%')
```

### Bubble Chart

1. Initialize the bubble chart data.

```
   // Import the chart-related components and classes.
   import {
     JArrayList,  // Utility class of the data set.
     XAxis,  // The X-axis component.
     XAxisPosition,  // Enum class of the X-axis label position.
     YAxis,  // The Y-axis component.
     Description,  // The Description component.
     Legend,  // The Legend component.
     OnChartValueSelectedListener,  // Value selection listener.
     Highlight,  // Highlighted data.
     EntryOhos,  // Base class of the chart data structure.
     YAxisLabelPosition   // Enum class of the Y-axis label position.
     BubbleEntry,  // Data structure of the bubble chart.
     ChartPixelMap, // Chart pixel mapping class.
     IBubbleDataSet,  // API of the bubble dataset.
     BubbleDataSet,  // Dataset of the bubble chart.
     MPPointF,  // MPPonitF
     BubbleChart,  // The BubbleChart component.
     BubbleChartModel,  // Configuration builder class of the bubble chart.
     BubbleData // Bubble data.
   } from '@ohos/mpchart';
  
  // Construct a listener for value selection events.
  private valueSelectedListener: OnChartValueSelectedListener = {
    onValueSelected: (e: EntryOhos, h: Highlight) => {
    // ...todoSomething
    },
    onNothingSelected: () => {
    // ...todoSomething
    }
  }
  
  // Initialize the chart data.
  private model: BubbleChartModel | null = null;
    
  aboutToAppear() {
    // Initialize the configuration builder class of the chart. This step is mandatory.
    this.model = new BubbleChartModel();
    // Configure the chart style. The components can be configured in any sequence.

    // Set the Legend component.
    let l: Legend | null = this.model.getLegend();
    if (l) {
      l.setEnabled(true);
    }
    
    // Set the Description component.
    let description: Description | null = this.model.getDescription();
    if (description) {
      description.setEnabled(false);
    }

    // Set the maximum visible value to 60.
   this.model.setMaxVisibleValueCount(60);
   
   // Set whether to enable pinch-zooming. If the value is set to false, X- and Y-axis can be zoomed separately.
   this.model.setPinchZoom(false);
   
   // Obtain the X-axis object.
   let xAxis: XAxis | null = this.model.getXAxis();
   if (xAxis) {
     // Set the position of the X-axis.
     xAxis.setPosition(XAxisPosition.BOTTOM);
     
     // Set whether to draw gridlines on the X-axis.
     xAxis.setDrawGridLines(false);
   
     // Set the minimum interval of the X-axis coordinate.
     xAxis.setGranularity(1);
   
     // Set the number of labels on the X-axis.
     xAxis.setLabelCount(7);
   }
   
   // Obtain the left Y-axis object.
   let leftAxis: YAxis | null = this.model.getAxisLeft();
   if (leftAxis) {
     // Set the number of labels on the left Y-axis and set whether to use integer values.
     leftAxis.setLabelCount(8, false);
     
     // Set the position of the left Y-axis label.
     leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
     
     // Set the top space.
     leftAxis.setSpaceTop(15);
     
     // Set the minimum value of the left Y-axis.
     leftAxis.setAxisMinimum(0);
   }
   
   // Obtain the right Y-axis object.
   let rightAxis: YAxis | null = this.model.getAxisRight();
   if (rightAxis) {
     // Set the number of labels on the right Y-axis and set whether to use integer values.
     rightAxis.setLabelCount(8, false);
     
     // Set whether to draw gridlines on the right Y-axis.
     rightAxis.setDrawGridLines(false);
     
     // Set the top space.
     rightAxis.setSpaceTop(15);
     
     // Set the minimum value of the right Y-axis.
     rightAxis.setAxisMinimum(0);
   }
   
   // Initialize the start value.
   let start: number = 1;
   
   // Create a bubble dataset.
   let values: JArrayList<BubbleEntry> = new JArrayList<BubbleEntry>();
   for (let i = start; i < 20; i++) {
     // Generate a random value (between 0 and 40).
     let val = Number(Math.random() * 41);
   
     // Add bubble data based on a certain probability.
     if (Math.random() * 100 < 25) {
       values.add(new BubbleEntry(i, val));
     } else {
       values.add(new BubbleEntry(i, val));
     }
   }
   
   // Create a bubble dataset.
   let dataSet: BubbleDataSet = new BubbleDataSet(values, 'DataSet');
   dataSet.setHighLightColor(Color.Black);
   dataSet.setDrawIcons(false);
   
   // Create a bubble dataset list.
   let dataSetList: JArrayList<IBubbleDataSet> = new JArrayList<IBubbleDataSet>();
   dataSetList.add(dataSet);
   
   // Set the chart data.
   this.setData(5, 50);
   
   // Set the maximum visible range of the X-axis.
   this.model.setVisibleXRangeMaximum(7);

  }

   // Initialize the bubble data.
     /**
    * Set the data,
    * @param count number of data points,
    * @param range and range of data points for the bubble chart.
    */
   private setData(count: number, range: number): void {

    // Create three bubble datasets.
    let values1 = new JArrayList<BubbleEntry>();
    let values2 = new JArrayList<BubbleEntry>();
    let values3 = new JArrayList<BubbleEntry>();
    
    // Create an icon drawing object.
    let imgePaint: ChartPixelMap = new ChartPixelMap();

    // Set the width and height of the icon.
    imgePaint.setWidth(32);
    imgePaint.setHeight(32);

    // Generate a data point cyclically.
    for (let i = 0; i < count; i++) {
      // Add the bubble data points with icons to the dataset.
      values1.add(new BubbleEntry(i, Math.random() * range, Math.random() * range, imgePaint));
      values2.add(new BubbleEntry(i, Math.random() * range, Math.random() * range, imgePaint));
      
      // Add the bubble data point without an icon to the dataset.
      values3.add(new BubbleEntry(i, Math.random() * range, Math.random() * range));
    }

    // Create the bubble dataset 1.
    let set1: BubbleDataSet = new BubbleDataSet(values1, "DS 1");
    set1.setDrawIcons(false);
    set1.setColorByColor(0x88c12552);
    set1.setIconsOffset(new MPPointF(0, 0));
    set1.setDrawValues(this.isDrawValuesEnable);

    // Create the bubble dataset 2.
    let set2: BubbleDataSet = new BubbleDataSet(values2, "DS 2");
    set2.setDrawIcons(false);
    set2.setIconsOffset(new MPPointF(0, 0));
    set2.setColorByColor(0x88ff6600);
    set2.setDrawValues(this.isDrawValuesEnable);

    // Create the bubble dataset 3.
    let set3: BubbleDataSet = new BubbleDataSet(values3, "DS 3");
    set3.setDrawIcons(false);
    set3.setIconsOffset(new MPPointF(0, 0));
    set3.setColorByColor(0x88f5c700);
    set3.setDrawValues(this.isDrawValuesEnable);

    // Create a bubble dataset list.
    let dataSets = new JArrayList<IBubbleDataSet>();
    dataSets.add(set1);
    dataSets.add(set2);
    dataSets.add(set3);

    // Create a bubble data object.
    let dataResult: BubbleData = new BubbleData(dataSets);
    dataResult.setDrawValues(this.isDrawValuesEnable);
    dataResult.setValueTextSize(8);
    dataResult.setValueTextColor(Color.White);
    dataResult.setHighlightCircleWidth(1.5);
    dataResult.setHighlightEnabled(true);
    dataResult.notifyDataChanged();

    // Set the chart data if a chart model exists. 
    if (this.model) {
      this.model.setData(dataResult);
    }
   }  
  
```

2. Add data to the custom bubble chart component.

```
// Set the configuration builder class for the component. If the chart needs to be displayed during page initialization, use aboutToAppear to build the chart data.
// If the chart is triggered by event,
// use dataResult.notifyDataSetChanged(); to update data,
// use this.model.notifyDataSetChanged(); to update axis data,
// and use this.model.invalidate(); to update drawing.
BubbleChart({ model: this.model })
  .width('100%')
  .height('70%')
```

### Candle Chart

1. Initialize the candle data.

```
   // Import the chart-related components and classes.
   import {
     AxisDependency,  // The axis dependency class.
     CandleData, // Data packet of the candlestick chart.
     CandleDataSet, // Dataset of the candlestick chart.
     CandleEntry, // Data structure of the candlestick chart.
     CandleStickChart, // Candlestick chart class.
     CandleStickChartModel, // Configuration builder class of the candlestick chart.
     ChartPixelMap, // Chart pixel mapping class.
     Description, // The Description component.
     JArrayList, // Utility class of the dataset.
     Legend, // The Legend component.
     Style, // Chart style class.
     XAxis, // The X-axis component.
     XAxisPosition, // Enum class of the X-axis label position.
     YAxis, // The Y-axis component.
   } from '@ohos/mpchart';
    
  aboutToAppear() {
    // Initialize the chart data.
    // Initialize the configuration builder class of the chart. This step is mandatory.
    this.model = new CandleStickChartModel();
    
    // Configure the chart style. The components can be configured in any sequence.
    // Set the Description component.
    let description: Description | null = this.model.getDescription();
    if (description) {
      description.setEnabled(false);
    }

    // Set the Legend component.
    let l: Legend | null = this.model.getLegend();
    if (l) {
      l.setEnabled(true);
    }
    
      this.model.setMaxVisibleValueCount(60); // Set the maximum visible value in the chart.
      this.model.setPinchZoom(false); // Set whether to enable pinch zooming.
      this.model.setDrawGridBackground(true);  // Set whether to draw grid background.
      this.model.setGridBackgroundColor(Color.White);  // Set the grid background color.
      this.setData (40, 100); // Set the chart data.
      
      let xAxis: XAxis | null = this.model.getXAxis();
      if (xAxis) {
        xAxis.setPosition(XAxisPosition.BOTTOM); // Set the position of X-axis.
        xAxis.setDrawGridLines(true);  // Set whether to draw gridlines on the X-axis.
      }
      
      let leftAxis: YAxis | null = this.model.getAxisLeft();
      if (leftAxis) {
        leftAxis.setLabelCount(7, false);  // Set the number of labels on the left Y-axis. The value can be a non-integer.
        leftAxis.setDrawGridLines(true);  // Set whether to draw gridlines on the left Y-axis.
        leftAxis.setDrawAxisLine(true);  // Set whether to draw the left Y-axis.
      }
      
      let rightAxis: YAxis | null = this.model.getAxisRight();
      if (rightAxis) {
        rightAxis.setLabelCount(7, false); // Set the number of labels on the right Y-axis. The value can be a non-integer.
        rightAxis.setDrawGridLines(true);  // Set whether to draw gridlines on the right Y-axis.
        rightAxis.setDrawAxisLine(true);  // Set whether to draw the right Y-axis.
      }
      
      let legend: Legend | null = this.model.getLegend();
      if (legend) {
        legend.setEnabled(true);  // Set whether to enable the legend.
      }
  }

   // Initialize the candle data.
    private setData(count: number, range: number):void{

    let values: JArrayList<CandleEntry> = new JArrayList<CandleEntry>();

    for (let i = 0; i < count; i++) {
      let val: number = (Math.random() * 40) + (range + 3);

      let high: number = Number(Math.random() * 9) + 8.0;
      let low: number = Number(Math.random() * 9) + 8.0;

      let open: number = Number(Math.random() * 6) + 1.0;
      let close: number = Number(Math.random() * 6) + 1.0;

      let even: boolean = i % 2 == 0;

      values.add(new CandleEntry(
        i,
        val + high,
        val - low,
        even ? val + open : val - open,
        even ? val - close : val + close,
        new ChartPixelMap()));
    }

      // Create a candle dataset and set the dataset name to Data Set.
      let dataSet: CandleDataSet = new CandleDataSet(values, "Data Set");
      // Set whether to draw icons.
      dataSet.setDrawIcons(false);
      // Set the dependent axis of the dataset.
      dataSet.setAxisDependency(AxisDependency.LEFT);
      // Set the shadow color of the candle chart.
      dataSet.setShadowColor(Color.Gray);
      // Set the shadow width of the candle chart.
      dataSet.setShadowWidth(0.7);
      // Set the paint style of the decreasing candle.
      dataSet.setDecreasingPaintStyle(Style.FILL);
      // Set the color of the decreasing candle.
      dataSet.setDecreasingColor(Color.Red);
      // Set the color of the increasing candle.
      dataSet.setIncreasingColor(Color.Green);
      // Set the paint style of the increasing candle.
      dataSet.setIncreasingPaintStyle(Style.STROKE);
      // Set the color of the neutral candle.
      dataSet.setNeutralColor(Color.Blue);

    let data: CandleData = new CandleData([dataSet]);
    data.setValueTextSize(7);
    if (this.model) {
      this.model.resetTracking();
      this.model.setData(data);
    }
  }
  
```

### Combined Chart

1. Initialize the data.

```
   // Import the chart-related components and classes.
   import {
     ColorTemplate,  // Color template.
     CombinedChart,  // The combined chart class.
     CombinedChartModel,  // The configuration builder class of the combined chart.
     CombinedData,  // Data packet of the combined chart.
     EntryOhos,  // Base class of the chart data structure.
     JArrayList,  // Utility class of the dataset.
     LineData,  // Data packet of the line chart.
     LineDataSet,  // Dataset of the line chart.
     Color,  // The color class.
     Mode,  // Enum class of the chart mode.
     AxisDependency,  // The axis dependency class.
     BarEntry,  // Data structure of the bar chart.
     BarData,  // Data packet of the bar chart.
     BarDataSet,  // Dataset of the bar chart.
     IBarDataSet,  // Operation class of the bar chart dataset.
     ScatterData,  // Data packet of the scatter chart.
     ScatterDataSet,  // Dataset of the scatter chart.
     CandleData,  // Data packet of the candle chart.
     CandleEntry,  // Data structure of the candle chart.
     CandleDataSet,  // Dataset of the candle chart.
     BubbleData,  // Data packet of the bubble chart.
     BubbleEntry,  // Data structure of the bubble chart.
     BubbleDataSet,  // Dataset of the bubble chart.
     YAxisLabelPosition   // Enum class of the Y-axis label position.
     XAxisPosition,  // Enum class of the X-axis label position.
     XAxis,  // The X-axis component.
     YAxis,  // The Y-axis component.
     LegendHorizontalAlignment,  // Enum class of the legend horizontal alignment mode.
     LegendVerticalAlignment  // Enum class of the legend vertical alignment mode.
   } from '@ohos/mpchart';

   // Initialize the configuration builder class of the chart. This step is mandatory.
   private model: CombinedChartModel = new CombinedChartModel();
    
   aboutToAppear() {
    // Configure the chart style. The components can be configured in any sequence.
    
    // Set whether to enable the chart description.
    this.model.getDescription()?.setEnabled(false);

    // Obtain the legend object.
    let l = this.model.getLegend();
    if (l) {
      // Set whether to enable the legend.
      l.setEnabled(true);
      
      // Set whether to enable the text wrap function of the legend.
      l.setWordWrapEnabled(true);
      
      // Set the horizontal alignment mode of the legend.
      l.setHorizontalAlignment(LegendHorizontalAlignment.LEFT);
      
      // Set the vertical alignment mode of the legend.
      l.setVerticalAlignment(LegendVerticalAlignment.BOTTOM);
      
      // Set whether to draw the legend in the chart.
      l.setDrawInside(false);
    }

    // Set the maximum visible value in the chart.
    this.model.setMaxVisibleValueCount(60);

    // Set whether to enable the pinch zoom function of the chart.
    this.model.setPinchZoom(false);

    // Set whether to draw the grid background of the chart.
    this.model.setDrawGridBackground(false);

    // Obtain the X-axis object.
    let xAxis: XAxis | null = this.model.getXAxis();
    if (xAxis) {
      // Set the position of the X-axis.
      xAxis.setPosition(XAxisPosition.BOTTOM);
      
      // Set whether to draw gridlines on the X-axis.
      xAxis.setDrawGridLines(false);
      
      // Set the minimum interval of the X-axis coordinate.
      xAxis.setGranularity(1);
      
      // Set the number of labels on the X-axis.
      xAxis.setLabelCount(7);
    }

    // Obtain the left Y-axis object.
    let leftAxis: YAxis | null = this.model.getAxisLeft();
    if (leftAxis) {
      // Set the number of labels on the left Y-axis and set whether to use integer values.
      leftAxis.setLabelCount(8, false);
      
      // Set the position of the left Y-axis label.
      leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
      
      // Set the top space.
      leftAxis.setSpaceTop(15);
      
      // Set the minimum value of the left Y-axis.
      leftAxis.setAxisMinimum(0);
    }

    // Obtain the right Y-axis object.
    let rightAxis: YAxis | null = this.model.getAxisRight();
    if (rightAxis) {
      // Set the number of labels on the right Y-axis and set whether to use integer values.
      rightAxis.setLabelCount(8, false);
      
      // Set whether to draw gridlines on the right Y-axis.
      rightAxis.setDrawGridLines(false);
      
      // Set the top space.
      rightAxis.setSpaceTop(15);
      
      // Set the minimum value of the right Y-axis.
      rightAxis.setAxisMinimum(0);
    }

    // Create a combined chart data object.
    let data: CombinedData = new CombinedData();
    
    // Generate line data.
    this.generateLineData(data);
    
    // Generate bar data.
    this.generateBarData(data);
    
    // Generate bubble data.
    this.generateBubbleData(data);
    
    // Generate scatter data.
    this.generateScatterData(data);
    
    // Generate candle data.
    this.generateCandleData(data);
    
    // Set the chart data.
    this.model.setData(data);
  }

   // Generate line data.
    private generateLineData(data: CombinedData): void {

    let d: LineData = new LineData();

    let entries: JArrayList<EntryOhos> = new JArrayList();

    for (let index = 0; index < this.count; index++) {

      entries.add(new EntryOhos(index + 0.5, this.getRandom(15, 5)));
    }

    let set: LineDataSet = new LineDataSet(entries, "Line DataSet");
    set.setColorByColor(Color.rgb(240, 238, 70));
    set.setLineWidth(2.5);
    set.setCircleColor(Color.rgb(240, 238, 70));
    set.setCircleRadius(5);
    set.setFillColor(Color.rgb(240, 238, 70));
    set.setMode(Mode.CUBIC_BEZIER);
    set.setDrawValues(true);
    set.setValueTextSize(10);
    set.setValueTextColor(Color.rgb(240, 238, 70));

    set.setAxisDependency(AxisDependency.LEFT);
    d.addDataSet(set);
    data.setLineData(d);

  }
  
  // Generate bar data.
  private generateBarData(data: CombinedData): void {

    let entries1: JArrayList<BarEntry> = new JArrayList();
    let entries2: JArrayList<BarEntry> = new JArrayList();

    for (let index = 0; index < this.count; index++) {
      entries1.add(new BarEntry(0, this.getRandom(25, 25)));

      // stacked
      entries2.add(new BarEntry(0, [this.getRandom(13, 12), this.getRandom(13, 12)]));
    }

    let set1: BarDataSet = new BarDataSet(entries1, "Bar 1");
    set1.setColorByColor(Color.rgb(60, 220, 78));
    set1.setValueTextColor(Color.rgb(60, 220, 78));
    set1.setValueTextSize(10);
    set1.setAxisDependency(AxisDependency.LEFT);

    let set2: BarDataSet = new BarDataSet(entries2, "");
    set2.setStackLabels(["Stack 1", "Stack 2"]);
    set2.setColorsByArr([Color.rgb(61, 165, 255), Color.rgb(23, 197, 255)]);
    set2.setValueTextColor(Color.rgb(61, 165, 255));
    set2.setValueTextSize(10);
    set2.setAxisDependency(AxisDependency.LEFT);

    let groupSpace = 0.06;
    let barSpace = 0.02; // x2 dataset
    let barWidth = 0.45; // x2 dataset
    // (0.45 + 0.02) * 2 + 0.06 = 1.00 -> interval per "group"

    let set: JArrayList<IBarDataSet> = new JArrayList();
    set.add(set1);
    set.add(set2);
    let d: BarData = new BarData(set);
    d.setBarWidth(barWidth);

    // make this BarData object grouped
    d.groupBars(0, groupSpace, barSpace); // start at x = 0
    data.setBarData(d);

  }

  // Generate scatter data.
  private generateScatterData(data: CombinedData): void {

    let d: ScatterData = new ScatterData();

    let entries: JArrayList<EntryOhos> = new JArrayList();

    for (let index = 0; index < this.count; index += 0.5)
      entries.add(new EntryOhos(index + 0.25, this.getRandom(10, 55)));

    let set: ScatterDataSet = new ScatterDataSet(entries, "Scatter DataSet");
    set.setColorsByArr(ColorTemplate.MATERIAL_COLORS);
    set.setScatterShapeSize(7.5);
    set.setDrawValues(false);
    set.setValueTextSize(10);
    d.addDataSet(set);
    data.setScatterData(d);
  }

   // Generate candle data.
  private generateCandleData(data: CombinedData): void {

    let d: CandleData = new CandleData();

    let entries: JArrayList<CandleEntry> = new JArrayList();

    for (let index = 0; index < this.count; index += 2) {

      entries.add(new CandleEntry(index + 1, 90, 70, 85, 75));
    }

    let set: CandleDataSet = new CandleDataSet(entries, "Candle DataSet");
    set.setDecreasingColor(Color.rgb(142, 150, 175));
    set.setShadowColor(ColorTemplate.DKGRAY);
    set.setBarSpace(0.3);
    set.setValueTextSize(10);
    set.setDrawValues(false);
    d.addDataSet(set);
    data.setCandleData(d);
  }

  // Generate bubble data.
  private generateBubbleData(data: CombinedData): void {

    let bd: BubbleData = new BubbleData();

    let entries: JArrayList<BubbleEntry> = new JArrayList();

    for (let index = 0; index < this.count; index++) {
      let y = this.getRandom(10, 105);
      let size = this.getRandom(20, 30);
      entries.add(new BubbleEntry(index + 0.5, y, size));
    }

    let set: BubbleDataSet = new BubbleDataSet(entries, "Bubble DataSet");
    set.setColorsByArr(ColorTemplate.VORDIPLOM_COLORS);
    set.setValueTextSize(10);
    set.setValueTextColor(Color.rgb(255, 255, 255));
    set.setHighlightCircleWidth(1.5);
    set.setDrawValues(true);
    bd.addDataSet(set);
    data.setBubbleData(bd);
  }
  // Generate random data.
  getRandom(range: number, start: number): number {
    return (Math.random() * range) + start;
  }

```

2. Add data to the custom combined chart component.

```
// Set the configuration builder class for the component. If the chart needs to be displayed during page initialization, use aboutToAppear to build the chart data.
// If the chart is triggered by event,
// use data.notifyDataSetChanged(); to update data,
// use this.model.notifyDataSetChanged(); to update axis data,
// and use this.model.invalidate(); to update drawing.
CombinedChart({ model: this.model })
  .width('100%')
  .height('70%')
```

### Radar Chart

1. Initialize the radar data.

```
   // Import the chart-related components and classes.
   import {
     ChartColor,  // The color class.
     JArrayList,  // Utility class of the dataset.
     XAxis,  // The X-axis component.
     XAxisPosition,  // Enum class of the X-axis label position.
     Description,  // The Description component.
     Legend,  // The Legend component.
     OnChartValueSelectedListener,  // Value selection listener.
     Highlight,  // Highlighted data in the chart.
     EntryOhos,  // Base class of the chart data structure.
     RadarEntry,  // Data structure of the radar chart.
     RadarChart,  // The radar chart class.
     RadarDataSet,  // Dataset of the radar chart.
     RadarChartModel,  // Configuration builder class of the radar chart.
     IRadarDataSet,  // Operation class of the radar dataset.
     RadarData,  // Data packet of the radar chart.
     YAxis,  // The Y-axis component.
     IAxisValueFormatter,  // Axis value formatting API.
     AxisBase,  // Axis base class.
     LegendVerticalAlignment,  // Enum class of the legend vertical alignment mode.
     LegendHorizontalAlignment,  // Enum class of the legend horizontal alignment mode.
     LegendOrientation,  // Enum class of the legend orientation.
   } from '@ohos/mpchart';
    
    // Initialize the configuration builder class of the chart. This step is mandatory.
    private model: RadarChartModel = new RadarChartModel();
    
   aboutToAppear() {
    // Configure the chart style. The components can be configured in any sequence.
    // Set a value selection listener for the radar chart.
    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);

    // Obtain the chart description object.
    let description: Description | null = this.model.getDescription();
    if (description) {
      // Set whether to enable the chart description.
      description.setEnabled(false);
    }

    // Obtain the legend object.
    let l: Legend | null = this.model.getLegend();
    if (l) {
      // Set whether to enable the legend.
      l.setEnabled(true);
      
      // Set the vertical alignment mode of the legend.
      l.setVerticalAlignment(LegendVerticalAlignment.TOP);
      
      // Set the horizontal alignment mode of the legend.
      l.setHorizontalAlignment(LegendHorizontalAlignment.CENTER);
      
      // Set the legend orientation.
      l.setOrientation(LegendOrientation.HORIZONTAL);
      
      // Set whether to draw the legend in the chart.
      l.setDrawInside(false);
      
      // Set the interval of the legend on the X-axis.
      l.setXEntrySpace(7);
      
      // Set the interval of the legend on the Y-axis.
      l.setYEntrySpace(5);
      
      // Set the text color of the legend.
      l.setTextColor(Color.White);
    }

    // Set the gridline width, color, and alpha of the radar chart.
    this.model.setWebLineWidth(0.3);
    this.model.setWebColor(0xFFCCCCCC);
    this.model.setWebLineWidthInner(0.3);
    this.model.setWebColorInner(0xFFCCCCCC);
    this.model.setWebAlpha(100);

    // Set the extra offset at the top of the radar chart.
    this.model.setExtraTopOffset(-100);

    // Create a radar marker view object.
    this.normalMarker = new RadarMarkerView();
    
    // Set the radar mark.
    this.model.setMarker(this.normalMarker);

    // Obtain the X-axis object.
    let xAxis: XAxis | null = this.model.getXAxis();
    if (xAxis) {
      // Set the position of the X-axis.
      xAxis.setPosition(XAxisPosition.BOTTOM);
      
      // Set whether to draw gridlines on the X-axis.
      xAxis.setDrawGridLines(false);
      
      // Set the text size of the X-axis label.
      xAxis.setTextSize(20);
      
      // Set the offset of the X-axis and Y-axis.
      xAxis.setYOffset(0);
      xAxis.setXOffset(0);
      
      // Set the minimum interval of the X-axis coordinate.
      xAxis.setGranularity(1);
      
      // Set the number of labels on the X-axis.
      xAxis.setLabelCount(7);
      
      / /Set the X-axis value formatter.
      xAxis.setValueFormatter(new valueFormatter());
      
      // Set the text color of the X-axis label.
      xAxis.setTextColor(Color.White);
    }

    // Obtain the Y-axis object.
    let yAxis: YAxis | null = this.model.getYAxis();
    if (yAxis) {
      // Set the number of labels on the Y-axis and set whether to use integer values.
      yAxis.setLabelCount(5, false);
      
      // Set the text size of the X-axis label.
      yAxis.setTextSize(20);
      
      // Set the minimum value of the Y-axis.
      yAxis.setAxisMinimum(0);
      
      // Set the maximum value of the Y-axis.
      yAxis.setAxisMaximum(80);
      
      // Set whether to draw the Y-axis labels.
      yAxis.setDrawLabels(false);
    }

    // Set the radar data.
    this.setData();
  }

   /**
   * Set the radar data.
   */
   private setData(): void {
    let mul = 80;  // Magnification of the random value.
    let min = 20;  // Minimum value of the random value
    let cnt = 5;   // Number of data points.

    // Create array lists for radar data.
    let entries1: JArrayList<RadarEntry> = new JArrayList<RadarEntry>();
    let entries2: JArrayList<RadarEntry> = new JArrayList<RadarEntry>();

    // Note: The order in which data points are added to the array determines their positions around the center of the chart.
    for (let i = 0; i < cnt; i++) {
      let val1 = (Math.random() * mul) + min;
      entries1.add(new RadarEntry(val1));

      let val2 = (Math.random() * mul) + min;
      entries2.add(new RadarEntry(val2));
    }

    // Create a radar dataset.
    let set1: RadarDataSet = new RadarDataSet(entries1, "Last Week");
    set1.setColorByColor(ChartColor.rgb(103, 110, 129));
    set1.setFillColor(ChartColor.rgb(103, 110, 129));
    set1.setDrawFilled(true);
    set1.setFillAlpha(180);
    set1.setLineWidth(2);
    set1.setDrawHighlightCircleEnabled(true);
    set1.setDrawHighlightIndicators(false);

    // Create another radar dataset.
    let set2: RadarDataSet = new RadarDataSet(entries2, "This Week");
    set2.setColorByColor(ChartColor.rgb(121, 162, 175));
    set2.setFillColor(ChartColor.rgb(121, 162, 175));
    set2.setDrawFilled(true);
    set2.setFillAlpha(180);
    set2.setLineWidth(2);
    set2.setDrawHighlightCircleEnabled(true);
    set2.setDrawHighlightIndicators(false);

    // Create array lists for radar datasets.
    let sets: JArrayList<IRadarDataSet> = new JArrayList<IRadarDataSet>();
    sets.add(set1);
    sets.add(set2);

    // Create a radar data object.
    let data: RadarData = new RadarData(sets);
    data.setValueTextSize(20);
    
    // Set whether to draw data values.
    data.setDrawValues(false);
    
    // Set the text color of the data value.
    data.setValueTextColor(Color.White);
    
    // Set the chart data.
    this.model.setData(data);
   }


```

2. Add data to the radar chart component.

```
// Set the configuration builder class for the component. If the chart needs to be displayed during page initialization, use aboutToAppear to build the chart data.
// If the chart is triggered by event,
// use data.notifyDataSetChanged(); to update data,
// use this.model.notifyDataSetChanged(); to update axis data,
// and use this.model.invalidate(); to update drawing.
RadarChart({ model: this.model })
  .width('100%')
  .height('70%')
```

### Scatter Chart

1. Initialize the scatter data.

```
   // Import the chart-related components and classes.
   import {
     JArrayList,  // Utility class of the dataset.
     XAxis,  // The X-axis component.
     XAxisPosition,  // Enum class of the X-axis label position.
     YAxis,  // The Y-axis component.
     Description,  // The Description component.
     Legend,  // The Legend component.
     OnChartValueSelectedListener,  // Value selection listener.
     Highlight,  // Highlighted data in the chart.
     EntryOhos,  // Base class of the chart data structure.
     YAxisLabelPosition   // Enum class of the Y-axis label position.
     ScatterChart, // The scatter chart class.
     ScatterChartModel, // Configuration builder class of the scatter chart.
     ScatterData,  // Data packet of the scatter chart.
     ScatterDataSet,  // Dataset of the scatter chart.
     IScatterDataSet,  // Operation class of the scatter dataset.
     ColorTemplate,  // Color template.
     ChartShape,  // Enum class of the chart shape.
   } from '@ohos/mpchart';

    
   aboutToAppear() {
    // Initialize the configuration builder class of the chart. This step is mandatory.
    this.model = new ScatterChartModel();

    // Configure the chart style. The components can be configured in any sequence.
    // Obtain the legend object.
    let l: Legend | null = this.model.getLegend();
    if (l) {
      // Set whether to enable the legend.
      l.setEnabled(true);
    }
    
    // Set a value selection listener for the scatter chart.
    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);

    // Obtain the description object.
    let description: Description | null = this.model.getDescription();
    if (description) {
      // Set whether to enable the chart description.
      description.setEnabled(false);
    }

    // Set the maximum visible value in a chart, whether to enable pinch zooming, and whether to draw the grid background.
    this.model.setMaxVisibleValueCount(160);
    this.model.setPinchZoom(false);
    this.model.setDrawGridBackground(false);

    // Obtain the X-axis object.
    let xAxis: XAxis | null = this.model.getXAxis();
    if (xAxis) {
      // Set the position of the X-axis.
      xAxis.setPosition(XAxisPosition.BOTTOM);
      
      // Set whether to draw gridlines on the X-axis.
      xAxis.setDrawGridLines(false);
      
      // Set the minimum interval of the X-axis coordinate.
      xAxis.setGranularity(1);
      
      // Set the number of labels on the X-axis.
      xAxis.setLabelCount(7);
    }

    // Obtain the left Y-axis object.
    let leftAxis: YAxis | null = this.model.getAxisLeft();
    if (leftAxis) {
      // Set the number of labels on the left Y-axis and set whether to use integer values.
      leftAxis.setLabelCount(8, false);
      
      // Set the position of the left Y-axis label.
      leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
      
      // Set the top space of the left Y-axis.
      leftAxis.setSpaceTop(15);
      
      // Set the minimum value of the left Y-axis.
      leftAxis.setAxisMinimum(0);
    }

    // Obtain the right Y-axis object.
    let rightAxis: YAxis | null = this.model.getAxisRight();
    if (rightAxis) {
      // Set the number of labels on the right Y-axis and whether to draw gridlines on the right Y-axis.
      rightAxis.setLabelCount(8, false);
      rightAxis.setDrawGridLines(false);
      
      // Set the top space of the right Y-axis.
      rightAxis.setSpaceTop(15);
      
      // Set the minimum value of the right Y-axis.
      rightAxis.setAxisMinimum(0);
    }

    // Generate random data.
    let start: number = 1;
    let values: JArrayList<EntryOhos> = new JArrayList<EntryOhos>();
    for (let i = start; i < 20; i++) {
      let val = Number(Math.random() * 41);

      if (Math.random() * 100 < 25) {
        values.add(new EntryOhos(i, val));
      } else {
        values.add(new EntryOhos(i, val));
      }
    }

    // Create a scatter dataset.
    let dataSet: ScatterDataSet = new ScatterDataSet(values, 'DataSet');
    dataSet.setHighLightColor(Color.Black);
    dataSet.setDrawIcons(false);

    // Create a scatter dataset list.
    let dataSetList: JArrayList<IScatterDataSet> = new JArrayList<IScatterDataSet>();
    dataSetList.add(dataSet);

    // Set the chart data.
    this.setData(20, 100);
  }
   
   /**
    * Set the scatter data.
    * @param xRange - Data range of the X-axis.
    * @param yRange - Data range of the Y-axis.
    */
   private setData(xRange: number, yRange: number): void {
   
       // Generate a random dataset.
       let values1 = this.generateRandomData(xRange, yRange);
       let values2 = this.generateRandomData(xRange, yRange);
       let values3 = this.generateRandomData(xRange, yRange);
   
       // Create scatter dataset 1.
       let set1 = new ScatterDataSet(values1, "DS 1");
       set1.setScatterShape(ChartShape.SQUARE);
       set1.setColorByColor(ColorTemplate.COLORFUL_COLORS[0]);
   
       // Create scatter dataset 2.
       let set2 = new ScatterDataSet(values2, "DS 2");
       set2.setScatterShape(ChartShape.CIRCLE);
       set2.setScatterShapeHoleColor(ColorTemplate.COLORFUL_COLORS[3]);
       set2.setScatterShapeHoleRadius(3);
       set2.setColorByColor(ColorTemplate.COLORFUL_COLORS[1]);
   
       // Create scatter dataset 3.
       let set3 = new ScatterDataSet(values3, "DS 3");
       set3.setShapeRenderer(new CustomScatterShapeRenderer());
       set3.setColorByColor(ColorTemplate.COLORFUL_COLORS[2]);
   
       // Set the shape size of the scatter datasets.
       set1.setScatterShapeSize(8);
       set2.setScatterShapeSize(8);
       set3.setScatterShapeSize(8);
   
       // Create a scatter dataset list.
       let dataSets: JArrayList<IScatterDataSet> = new JArrayList<IScatterDataSet>();
       dataSets.add(set1); // Add a dataset.
       dataSets.add(set2);
       dataSets.add(set3);
   
       // Create a scatter data.
       let dataResult: ScatterData = new ScatterData(dataSets);
       dataResult.setDrawValues(this.isDrawValuesEnable);
       dataResult.setValueTextSize(8);
       dataResult.setHighlightEnabled(true);
   
       // Set the text size of the data.
       dataResult.setValueTextSize(10);
   
       // Set the chart data.
       if (this.model) {
         this.model.setData(dataResult);
       }
   }

   /**
    * Generate random scatter data.
    * @param xRange - Data range of the X-axis.
    * @param yRange - Data range of the Y-axis.
    * @returns Random scatter dataset.
    */
   private generateRandomData(xRange: number, yRange: number): JArrayList<EntryOhos> {
       let values = new JArrayList<EntryOhos>();
   
       // Generate random data cyclically.
       for (let i = 0; i < xRange; i++) {
         let x = i; // Generate a random X value within the specified range.
         let y = Math.random() * yRange; // Generate a random Y value within the specified range.
         values.add(new EntryOhos(x, y));
       }
       return values;
   }

```

2. Add data to the scatter chart component.

```
// Set the configuration builder class for the component. If the chart needs to be displayed during page initialization, use aboutToAppear to build the chart data.
// If the chart is triggered by event,
// use dataResult.notifyDataSetChanged(); to update data,
// use this.model.notifyDataSetChanged(); to update axis data,
// and use this.model.invalidate(); to update drawing.
ScatterChart({ model: this.model })
  .width('100%')
  .height('70%')
```
### Waterfall Chart

1. Initialize the waterfall data.

```
 import {
  WaterfallChart, // The waterfall chart class.
  WaterfallChartModel, // Configuration builder class of the waterfall chart.
  WaterfallData, // Data packet of the waterfall chart.
  WaterfallDataSet, // Dataset of the waterfall chart.
  WaterfallEntry, // Data structure of the waterfall chart.
  ChartGesture, // Gesture event mode.
  Description, // The Description component.
  EntryOhos, // Base class of the chart data structure.
  Fill, // Builder class of the chart filling type.
  Highlight, // Highlighted data in the chart.
  IBarDataSet, // Operation class of the waterfall dataset.
  JArrayList, // Utility class of the dataset.
  Legend, // The Legend component.
  LimitLabelPosition, // Enum class of the LimitLine label position.
  LimitLine, // The LimitLine component.
  MarkerView, // The Markerview component.
  OnChartGestureListener, // Gesture event listener.
  OnChartValueSelectedListener, // Value selection listener.
  XAxis, // The X-axis component.
  XAxisPosition, // Enum class of the X-axis label position.
  YAxis, // The Y-axis component.
  YAxisLabelPosition // Enum class of the Y-axis label position.
} from '@ohos/mpchart';

   // Construct a listener for value selection events.
   private valueSelectedListener: OnChartValueSelectedListener = {
    onValueSelected: (e: EntryOhos, h: Highlight) => {
      LogUtil.log("SimpleWaterfallChartPage onValueSelected: " + e.getX());
    },
    onNothingSelected: () => {
      LogUtil.log("SimpleWaterfallChartPage onNothingSelected");
    }
  }
  
  // Construct a listener for gesture events.
  private chartGestureListener: OnChartGestureListener = {
    onChartGestureStart: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, lastPerformedGestureMode: ChartGesture) => {
    },
    onChartGestureEnd: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, lastPerformedGestureMode: ChartGesture) => {
    // ...todoSomething
    },
    onChartLongPressed: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
    // ...todoSomething
    },
    onChartDoubleTapped: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
    // ...todoSomething
    },
    onChartSingleTapped: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
    // ...todoSomething
    },
    onChartFling: (isTouchEvent: boolean, me1: TouchEvent | GestureEvent, me2: TouchEvent, velocityX: number, velocityY: number) => {
    // ...todoSomething
    },
    onChartScale: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, scaleX: number, scaleY: number) => {
    // ...todoSomething
    },
    onChartTranslate: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, dX: number, dY: number) => {
    // ...todoSomething
    }
  }
  
  // Initialize the chart data.
  aboutToAppear() {
  
  	// Initialize the configuration builder class of the chart. This step is mandatory.
    this.model = new WaterfallChartModel();
    
    // Configure the chart style. The components can be configured in any sequence.
    
    // Add a value selection listener for the chart.
    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);
    // Add a gesture listener for the chart.
    this.model.setOnChartGestureListener(this.chartGestureListener);
	// Obtain the Description component and disable it. That is, the Description component is not drawn in the chart.
    let description: Description | null = this.model.getDescription()
    if (description) {
      description.setEnabled(false);
    }
	// Obtain the Legend component and disable it.
    let l: Legend | null = this.model.getLegend();
    if (l) {
      l.setEnabled(false);
    }
    // Set the maximum visible number of value labels drawn on the chart. If the data number exceeds this value, the value label of the chart is not drawn.
    this.model.setMaxVisibleValueCount(40);
	// Set whether to draw the background color of the chart. The background color is drawn in the waterfall chart, excluding the part outside the axis.
    this.model.setDrawGridBackground(false);
    // Set the background color of the chart. The color must meet the CanvasRenderingContext2D.fillstyle/strokestyle specifications.
    this.model.setGridBackgroundColor('#500000ff')
    // Set whether to draw the bar shadow.
    this.model.setDrawBarShadow(false);
	// Set whether to draw the value above the bar.
    this.model.setDrawValueAboveBar(false);
	// Set the LimitLine component of the left Y-axis, which includes the line width, line segment style, label position, and label font size.
    this.limitLine1 = new LimitLine(120, 'Upper Limit');
    this.limitLine1.setLineWidth(4);
    this.limitLine1.enableDashedLine(10, 10, 0);
    this.limitLine1.setLabelPosition(LimitLabelPosition.RIGHT_TOP);
    this.limitLine1.setTextSize(10);

    this.limitLine2 = new LimitLine(50, 'Lower Limit');
    this.limitLine2.setLineWidth(4);
    this.limitLine2.enableDashedLine(10, 10, 0);
    this.limitLine2.setLineColor(Color.Yellow);
    this.limitLine2.setLabelPosition(LimitLabelPosition.RIGHT_BOTTOM);
    this.limitLine2.setTextSize(10);

    // Set the information of the left Y-axis.
    this.leftAxis = this.model.getAxisLeft();
    if (this.leftAxis) {
      this.leftAxis.setAxisMinimum(0); // this replaces setStartAtZero(true)
      this.leftAxis.setDrawLimitLinesBehindData(false);

      // Add LimitLines to the left Y-axis.
      this.leftAxis.addLimitLine(this.limitLine1);
      this.leftAxis.addLimitLine(this.limitLine2);
    }
	// Set the information of the right Y-axis.
    this.rightAxis = this.model.getAxisRight();
    if (this.rightAxis) {
      this.rightAxis.setEnabled(false);
      this.rightAxis.setAxisMinimum(0);
    }
	// Set the information of the X-axis.
    this.xAxis = this.model.getXAxis();
    if (this.xAxis) {
      this.xAxis.setPosition(XAxisPosition.BOTTOM);
    }
	// Set the MarkerView component.
    this.normalMarker = new MarkerView();
    this.model.setMarker(this.normalMarker);
    // Set a custom MarkerView. This step is optional.
    this.stackMarker = new CustomMarkerView();
	// Generate normal data.
    this.data = this.getNormalData();
    
    // Draw data of multiple colors based on the scale range of the Y-axis.
    this.data = this.getSegmentationData();
    
    // Highlight the top and bottom data.
    this.data = this.getTopAndBottomHighlightData();
    
    // Bind the data to the chart configuration class.
    this.model.setData(this.data);
    // Set the maximum visible range of the X-axis. By default, all the data in X-axis is visible.
    this.model.setVisibleXRangeMaximum(20);
  }
  
   private getNormalData(): BarData {
    let values: JArrayList<WaterfallEntry> = new JArrayList<WaterfallEntry>();
    // Set the position and color of the mark point.
    let h1 = new WaterfallHighlight(20, 30, Color.Gray);
    let h2 = new WaterfallHighlight(30, 40, Color.Gray);
    let h3 = new WaterfallHighlight(40, 60, Color.Green);
    let h4 = new WaterfallHighlight(60, 70, Color.Red);

    values.add(new WaterfallEntry(1, 10, 30, undefined, undefined, h1));
    values.add(new WaterfallEntry(2, 15, 50));
    values.add(new WaterfallEntry(4, 5, 95, undefined, undefined, h2, h3, h4));
    values.add(new WaterfallEntry(6, 45, 80, undefined, undefined, h2, h3));

    this.dataSet = new WaterfallDataSet(values, 'DataSet');
    this.dataSet.setHighLightColor(Color.Gray);
    this.dataSet.setDrawIcons(false);
    // Set the color of the bar.
    this.dataSet.setColorByColor(Color.Pink);
    this.dataSet.setValueTextSize(10)

    let dataSetList: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
    dataSetList.add(this.dataSet);

    let barData: BarData = new BarData(dataSetList);
    // Set the width of the waterfall chart.
    barData.setBarWidth(0.3);
     // Set the top radius of the bar.
    barData.setTopRadius(1);
    return barData;
  }

  private getSegmentationData(): BarData {
     let values: JArrayList<WaterfallEntry> = new JArrayList<WaterfallEntry>();

    values.add(new WaterfallEntry(1, 10, 70));
    values.add(new WaterfallEntry(2, 15, 80));

    this.dataSet = new WaterfallDataSet(values, 'DataSet');
    this.dataSet.setHighLightColor(Color.Gray);
    this.dataSet.setDrawIcons(false);
    this.dataSet.setColorByColor(Color.Pink);
    this.dataSet.setValueTextSize(10)

    // Set the color based on the Y-axis scale range.
    let highlightList: WaterfallHighlight[] = [];
    highlightList.push(new WaterfallHighlight(0, 40, Color.Green));
    highlightList.push(new WaterfallHighlight(40, 60, Color.Orange));
    highlightList.push(new WaterfallHighlight(60, 100, Color.Pink));
    this.dataSet.setYAxisSegmentationColors(highlightList);

    let dataSetList: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
    dataSetList.add(this.dataSet);

    let barData: BarData = new BarData(dataSetList);
    barData.setBarWidth(0.3);
    return barData;
  }
  
  private getTopAndBottomHighlightData(): BarData {
    let values: JArrayList<WaterfallEntry> = new JArrayList<WaterfallEntry>();
    let highlightList: WaterfallHighlight[] = [];
    // Set the color of the Y-axis scale range.
    highlightList.push(new WaterfallHighlight(0, 40, Color.Green));
    highlightList.push(new WaterfallHighlight(40, 60, Color.Orange));
    highlightList.push(new WaterfallHighlight(60, 100, Color.Pink));

    // Encapsulate the waterfall data.
    values.add(new WaterfallEntry(1, 10, 90));
    values.add(new WaterfallEntry(2, 15, 80));
    values.add(new WaterfallEntry(3, 20, 90));

    this.dataSet = new WaterfallDataSet(values, 'DataSet');
    // Set the color of the waterfall chart when it is selected.
    this.dataSet.setHighLightColor(Color.Gray);
    this.dataSet.setDrawIcons(false);
    // Set the color of the waterfall chart.
    this.dataSet.setColorByColor(Color.Pink);
    // Set the color of the Y-axis scale range.
     this.dataSet.setYAxisSegmentationColors(highlightList);
    // Set whether to highlight the maximum value and the minimum value.
    this.dataSet.setEnableMaxOrMinHighlightColor(true);
    // Set the highlight color of the maximum value.
    this.dataSet.setMaxyHighlightColor(Color.Brown);
    // Set the highlight color of the minimum value.
    this.dataSet.setMinyHighlightColor(Color.Yellow);

    let dataSetList: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
    dataSetList.add(this.dataSet);

    let barData: BarData = new BarData(dataSetList);
    barData.setBarWidth(0.3);
    return barData;
  }
```
2. Add data to the waterfall chart component.

```
// Set the configuration builder class for the component. If the chart needs to be displayed during page initialization, use aboutToAppear to build the chart data.
// If the chart is triggered by event,
// use dataResult.notifyDataSetChanged(); to update data,
// use this.model.notifyDataSetChanged(); to update axis data,
// and use this.model.invalidate(); to update drawing.
WaterfallChart({ model: this.model })
  .width('100%')
  .height('70%')
```

### Adding a Custom Icon (to a Bar Chart)

```
    let charPixelMap: ChartPixelMap = new ChartPixelMap();
    let fileData = resourceMgr.getMediaContentSync($r('app.media.star').id)
    const buffer:ArrayBuffer =fileData.buffer.slice(0,fileData.buffer.byteLength);
    const imageSource = image.createImageSource(buffer)
    let pixelMap=await imageSource.createPixelMap()
    // Set the custom icon resource (PixelMap).
    charPixelMap.setIcon(pixelMap)
    // Set the resId of the custom icon resource.
    //charPixelMap.setIcon($r('app.media.star').id)
    // Set the width of the custom icon.
    charPixelMap.setWidth(20)
     // Set the height of the custom icon.
    charPixelMap.setHeight(20);

    let values: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    for (let i = startAxis; i < endAxis; i++) {
      values.add(new BarEntry(i, Math.random() * range,charPixelMap))
    }

    let dataSet: BarDataSet = new BarDataSet(values, 'DataSet');
    dataSet.setHighLightColor(Color.Black);
    // Set whether to draw the custom icons.
    dataSet.setDrawIcons(true);
    // Set the offset of the custom icon.
    dataSet.setIconsOffset(new MPPointF(0, 40));
    dataSet.setColorsByVariable(ColorTemplate.VORDIPLOM_COLORS);
    dataSet.setDrawValues(false)
```


### Custom Chart

1. Customize the data structure

   Chart data consists of three base classes, including EntryOhos, DataSet, and ChartData.

   - CustomEntry: basic data entity of the custom chart.

     Inherited from EntryOhos, this class provides the **get** and **set** functions to obtain and set the properties of the data entity class.

   - CustomDataSet: collection class of the custom chart entities.

     Inherited from DataSet, this class describes the information of a group of entities and manages the common properties of each entry. For example, you can obtain the maximum and minimum values of the X-axis and Y-axis in the dataset, and set the highlight color of the chart.

   - CustomData: data container of the custom chart.

     Inherited from ChartData, this class processes the information of common entity set (dataset). For example, you can calculate the maximum and minimum values of the X-axis and Y-axis during final display, and the bar width of a bar chart.

2. Customize the configuration builder class.

   Inherited from the **ChartModel** class, the child classes **BarLineChartBaseModel** and **PieRadarChartBaseModel** can be used as required. In addition, this class need to implement the **DataProvider** API of the corresponding chart to obtain the chart data container (CustomData). The **DataProvider** is inherited from **ChartInterface**.

   To customize the chart configuration builder class, the following functions need to be overridden:

   - setContext2D <font color='red'>Mandatory</font>

     Binds **CanvasRenderingContext2D** with the chart drawing process for subsequent chart drawing operations. Since chart drawing depends on the Canvas component, it is necessary to obtain the **CanvasRenderingContext2D** API of the Canvas component during the drawing process.

     ```
     public setContext2D(context2d: CanvasRenderingContext2D) {
       this.context2d = context2d;
     }
     ```

   - onChartSizeChanged <font color='red'>Mandatory</font>

     Obtains the size change of the component. To display a chart during page initialization, you must use **aboutToAppear()** to build and configure the chart data. However, in this case, the width and height of the component obtained in the chart drawing process will be incorrect.

     Therefore, after the Canvas component completes the layout, you need to use **onAreaChange()** to notify the drawing process of the component size change, so that the drawing process can calculate the scaling ratio and correct coordinates based on the correct component size.

     ```
     public onChartSizeChanged(newWidth: number, newHeight: number, oldWidth: number, oldHeight: number) {
       super.onSizeChanged(newWidth, newHeight, oldWidth, oldHeight);
     }
     ```

   - invalidate   <font color='red'>Mandatory</font>

     Triggers chart redrawing. After operations such as updating chart data, axis boundaries, and display settings, you need to manually trigger chart redrawing to refresh the latest display effect.

     ```
     public invalidate(): void {
       if (this.context2d) {
         super.onDraw(this.context2d);
       }
     }
     ```

   - init <font color='red'>Mandatory</font>

     Initializes the components of a chart and their rendering classes.

     You need to customize components such as **XAxis**, **YAxis**, **Description**, and **Legend**,

     coordinate conversion class such as **Transformer**,

     and rendering classes such as **XAxisRenderer**, **YAxisRenderer**, and **DataRenderer**.

     ```
     public init(): void {
       super.init();
     
       this.mRenderer = new BarChartRenderer(this, this.mAnimator!, this.mViewPortHandler);
     
       this.setHighlighter(new BarHighlighter(this));
     
       let xAxis : XAxis | null = this.getXAxis();
       if (xAxis) {
         xAxis.setSpaceMin(0.5);
         xAxis.setSpaceMax(0.5);
       }
     }
     ```

   - onDraw <font color='red'>Mandatory</font>

     Specifies the chart drawing process. **BarLineChartBaseModel** and **PieRadarChartBaseModel** meet the general drawing requirements and they can also be customized based on services. This function is used to refresh the chart by redrawing the chart.

     <font color='red'>NOTE</font>

     Do not perform time-consuming operations in **onDraw**.

     Do not create an object in **onDraw**.

3. Customize the chart components.

   The chart components such as **Description**, **Legend**, **LimitLine**, **XAxis**, **YAxis** inherit from the base class ComponentBase and provides the basic properties such as **xOffset**, **yOffset**, **enabled**, **textSize**, and **textColor**. In addition, custom properties can be added for each component based on features. For example, **calculate()** is used to calculate the expected maximum and minimum values of the X-axis and Y-axis. (The maximum and minimum values may be different from those in the dataset. You can set the reserved space for the maximum and minimum values of the X-axis and Y-axis.)

   ```
   /**
    * Calculate the maximum and minimum values of the coordinate axis to be displayed.
    * @param dataMin: minimum value.
    * @param dataMax: maximum value.
    */
   public calculate(dataMin: number, dataMax: number): void {
   
     // if custom, use value as is, else use data value
     let min: number = this.mCustomAxisMin ? this.mAxisMinimum : (dataMin - this.mSpaceMin);
     let max: number = this.mCustomAxisMax ? this.mAxisMaximum : (dataMax + this.mSpaceMax);
   
     // temporary range (before calculations)
     let range: number = Math.abs(max - min);
   
     // in case all values are equal
     if (range == 0) {
       max = max + 1;
       min = min - 1;
     }
   
     this.mAxisMinimum = min;
     this.mAxisMaximum = max;
   
     // actual range
     this.mAxisRange = Math.abs(max - min);
   }
   ```

4. Customize the rendering classes

   CustomXAxisRenderer/CustomYAxisRenderer: axis renderer.

   - computeAxis

     Determines whether to correct the maximum and minimum values of the X-axis and Y-axis to be displayed based on the configuration information.

     ```
     /**
      * Use X-axis renderer to calculate the minimum and maximum values of the X-axis to be displayed.
      * @param min: minimum scale value of the X-axis that is expected to be displayed, which is obtained through calculate() of X-axis renderer.
      * @param max: maximum scale value of the X-axis that is expected to be displayed, which is obtained through calculate() of X-axis renderer.
      * @param inverted: whether to invert the coordinate axis.
      */
     public computeAxis(min: number, max: number, inverted: boolean) {
     
       // calculate the starting and entry point of the y-labels (depending on
       // zoom / contentrect bounds)
       if (!this.mViewPortHandler || !this.mTrans) {
         return;
       }
       
       // If the ratio of the X-axis is changed, recalculate the maximum and minimum values of the X-axis based on the width of the current component and scalex.
       if (this.mViewPortHandler.contentWidth() > 10 && !this.mViewPortHandler.isFullyZoomedOutX()) {
     
         let p1: MPPointD | undefined = this.mTrans?.getValuesByTouchPoint(this.mViewPortHandler.contentLeft(), this.mViewPortHandler.contentTop());
         let p2: MPPointD | undefined = this.mTrans?.getValuesByTouchPoint(this.mViewPortHandler.contentRight(), this.mViewPortHandler.contentTop());
     
         if (!!p1 && !!p2) {
           if (inverted) {
     
             min = p2.x;
             max = p1.x;
           } else {
     
             min = p1.x;
             max = p2.x;
           }
     
           MPPointD.recycleInstance(p1);
           MPPointD.recycleInstance(p2);
         }
       }
     
       this.computeAxisValues(min, max);
     }
     ```

   - computeAxisValues

     Calculates the number of scale labels to be displayed between the maximum and minimum values and the corresponding values. This function is used after the maximum and minimum values of the X-axis and Y-axis are determined.

   - renderAxisLabels

     Draws the scale labels of the coordinate axes.

   - renderGridLines

     Draws the gridlines of the coordinate axes.

   - renderAxisLine

     Draws the axis lines of the coordinate axes.

   - renderLimitLines

     Draws the limit lines of the coordinate axes.

   CustomDataRender: data renderer.

   - initBuffers

     Processes cached data of the chart. Since this method allocates memory, exercise caution when using it.

   - drawData

     Draws charts and convert data into charts.

   - drawValues

     Displays data values in charts.

   - drawExtras

     Draws extra chart content.

   - drawHighlighted

     Draws the chart highlight style.

5. Customize a chart component

   Chart drawing is implemented based on the **Canvas** component. Therefore, the custom chart model must contain the **Canvas** component. Enable drawing in **onReady()** of the **Canvas** component and use **onAreaChange()** to correct the chart size.

   ```
   import CustomModel from '../xx/CustomModel';
   
   @Component
   export struct CustomChart {
     model: CustomModel | null = null;
     private setting: RenderingContextSettings = new RenderingContextSettings(true);
     private context2D: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.setting);
   
     build() {
       Column() {
         Canvas(this.context2D)
           .onReady(() => {
             if (this.model) {
             
               / /Bind the CanvasRenderingContext2D to the chart drawing process.
               this.model.setContext2D(this.context2D);
               
               // Start to draw a chart.
               this.model.onDraw(this.context2D);
             }
           })
           .onAreaChange((oldArea: Area, newArea: Area) => {
             if (this.model && ((newArea.width !== oldArea.width) || (newArea.height !== oldArea.height))) {
             
               // Correct the chart size.
               this.model.onChartSizeChanged(Number(newArea.width), Number(newArea.height), Number(oldArea.width), Number(oldArea.height));
             }
           })
           // Bind the gesture recognition events.
           .priorityGesture(GestureGroup(GestureMode.Parallel,
             TapGesture({ count: 1 })
               .onAction((event?: GestureEvent) => {
                 if (event && this.model) {
                   this.model.onSingleTapUp(false, event);
                 }
               }),
             PanGesture()
               .onActionStart((event?: GestureEvent) => {
                 if (this.model && event) {
                   this.model.onPan(false, 'Down', event);
                 }
               })
               .onActionUpdate((event?: GestureEvent) => {
                 if (this.model && event) {
                   this.model.onPan(false, 'Move', event);
                 }
               })
               .onActionEnd((event?: GestureEvent) => {
                 if (this.model && event) {
                   this.model.onPan(false, 'Up', event);
                 }
               })
           ))
       }
     }
   }
   ```

6. Display the chart.

   Initialize data and set the UI display.

   ```
   aboutToAppear() {
     this.model = new CustomModel();
     // ... Configure the style of each component.
     // ... Initialize the data.
   }
   
   build() {
     Column() {
       CustomChart({ model: this.model })
         .width('100%')
         .height('100%')
     }
   }
   ```

## Available APIs
**1. ChartModel**

Indicates the base class for all chart configuration builder classes.

| API                    | Description                                                        |
| -------------------------- | ------------------------------------------------------------ |
| animateX                   | Animates the chart values on the X-axis.                                     |
| animateXY                  | Animates the chart values on the X-axis and Y-axis.                            |
| animateY                   | Animates the chart values on the Y-axis.                                     |
| calcMinMax                 | Calculates the values of y-min and y-max, and the values of y-delta and x-delta.          |
| calculateOffsets           | Calculates the offset of the chart to the border, which depends on the position of the legend, the length of the X-axis and Y-axis, and the position of their labels.|
| clear                      | Deletes all data of a chart, sets the data to null, and refreshes the chart.              |
| clearValues                | Deletes all datasets (and entries) from the chart and refreshes the chart.            |
| getAnimator                | Returns the animator responsible for animating chart values.                  |
| getCenter                  | Returns a recyclable MPPointF instance of the center point of the chart (the entire view).          |
| getCenterOffsets           | Returns a recyclable MPPointF instance of the central drawing area of the chart.        |
| getCenterOfView            | Returns a recyclable MPPointF instance of the center point of the chart (the entire view).          |
| getContentRect             | Returns the rectangle of the content area of the chart.                              |
| getData                    | Returns the ChartData object that has been set for the chart.                         |
| getDefaultValueFormatter   | Returns the default implementation class of the data formatting APIs.                            |
| getHighlightByTouchPoint   | Returns the **Highlight** object (including x-index and DataSet index) of the LineChart, ScatterChart, CandleStickChart, when the chart is touched on the screen.|
| getHighlighted             | Returns the array of currently highlighted values. The value may be null, or an empty array if there is no highlighted values.|
| getHighlighter             | Returns the API used to highlight the items.                                  |
| getLegend                  | Obtains the legend object. You can set the value of **setEnable()** to **false** to disable the legend.|
| getLegendRenderer          | Obtains the legend renderer.                                            |
| getRenderer                | Obtains the chart data renderer.                                        |
| getXAxis                   | Obtains the X-axis object.                                               |
| invalidate                 | Refreshes the chart.                                                  |
| notifyDataSetChanged       | Notifies that the dataset is refreshed.                                              |
| onChartSizeChanged         | Updates the size of the chart.                                          |
| onDraw                     | Draws a chart.                                              |
| setContext2D               | Sets the canvas for drawing a chart.                                        |
| setData                    | Sets the chart data.                                              |
| setDragDecelerationEnabled | Sets whether to enable scrolling with deceleration when the chart is dragged.                                      |
| setHitTestMode             | Sets the hit test mode.                                          |
| getHitTestMode             | Obtains the hit test mode.                                          |

**1.animateX**

public animateX(durationMillis: number): void;

Animates the chart values on the X-axis.

Parameters

| Name        | Type  | Mandatory| Description                      |
| -------------- | ------ | ---- | -------------------------- |
| durationMillis | number | Yes  | Duration of the animation, in milliseconds.|

public animateX(durationMillis: number, easing: string): void;

Animates the X-axis with the easing curve specified.

Parameters

| Name        | Type  | Mandatory| Description                                                        |
| -------------- | ------ | ---- | ------------------------------------------------------------ |
| durationMillis | number | Yes  | Duration of the animation, in milliseconds.                                  |
| easing         | string | Yes  | [Animation interpolation curve](https://docs.openharmony.cn/pages/v5.0/en/application-dev/reference/apis-arkui/js-apis-animator.md).|

**2.animateXY**

public animateXY(durationMillisX: number, durationMillisY: number): void;

Animates the chart values on the X-axis and Y-axis.

Parameters

| Name         | Type  | Mandatory| Description                             |
| --------------- | ------ | ---- | --------------------------------- |
| durationMillisX | number | Yes  | Duration of the animation of the X-axis, in milliseconds.|
| durationMillisY | number | Yes  | Duration of the animation of the Y-axis, in milliseconds.|

public animateXY(durationMillisX: number, durationMillisY: number, easingX: string): void;

Animates both the X-axis and Y-axis with the easing curve of the X-axis specified.

Parameters

| Name         | Type  | Mandatory| Description                                                        |
| --------------- | ------ | ---- | ------------------------------------------------------------ |
| durationMillisX | number | Yes  | Duration of the animation of the X-axis, in milliseconds.                           |
| durationMillisY | number | Yes  | Duration of the animation of the Y-axis, in milliseconds.                           |
| easingX         | string | Yes  | [Animation interpolation curve](https://docs.openharmony.cn/pages/v5.0/en/application-dev/reference/apis-arkui/js-apis-animator.md) of the X-axis.|

public animateXY(durationMillisX: number, durationMillisY: number, easingX: string, easingY: string): void;

Animates both the X-axis and Y-axis with the easing curves specified.

Parameters

| Name         | Type  | Mandatory| Description                                                        |
| --------------- | ------ | ---- | ------------------------------------------------------------ |
| durationMillisX | number | Yes  | Duration of the animation of the X-axis, in milliseconds.                           |
| durationMillisY | number | Yes  | Duration of the animation of the Y-axis, in milliseconds.                           |
| easingX         | string | Yes  | [Animation interpolation curve](https://docs.openharmony.cn/pages/v5.0/en/application-dev/reference/apis-arkui/js-apis-animator.md) of the X-axis.|
| easingY         | string | Yes  | [Animation interpolation curve](https://docs.openharmony.cn/pages/v5.0/en/application-dev/reference/apis-arkui/js-apis-animator.md) of the Y-axis.|

**3.animateY**

Animates the chart values on the Y-axis.

public animateY(durationMillis: number): void;

Parameters

| Name        | Type  | Mandatory| Description                      |
| -------------- | ------ | ---- | -------------------------- |
| durationMillis | number | Yes  | Duration of the animation, in milliseconds.|

public animateY(durationMillis: number, easing: string): void;

Parameters

| Name        | Type  | Mandatory| Description                                                        |
| -------------- | ------ | ---- | ------------------------------------------------------------ |
| durationMillis | number | Yes  | Duration of the animation, in milliseconds.                                  |
| easing         | string | Yes  | [Animation interpolation curve](https://docs.openharmony.cn/pages/v5.0/en/application-dev/reference/apis-arkui/js-apis-animator.md).|

**4.calcMinMax**

protected abstract calcMinMax() : void;

Calculates the values of y-min and y-max, and the values of y-delta and x-delta.

**5.calculateOffsets**

protected abstract calculateOffsets() : void;

Calculates the offset of the chart to the border, which depends on the position of the legend, the length of the X-axis and Y-axis, and the position of their labels.

**6.clear**

public clear() : void;

Deletes all data of a chart, sets the data to null, and refreshes the chart.

**7.clearValues**

public clearValues() : void;

Deletes all datasets (and entries) from the chart and refreshes the chart.

**8.getAnimator**

public getAnimator(): ChartAnimator | null;

Returns the animator responsible for animating chart values.

Return Value

| Type                     | Description        |
| ------------------------- | ------------ |
| ChartAnimator &#124; null | Animation management class.|

**9.getCenter**

public getCenter(): MPPointF;

Returns a recyclable MPPointF instance of the center point of the chart (the entire view).

Return Value

| Type    | Description                  |
| -------- | ---------------------- |
| MPPointF | The recyclable MPPointF instance.|

**10.getCenterOffsets**

public getCenterOffsets(): MPPointF | null;

Returns a recyclable MPPointF instance of the central drawing area of the chart.

Return Value

| Type                | Description                  |
| -------------------- | ---------------------- |
| MPPointF &#124; null | The recyclable MPPointF instance.|

**11.getCenterOfView**

public getCenterOfView(): MPPointF;

Returns a recyclable MPPointF instance of the center point of the chart (the entire view).

Return Value

| Type    | Description                  |
| -------- | ---------------------- |
| MPPointF | The recyclable MPPointF instance.|

**12.getContentRect**

public getContentRect(): Rect;

Returns the rectangle of the content area of the chart.

Return Value

| Type| Description                      |
| ---- | -------------------------- |
| Rect | The rectangle of the content area.|

**13.getData**

public getData(): T | null ;

Returns the ChartData object that has been set for the chart.

Return Value

| Type        | Description      |
| ------------ | ---------- |
| T&#124; null | The data object.|

**14.getDefaultValueFormatter**

public getDefaultValueFormatter(): IValueFormatter;

Returns the default implementation class of the data formatting APIs.

Return Value

| Type           | Description                  |
| --------------- | ---------------------- |
| IValueFormatter | Implementation class of the data formatting APIs.|

**15.getHighlightByTouchPoint**

public getHighlightByTouchPoint(x: number, y: number): Highlight | null;

Returns the **Highlight** object (including x-index and DataSet index) of the LineChart, ScatterChart, and CandleStickChart, when the chart is touched on the screen.

Parameters

| Name| Type  | Mandatory| Description                 |
| ------ | ------ | ---- | --------------------- |
| x      | number | Yes  | X-coordinate of the touch point on the screen.|
| y      | number | Yes  | Y-coordinate of the touch point on the screen.|

Return Value

| Type                 | Description              |
| --------------------- | ------------------ |
| Highlight &#124; null | Highlighted object of the touch point.|

**16.getHighlighted**

public getHighlighted(): Highlight[] | null;

Returns the array of currently highlighted values. The value may be null, or an empty array if there is no highlighted values.

Parameters

| Name| Type  | Mandatory| Description                 |
| ------ | ------ | ---- | --------------------- |
| x      | number | Yes  | X-coordinate of the touch point on the screen.|
| y      | number | Yes  | Y-coordinate of the touch point on the screen.|

Return Value

| Type                 | Description              |
| --------------------- | ------------------ |
| Highlight &#124; null | Highlighted object of the touch point.|

**17.getHighlighter**

public getHighlighter(): IHighlighter | null;

Returns the API used to highlight the items.

Return Value

| Type                    | Description                        |
| ------------------------ | ---------------------------- |
| IHighlighter &#124; null | API used to obtain the highlighted object of a touch point.|

**18.getLegend**

public getLegend(): Legend | null ;

Obtains the legend object.

Return Value

| Type              | Description          |
| ------------------ | -------------- |
| Legend &#124; null | Obtains the legend object.|

**19.getLegendRenderer**

public getLegendRenderer(): LegendRenderer | null;

Obtains the legend render.

Return Value

| Type                     | Description            |
| ------------------------- | ---------------- |
| LegendRenderer&#124; null | Obtains the legend renderer.|

**20.getRenderer**

public getRenderer(): DataRenderer | null;

Obtains the chart data renderer.

Return Value

| Type                    | Description                |
| ------------------------ | -------------------- |
| DataRenderer &#124; null | Obtains the chart data renderer.|

**21.getXAxis**

public getXAxis(): XAxis | null;

Obtains the X-axis object.

Return Value

| Type             | Description         |
| ----------------- | ------------- |
| XAxis &#124; null | Obtains the X-axis object.|


**22.invalidate**

public abstract invalidate();

Refreshes the chart.

**23.notifyDataSetChanged**

public abstract notifyDataSetChanged();

Notifies that the dataset is refreshed.

**24.onChartSizeChanged**

public abstract onChartSizeChanged(newWidth: number, newHeight: number, oldWidth: number, oldHeight: number): void;

Updates the size of the chart.

Parameters

| Name   | Type  | Mandatory| Description                    |
| --------- | ------ | ---- | ------------------------ |
| newWidth  | number | Yes  | Width of the chart.      |
| newHeight | number | Yes  | Height of the chart.      |
| oldWidth  | number | Yes  | Width of the chart before the chart size is changed.|
| oldHeight | number | Yes  | Height of the chart before the chart size is changed.|

**25.onDraw**

protected onDraw(c: CanvasRenderingContext2D): void ;

Draws a chart.

Parameters

| Name| Type                    | Mandatory| Description      |
| ------ | ------------------------ | ---- |----------|
| c      | CanvasRenderingContext2D | Yes  | Canvas for drawing a chart.|

**26.setContext2D**

public abstract setContext2D(context2d: CanvasRenderingContext2D);

Sets the canvas for drawing a chart.

Parameters

| Name| Type                    | Mandatory| Description      |
| ------ | ------------------------ | ---- |----------|
| c      | CanvasRenderingContext2D | Yes  | Canvas for drawing a chart.|

**27.setData**

public setData(data: T) : void;

Sets the chart data.

Parameters

| Name| Type| Mandatory| Description                        |
| ------ | ---- | ---- | ---------------------------- |
| data   | T    | Yes  | Chart data. T is inherited from **ChartData**.|

**28.setDragDecelerationEnabled**

public setDragDecelerationEnabled(enabled: boolean);

Sets whether to enable scrolling with deceleration when the chart is dragged.

Parameters

| Name| Type| Mandatory| Description                |
| ------ | ---- | ---- |--------------------|
| enabled   | boolean    | Yes  | Capability identifier.|

**29.setHitTestMode**

public setHitTestMode(hitTestMode: HitTestMode)

Sets the hit test mode.

Parameters

| Name| Type| Mandatory| Description                |
| ------ | ---- | ---- |--------------------|
| hitTestMode   | HitTestMode    | Yes  | Hit test mode.|

**30.getHitTestMode**

public getHitTestMode(): HitTestMode

Obtains the hit test mode.

Return Value

| Type   | Description       |
|-------| ----------- |
| HitTestMode | Hit test mode.|

**2. BarLineChartBaseModel**

Defines base class of the bar chart.

| API                   | Description                                              |
| ------------------------- | -------------------------------------------------- |
| calcMinMax                | Calculates the maximum and minimum values of coordinates.                            |
| calculateOffsets          | Calculates the chart offsets.                                  |
| drawGridBackground        | Draws the chart background.                                    |
| getAxis                   | Obtains the Y-axis object based on the Y-axis position.                          |
| getAxisLeft               | Obtains the left Y-axis object.                                   |
| getAxisRight              | Obtains the right Y-axis object.                                   |
| getMinOffset              | Obtains the minimum offset value.                                |
| getRendererLeftYAxis      | Obtains the left Y-axis renderer.                                 |
| getRendererRightYAxis     | Obtains the right Y-axis renderer.                                 |
| getRendererXAxis          | Obtains the X-axis renderer.                                   |
| setLongPressCursorEnabled | Sets whether to display the cursor when a chart is long-pressed on.                            |
| getLongPressCursorEnabled | Obtains whether the cursor is displayed when a chart is long-pressed on.                            |
| setLongPressDuration      | Sets the response duration for a long press event.                                |
| getLongPressDuration      | Obtains the response duration of a long press.                                |
| setSwipeEnabled           | Sets whether to enable the capability to respond to swipe gestures. This API is used together with **setLongPressCursorEnabled**.    |
| getSwipeEnabled           | Obtains whether the capability to respond to swipe gestures is enabled. This API is used together with **setLongPressCursorEnabled**.|

**1.calcMinMax**

protected calcMinMax(): void;

Calculates the maximum and minimum values of coordinates.

**2.calculateOffsets**

public calculateOffsets(): void;

Calculates the chart offsets.

**3.drawGridBackground**

protected drawGridBackground(c: CanvasRenderingContext2D): void;

Draws the chart background.

Parameters

| Name| Type                    | Mandatory| Description              |
| ------ | ------------------------ | ---- | ------------------ |
| c      | CanvasRenderingContext2D | Yes  | Context for drawing a chart.|

**4.getAxis**

public getAxis(axis: AxisDependency): YAxis | null;

Obtains the Y-axis object based on the Y-axis position.

Parameters

| Name| Type          | Mandatory| Description     |
| ------ | -------------- | ---- | --------- |
| axis   | AxisDependency | Yes  | Position of the Y-axis.|

Return Value

| Type             | Description         |
| ----------------- | ------------- |
| YAxis &#124; null | Obtains the Y-axis object.|

**5.getAxisLeft**

public getAxisLeft(): YAxis | null;

Obtains the left Y-axis object.

Return Value

| Type             | Description         |
| ----------------- | ------------- |
| YAxis &#124; null | Obtains the Y-axis object.|

**6.getAxisRight**

public getAxisRight(): YAxis | null;

Obtains the right Y-axis object.

Return Value

| Type             | Description         |
| ----------------- | ------------- |
| YAxis &#124; null | Obtains the Y-axis object.|

**7.getMinOffset**

public getMinOffset(): number;

Obtains the minimum offset value.

Return Value

| Type  | Description              |
| ------ | ------------------ |
| number | Obtains the minimum offset value.|

**8.getRendererLeftYAxis**

public getRendererLeftYAxis(): YAxisRenderer | null;

Obtains the left Y-axis renderer.

Return Value

| Type                     | Description             |
| ------------------------- | ----------------- |
| YAxisRenderer &#124; null | Obtains the left Y-axis renderer.|

**9.getRendererRightYAxis**

public getRendererRightYAxis(): YAxisRenderer | null;

Obtains the right Y-axis renderer.

Return Value

| Type                     | Description             |
| ------------------------- | ----------------- |
| YAxisRenderer &#124; null | Obtains the right Y-axis renderer.|

**10.getRendererXAxis**

public getRendererXAxis(): XAxisRenderer | null;

Obtains the X-axis renderer.

Return Value

| Type                     | Description           |
| ------------------------- | --------------- |
| XAxisRenderer &#124; null | Obtains the X-axis renderer.|

**11.setLongPressCursorEnabled**

public setLongPressCursorEnabled(enabled: boolean): void;

Sets whether to display the cursor when a chart is long-pressed on.

Parameters

| Name| Type     | Mandatory| Description   |
| ------ |---------| ---- |-------|
| enabled   | boolean | Yes  | Capability identifier.|

Return value: none

**12.getLongPressCursorEnabled**

public getLongPressCursorEnabled(): boolean;

Obtains whether the cursor is displayed when a chart is long-pressed on.

Parameter: none

Return Value

| Type     | Description       |
|---------|-----------|
| boolean | Identifier of whether the cursor is displayed when the chart is long-pressed on.|

**13.setLongPressDuration**

public setLongPressDuration(duration: number): void;

Sets the response duration for a long press event.

Parameters

| Name| Type     | Mandatory| Description |
| ------ |---------| ---- |-----|
| duration   | number | Yes  | Duration|

Return value: none

**14.getLongPressDuration**

public getLongPressDuration(): number;

Obtains the response duration for a long press.

Parameter: none

Return Value

| Type     | Description   |
|---------|-------|
| number | Response duration.|

**15.setSwipeEnabled**

public setSwipeEnabled(enabled: boolean): void;

Sets whether to enable the capability to respond to swipe gestures. This API is used together with **setLongPressCursorEnabled**.

Parameters

| Name| Type     | Mandatory| Description   |
| ------ |---------| ---- |-------|
| enabled   | boolean | Yes  | Capability identifier.|

Return value: none

**16.getSwipeEnabled**

public getSwipeEnabled(): boolean;

Obtains whether the capability to respond to swipe gestures is enabled. This API is used together with **setLongPressCursorEnabled**.

Parameter: none

Return Value

| Type     | Description       |
|---------|-----------|
| boolean | Identifier of whether the capability to respond to swipe gestures is enabled.|

**3. ComponentBase**

Defines the base class of chart components.

| API      | Description                                |
| ------------ | ------------------------------------ |
| setTextColor | Sets the rendering color of the component text.              |
| getTextColor | Obtains the rendering color of the component text.              |
| setTextSize  | Sets the size of the component text, which needs to be converted into a value in units of vp.|
| getTextSize  | Obtains the size of the component text.                  |
| setXOffset   | Sets the offset between the X-axis and the chart content area.   |
| getXOffset   | Obtains the offset between the X-axis and the chart content area.   |
| setYOffset   | Sets the offset between the Y-axis and the chart content area.   |
| getYOffset   | Obtains the offset between the Y-axis and the chart content area.   |

**1.setTextColor**

public setTextColor(color: string | number | CanvasGradient | CanvasPattern): void;

Sets the rendering color of the component text.

Parameters

| Name| Type                                                       | Mandatory| Description                  |
| ------ | ----------------------------------------------------------- | ---- | ---------------------- |
| color  | string &#124;number&#124;CanvasGradient &#124;CanvasPattern | Yes  | Sets the rendering color of the component text.|

**2.getTextColor**

public getTextColor(): string | number | CanvasGradient | CanvasPattern;

Obtains the rendering color of the component text.

Return Value

| Type                                                       | Description                  |
| ----------------------------------------------------------- | ---------------------- |
| string &#124;number&#124;CanvasGradient &#124;CanvasPattern | Obtains the rendering color of the component text.|

**3.setTextSize**

public setTextSize(size: number): void;

Sets the size of the component text, in vp.

Parameters

| Name| Type  | Mandatory| Description              |
| ------ | ------ | ---- | ------------------ |
| size   | number | Yes  | Sets the size of the component text.|

**4.getTextSize**

public getTextSize(): number;

Obtains the size of the component text.

Return Value

| Type  | Description              |
| ------ | ------------------ |
| number | Obtains the size of the component text.|

**5.setXOffset**

public setXOffset(xOffset: number): void;

Sets the offset between the X-axis and the chart content area.

Parameters

| Name | Type  | Mandatory| Description                             |
| ------- | ------ | ---- | --------------------------------- |
| xOffset | number | Yes  | Sets the offset between the X-axis and the chart content area.|

**6.getXOffset**

public getXOffset(): number;

Obtains the offset between the X-axis and the chart content area.

Return Value

| Type  | Description                             |
| ------ | --------------------------------- |
| number | Obtains the offset between the X-axis and the chart content area.|

**7.setYOffset**

public setYOffset(yOffset: number): void;

Sets the offset between the Y-axis and the chart content area.

Parameters

| Name | Type  | Mandatory| Description                             |
| ------- | ------ | ---- | --------------------------------- |
| yOffset | number | Yes  | Sets the offset between the Y-axis and the chart content area.|

**8.getYOffset**

public getYOffset(): number;

Obtains the offset between the Y-axis and the chart content area.

Return Value

| Type  | Description                             |
| ------ | --------------------------------- |
| number | Obtains the offset between the Y-axis and the chart content area.|

**4. AxisBase**

Defines the base class of the axis.

| API                     | Description                                              |
| --------------------------- | -------------------------------------------------- |
| addLimitLine                | Adds a limit line for the coordinate axis.                          |
| calculate                   | Calculates the expected maximum and minimum values of the coordinate axis, which are used to customize the coordinate axis.|
| setAxisMaximum              | Sets the maximum value of the custom axis.                          |
| setAxisMinimum              | Sets the minimum value of the custom axis.                          |
| setDrawAxisLine             | Sets whether to draw the axis line.                                |
| setDrawGridLines            | Sets whether to draw gridlines.                              |
| setDrawGridLinesBehindData  | Sets whether to draw gridlines behind data.                      |
| setDrawLimitLinesBehindData | Sets whether to draw limit lines behind data.                      |
| setValueFormatter           | Sets the value formatter.                            |

**1.addLimitLine**

public addLimitLine(limitLine: LimitLine): void;

Adds a limit line for the coordinate axis.

Parameters

| Name   | Type     | Mandatory| Description                    |
| --------- | --------- | ---- | ------------------------ |
| limitLine | LimitLine | Yes  | Limit line for the coordinate axis.|

**2.calculate**

public calculate(dataMin: number, dataMax: number): void;

Calculates the expected maximum and minimum values of the coordinate axis, which are used to customize the coordinate axis.

Parameters

| Name | Type  | Mandatory| Description                |
| ------- | ------ | ---- | -------------------- |
| dataMin | number | Yes  | Minimum value in a dataset.|
| dataMax | number | Yes  | Maximum value in a dataset.|

**3.setAxisMaximum**

public setAxisMaximum(max: number): void;

Sets the maximum value of the custom axis.

Parameters

| Name| Type  | Mandatory| Description              |
| ------ | ------ | ---- | ------------------ |
| max    | number | Yes  | Maximum value of a custom axis.|

**4.setAxisMinimum**

public setAxisMinimum(min: number): void;

Minimum value of the custom axis.

Parameters

| Name| Type  | Mandatory| Description                |
| ------ | ------ | ---- | -------------------- |
| min    | number | Yes  | Minimum value of a custom axis.|

**5.setDrawAxisLine**

public setDrawAxisLine(enabled: boolean): void;

Sets whether to draw the axis line.

Parameters

| Name | Type   | Mandatory| Description              |
| ------- | ------- | ---- | ------------------ |
| enabled | boolean | Yes  | Whether to draw the axis line.|

**6.setDrawGridLines**

public setDrawGridLines(enabled: boolean): void;

Sets whether to draw gridlines.

Parameters

| Name | Type   | Mandatory| Description                |
| ------- | ------- | ---- | -------------------- |
| enabled | boolean | Yes  | Whether to draw gridlines.|

**7.setDrawGridLinesBehindData**

public setDrawGridLinesBehindData(enabled: boolean): void;

Sets whether to draw gridlines behind data.

Parameters

| Name | Type   | Mandatory| Description                        |
| ------- | ------- | ---- | ---------------------------- |
| enabled | boolean | Yes  | Whether to draw gridlines behind data.|

**8.setDrawLimitLinesBehindData**

public setDrawLimitLinesBehindData(enabled: boolean): void ;

Sets whether to draw limit lines behind data.

Parameters

| Name | Type   | Mandatory| Description                        |
| ------- | ------- | ---- | ---------------------------- |
| enabled | boolean | Yes  | Whether to draw limit lines behind data.|

**9.setValueFormatter**

public setValueFormatter(formatter: IAxisValueFormatter): void ;

Sets the data formatter.

Parameters

| Name   | Type               | Mandatory| Description                  |
| --------- | ------------------- | ---- | ---------------------- |
| formatter | IAxisValueFormatter | Yes  | Data formatter.|

**5. XAxis**

Defines an X-axis object.

| API               | Description                          |
| --------------------- | ------------------------------ |
| setPosition           | Sets the position of the X-axis.             |
| getPosition           | Obtains the position of the X-axis.             |
| setLabelRotationAngle | Sets the rotation angle of the X-axis label.         |
| getLabelRotationAngle | Obtains the rotation angle of the X-axis label.         |
| setLabelXOffset       | Sets the offset of the X-axis label.           |
| getLabelXOffset       | Obtains the offset of the X-axis label.   |
| setXAxisLabelPosition | Sets the position of the X-axis label.             |
| getXAxisLabelPosition | Obtains the position of the X-axis label.             |
| setCustomLabels       | Sets custom labels for the X-axis.         |
| getCustomLabels       | Obtains custom labels.            |
| setDrawCustomLabels   | Sets whether to draw custom labels.    |
| isDrawCustomLabels    | Checks whether custom labels are drawn.|

**1.setPosition**

public setPosition(pos: XAxisPosition): void;

Sets the position of the X-axis.

Parameters

| Name| Type         | Mandatory| Description             |
| ------ | ------------- | ---- | ----------------- |
| pos    | XAxisPosition | Yes  | Position of the X-axis.|

**2.getPosition**

public getPosition(): XAxisPosition;

Obtains the position of the X-axis.

Return Value

| Type         | Description             |
| ------------- | ----------------- |
| XAxisPosition | Position of the X-axis.|

**3.setLabelRotationAngle**

public setLabelRotationAngle(angle: number): void;

Sets the rotation angle of the X-axis label.

Parameters

| Name| Type  | Mandatory| Description                 |
| ------ | ------ | ---- | --------------------- |
| angle  | number | Yes  | Rotation angle of the X-axis label.|

**4.getLabelRotationAngle**

public getLabelRotationAngle(): number;

Obtains the rotation angle of the X-axis label.

Return Value

| Type  | Description                 |
| ------ | --------------------- |
| number | Rotation angle of the X-axis label.|

**5.setLabelXOffset**

public setLabelXOffset(xOffset: number): void

Sets the offset of the X-axis label.

Return value: none

Parameters

| Name| Type  | Mandatory| Description        |
| ------ | ------ | ---- |------------|
| xOffset  | number | Yes  | Offset of the X-axis label.|

**6.getLabelXOffset**

public getLabelXOffset(): number

Obtains the offset of the X-axis label.

Return Value

| Type  | Description                 |
| ------ | --------------------- |
| number | Obtains the offset of the X-axis label.|

Parameter: none

**7.setXAxisLabelPosition**

public setXAxisLabelPosition(position: XAxisLabelPosition): void

Sets the position of the X-axis label.

Return value: none

Parameters

| Name| Type  | Mandatory| Description        |
| ------ | ------ | ---- |------------|
| position  | XAxisLabelPosition | Yes  | Position of the X-axis label.|

**8.getXAxisLabelPosition**

public getXAxisLabelPosition(): XAxisLabelPosition

Obtains the position of the X-axis label.

Return Value

| Type  | Description                 |
| ------ | --------------------- |
| XAxisLabelPosition | Position of the X-axis label.|

Parameter: none


**9.setCustomLabels**

public setCustomLabels(numbers: number[]): void

Sets custom labels for the X-axis.

Return value: none

Parameters

| Name| Type  | Mandatory| Description        |
| ------ | ------ | ---- |------------|
| numbers  | number[] | Yes  | Custom labels for the X-axis.|

**10.getCustomLabels**

public getCustomLabels(): number[]

Obtains custom labels.

Return Value

| Type  | Description                 |
| ------ | --------------------- |
| number[] | Custom Labels.|

Parameter: none

**11.setDrawCustomLabels**

public setDrawCustomLabels(flag: boolean): void

Sets whether to draw custom labels.

Return value: none

Parameters

| Name| Type     | Mandatory| Description        |
| ------ |---------| ---- |------------|
| flag  | boolean | Yes  | Whether to draw custom labels.|

**12.isDrawCustomLabels**

public isDrawCustomLabels(): boolean

Checks whether custom labels are drawn.

Return Value

| Type  | Description                 |
| ------ | --------------------- |
| boolean | Whether custom labels are drawn.|

Parameter: none


**6. YAxis**

Defines an Y-axis object.

| API               | Description                                                        |
| --------------------- | ------------------------------------------------------------ |
| calculate             | Calculates the expected maximum and minimum values on the axis.                                |
| getAxisDependency     | Obtains the position of the Y-axis.                                               |
| setPosition           | Sets the position of the Y-axis label.                                         |
| getLabelPosition      | Obtains the position of the Y-axis label.                                         |
| setLabelXOffset       | Sets the offset of the Y-axis label to the X-axis.                            |
| getLabelXOffset       | Obtains the offset of the Y-axis label to the X-axis.                            |
| setDrawZeroLine       | Sets whether to draw the 0 line.                                           |
| EventControl          | Defines the event control class. EventType is of the enum class (**SingleTap**, **DoubleTap**, and **LongPress**).          |
| setEventEnable        | Enables an event.                                                  |
| setEventDisable       | Disable an event.                                                  |
| eventIsEnable         | Checks whether an event is enabled.                                              |
| eventIsDisable        | Checks whether an event is disabled.                                            |
| setYAxisExtensionLine | Sets the extension line of the Y-axis.                                             |
| addGridLine           | Adds a custom gridline.                                          |
| getGridLines          | Obtains a custom gridline.                                          |
| setGridAlpha          | Sets the alpha of the gridline color. (The custom gridline added by **addGridLine** is not included. The color with alpha can be passed to the custom gridline through the **color** property.)|

**1.calculate**

public calculate(dataMin: number, dataMax: number): void;

Calculates the expected maximum and minimum values on the axis.

Parameters

| Name | Type  | Mandatory| Description                 |
| ------- | ------ | ---- | --------------------- |
| dataMin | number | Yes  | Minimum value on the Y-axis in a dataset.|
| dataMax | number | Yes  | Maximum value on the Y-axis in a dataset.  |

**2.getAxisDependency**

public getAxisDependency(): AxisDependency;

Obtains the position of the Y-axis.

Return Value

| Type  | Description         |
| ------ | ------------- |
| number | Obtains the position of the Y-axis.|

**3.setPosition**

public setPosition(pos: YAxisLabelPosition): void;

Sets the position of the Y-axis label.

Parameters

| Name| Type              | Mandatory| Description               |
| ------ | ------------------ | ---- | ------------------- |
| pos    | YAxisLabelPosition | Yes  | Position of the Y-axis label.|

**4.getLabelPosition**

public getLabelPosition(): YAxisLabelPosition;

Obtains the position of the Y-axis label.

Return Value

| Type              | Description             |
| ------------------ | ----------------- |
| YAxisLabelPosition | Position of the Y-axis label.|

**5.setLabelXOffset**

public setLabelXOffset(xOffset: number): void;

Sets the offset of the Y-axis label to the X-axis.

Parameters

| Name | Type  | Mandatory| Description                            |
| ------- | ------ | ---- | -------------------------------- |
| xOffset | number | Yes  | Offset of the Y-axis label to the X-axis.|

**6.getLabelXOffset**

public getLabelXOffset(): number;

Obtains the offset of the Y-axis label to the X-axis.

Return Value

| Type  | Description                            |
| ------ | -------------------------------- |
| number | Offset of the Y-axis label to the X-axis.|

**7.setDrawZeroLine**

public setDrawZeroLine(mDrawZeroLine: boolean): void;

Sets whether to draw the 0 line.

Parameters

| Name       | Type   | Mandatory| Description         |
| ------------- | ------- | ---- | ------------- |
| mDrawZeroLine | boolean | Yes  | Whether to draw 0 line.|

**8. EventControl**

Defines the event control class. The value of **EventType** is of the enum class, including **SingleTap**, **DoubleTap**, and **LongPress**.

enum EventType {
SingleTap,
DoubleTap,
LongPress
}

**9.setEventEnable**

public setEventEnable(evType: EventType): EventControl

Enables an event.

Parameters

| Name| Type              | Mandatory| Description                               |
| ------ | ------------------ | ---- |-----------------------------------|
| evType    | EventType | Yes  | **SingleTap**, **DoubleTap**, and **LongPress**.|

**10.setEventDisable**

public setEventDisable(evType: EventType): EventControl

Disable an event.

Parameters

| Name| Type              | Mandatory| Description                               |
| ------ | ------------------ | ---- |-----------------------------------|
| evType    | EventType | Yes  | **SingleTap**, **DoubleTap**, and **LongPress**.|

**11.eventIsEnable**

public eventIsEnable(evType: EventType): boolean; 

Checks whether an event is enabled.

Parameters

| Name| Type              | Mandatory| Description                               |
| ------ | ------------------ | ---- |-----------------------------------|
| evType    | EventType | Yes  | **SingleTap**, **DoubleTap**, and **LongPress**.|

**12.eventIsDisable** 

public eventIsDisable(evType: EventType): boolean;

Checks whether an event is disabled.

Parameters

| Name| Type              | Mandatory| Description                                |
| ------ | ------------------ | ---- |------------------------------------|
| evType    | EventType | Yes  | **SingleTap**, **DoubleTap**, and **LongPress**. |

**13.setYAxisExtensionLine**

public setYAxisExtensionLine(opt: boolean): void

Sets the extension line of the Y-axis.

Parameters

| Name| Type     | Mandatory| Description        |
|-----|---------| ---- |------------|
| opt | boolean | Yes  | Whether to draw the extension line of the Y-axis.|

**14.addGridLine**

public addGridLine(customLine: GridLineConfig): void

Adds a custom gridline.

Parameters

| Name| Type     | Mandatory| Description      |
|-----|---------| ---- |----------|
| customLine | GridLineConfig | Yes  | Gridline style.|

**15.getGridLines**

public getGridLines(): JArrayList<LimitLine>

Obtains a custom gridline.

Return Value

| Type                   | Description      |
|-----------------------|----------|
| JArrayList<LimitLine> | Custom gridline array.|

**16.setGridAlpha**

public setGridAlpha(alpha: number): void

Sets the alpha of the gridline. (The custom gridline added by **addGridLine** is not included. The color with alpha can be passed to the custom gridline through the **color** property.)

Parameters

| Name  | Type    | Mandatory | Description                                    |
|-------|--------|-----|----------------------------------------|
| alpha | number | Yes  | Gridline opacity. The value ranges from 0 to 255. The value 0 indicates that the gridline is completely transparent, and the value 255 indicates that the gridline is completely opaque.|

```
// For the sample code, see CustomGridLineChartPage.
export interface GridLineConfig {
   position: number;
   lineWidth?: number;
   lineColor?: number;
   dashPathEffect?: DashPathEffectConfig
}

export interface DashPathEffectConfig {
   lineLength: number;
   spaceLength: number;
   phase: number
}
```

**6. Passing Custom MarkerView**
```typescript

@State customUiInfo: CustomUiInfo = new CustomUiInfo(90, 50);
        
@Builder customUi() {
   // Set whether to place the custom UI in the chart content.
   if (this.customUiInfo.isInbounds && this.customUiInfo.data) {
      Column() {
        Text(`2023-12-15`).fontColor(Color.Gray).fontSize(12).fontWeight(FontWeight.Bold)
        Text(`X: ${this.customUiInfo.data.getX()}`).fontColor(Color.Gray).fontSize(12)
        Text(`Y: ${this.customUiInfo.data.getY()}`).fontColor(Color.Gray).fontSize(12)
      }
   .padding(4)
      .borderRadius(6)
      .border({ width: 1, color: Color.Orange})
      .backgroundColor(0xf0f0f0)
      .width(this.customUiInfo.width)
      .height(this.customUiInfo.height)
      .margin({ left: this.customUiInfo.x, top: this.customUiInfo.y })
      .alignItems(HorizontalAlign.Start)
      .onClick(ev => {
         this.customUiInfo.showUi = false;
      })
   }
}


BarChart({ model: this.model,
   // Pass the custom UI to the builder.
   customUiBuilder: this.customUi,
   // Set the event type to trigger the custom UI.
   customUiTriggerEvent: EventType.SingleTap,
   // Set the location information of the custom UI.
   customUiInfo: this.customUiInfo,
})
```
**7. WaterfallDataSet**

Defines the data class of the waterfall chart.

| API       | Description                        |
| ------------- | ---------------------------- |
| setDotsColors | Sets the color of all highlighted points in the waterfall chart.|
| getDotsColors | Obtains the color of all highlighted points in the waterfall chart.|

**1.setDotsColors**

public setDotsColors(color: number | string): void;

Sets the color of all highlighted points in the waterfall chart.

Parameters

| Name| Type                  | Mandatory| Description                      |
| ------ |----------------------| ---- |--------------------------|
| color  | number &#124; string | Yes  | Color to set. The value can be a number or string.|

**2.getDotsColors**

public getDotsColors(): Array<string | number> {

Obtains the color of all highlighted points in the waterfall chart.

Return Value

| Type                         | Description                       |
|-----------------------------| --------------------------- |
| Array<string &#124; number> | Array of colors of all highlighted points.|

**8. ChartData**

Defines the chart data class.

| API                 | Description                                   |
| ----------------------- | --------------------------------------- |
| setHighlightByXDistance | Sets whether to highlight a point based only on its distance to the X-axis.|
| isHighlightByXDistance  | Checks whether the point is highlighted based only on its distance to the X-axis.|

**1.setHighlightByXDistance**

public setHighlightByXDistance(highlightByXDistance: boolean): void;

Sets whether to highlight a point based only on its distance to the X-axis.

Parameters

| Name| Type     | Mandatory| Description                      |
| ------ |---------| ---- |--------------------------|
| highlightByXDistance  | boolean | Yes  | Whether to highlight a point based only on its distance to the X-axis.|

**2.isHighlightByXDistance**

public isHighlightByXDistance(): boolean

Checks whether the point is highlighted based only on its distance to the X-axis.

Return Value

| Type     | Description    |
|---------|--------|
| boolean | Whether the point is highlighted based only on its distance to the X-axis.|

**9. BarLineScatterCandleBubbleDataSet**

Defines the abstract base class for the data of bar chart, line chart, scatter chart, candle chart, and bubble chart.

**1.setHighLightColor**

public setHighLightColor(color: number): void;

Sets the color of the highlight indicator.

Parameters

| Name| Type  | Mandatory| Description            |
| ------ | ------ | ---- | ---------------- |
| color  | number | Yes  | Color of the highlight indicator.|

**2.getHighLightColor**

public getHighLightColor(): number;

Obtains the color of the highlight indicator.

Return Value

| Type  | Description            |
| ------ | ---------------- |
| number | Color of the highlight indicator.|

**3.copyTo**

protected copyTo(barLineScatterCandleBubbleDataSet: BarLineScatterCandleBubbleDataSet<T>): void

Copies data to a specified dataset.

Parameters

| Name                           | Type                                | Mandatory| Description                   |
| --------------------------------- | ------------------------------------ | ---- | ----------------------- |
| barLineScatterCandleBubbleDataSet | BarLineScatterCandleBubbleDataSet<T> | Yes  | Data copied to a specified dataset.|

**10. LineScatterCandleRadarDataSet**

**1.setDrawHorizontalHighlightIndicator**

public setDrawHorizontalHighlightIndicator(enabled: boolean): void;

Sets whether to enable the horizontal highlight indicator.

Parameters

| Name | Type   | Mandatory| Description                                                   |
| ------- | ------- | ---- | ------------------------------------------------------- |
| enabled | boolean | Yes  | Whether to enable the horizontal highlight indicator. If the value is **false**, the indicator is not drawn.|

**2.setDrawVerticalHighlightIndicator**

public setDrawVerticalHighlightIndicator(enabled: boolean): void;

Sets whether to enable the vertical highlight indicator.

Parameters

| Name | Type   | Mandatory| Description                                                   |
| ------- | ------- | ---- | ------------------------------------------------------- |
| enabled | boolean | Yes  | Whether to enable the vertical highlight indicator. If the value is **false**, the indicator is not drawn.|

**3.setDrawHighlightIndicators**

public setDrawHighlightIndicators(enabled: boolean): void;

Sets whether to enable the horizontal and vertical highlight indicators.

Parameters

| Name | Type   | Mandatory| Description                               |
| ------- | ------- | ---- | ----------------------------------- |
| enabled | boolean | Yes  | Whether to enable the horizontal and vertical highlight indicators.|



**4.isVerticalHighlightIndicatorEnabled**

public isVerticalHighlightIndicatorEnabled(): boolean;

Obtains whether the vertical highlight indicator is enabled.

Return Value

| Type   | Description                        |
| ------- | ---------------------------- |
| boolean | Whether the vertical highlight indicator is enabled.|

**5.isHorizontalHighlightIndicatorEnabled**

public isVerticalHighlightIndicatorEnabled(): boolean;

Obtains whether the horizontal highlight indicator is enabled.

Return Value

| Type   | Description                        |
| ------- | ---------------------------- |
| boolean | Whether the horizontal highlight indicator is enabled.|

**6.setHighlightLineWidth**

public setHighlightLineWidth(width: number): void;

Sets the width of the highlight line, in vp.

Parameters

| Name| Type  | Mandatory| Description                        |
| ------ | ------ | ---- | ---------------------------- |
| width  | number | Yes  | Width of the highlight line, in vp.|

**7.getHighlightLineWidth**

public getHighlightLineWidth(): number;

Obtains the width of the highlight line.

Return Value

| Type  | Description          |
| ------ | -------------- |
| number | Width of the highlight line.|

**8.enableDashedHighlightLine**

public enableDashedHighlightLine(lineLength: number, spaceLength: number, phase: number): void;

Enables the dashed highlight line.

Parameters

| Name     | Type  | Mandatory| Description                             |
| ----------- | ------ | ---- | --------------------------------- |
| lineLength  | number | Yes  | Length of a line segment.                     |
| spaceLength | number | Yes  | Length of the space.                     |
| phase       | number | Yes  | Phase offset, in degree. Generally, degree (°) is used.|

**9.disableDashedHighlightLine**

public disableDashedHighlightLine(): void;

Disables the dashed highlight line.

No return value

**10.isDashedHighlightLineEnabled**

public isDashedHighlightLineEnabled(): boolean;

Returns **true** if the dashed highlight line is enabled. Otherwise, returns **false**.

Return Value

| Type   | Description                    |
| ------- | ------------------------ |
| boolean | Whether the dashed highlight line is enabled.|

**11.getDashPathEffectHighlight**

public getDashPathEffectHighlight(): DashPathEffect;

Obtains the dash path effect.

Return Value

| Type          | Description    |
| -------------- | -------- |
| DashPathEffect | Dash path effect.|

**12.copyTo**

protected copyTo(lineScatterCandleRadarDataSet: LineScatterCandleRadarDataSet<T>): void;

Copies data to a specified dataset.

Parameters

| Name                       | Type                         | Mandatory| Description       |
| ----------------------------- | ----------------------------- | ---- | ----------- |
| lineScatterCandleRadarDataSet | LineScatterCandleRadarDataSet | Yes  | Dataset.|

## FAQs

**1. How to remove the legend?**

```typescript
this.model.getLegend()?.setEnabled(false);
```

**2. How to remove the description in the lower right corner?**

```typescript
this.model.getDescription()?.setEnabled(false);
```

**3. How to hide the Y-axis?**

```typescript
this.model.getAxisLeft().setEnabled(false) // Hide the left Y-axis. In this case, the label is also hidden.
```

**4. How to hide the axis but display the label?**

```typescript
this.model.getAxisRight().setDrawAxisLine(false);
```

**5. How to set the number of Y-Axis Labels?**

```typescript
this.model.getAxisLeft().setLabelCount(8, false);// Set eight labels.
```

**6. How to set the axis color and width?**

```typescript
let leftAxis = this.model.getAxisLeft();
leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);// If the axis is inside the chart, use INSIDE_CHART.
this.leftAxis.setAxisLineColor(ColorTemplate.rgb("#ff0000"));// Set the axis color.
leftAxis.setAxisLineWidth(1);// Set the axis width.
leftAxis.setTextSize(20);// Set the font size of the Y-axis label.
leftAxis.setDrawGridLines(true);// Set to enable the gridlines.
```

**7. How to customize axis labels?**

If you do not want to use the Arabic numeral label of the axis, you can customize the axis label by creating a custom class to implement the **IAxisValueFormatter** API, modifying **getFormattedValue()**, and then calling **setValueFormatter()** of the axis object.

```typescript
class MyAxisValueFormatter implements IAxisValueFormatter {
  getFormattedValue(value: number, axis: AxisBase): string {
    // Convert the original value to a string as required.
    switch (value) {
      case 1:
        return "one";
      case 2:
        return "two";
      case 3:
        return "three";
    }
    return '';
  }
}
...
this.topAxis.setValueFormatter(new TopAxisValueFormatter())
```

**8. How to enable or disable the interactive settings such as zooming and touching a chart?**

```typescript
setTouchEnabled(enabled: boolean)// Set whether to enable all touch interactions with a chart.
setDragEnabled(enabled: boolean)// Set whether to enable the dragging function.
setScaleEnabled(enabled: boolean)// Set whether to enable scaling in all directions.
setScaleXEnabled(enabled: boolean)// Set whether to enable scaling on the X-axis.
setScaleXEnabled(enabled: boolean)// Set whether to enable scaling on the Y-axis.
setPinchZoom(enabled: boolean)// Set whether to enable the pinch zooming function. If the value is true, the pinch zooming function is enabled. If false, the pinch zooming function is disabled.
setHighlightPerTapEnabled(enabled: boolean)// Set whether to enable highlight effect for tapping events. If this value is set to true, the highlight effect for tapping events is enabled.
setHighlightPerDragEnabled(enabled: boolean)// Set whether to enable highlight effect for dragging events. If the value is set to true, the highlight effect is displayed when the dragging ends. The default value is true.
setHighlightIndicatorEnabled(enabled: boolean)// Set whether to enable the highlight indicator. If the value is set to true, the indicator line is displayed when data is selected.
// This function is used to set the dataset.
setVisibleXRangeMaximum (maxXRange: number) // Set the maximum number of visible data on the X-axis. (This function must be called after the data source is set. Otherwise, this function is invalid.)
```

**9. How to customize the axis styles?**

1) Obtain the X-axis or Y-axis object.

```
// Obtain the X-axis.
let xAxis = model.getXAxis();
// Obtain the left Y-axis.
let leftAxis = model.getAxisLeft();
// Obtain the right Y-axis.
let rightAxis = model.getAxisRight();
```

2) After obtaining the x-axis and y-axis objects, call the following functions to set their properties.

```typescript
setEnabled (enabled: boolean)// Set whether to draw the axis. The axis is drawn by default. If the value is set to false, the axis is not drawn.
setDrawLabels (enabled: boolean)// Set whether to draw the axis labels. If the value is set to true, the axis labels are drawn.
setDrawAxisLine (enabled: boolean)// Set whether to draw the axis line. If the value is set to true, the axis line is drawn.
setDrawGridLines (enabled: boolean)// Set whether to draw the gridlines. If the value is set to true, the gridlines are drawn.
setTextColor(color: string | number | CanvasGradient | CanvasPattern)// Set the text color of the axis label.
setTextSize (size: number)// Set the text size of the axis label.
setTypeface (tf: FontFamily)// Set the font family of the axis label and specify the typeface. The supported types are 'sans-serif', 'serif', and 'monospace'.
setGridColor (color: number)// Set the grid color.
setGridLineWidth (width: number)// Set the width of the gridline.
setAxisLineColor (color: number)// Set the color of the axis.
setAxisLineWidth (width: number)// Set the width of the axis.
enableGridDashedLine(lineLength: number, spaceLength: number, phase: number)// Set the dashed style of the gridline. lineLength specifies the length of the dashed line, spaceLength specifies the interval between two lines, and phase specifies the start point.
setAxisMaxValue (max: number)// Set the custom maximum value. If this value is set, it will not be automatically calculated based on the provided data.
setAxisMinValue (min: number)// Set the custom minimum value. If this value is set, it will not be automatically calculated based on the provided data.
```

(3) Set the X-axis.

```typescript
setAvoidFirstLastClipping (enabled: boolean)// Set whether to prevent the first and last labels from being clipped. If the value is set to true, the labels on the edge of the chart or screen will not be clipped.
setPosition (pos: XAxisPosition)// Set the position of the X-axis. The value can be TOP, BOTTOM, BOTH\_SIDED, TOP\_INSIDE, or BOTTOM\_INSIDE.
```

(4) Set the Y-axis.

```typescript
setInverted (enabled: boolean)// Set whether to invert the Y-axis. If the value is set to true, the axis is reversed, indicating that the maximum value is placed at the bottom and the minimum value is placed at the top.
setSpaceTop (percent: number)// Set the top space (the percentage of the total axis range occupied by the space between the highest value on the axis and the highest value on the chart).
setSpaceBottom (percent: number)// Set the bottom space (the percentage of the total axis range occupied by the space between the lowest value on the axis and the lowest value on the chart).
setPosition (pos: YAxisLabelPosition)// Set the position of the axis label. The value can be INSIDE\_CHART or OUTSIDE\_CHART.
```

## Common Custom Charts

### **1. A Line Chart with the Dashed Line Connected to the Solid Line**

The reference code is in **entry/src/main/ets/pages/customCharts/DashedSolidLinePage**.

mpchart does not support the curves that are connected by dashed and solid lines. The curves must be solid lines or dashed lines. If half of the curves are dashed lines and the other half are solid lines, you need to define a function to draw the curves.

First, create the **MyLineDataSet** class that inherited from **LineDataSet** (data class of the line chart).  Since the start point of the solid line after the dashed line need to be defined when initializing data. The **MyLineDataSet** class is necessary because it has the **interval** parameter that its parent class does not have, and this parameter specifies the separation point between the dashed line and the solid line.

```typescript
import { EntryOhos, JArrayList, LineDataSet } from '@ohos/mpchart';
 
export class MyLineDataSet extends LineDataSet {
  interval: number = 0;
  constructor(yVals: JArrayList<EntryOhos> | null, label: string, interval: number) {
    super(yVals, label);
    this.interval = interval;
  }
}
```

Then, define the **MyRender** class to draw solid lines. The **MyRender** class inherits from the **LineChartRenderer** class. To draw curves, you need to override **drawCubicBezier**. The code of the **MyRender** class is as follows:

```typescript
import { EntryOhos, ILineDataSet, Style, Transformer, Utils, LineChartRenderer } from '@ohos/mpchart';
import { MyLineDataSet } from './MyLineDataSet';
 
export default class MyRender extends LineChartRenderer{
  protected drawCubicBezier(c: CanvasRenderingContext2D, dataSet: MyLineDataSet) {
    if(dataSet.interval == undefined){
      super.drawCubicBezier(c, dataSet);
      return;
    }
    if (!this.mChart || !this.mXBounds) {
      return;
    }
    const phaseY: number = this.mAnimator ? this.mAnimator.getPhaseY() : 1;
    const trans: Transformer | null = this.mChart.getTransformer(dataSet.getAxisDependency());
 
    this.mXBounds.set(this.mChart, dataSet);
 
    const intensity: number = dataSet.getCubicIntensity();
 
    let cubicPath = new Path2D();
    // Solid line.
    let solidLinePath = new Path2D();
    // Dashed line.
    let dashedLinePath = new Path2D();
    if (this.mXBounds.range >= 1) {
      let prevDx: number = 0;
      let prevDy: number = 0;
      let curDx: number = 0;
      let curDy: number = 0;
 
      // Take an extra point from the left, and an extra from the right.
      // That's because we need 4 points for a cubic bezier (cubic=4), otherwise we get lines moving and doing weird stuff on the edges of the chart.
      // So in the starting `prev` and `cur`, go -2, -1
      // And in the `lastIndex`, add +1
 
      const firstIndex: number = this.mXBounds.min + 1;
      const lastIndex: number = this.mXBounds.min + this.mXBounds.range;
 
      let prevPrev: EntryOhos | null;
      let prev: EntryOhos | null = dataSet.getEntryForIndex(Math.max(firstIndex - 2, 0));
      let cur: EntryOhos | null = dataSet.getEntryForIndex(Math.max(firstIndex - 1, 0));
      let next: EntryOhos | null = cur;
      let nextIndex: number = -1;
 
      if (cur === null) return;
 
      Utils.resetContext2DWithoutFont(c, this.mRenderPaint);
      // let the spline start
      cubicPath.moveTo(cur.getX(), cur.getY() * phaseY);
      solidLinePath.moveTo(cur.getX(), cur.getY() * phaseY);
 
      for (let j: number = this.mXBounds.min + 1; j <= this.mXBounds.range + this.mXBounds.min; j++) {
        prevPrev = prev;
        prev = cur;
        cur = nextIndex === j ? next : dataSet.getEntryForIndex(j);
 
        nextIndex = j + 1 < dataSet.getEntryCount() ? j + 1 : j;
        next = dataSet.getEntryForIndex(nextIndex);
 
        prevDx = (cur.getX() - prevPrev.getX()) * intensity;
        prevDy = (cur.getY() - prevPrev.getY()) * intensity;
        curDx = (next.getX() - prev.getX()) * intensity;
        curDy = (next.getY() - prev.getY()) * intensity;
 
        cubicPath.bezierCurveTo(
          prev.getX() + prevDx,
          (prev.getY() + prevDy) * phaseY,
          cur.getX() - curDx,
          (cur.getY() - curDy) * phaseY,
          cur.getX(),
          cur.getY() * phaseY
        );
        if(j <= dataSet.interval){
          solidLinePath.bezierCurveTo(
            prev.getX() + prevDx,
            (prev.getY() + prevDy) * phaseY,
            cur.getX() - curDx,
            (cur.getY() - curDy) * phaseY,
            cur.getX(),
            cur.getY() * phaseY
          );
          if(j == dataSet.interval) {
            dashedLinePath.moveTo(cur.getX(),
              cur.getY() * phaseY);
          }
        }else{
          dashedLinePath.bezierCurveTo(
            prev.getX() + prevDx,
            (prev.getY() + prevDy) * phaseY,
            cur.getX() - curDx,
            (cur.getY() - curDy) * phaseY,
            cur.getX(),
            cur.getY() * phaseY
          );
        }
      }
    }
 
    // if filled is enabled, close the path
    if (dataSet.isDrawFilledEnabled()) {
      let cubicFillPath: Path2D = new Path2D();
      // cubicFillPath.reset();
      cubicFillPath.addPath(cubicPath);
 
      if (c && trans) {
        this.drawCubicFill(c, dataSet, cubicFillPath, trans, this.mXBounds);
      }
    }
 
    this.mRenderPaint.setColor(dataSet.getColor());
    this.mRenderPaint.setStyle(Style.STROKE);
 
    if (trans && trans.pathValueToPixel(cubicPath)) {
      cubicPath = trans.pathValueToPixel(cubicPath);
      solidLinePath = trans.pathValueToPixel(solidLinePath);
      dashedLinePath = trans.pathValueToPixel(dashedLinePath);
    }
 
    Utils.resetContext2DWithoutFont(c, this.mRenderPaint);
    c.beginPath();
    c.stroke(solidLinePath);
    c.closePath();
 
    Utils.resetContext2DWithoutFont(c, this.mRenderPaint);
    c.beginPath();
    c.setLineDash([5,5,0]);
    c.stroke(dashedLinePath);
    c.closePath();
    this.mRenderPaint.setDashPathEffect(null);
  }
 
}
```

This function defines two **path2D**, that is, line segments, to draw the solid line and dashed line.

```typescript
// Solid line.
let solidLinePath = new Path2D();
// Dashed line.
let dashedLinePath = new Path2D();
```

The drawing function is as follows:

```typescript
solidLinePath.bezierCurveTo(
  prev.getX() + prevDx,
  (prev.getY() + prevDy) * phaseY,
  cur.getX() - curDx,
  (cur.getY() - curDy) * phaseY,
  cur.getX(),
  cur.getY() * phaseY
);
```

Call **bezierCurveTo** of **path2D**. This function provides six parameters, including the x value and y value of control point 1, the x value and y value of control point 2, and the x value and y value of the target point. This function can be copied from the parent class.

Define **If (j <= dataSet.interval)**. This if judgment specifies that when **j** is less than **dataSet.interval**, the solid line is drawn; when **j** is equal to **dataSet.interval**, the dashed line is moved to the current position; and when **j** is greater than **dataSet.interval**, **dashedLinePath.bezierCurveTo** is called to draw the dashed line.

Call the **c.stroke** function. **c.setLineDash ([5,5,0])** is used to set the dashes line style.

```typescript
 Utils.resetContext2DWithoutFont(c, this.mRenderPaint);
    c.beginPath();
    c.stroke(solidLinePath);
    c.closePath();
 
    Utils.resetContext2DWithoutFont(c, this.mRenderPaint);
    c.beginPath();
    c.setLineDash([5,5,0]);
    c.stroke(dashedLinePath);
    c.closePath();
```

 The code for the last step is as follows:

```typescript
import {
  JArrayList,EntryOhos,ILineDataSet,LineData,LineChart,LineChartModel,
  Mode,
} from '@ohos/mpchart';
import { MyLineDataSet } from './MyLineDataSet';
import MyRender from './MyRender';
import data from '@ohos.telephony.data';
 
@Entry
@Component
struct Index {
  private model: LineChartModel = new LineChartModel();
 
  aboutToAppear() {
    // Create a JArrayList object to store data of the EntryOhos class.
    let values: JArrayList<EntryOhos> = new JArrayList<EntryOhos>();
    // Generate random data from 1 to 20 cyclically and add the data to values.
    for (let i = 1; i <= 20; i++) {
      values.add(new EntryOhos(i, Math.random() * 100));
    }
    // Create a LineDataSet object, use the values data, and set the dataset name to 'DataSet'.
    let dataSet = new MyLineDataSet(values, 'DataSet', 6);
    dataSet.setMode(Mode.CUBIC_BEZIER);
    dataSet.setDrawCircles(false);
    dataSet.setColorByColor(Color.Blue)
    let dataSetList: JArrayList<ILineDataSet> = new JArrayList<ILineDataSet>();
    dataSetList.add(dataSet);
    // Create a LineData object to hold dataSetList, and transfer the data to the model.
    let lineData: LineData = new LineData(dataSetList);
    this.model?.setData(lineData);
    this.model.setRenderer(new MyRender(this.model, this.model.getAnimator()!, this.model.getViewPortHandler()))
 
  }
 
  build() {
    Column() {
      LineChart({ model: this.model })
        .width('100%')
        .height('100%')
        .backgroundColor(Color.White)
    }
  }
}
```

### 2. A chart with the left Y-axis displaying values and the right Y-axis displaying percentages.

The reference code is in **entry/src/main/ets/pages/customCharts/LeftRightAxisPage.ets**.

To change the values on the right Y-axis to percentages and retain the values on the left Y-axis, you need to customize **RightAxisFormatter** to implement the **IAxisValueFormatter** API. The code of the **RightAxisFormatter** class is as follows:

```typescript
class RightAxisFormatter implements IAxisValueFormatter {
  maxNumber: number = 0;
  constructor(maxNumber: number) {
    this.maxNumber = maxNumber;
  }
  getFormattedValue(value: number, axis: AxisBase): string {
    switch (value) {
      case this.maxNumber:
        return "100%";
      case this.maxNumber * 4 / 5:
        return "80%";
      case this.maxNumber * 3 / 5:
        return "60%";
      case this.maxNumber * 2 / 5:
        return "40%";
      case this.maxNumber * 1 / 5:
        return "20%";
      case 0:
        return "0%";
    }
    return "";
  }
}
```

Use the following function:

```typescript
// Set the label text formatter.
rightAxis?.setValueFormatter(new RightAxisFormatter(this.maxNumber));
```

### 3. A Chart with Short Scale Lines

The reference code is in **entry/src/main/ets/pages/customCharts/ScaleLinePage**.

To draw short scale lines for the X-axis, you can modify the original gridlines to short lines based on the existing APIs.

Specifically, you can create the **MyXAxisRender** class that inherited from the **XAxisRenderer** class, and and override **drawGridLine()** of the parent class.

The code is as follows:

```typescript
import { Utils, XAxisRenderer } from '@ohos/mpchart';
 
export default class MyXAxisRender extends XAxisRenderer{
  protected drawGridLine(c: CanvasRenderingContext2D, x: number, y: number): void {
    Utils.resetContext2DWithoutFont(c, this.mGridPaint);
    c.beginPath();
    let bottom = this.mViewPortHandler?.contentBottom()??0;
    c.moveTo(x, bottom);
    c.lineTo(x, bottom - 10);
    c.stroke();
    c.closePath();
  }
}
```

**c.moveTo (x, bottom)** and **c.lineTo (x, bottom-10)** are important because they determine the position and length of the scale line. That is, a 10vp line is drawn from the bottom of the chart to the top.

In this case, use the following code to change the drawing class of the X-axis to the custom drawing class **MyXAxisRender**:

```typescript
this.model.setXAxisRenderer(new MyXAxisRender(this.model.getViewPortHandler(), this.model.getXAxis()!, this.model.getTransformer(AxisDependency.LEFT)!));
```

### 4. A Chart with a Gradient Line Based on the Scale of the Y-Axis

The reference code is in **entry/src/main/ets/pages/customCharts/GradientLinePage**.

To draw gradient lines, you need to modify the drawing effects of the left Y-axis and the data line components. That is, the **YAxisRenderer** and **LineChartRenderer** classes.

To modify the drawing effect of the left Y-axis, you need to create the **MyAxisRender** class that inherited from the **YAxisRenderer** class and modify **renderAxisLine()** . Set **strokeStyle** of the canvas to the gradient effect created by using **createLinearGradient** and apply the **strokeStyle** to the Y-axis. In this way, the left Y-axis presents a gradient effect.

Then, create the **MyDataRender** class that inherited from the **LineChartRenderer** class and modify **drawCubicBezier()** to modify the drawing effect of the data line. Create a vertical gradient effect, and then apply the gradient effect to the stroke style of the data line.



## Constraints

This project has been verified in the following versions:
- DevEco Studio: 5.0 (5.0.3.500), SDK: API 12 (5.0.0.25)

## Directory Structure

````
|---- ohos-MPChart
|     |---- entry  # Sample code
|     |---- library  # mpchart library 
|           |---- index.ets  # External APIs
│           ├----components # Framework code
│                 ├----animation # Animation code
│                 │      
│                 ├----buffer # Buffers
│                 │      
│                 ├----chartcomponents # Chart components
│                 │      
│                 ├----charts # Chart models
│                 │      
│                 ├----components   # Custom components
│                 │          
│                 ├----data   # Data entities
│                 │      
│                 ├----formatter # Data formatters
│                 │      
│                 ├----highlight # Highlight components
│                 │      
│                 ├----interfaces   # External APIs
│                 │          
│                 ├----jobs   # Animation working threads
│                 │      
│                 ├----listener  # Gesture listeners
│                 │      
│                 ├----renderer  # Chart renderers
│                 │          
│                 └----utils  # Utility classes
│                  
└─resources # Resource files
|     |---- README.md  # Readme 

````

## How to Contribute
If you find any problem during the use, submit an [Issue](https://gitee.com/openharmony-sig/ohos-MPChart/issues) or [PR](https://gitee.com/openharmony-sig/ohos-MPChart/pulls) to us.

## License
This project is licensed under [Apache License 2.0](https://gitee.com/openharmony-sig/ohos-MPChart/blob/master/LICENSE).
