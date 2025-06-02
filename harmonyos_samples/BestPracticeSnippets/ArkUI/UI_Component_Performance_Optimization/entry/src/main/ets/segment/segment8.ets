interface User {
  id: string;
  name: string;
  age?: number;
  avatarImage?: ResourceStr;
}

const getUsers = (): User[] => {
  return [];
};

// [Start Case6]
// 1. Customizing @Builder Function Components
@Builder
function UserCardBuilder(name: string, age?: number, avatarImage?: ResourceStr) {
  Row() {
    Row() {
      Image(avatarImage)
        .size({ width: 50, height: 50 })
        .borderRadius(25)
        .margin(8)
      Text(name)
        .fontSize(30)
    }
    Text(`age：${age?.toString()}`)
      .fontSize(20)
  }
  .backgroundColor(Color.Blue)
  .justifyContent(FlexAlign.SpaceBetween)
  .borderRadius(8)
  .padding(8)
  .height(66)
  .width('80%')
}

@Component
export struct UserCardList {
  @State users: User[] = getUsers();

  build() {
    List({ space: 8 }) {
      ForEach(this.users, (item: User) => {
        ListItem() {
          // 2. Using the @Builder function in a build function
          UserCardBuilder(item.name, item.age, item.avatarImage)
        }
      }, (item: User) => item.id)
    }
    .alignListItem(ListItemAlign.Center)
  }
}
// [End Case6]