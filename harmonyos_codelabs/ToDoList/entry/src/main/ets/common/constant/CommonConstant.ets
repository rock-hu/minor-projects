/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Style constants that can be used by all modules
 */
export default class CommonConstants {
  /**
   * Full width or height.
   */
  public static readonly FULL_LENGTH: string = '100%';

  /**
   * Title height.
   */
  public static readonly TITLE_WIDTH: string = '80%';

  /**
   * List default width.
   */
  public static readonly LIST_DEFAULT_WIDTH: string = '93.3%';

  /**
   * Opacity of default.
   */
  public static readonly OPACITY_DEFAULT: number = 1;

  /**
   * Opacity of default.
   */
  public static readonly OPACITY_COMPLETED: number = 0.4;

  /**
   * BorderRadius of list item.
   */
  public static readonly BORDER_RADIUS: number = 24;

  /**
   * BorderRadius of list item.
   */
  public static readonly FONT_WEIGHT: number = 500;

  /**
   * Space of column.
   */
  public static readonly COLUMN_SPACE: number = 16;

  /**
   * agents data.
   */
  public static readonly TODO_DATA: Array<Resource> = [
    $r('app.string.todo_list_one'),
    $r('app.string.todo_list_two'),
    $r('app.string.todo_list_three'),
    $r('app.string.todo_list_four'),
    $r('app.string.todo_list_five')
  ];
}

