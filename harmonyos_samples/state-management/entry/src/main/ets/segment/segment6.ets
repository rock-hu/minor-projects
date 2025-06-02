interface IUserAccountRepository {
  getUserData(): Promise<UserData>;
}
// [Start Case3_start]
// Data structure of user information UserData
export interface UserData {
  id: string;
  username: string;
  description: string;
  // ...
}
// [StartExclude Case3_start]
class bb {
  private userAccountRepository: IUserAccountRepository;

  constructor(repository: IUserAccountRepository) {
    this.userAccountRepository = repository;
  }
  // [EndExclude Case3_start]
  // Getting server-side user information in a business class
  getUserData(): void {
    this.userAccountRepository.getUserData().then((data: UserData) => {
      // 1.Storing user information data into AppStorage
      AppStorage.setOrCreate('userData', data);
    });
  }
  // [StartExclude Case3_start]
}
// [EndExclude Case3_start]
// View component for displaying user information at the top of the “My” module
@Component
struct UserInfoView {
  // 2.Receive user information stored in AppStorage using @StorageLink decorator in UI
  @StorageLink('userData') userData: UserData | null = null;
  build() {
    Column() {
      Row() {
        // ...
        Column() {
          // 3.Display the user name in the userData.
          Text(this.userData ? this.userData.username : 'default_login')
          // ...
        }
      }
      // ...
    }
    // ...
  }
}
// [End Case3_start]