// [Start Case9]
import { emitter } from '@kit.BasicServicesKit';
const CHANGE_COLOR_EVENT_ID = 1;
@Component
export struct ButtonComponent {
  value: number = 2;
  build() {
    Button(`下标是${this.value}的倍数的组件文字变为红色`)
      .onClick(() => {
        let event: emitter.InnerEvent = {
          eventId: CHANGE_COLOR_EVENT_ID,
          priority: emitter.EventPriority.LOW
        };
        let eventData: emitter.EventData = {
          data: {
            value: this.value
          }
        };
        // Sends an event with eventId of CHANGE_COLOR_EVENT_ID and event content of eventData
        emitter.emit(event, eventData);
        this.value++;
      })
  }
}
// [End Case9]