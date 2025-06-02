interface VideoItem {
  id: string;
}

function getVideoList(): VideoItem[] {
  return [];
}

const screenHeight: number = 800;

// [Start Case1]
@Component
export struct VideoCard {
  @State isVideoInit: boolean = false;
  // [StartExclude Case1]
  @Prop item: VideoItem = { id: '' };

  private createComplexVideoPlayer(): void {
  }

  // [EndExclude Case1]
  build() {
    Column() {
      // Video Playback Component
    }
    .onAreaChange((old, newValue) => {
      if (!this.isVideoInit) {
        let positionY: number = newValue.position.y as number
        if (positionY < screenHeight / 3) {
          this.createComplexVideoPlayer();
          this.isVideoInit = true;
        }
      }
    })
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
// [End Case1]