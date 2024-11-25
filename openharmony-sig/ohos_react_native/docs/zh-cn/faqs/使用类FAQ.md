# 使用类FAQ

### 加载沙盒中的bundle时RN界面图片显示问题
 
- 现象
 
    鸿蒙项目中加载本地 rawfile 中的 bundle 时 **RN** 界面Image组件可以正常显示本地图片，从沙盒中加载 bundle 时 **RN** 界面 Image 组件图片显示不出来。
 
- 原因
 
    对于从沙盒中加载图片，放资源文件，需要按照 bundles 本身文件的解压位置来确定。
 
- 解决
 
    读取沙箱中的图片资源请参考[文档](../常见开发场景.md#如何加载沙箱路径bundle和图片)。
 
    对于从沙盒中加载图片，放资源文件，需要按照 bundles 本身文件的解压位置来确定，可参考以下：
- 打包场景
  - 打包路径
    
    假设为打包路径为根路径：/，**RN**代码路径：/aaa/bbb/c.tsx，
 
    图片1所在路径：/aaa/bbb/d.png **RN**使用写法：require('./d.png')，
 
    图片2所在路径：/eee/f.png **RN**使用写法：require('../../eee/f.png')，
 
    图片3所在路径：/aaa/ggg/h.png **RN**使用写法：require('../ggg/h.png')。
 
  - 图片路径前缀
 
    图片1生成的路径前缀：/aaa/bbb/
 
    图片2生成的路径前缀：/eee/
    
    图片3生成的路径前缀：/aaa/ggg/
 
  - 资源打包结果
 
    assets 是在 `package.json` 里面指定的目录,开发者可以在 assets 下任意设置目录。
    
    参考事例，用户可以自定义 aaa, bbb , ggg , eee 等子目录：
 
    assets/aaa/bbb/d.png ，assets/aaa/ggg/h.png ，assets/eee/f.png。
 
- 沙箱场景图片路径
 
  在沙箱场景下假设 bundle 所在路径 `/data/storage/base/files/dir1/bundle.harmony.js`。
 
  - 图片1沙箱场景实际路径： `/data/storage/base/files/dir1/aaa/bbb/d.png`。
  - 图片2沙箱场景实际路径： `/data/storage/base/files/dir1/eee/f.png`。
  - 图片3沙箱场景实际路径： `/data/storage/base/files/dir1/aaa/ggg/h.png`。
 
  **重点**：沙箱场景下图片的路径是根据 bundle 路径作为前缀，图片与工程打包路径的相对路径为后缀，合成完整的路径。这里没有 assets 目录。
 
  **路径示例：**
 
  ```CMAKE
  <Image source={{ uri: 'file:///data/storage/el2/base/files/10.png' }}
            style={{ width: 200, height: 200 }} />
  ```
 
- RAWFILE场景图片路径
 
  **RAWFILE** 场景下假设 bundle 所在路径 `resource://RAWFILE/dir1/bundle.harmony.js`。
 
  - 图片1rawfile场景实际路径： `resource://RAWFILE/assets/aaa/bbb/d.png`。
  - 图片2rawfile场景实际路径： `resource://RAWFILE/assets/eee/f.png`。
  - 图片3rawfile场景实际路径： `resource://RAWFILE/assets/aaa/ggg/h.png`。
  **重点**：**RAWFILE** 场景下图片路径的前缀指定为 `resource://RAWFILE/assets/` ，图片与工程打包路径的相对路径为后缀，合成完整的路径。这里有 assets 目录。

### modal弹框，进行路由跳转后modal 仍然显示在最上层

- 现象
	
    modal 弹框，进行路由跳转后 modal 仍然显示在最上层。

- 原因

    这个是规格；鸿蒙的 modal 对应的是 Arkui 的 Dialog ，当前 Dialog 的 **UI** 规范是在 window 最上层，所以才会有这个问题。

- 解决

    有以下两种规避方案：
    - 跳转时主动关闭 modal。
    - 不使用 modal，通过view实现 modal 的 ui 效果。

- 参考

    鸿蒙[规格文档](https://gitee.com/openharmony/docs/blob/master/zh-cn/release-notes/changelogs/OpenHarmony_5.0.0.17/changelogs-arkui.md#clarkui2--dialog%E5%9C%A8%E9%A1%B5%E9%9D%A2%E8%B7%AF%E7%94%B1%E8%B7%B3%E8%BD%AC%E6%97%B6%E5%85%B3%E9%97%AD%E8%A1%8C%E4%B8%BA%E5%8F%98%E6%9B%B4)。
   
### FlashList 列表右侧滚动条如何隐藏

- 现象
	
    想隐藏 FlashList 列表右侧滚动条。

- 原因

    无。

- 解决

    - 隐藏垂直的滚动条

        showsVerticalScrollIndicator={false}//隐藏垂直滚动条

    - 隐藏水平滚动条

        showsHorizontalScrollIndicator={false}//隐藏水平滚动条


### Animated动画组件设置useNativeDriver为false卡顿问题

- 现象

   使用 RNAnimated 动画组件设置 useNativeDriver 为`false`时，会出现卡顿现象（因 useNativeDriver 不支持 Animated 中 `translateX` 属性，无法设置 useNativeDriver 为`true`），也会导致一些其他性能问题。
   
   使用 `react-native-page-view` 库时，在 `onPageScroll` 配置 useNativeDriver 的方法也会有此问题。

- 原因

    非 bug ,使用方式不当导致。

- 解决

   用 `transform` 代替 `top` ，即用 `transform` 的 `scaleX` 和 `scaleY` 对宽度和高度进行变换。

   （1） 原问题代码示例：

    ```typescript 
    <Animated.View
        style={{
          width: 300,
          height: 204,
          position: 'absolute',
          top:macTop,
          left:macTop,
        }}>
        <Image ref="image" style={{ width: 375, height: 242 }}
          source={require('./keli.png')}>
        </Image>
      </Animated.View>
    ```

   （2） 解决方案代码示例：

    ```typescript 
    // 初始化动画值
    const scaleX = useRef(new Animated.Value(1)).current;
      const scaleY = useRef(new Animated.Value(1)).current;

      return (
         <View style={styles.container}>
            <Animated.View
              style={[
                styles.box,
                {
                  transform: [
                  { scaleX: scaleX }, // 变换宽度
                  { scaleY: scaleY }, // 变换高度
                  ],
                },
              ]}
            />
            <Button title="Start Animation" onPress={startScaleAnimation} />
         </View>
      );
    ```

### 5.0.0.500 版本显示字体变小的问题

- 现象

  - 现象1：同一个 bundle 包，在自定义的 `UIAbility` 场景下，可能存在显示字体明显变小。
  - 现象2：使用了 RNAbility 的项目，在使用 Metro 服务加载 **RN** 页面时字体变小；加载本地 bundle 时，字体显示正常。

- 原因

    这个版本 **RNOH** 的 `RNInstancesCoordinator` 新增了 `fontSizeScale` 参数，对应原生中的 `fontSizeScale` ，表示字体大小缩放比例，取值范围：0~3.2，默认值为 1。

    最近遇到的两个关于 **RN** 页面字体变小的案例，分析原因后发现都是因为加载 bundle 太早或者说创建 RNInstancesCoordinator 太晚导致的。CPP 侧拿到 `fontScale` 值是通过 RNInstancesCoordinator 的 `fontSizeScale` 传递的，如果RNInstancesCoordinator 创建过晚，提前加载了 **RN** 页面，CPP 侧渲染时的 `fontScale` 将为 0，而正常大小值是 1，所以导致了显示明显变小。

    以上两种场景具体原因分别是：

      - 现象 1 原因：使用自定义的 `UIAbility` 时，在创建 RNInstancesCoordinator 并打开 **RN** 页面的时候，没有提前初始化 RNInstancesCoordinator 导致 `fontSizeScale` 未成功传递给 CPP 侧；
      - 现象 2 原因：代码中 `onWindowStageCreate` 执行了预加载 Metro bundle 的操作，加载 Metro 的时候，**RNOH** 的 CPP 侧还未拿到 RNInstancesCoordinator 传递的 `fontScale` 默认值，所以出现了 Metro 服务加载的 **RN** 页面字体变小。

- 解决

  - 现象1解决方案：在 `Ability.onWindowStageCreate()` 生命周期里提交初始化 `RNInstancesCoordinator`。
  - 现象2解决方案：将预加载 Metro bundle 的代码延迟执行。
    
### 当一个页面的TABHost嵌套一个RN页面时，水平滑动会很容易触发RN页面中的点击事件

- 现象

    当一个页面的TABHost嵌套一个 **RN** 页面时，水平滑动会高概率触发 **RN** 页面中的点击事件。

- 原因

    水平滑动造成原生层面手势冲突，需要在原生滑动时调用对应方法取消点击事件。

- 解决

    在原生滑动时调用`this.rnohCoreContext?.cancelTouches()`方法，具体代码如下：

    ```javascript
    private rnContext:RNOHCoreContext|undefined = AppStorage.get('RNOHCoreContext'); //RN-500版本
      .onGestureSwipe((index: number, event: TabsAnimationEvent) => {
      if(this.rnContext) {
      this.rnContext.cancelTouches();
      }
      })
    ```