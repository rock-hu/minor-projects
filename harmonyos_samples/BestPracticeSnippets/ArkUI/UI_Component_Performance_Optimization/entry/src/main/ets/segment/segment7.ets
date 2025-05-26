// [Start Case5]
import { util } from '@kit.ArkTS';

interface User {
  id: string;
  name: string;
  age?: number;
  avatarImage?: ResourceStr;
  //introduction: string;
  // ...
}


// create data
const DEFAULT_BACKGROUND_COLOR = Color.Pink;
const getUsers = () => {
  const USERS: User[] = [{
    id: '1',
    name: '张三',
  }, {
    id: '2',
    name: '李四',
  }, {
    id: '3',
    name: '王五',
  }];
  return Array.from(Array(30), (item: User, i: number) => {
    return {
      id: util.generateRandomUUID(),
      name: USERS[i%3].name,
      avatarImage: $r('app.media.startIcon'),
      age: 18 + i
    } as User;
  });
}


// User Card List Component
@Component
export struct UserCardList {
  @State users: User[] = getUsers();

  build() {
    List({ space: 8 }) {
      ForEach(this.users, (item: User) => {
        ListItem() {
          UserCard({ name: item.name, age: item.age, avatarImage: item.avatarImage })
        }
      }, (item: User) => item.id)
    }
    .alignListItem(ListItemAlign.Center)
  }
}


// User Card Customization Component
@Component
struct UserCard {
  @Prop avatarImage: ResourceStr;
  @Prop name: string;
  @Prop age: number;

  build() {
    Row() {
      Row() {
        Image(this.avatarImage)
          .size({ width: 50, height: 50 })
          .borderRadius(25)
          .margin(8)
        Text(this.name)
          .fontSize(30)
      }

      Text(`age：${this.age.toString()}`)
        .fontSize(20)
    }
    .backgroundColor(DEFAULT_BACKGROUND_COLOR)
    .justifyContent(FlexAlign.SpaceBetween)
    .borderRadius(8)
    .padding(8)
    .height(66)
    .width('80%')
  }
}

// [End Case5]