// [Start Case10]
import { emitter } from '@kit.BasicServicesKit';
const CHANGE_COLOR_EVENT_ID = 1;
@Component
export struct ListItemComponent {
  @State color: Color = Color.Black;
  @Prop index: number;
  @Prop myItem: string;
  aboutToAppear(): void {
    let event: emitter.InnerEvent = {
      eventId: CHANGE_COLOR_EVENT_ID
    };
    // Execute this callback after receiving an event with eventId of CHANGE_COLOR_EVENT_ID
    let callback = (eventData: emitter.EventData): void => {
      if (eventData.data?.value !== 0 && this.index % eventData.data?.value === 0) {
        this.color = Color.Red;
      }
    };
    // Subscribe to events with eventId of CHANGE_COLOR_EVENT_ID
    emitter.on(event, callback);
  }
  build() {
    Column() {
      Text(this.myItem)
        .fontSize(50)
        .fontColor(this.color)
    }
  }
}
// [End Case10]