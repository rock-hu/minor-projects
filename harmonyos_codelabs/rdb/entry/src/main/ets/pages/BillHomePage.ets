/*
 *  Copyright (c) 2025 Huawei Device Co., Ltd.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

import { common } from '@kit.AbilityKit';
import { LengthMetrics } from '@kit.ArkUI';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { systemDateTime } from '@kit.BasicServicesKit';
import { AccountData, ImageList, TextList } from '../viewmodel/BillViewModel';
import CommonConstants from '../common/CommonConstants';
import { BillDialog } from '../components/BillDialog';
import { RdbManager } from '../utils/RdbManager';

@Entry
@Component
struct BillHomePage {
  @State accounts: Array<AccountData> = [];
  @State searchText: string = '';
  @State isEdit: boolean = false;
  @State isInsert: boolean = false;
  @State newAccount: AccountData = {
    id: '',
    accountType: 0,
    typeText: '',
    amount: 0
  };
  @State index: number = -1;
  private deleteList: Array<AccountData> = [];
  private context: common.UIAbilityContext = this.getUIContext().getHostContext() as common.UIAbilityContext;
  private rdbManager: RdbManager = new RdbManager(this.context);
  searchController: SearchController = new SearchController();
  dialogController: CustomDialogController = new CustomDialogController({
    builder: BillDialog({
      isInsert: $isInsert,
      newAccount: $newAccount,
      confirm: (isInsert: boolean, newAccount: AccountData) => this.accept(isInsert, newAccount)
    }),
    keyboardAvoidDistance: LengthMetrics.vp(0),
    customStyle: true,
    alignment: DialogAlignment.Bottom
  });

  async accept(isInsert: boolean, newAccount: AccountData): Promise<void> {
    if (isInsert) {
      const time = systemDateTime.getTime();
      newAccount.id = `id_${time}`;
      await this.rdbManager.insertData(newAccount);
      this.accounts.push(newAccount);
      hilog.info(0x0000, 'hilog', `The account inserted is:  ${JSON.stringify(newAccount)}`);
    } else {
      await this.rdbManager.updateData(newAccount);
      const list = this.accounts.map((item: AccountData) => {
        const result = item.id === newAccount.id ? newAccount : item;
        return result;
      });
      this.accounts = list;
      this.index = -1;
    }
  }

  async aboutToAppear() {
    await this.rdbManager.setDistributedTables(this.context);
    this.rdbManager.subscribeDataChange();
    const result: AccountData[] = await this.rdbManager.query('', true);
    this.accounts = result;
    this.context.eventHub.on('dataChange', this.eventFunc);
  }

  aboutToDisappear(): void {
    this.context.eventHub.off('dataChange', this.eventFunc);
  }

  // Function for Monitoring Data Changes.
  eventFunc = (value: string) => {
    const list: AccountData[] = JSON.parse(value);
    this.rdbManager.updateLocalDataBase(this.accounts, list);
    this.accounts = list;
  };

  selectListItem(item: AccountData) {
    this.isInsert = false;
    this.index = this.accounts.indexOf(item);
    this.newAccount = {
      id: item.id,
      accountType: item.accountType,
      typeText: item.typeText,
      amount: item.amount
    };
  }

  deleteListItem() {
    this.deleteList.forEach(async (item: AccountData) => {
      const index = this.accounts.findIndex((account: AccountData) => account.id === item.id);
      this.accounts.splice(index, 1);
      await this.rdbManager.deleteData(item);
    });
    this.deleteList = [];
    this.isEdit = false;
  }

  build() {
    Column() {
      Stack({ alignContent: Alignment.TopStart }) {
        Flex({ direction: FlexDirection.Column }) {
          this.NavigationTitle();

          Search({
            value: this.searchText,
            placeholder: CommonConstants.SEARCH_TEXT,
            controller: this.searchController
          })
            .width('100%')
            .height(40)
            .flexShrink(0)
            .borderRadius(24)
            .margin({
              top: 8,
              bottom: 16
            })
            .placeholderColor('rgba(0, 0, 0, 0.6)')
            .placeholderFont({
              size: 16,
              weight: 400,
            })
            .textFont({ size: 16 })
            .padding({
              top: 9,
              right: 12,
              bottom: 9,
              left: 12
            })
            .onChange(async (searchValue: string) => {
              this.searchText = searchValue;
              if (searchValue === '') {
                const result: AccountData[] = await this.rdbManager.query('', true);
                this.accounts = result;
              }
            })
            .onSubmit(async (searchValue: string) => {
              if (searchValue === '') {
                const result: AccountData[] = await this.rdbManager.query('', true);
                this.accounts = result;
              } else {
                const keyList: Array<string> = Object.keys(TextList);
                const typeText = keyList.find(key => {
                  let result: string = '';
                  try {
                    result = this.context.resourceManager.getStringByNameSync(key);
                  } catch (err) {
                    hilog.error(0x0000, 'BillHomePage', `getStringByNameSync failed, Code:${err.code},message: ${err.message}`);
                  }
                  return result === searchValue;
                });
                const result: AccountData[] = await this.rdbManager.query(typeText || '', false);
                this.accounts = result;
              }
            })

          List({ space: CommonConstants.FULL_SIZE }) {
            ForEach(this.accounts, (item: AccountData, index: number) => {
              ListItem() {
                Column() {
                  Row() {
                    Row() {
                      SymbolGlyph(ImageList[item.typeText])
                        .fontSize(24)
                        .fontWeight(400)
                        .fontColor(['#0A59F7'])
                    }
                    .alignItems(VerticalAlign.Center)
                    .justifyContent(FlexAlign.Center)
                    .width(40)
                    .height(40)
                    .backgroundColor('rgba(10, 89, 247, 0.15)')
                    .borderRadius(40)
                    .margin({ right: 16 })

                    Text(TextList[item.typeText])
                      .fontSize($r('app.float.font_size_M'))
                      .fontColor('rgba(0, 0, 0, 0.9)')
                      .lineHeight(21)

                    Blank()
                      .layoutWeight(1)

                    Text(item.accountType === 0 ? '-' + item.amount.toString() : '+' + item.amount.toString())
                      .fontSize(16)
                      .fontColor(item.accountType === 0 ? $r('app.color.pay_color') : $r('app.color.main_color'))

                    if (this.isEdit) {
                      Row() {
                        Toggle({ type: ToggleType.Checkbox })
                          .onChange((isOn) => {
                            if (isOn) {
                              this.deleteList.push(item);
                            } else {
                              let index = this.deleteList.indexOf(item);
                              this.deleteList.splice(index, 1);
                            }
                          })
                      }
                      .margin({ left: 10 })
                    }
                  }
                  .width('100%')
                  .padding({ top: 4, bottom: 4 })

                  if (index !== this.accounts.length - 1) {
                    Divider()
                      .color('rgba(0,0,0,0.2)')
                      .strokeWidth(0.5)
                      .margin({
                        left: 68
                      })
                  }
                }
              }
              .width('100%')
              .height(48)
              .padding({
                left: 12,
                right: 12,
              })
              .backgroundColor(Color.White)
              .onClick(() => {
                this.selectListItem(item);
                this.dialogController.open();
              })
            },(item: AccountData) => item.id)
          }
          .width('100%')
          .flexGrow(1)
          .borderRadius(16)
          .clip(true)
          .padding({
            top: this.accounts.length ? 4 : 0,
            bottom: this.accounts.length ? 4 : 0
          })
        }

        if (!this.isEdit) {
          Row() {
            SymbolGlyph($r('sys.symbol.plus'))
              .fontSize(24)
              .fontColor([Color.White])
              .fontWeight(400)
          }
          .alignItems(VerticalAlign.Center)
          .justifyContent(FlexAlign.Center)
          .width(48)
          .height(48)
          .backgroundColor('#0A59F7')
          .borderRadius('50%')
          .position({ left: '50%', bottom: 26 })
          .translate({ x: '-50%' })
          .onClick(() => {
            this.isInsert = true;
            this.newAccount = {
              id: '',
              accountType: 0,
              typeText: '',
              amount: 0
            };
            this.dialogController.open();
          })
        }

        if (this.isEdit) {
          Column() {
            SymbolGlyph($r('sys.symbol.trash'))
              .fontSize(20)
              .fontColor([Color.Black])
              .fontWeight(400)
            Text($r('app.string.delete_text'))
              .fontSize(12)
              .fontColor('rgba(0, 0, 0, 0.9)')
              .lineHeight(16)
              .fontWeight(400)
              .margin({ top: 6 })
          }
          .position({ left: '50%', bottom: 0 })
          .translate({ x: '-50%' })
          .onClick(() => {
            this.deleteListItem();
          })
        }
      }
      .width('100%')
      .height('100%')
    }
    .width('100%')
    .height('100%')
    .padding({ left: 16, right: 16 })
    .backgroundColor($r('app.color.background_color'))
    .expandSafeArea([SafeAreaType.SYSTEM], [SafeAreaEdge.TOP, SafeAreaEdge.BOTTOM])
  }

  @Builder
  NavigationTitle() {
    Flex({ alignItems: ItemAlign.Center }) {
      if (this.isEdit) {
        Row() {
          SymbolGlyph($r('sys.symbol.xmark'))
            .fontColor(['rgba(0,0,0,0.9)'])
            .fontSize(24)
        }
        .alignItems(VerticalAlign.Center)
        .justifyContent(FlexAlign.Center)
        .width(40)
        .height(40)
        .backgroundColor('rgba(0, 0, 0, 0.05)')
        .borderRadius(40)
        .margin({ right: 8 })
        .onClick(() => {
          if (this.isEdit) {
            this.isEdit = !this.isEdit;
          }
        })
      }

      Text(this.isEdit ? $r('app.string.edit_text') : $r('app.string.billing_title'))
        .fontSize(20)
        .fontWeight(700)
        .lineHeight(27)
        .fontColor('rgba(0, 0, 0, 0.9)')
        .flexGrow(1)

      Row() {
        if (this.isEdit) {
          SymbolGlyph($r('sys.symbol.checkmark'))
            .fontColor(['rgba(0,0,0,0.9)'])
            .fontSize(24)
        } else {
          SymbolGlyph($r('sys.symbol.square_and_pencil'))
            .fontSize(24)
            .fontWeight(400)
        }
      }
      .alignItems(VerticalAlign.Center)
      .justifyContent(FlexAlign.Center)
      .width(40)
      .height(40)
      .backgroundColor('rgba(0, 0, 0, 0.05)')
      .borderRadius(40)
      .onClick(() => {
        if (this.accounts.length === 0) {
          try {
            this.getUIContext().getPromptAction().showToast({ message: $r('app.string.add_bill') });
          } catch (error) {
            hilog.error(0x0000, 'BillHomePage', `have error .Code:${error.code},message: ${error.message}`);
          }
          return;
        }
        this.isEdit = !this.isEdit;
      })
    }
    .height(56)
    .flexShrink(0)
  }
}