function   convertColorSegmentsToString(name:string){
  return name
}
function convertMatrixArrayToMatrix4(name:string){
  return undefined
}
@Component
export struct UserCardList1 {
  @Builder
  renderChildren() {
    Text('Hello World')
      .fontSize(30)
      .fontWeight(FontWeight.Bold)
  }
  private  descriptor:ESObject
  getBorderStyle():BorderStyle | EdgeStyles{
		return BorderStyle.Dashed
	}
  getOpacity():number | Resource{
    return 1
  }
  getClip():boolean{
    return true
  }
  getHitTestMode(){
    return 1
  }
  getShadow():ShadowOptions | ShadowStyle{
    return ShadowStyle.OUTER_DEFAULT_XS
  }
  // [Start usercard_list1]
  build() {
    Stack() {
      this.renderChildren()
    }
    .width(this.descriptor.layoutMetrics.frame.size.width)
    .height(this.descriptor.layoutMetrics.frame.size.height)
    .backgroundColor(convertColorSegmentsToString(this.descriptor.props.backgroundColor))
    .position({ y: this.descriptor.layoutMetrics.frame.origin.y, x: this.descriptor.layoutMetrics.frame.origin.x })
    .borderWidth(this.descriptor.props.borderWidth)
    .borderColor({
      left: convertColorSegmentsToString(this.descriptor.props.borderColor.left),
      top: convertColorSegmentsToString(this.descriptor.props.borderColor.top),
      right: convertColorSegmentsToString(this.descriptor.props.borderColor.right),
      bottom: convertColorSegmentsToString(this.descriptor.props.borderColor.bottom)
    })
    .borderRadius(this.descriptor.props.borderRadius)
    .borderStyle(this.getBorderStyle())
    .opacity(this.getOpacity())
    .transform(this.descriptor.props.transform != undefined ? convertMatrixArrayToMatrix4(this.descriptor.props.transform) : undefined)
    .clip(this.getClip())
    .hitTestBehavior(this.getHitTestMode())
    .shadow(this.getShadow())
  }
  // [Start usercard_list1]
}
