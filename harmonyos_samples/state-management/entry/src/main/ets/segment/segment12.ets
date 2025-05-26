// [Start Case8]
import { ButtonComponent } from '../components/ButtonComponent';
import { ListItemComponent } from '../components/ListItemComponent';

@Entry
@Component
struct UseEmitterPublish {
  listData: string[] = ['A', 'B', 'C', 'D', 'E', 'F'];


  build() {
    Column() {
      Row() {
        Column() {
          ButtonComponent()
        }
      }


      Column() {
        Column() {
          List() {
            ForEach(this.listData, (item: string, index: number) => {
              ListItemComponent({ myItem: item, index: index })
            })
          }
          .height('100%')
          .width('100%')
          .alignListItem(ListItemAlign.Center)
        }
      }
    }
  }
}
// [End Case8]