interface VideoItem {
  id: string;
}

function getVideoList(): VideoItem[] {
  return [];
}

// [Start Counter_example1]
@Component
export struct VideoCard {
  // [StartExclude Counter_example1]
  @Prop item: VideoItem = { id: '' };

  private createComplexVideoPlayer(): void {
  }

  build() {
    Column() {
    }
  }

  // [EndExclude Counter_example1]
  aboutToAppear(): void {
    // Create a complex object task, if the task takes 1s to execute, the component will be rendered again after 1s
    this.createComplexVideoPlayer();
  }
  // ...
}

@Component
export struct CardList {
  @State videoList: VideoItem[] = getVideoList();

  build() {
    List() {
      ForEach(this.videoList, (item: VideoItem) => {
        ListItem() {
          VideoCard({ item })
        }
      }, (item: VideoItem) => item.id)
    }
  }
}
// [End Counter_example1]