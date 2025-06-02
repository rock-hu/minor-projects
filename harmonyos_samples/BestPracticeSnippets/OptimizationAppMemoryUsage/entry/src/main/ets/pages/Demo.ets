// [Start demo_page]
import { LRUCacheUtil } from '../utils/LRUCacheUtil';

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  aboutToAppear(): void {
    let lruCache = LRUCacheUtil.getInstance();
    // Add a <key, value> to lrucache
    lruCache.putCache('nation',10);
    // Add another <key, value> to lrucache
    lruCache.putCache('menu',8);
    // Query value through key
    let result0 = lruCache.getCache('2') as number;
    console.log('result0:' + result0);
    // Delete the specified key and its associated values from the current buffer
    let result1 = lruCache.remove('2');
    console.log('result1:' + result1);
    // Check whether the current buffer contains the specified object
    let result2 = lruCache.contains('1');
    console.log('result2:' + result2);
    // Set a new capacity size
    let result4 = lruCache.updateCapacity(110);
    console.log('result4:' + result4);
  }


  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
        // [Start media_image]
        Column() {
          Image($r('app.media.image'))
            .width("500px")
            .height("500px")
        }
        // [End media_image]
      }
      .width('100%')
    }
    .height('100%')
  }
}
// [End demo_page]