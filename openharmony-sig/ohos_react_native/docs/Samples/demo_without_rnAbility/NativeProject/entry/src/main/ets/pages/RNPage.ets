import { RNInfo, UPReactView, UPRNSDK } from 'library'
@Entry
@Component
export struct RNPage {
  rnInfo: RNInfo = {businessId: 'App', bundlePath: 'bundle.harmony.js', isFromMetro: false};
  @StorageLink('UPRNSDK') rnSdk: UPRNSDK | undefined = undefined;

  onBackPress(): boolean | void {
    this.rnSdk?.onBackPress();
  }

  build() {
    Stack() {
      UPReactView({
        rnInfo: this.rnInfo
      })
    }.width('100%').height('100%')
  }
}