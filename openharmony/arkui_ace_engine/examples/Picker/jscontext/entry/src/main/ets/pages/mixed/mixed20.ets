import { MultiComponent20 } from '../../framework/PComponent'
import * as comps from '../../framework/normalComponent'

@Entry
@Component
struct Plugins {
  scroller : Scroller = new Scroller()

  build() {
    Scroll(this.scroller) {
      Column() {
        Text('普通模式').fontSize(40).backgroundColor(Color.Gray)
        comps.DatePickerComponent()
        comps.CalendarPickerComponent()
        comps.TimePickerComponent()
        comps.TextPickerComponent()
        comps.DatePickerDialogComponent()
        comps.CalendarPickerDialogComponent()
        comps.TimePickerDialogComponent()
        comps.TextPickerDialogComponent()
        comps.ImageComponent()
        comps.ImageAnimatorComponent()
        comps.CounterComponent()
        comps.PatternLockComponent()
        comps.TextClockComponent()
        comps.TextTimerComponent()
        Text('沙箱模式随机').fontSize(40).backgroundColor(Color.Gray)
        MultiComponent20()
      }
    }
  }
}