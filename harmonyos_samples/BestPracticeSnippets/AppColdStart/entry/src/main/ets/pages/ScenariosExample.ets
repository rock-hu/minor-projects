// [Start scenarios_example]
// Index.ets
import { http } from '@kit.NetworkKit';
import { image } from '@kit.ImageKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { abilityAccessCtrl, common, Permissions } from '@kit.AbilityKit';
import { fileIo as fs } from '@kit.CoreFileKit';

const PERMISSIONS: Array<Permissions> = [
  'ohos.permission.READ_MEDIA',
  'ohos.permission.WRITE_MEDIA'
];
AppStorage.link('net_picture');
PersistentStorage.persistProp('net_picture', '');

@Entry
@Component
struct Index {
  @State image: PixelMap | undefined = undefined;
  @State imageBuffer: ArrayBuffer | undefined = undefined; // Picture ArrayBuffer

  /**
   * Download picture resources from the Internet through the http request method
   */
  async getPicture() {
    http.createHttp()
      .request('https://www.example1.com/POST?e=f&g=h',
        (error: BusinessError, data: http.HttpResponse) => {
          if (error) {
            return;
          }
          // Determine whether the resources obtained by the network are of the ArrayBuffer type.
          if (data.result instanceof ArrayBuffer) {
            this.imageBuffer = data.result as ArrayBuffer;
          }
          this.transcodePixelMap(data);
        }
      )
  }

  /**
   * Use createPixelMap to replace pictures of ArrayBuffer type with PixelMap type
   * @param data：Resources obtained from the network
   */
  transcodePixelMap(data: http.HttpResponse) {
    if (http.ResponseCode.OK === data.responseCode) {
      const imageData: ArrayBuffer = data.result as ArrayBuffer;
      // Create a picture source instance through ArrayBuffer
      const imageSource: image.ImageSource = image.createImageSource(imageData);
      const options: image.InitializationOptions = {
        'alphaType': 0, // Transparency
        'editable': false, // Is it editable?
        'pixelFormat': 3, // Pixel format
        'scaleMode': 1, // Abbreviation
        'size': { height: 100, width: 100 }
      }; // Create the size of the picture

      // Create PixelMap through attributes
      imageSource.createPixelMap(options).then((pixelMap: PixelMap) => {
        this.image = pixelMap;
        setTimeout(() => {
          if (this.imageBuffer !== undefined) {
            this.saveImage(this.imageBuffer);
          }
        }, 0)
      });
    }
  }


  /**
   * 保存ArrayBuffer到沙箱路径
   * @param buffer：图片ArrayBuffer
   * @returns
   */
  async saveImage(buffer: ArrayBuffer | string): Promise<void> {
    const context = this.getUIContext().getHostContext() as common.UIAbilityContext;
    const filePath: string = context.cacheDir + '/test.jpg';
    AppStorage.set('net_picture', filePath);
    const file = await fs.open(filePath, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
    await fs.write(file.fd, buffer);
    await fs.close(file.fd);
  }


  async useCachePic(): Promise<void> {
    if (AppStorage.get('net_picture') !== '') {
      // 获取图片的ArrayBuffer
      const imageSource: image.ImageSource = image.createImageSource(AppStorage.get('net_picture'));
      const options: image.InitializationOptions = {
        'alphaType': 0, // 透明度
        'editable': false, // 是否可编辑
        'pixelFormat': 3, // 像素格式
        'scaleMode': 1, // 缩略值
        'size': { height: 100, width: 100 }
      };
      imageSource.createPixelMap(options).then((pixelMap: PixelMap) => {
        this.image = pixelMap;
      });
    }
  }


  async aboutToAppear(): Promise<void> {
    const context = this.getUIContext().getHostContext() as common.UIAbilityContext;
    const atManager = abilityAccessCtrl.createAtManager();
    await atManager.requestPermissionsFromUser(context, PERMISSIONS);
    this.useCachePic(); // 从本地存储获取数据
    this.getPicture(); // 从网络端获取数据
  }


  build() {
    Column() {
      Image(this.image)
        .objectFit(ImageFit.Contain)
        .width('50%')
        .height('50%')
    }
  }
}
// [End scenarios_example]