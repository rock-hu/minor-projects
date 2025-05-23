/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import EntryOhos from '../data/EntryOhos';
import ViewPortHandler from '../utils/ViewPortHandler';

/**
 * Interface that allows custom formatting of all values inside the chart before they are
 * being drawn to the screen. Simply create your own formatting class and let
 * it implement IValueFormatter. Then override the getFormattedValue(...) method
 * and return whatever you want.
 *
 *
 */
export default interface IValueFormatter {

  /**
   * Called when a value (from labels inside the chart) is formatted
   * before being drawn. For performance reasons, avoid excessive calculations
   * and memory allocations inside this method.
   *
   * @param value           the value to be formatted
   * @param entry           the entry the value belongs to - in e.g. BarChart, this is of class BarEntry
   * @param dataSetIndex    the index of the DataSet the entry in focus belongs to
   * @param viewPortHandler provides information about the current chart state (scale, translation, ...)
   * @return the formatted label ready for being drawn
   */
  getFormattedValue(value: number, entry: EntryOhos, dataSetIndex: number, viewPortHandler: ViewPortHandler): string;
}
