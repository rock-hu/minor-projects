import { CustomAnimation } from './CustomAnimation';
import { PropertyAnimateTo } from './PropertyAnimateTo';
import { ExplicitAnimateTo } from './ExplicitAnimateTo';
import { ImageAnimation } from './ImageAnimation';
import { MultipleProperties } from './MultipleProperties';
import { UpdateMultipleProperties } from './UpdateMultipleProperties';
import { RenderGroup } from './RenderGroup';

@Entry
@Component
struct Index {
  @Provide('NavPathStack') pageStack: NavPathStack = new NavPathStack();

  @Builder
  PagesMap(name: string) {
    NavDestination() {
      if (name === 'CustomAnimation') {
        CustomAnimation()
      } else if (name === 'PropertyAnimateTo') {
        PropertyAnimateTo()
      } else if (name === 'ExplicitAnimateTo') {
        ExplicitAnimateTo()
      } else if (name === 'ImageAnimation') {
        ImageAnimation()
      } else if (name === 'MultipleProperties') {
        MultipleProperties()
      } else if (name === 'UpdateMultipleProperties') {
        UpdateMultipleProperties()
      } else if (name === 'RenderGroup') {
        RenderGroup()
      }

    }
    .hideTitleBar(true)
    .backgroundColor('#F1F3F5')
    .expandSafeArea([SafeAreaType.SYSTEM], [SafeAreaEdge.TOP, SafeAreaEdge.BOTTOM])

  }

  build() {
    Navigation(this.pageStack) {
      Column() {

        Blank()

        Button('CustomAnimation')
          .width('100%')
          .margin({ top: 12 })
          .onClick(() => {
            this.pageStack.pushPathByName('CustomAnimation', '');
          })
        Button('PropertyAnimateTo')
          .width('100%')
          .margin({ top: 12 })
          .onClick(() => {
            this.pageStack.pushPathByName('PropertyAnimateTo', '');
          })
        Button('ExplicitAnimateTo')
          .width('100%')
          .margin({ top: 12 })
          .onClick(() => {
            this.pageStack.pushPathByName('ExplicitAnimateTo', '');
          })
        Button('ImageAnimation')
          .width('100%')
          .margin({ top: 12 })
          .onClick(() => {
            this.pageStack.pushPathByName('ImageAnimation', '');
          })
        Button('MultipleProperties')
          .width('100%')
          .margin({ top: 12 })
          .onClick(() => {
            this.pageStack.pushPathByName('MultipleProperties', '');
          })
        Button('UpdateMultipleProperties')
          .width('100%')
          .margin({ top: 12 })
          .onClick(() => {
            this.pageStack.pushPathByName('UpdateMultipleProperties', '');
          })
        Button('RenderGroup')
          .width('100%')
          .margin({ top: 12 })
          .onClick(() => {
            this.pageStack.pushPathByName('RenderGroup', '');
          })
      }
      .padding({
        left: 16,
        right: 16,
        bottom: 16
      })
      .width('100%')
      .height('100%')
      .justifyContent(FlexAlign.Start)
    }
    .hideTitleBar(true)
    .mode(NavigationMode.Stack)
    .backgroundColor('#F1F3F5')
    .navDestination(this.PagesMap)
    .expandSafeArea([SafeAreaType.SYSTEM], [SafeAreaEdge.TOP, SafeAreaEdge.BOTTOM])
  }
}