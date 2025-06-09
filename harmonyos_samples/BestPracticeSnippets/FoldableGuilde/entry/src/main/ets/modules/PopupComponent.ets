// [Start PopupExample]
@Entry
@Component
export struct PopupExample {
  @State customPopup1: boolean = false
  @State customPopup2: boolean = false
  build() {
    Row() {
      Button("popup1")
        .onClick(()=>{
          this.customPopup1 = !this.customPopup1
        })
        // Bind the Popup window to the component and align it near the edge
        .bindPopup(this.customPopup1, {
          message: "this is a popup1",
          popupColor: Color.Pink,
        })
      Blank()
      Button("popup2")
        .onClick(()=>{
          this.customPopup2 = !this.customPopup2
        })
        // Bind the Popup window to the component and align it near the edge
        .bindPopup(this.customPopup2, {
          message: "this is a popup2",
          popupColor: Color.Pink,
        })
    }
    .width('100%')
    .height('100%')
  }
}
// [End PopupExample]