/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
import { ImperativeViewComponent } from '../view/ImperativeView';
import { MainPage } from './MainPage';

@Entry
@Component
struct mainPageView {
  @Builder
  NavigationTitle() {
    Column() {
      Text($r('app.string.tab1'))
        .width(328)
        .height(40)
        .margin({ left: '16' })
        .fontSize(30)
        .fontWeight(FontWeight.Bold)
    }.alignItems(HorizontalAlign.Start)
  }


  build() {
    Navigation() {
      Column() {
        NavRouter() {
          Button($r('app.string.tab2'))
            .width(328)
            .height(40)
            .margin({ bottom: '12' })

          NavDestination() {
            Flex({ direction: FlexDirection.Row }) {
              Column() {
                MainPage();
              }
            }
          }.title(this.ResourceToString($r('app.string.tab2')))
        }

        NavRouter() {
          Button($r('app.string.tab3'))
            .width(328)
            .height(40)
            .margin({ bottom: '16' })

          NavDestination() {
            Flex({ direction: FlexDirection.Row }) {
              ImperativeViewComponent();
            }
          }.title(this.ResourceToString($r('app.string.tab3')))
        }
      }
      .justifyContent(FlexAlign.End)
      .width('100%')
      .height('100%')
    }
    .title(this.NavigationTitle())
    .titleMode(NavigationTitleMode.Full)
    .mode(NavigationMode.Stack)

  }

  ResourceToString(resource:Resource):string{
    return getContext(this).resourceManager.getStringSync(resource);
  }
}