import type UIAbility from '@ohos.app.ability.UIAbility'
import { CommandDispatcher, RNComponentCommandHub } from './RNComponentCommandHub'
import { DescriptorRegistry, DescriptorWrapperFactory } from './DescriptorRegistry'
import { ComponentManagerRegistry } from './ComponentManagerRegistry'
import { SurfaceHandle } from './SurfaceHandle'
import { TurboModuleProvider } from './TurboModuleProvider'
import { EventEmitter } from './EventEmitter'
import type { RNOHLogger } from './RNOHLogger'
import type { CppFeatureFlag, NapiBridge } from './NapiBridge'
import type { RNOHContext } from './RNOHContext'
import { RNOHCorePackage } from '../RNOHCorePackage/ts'
import type { JSBundleProvider } from './JSBundleProvider'
import { JSBundleProviderError } from './JSBundleProvider'
import type { Tag } from './DescriptorBase'
import type { RNPackage, RNPackageContext } from './RNPackage'
import type { TurboModule } from './TurboModule'
import { ResponderLockDispatcher } from './ResponderLockDispatcher'
import { DevToolsController } from './DevToolsController'
import { RNOHError } from './RNOHError'
import window from '@ohos.window'
import { DevServerHelper } from './DevServerHelper'
import { HttpClient } from '../HttpClient/HttpClient'
import type { HttpClientProvider } from './HttpClientProvider'
import resourceManager from '@ohos.resourceManager'
import font from '@ohos.font'

export type Resource = Exclude<font.FontOptions["familySrc"], string>
export type SurfaceContext = {
  width: number
  height: number
  surfaceOffsetX: number
  surfaceOffsetY: number
  pixelRatio: number
  isRTL: boolean
}

export enum LifecycleState {
  BEFORE_CREATE,
  PAUSED,
  READY,
}

export type LifecycleEventArgsByEventName = {
  CONFIGURATION_UPDATE: Parameters<UIAbility["onConfigurationUpdate"]>
  FOREGROUND: [];
  BACKGROUND: [];
  JS_BUNDLE_EXECUTION_FINISH: [{
    jsBundleUrl: string,
    appKeys: string[]
  }];
  RELOAD: [{ reason: string | undefined }];
  WINDOW_SIZE_CHANGE: [windowSize: window.Size];
}

export type StageChangeEventArgsByEventName = {
  APP_STATE_FOCUS: [];
  APP_STATE_BLUR: [];
}


export type BundleExecutionStatus = "RUNNING" | "DONE"

const rootDescriptor = {
  isDynamicBinder: false,
  type: 'RootView',
  tag: 1,
  childrenTags: [],
  rawProps: {},
  props: {},
  state: {},
  layoutMetrics: {
    frame: {
      origin: {
        x: 0,
        y: 0,
      },
      size: {
        width: 0,
        height: 0,
      }
    }
  }
}

const DEFAULT_ASSETS_DEST: string = "assets/"; // assets destination path "assets/subpath/"

type FeatureFlagName = "ENABLE_RN_INSTANCE_CLEAN_UP" | "NDK_TEXT_MEASUREMENTS" | "IMAGE_LOADER" | "C_API_ARCH"

/**
 * Coordinates information flow by passing information to other objects to facilitate their operations.
 * Each RNInstance has its own JavaScript environment. A single application can utilize multiple RNInstances.
 */
export interface RNInstance {
  /**
   * Check DescriptorRegistry documentation for more information.
   */
  descriptorRegistry: DescriptorRegistry;
  /**
   * @architecture: C-API
   * Relays messages emitted from C++ by RNInstanceCAPI::postMessageToArkTS
   */
  cppEventEmitter: EventEmitter<Record<string, unknown[]>>
  /**
   * @deprecated Use RNOHContext::componentCommandReceiver
   */
  commandDispatcher: CommandDispatcher;
  /**
   * Check ComponentManagerRegistry documentation for more information.
   */
  componentManagerRegistry: ComponentManagerRegistry;
  /**
   * Mindless copy of Android's `ReactInstanceManager::getLifecycleState` per specific request.
   * It probably behaves differently to Android because UIAbility is a different concept than Android's Activity despite
   * certain similarities.
   */
  getLifecycleState(): LifecycleState;
  /**
   * Allows subscribing to various events. Check LifecycleEventArgsByEventName type for more information.
   */
  subscribeToLifecycleEvents: <TEventName extends keyof LifecycleEventArgsByEventName>(
    eventName: TEventName,
    listener: (...args: LifecycleEventArgsByEventName[TEventName]) => void
  ) => () => void;
  /**
   * Similar to subscribeToLifecycleEvents but handles different set of events. It may be removed to unify subscribing to events.
   */
  subscribeToStageChangeEvents: <TEventName extends keyof StageChangeEventArgsByEventName>(
    eventName: TEventName,
    listener: (...args: StageChangeEventArgsByEventName[TEventName]) => void
  ) => () => void;
  /**
   * Asynchronously executes a JS function. It may be renamed in the future because "call" suggest synchronous communication.
   * @param moduleName — name of the JavaScript file
   * @param functionName — name of the function defined in module.exports in that JS file
   * @param args - args that function should receive
   */
  callRNFunction(moduleName: string, functionName: string, args: unknown[]): void;
  /**
   * Sends device events (e.g. "appearanceChanged") to React Native.
   */
  emitDeviceEvent(eventName: string, payload: any): void;
  /**
   * Sends component events like "onScroll". That event is received by "EventEmitRequestHandler" on the C++ side. That handler
   * calls proper method on Component's EventEmitter.
   */
  emitComponentEvent(tag: Tag, eventName: string, payload: any): void;
  /**
   * Used by RNApp to avoid loading the same bundle twice.
   */
  getBundleExecutionStatus(bundleURL: string): BundleExecutionStatus | undefined
  /**
   * Enables feature flag. It may be removed in the future because usually feature flags need to be provided when creating
   * RNInstance.
   */
  enableFeatureFlag(featureFlagName: FeatureFlagName): void
  /**
   * Checks if given feature flag is enabled.
   */
  isFeatureFlagEnabled(featureFlagName: FeatureFlagName): boolean
  /**
   * Reads JS Bundle and executes loaded code.
   */
  runJSBundle(jsBundleProvider: JSBundleProvider): Promise<void>;
  /**
   * Provides TurboModule instance. Currently TurboModule live on UI thread. This method may be deprecated once "Worker" turbo module are supported.
   */
  getTurboModule<T extends TurboModule>(name: string): T;
  /**
   * Used by RNSurface. It creates a surface somewhere in React Native.
   */
  createSurface(appKey: string): SurfaceHandle;
  /**
   * Sets (Right-To-Left) mode in all surfaces.
   */
  updateRTL(isRTL: boolean): void;
  /**
   * Sends state update request to ComponentNapiBinder.h::updateState to update the state on C++ side.
   */
  updateState(componentName: string, tag: Tag, state: unknown): void;
  /**
   * @returns RNInstance ID.
   */
  getId(): number;

  /**
   * This method can be used to replace a core component with a custom one.
   * @param descriptorType - type of the descriptor
   * @param componentName - value to be provided in ComponentBuilderContext for given `descriptorType`
   */
  bindComponentNameToDescriptorType(componentName: string, descriptorType: string);

  /**
   * Internal method related to bindComponentNameToDescriptorType.
   */
  getComponentNameFromDescriptorType(descriptorType: string): string

  /**
   * @architecture: ArkTS
   * Blocks gestures in targetComponent and its ancestors. Used by react-native-gesture-handler when panning in
   * RNScrollView or other scrollable components.
   * @returns a function that cancels this this effect
   */
  blockComponentsGestures(targetComponentTag: Tag): (() => void)
  /**
   * @returns the first loaded JS Bundle URL. This method is used by SourceCodeTurboModule to generate proper stack traces.
   * Some apps split their bundles to improve the start up performance, hence the word "initial".
   */
  getInitialBundleUrl(): string | undefined
  /**
   * @returns current RNOH architecture name
   */
  getArchitecture(): "ARK_TS" | "C_API"
  /**
   * (Almost) all network request go through HttpClient which can be used to improve logging or unify request handling
   * in hybrid (JS+Native) applications.
   */
  get httpClient(): HttpClient,

  /**
   * @returns base path specifying where to look for bundled assets
   */
  getAssetsDest(): string

  /**
   * @architecture: C-API
   * Sends message to C++ side. Handled by ArkTSMessageHub::Observer or ArkTSMessageHandler.
   */
  postMessageToCpp(name: string, payload: any): void

  /**
   * @architecture: C-API
   * @internal
   */
  setFrameNodeFactory(frameNodeFactory: FrameNodeFactory | null): void

  /**
   * @architecture: C-API
   * Native apps can place RNSurface inside a Scroll component. When that Scroll
   * is being scrolled, RN shouldn't recognize swipe gestures as clicks. To
   * achieve this, RN needs to receive a CANCEL event. At the time of creating
   * this method, the platform didn't send the CANCEL touch event, so RNOH
   * provides a way for RN developers to cancel those events manually. They can
   * register an appropriate onScroll callback and call this method.
   */
  cancelTouches(): void
}

export type RNInstanceOptions = {
  /**
   * Creates RNPackages provided by third-party libraries.
   */
  createRNPackages: (ctx: RNPackageContext) => RNPackage[],
  /**
   * Enables the Hermes debugger. Should be disabled in production environments to avoid performance degradation.
   */
  enableDebugger?: boolean,
  /**
   * UNSTABLE: Enables an additional BACKGROUND thread for layout calculations. This improves performance
   * but increases the risk of deadlocks and crashes. Usually crashes the app due to "Assertion failed: parser was already assigned" error.
   * @deprecated: Do not enable this option. This feature causes too many problems to justify performance improvements (latestRNOHVersion: 0.72.27)
   */
  enableBackgroundExecutor?: boolean,
  /**
   * @architecture: ArkTS
   * Enables text measurement using NDK (C++) interface.
   * The NDK approach is faster and relatively stable, and will become the default in future versions.
   */
  enableNDKTextMeasuring?: boolean,
  /**
   * @architecture: ArkTS
   * Manages image loading and caching. When enabled, RNOH takes responsibility; when disabled, it delegates to ArkUI's Image component.
   * Each approach presents issues under different scenarios.
   */
  enableImageLoader?: boolean,
  /**
   * Toggles between the ArkTS (older, more stable but slower) and C-API (newer, less stable but faster) architectures.
   * Choose based on preference for stability or performance.
   */
  enableCAPIArchitecture?: boolean,
  /**
   * Specifies the path for RN to locate assets. Necessary in production environments where assets are not hosted by the Metro server.
   * Required if using a custom `--assets-dest` with `react-native bundle-harmony`.
   */
  assetsDest?: string,
  /**
   * config the ArkTsComponent names to be use
   */
  arkTsComponentNames: Array<string>,
  /**
   * Specifies the custom backpress handler to be used when RN application does not handle the event itself
   */
  backPressHandler?: () => void;
  /**
   If not provided, the defaultHttpClient created by `RNAbility::onCreateDefaultHttpClient` will be used.
   */
  httpClient?: HttpClient,
  /**
   * Specifies custom fonts used by RN application.
   * @example { "Pacifico-Regular": $rawfile("fonts/Pacifico-Regular.ttf") }
   */
  fontResourceByFontFamily?: Record<string, Resource>
}

/**
 * Used in the C-API architecture
 */
export interface FrameNodeFactory {
  create(tag: Tag, componentName: string);
}


export class RNInstanceImpl implements RNInstance {
  private turboModuleProvider: TurboModuleProvider
  private surfaceCounter = 0;
  private lifecycleState: LifecycleState = LifecycleState.BEFORE_CREATE
  private bundleExecutionStatusByBundleURL: Map<string, BundleExecutionStatus> = new Map()
  public descriptorRegistry: DescriptorRegistry;
  public componentCommandHub: RNComponentCommandHub;
  public componentManagerRegistry: ComponentManagerRegistry;
  public lifecycleEventEmitter = new EventEmitter<LifecycleEventArgsByEventName>();
  public stageEventEmitter = new EventEmitter<StageChangeEventArgsByEventName>();
  public cppEventEmitter = new EventEmitter<Record<string, unknown[]>>()
  public httpClient: HttpClient;
  public backPressHandler: () => void | undefined;
  private componentNameByDescriptorType = new Map<string, string>()
  private logger: RNOHLogger
  private surfaceHandles: Set<SurfaceHandle> = new Set()
  private responderLockDispatcher: ResponderLockDispatcher
  private isFeatureFlagEnabledByName = new Map<FeatureFlagName, boolean>()
  private initialBundleUrl: string | undefined = undefined
  private frameNodeFactoryRef: { frameNodeFactory: FrameNodeFactory | null } = { frameNodeFactory: null };

  /**
   * @deprecated
   */
  public get commandDispatcher() {
    return this.componentCommandHub
  }

  constructor(
    private id: number,
    private injectedLogger: RNOHLogger,
    private napiBridge: NapiBridge,
    private defaultProps: Record<string, any>,
    private devToolsController: DevToolsController,
    private createRNOHContext: (rnInstance: RNInstance) => RNOHContext,
    private shouldEnableDebugger: boolean,
    private shouldEnableBackgroundExecutor: boolean,
    private shouldUseNDKToMeasureText: boolean,
    private shouldUseImageLoader: boolean,
    private shouldUseCApiArchitecture: boolean,
    private assetsDest: string,
    private resourceManager: resourceManager.ResourceManager,
    private arkTsComponentNames: Array<string>,
    private fontFamilyNameByFontPathRelativeToRawfileDir: Record<string, string>,
    httpClientProvider: HttpClientProvider,
    httpClient: HttpClient | undefined, // TODO: remove "undefined" when HttpClientProvider is removed
    backPressHandler: () => void,
  ) {
    this.httpClient = httpClient ?? httpClientProvider.getInstance(this)
    this.logger = injectedLogger.clone("RNInstance")
    this.frameNodeFactoryRef = { frameNodeFactory: null }
    this.backPressHandler = backPressHandler;
    if (this.shouldUseNDKToMeasureText) {
      this.enableFeatureFlag("NDK_TEXT_MEASUREMENTS")
    }
    if (this.shouldUseImageLoader) {
      this.enableFeatureFlag("IMAGE_LOADER")
    }
    if (this.shouldUseCApiArchitecture) {
      this.enableFeatureFlag("C_API_ARCH")
    }
    this.onCreate()
  }

  public getArchitecture() {
    return this.shouldUseCApiArchitecture ? "C_API" : "ARK_TS"
  }

  public getAssetsDest(): string {
    return this.assetsDest ?? DEFAULT_ASSETS_DEST
  }

  public onCreate() {
    this.componentManagerRegistry = new ComponentManagerRegistry(this.injectedLogger);
    this.componentCommandHub = new RNComponentCommandHub();
    this.responderLockDispatcher =
      new ResponderLockDispatcher(this.componentManagerRegistry, this.componentCommandHub, this.injectedLogger)
    this.subscribeToDevTools();
  }

  public async onDestroy() {
    const stopTracing = this.logger.clone("onDestroy").startTracing()
    for (const surfaceHandle of this.surfaceHandles) {
      if (surfaceHandle.isRunning()) {
        this.logger.warn("Destroying instance with running surface with tag: " + surfaceHandle.getTag());
        surfaceHandle.stop();
      }
      surfaceHandle.destroy()
    }
    if (this.isFeatureFlagEnabled("ENABLE_RN_INSTANCE_CLEAN_UP")) {
      this.napiBridge.destroyReactNativeInstance(this.id)
    }
    this.turboModuleProvider.onDestroy()
    stopTracing()
  }

  public getId(): number {
    return this.id;
  }

  enableFeatureFlag(featureFlagName: FeatureFlagName): void {
    this.isFeatureFlagEnabledByName.set(featureFlagName, true)
  }

  isFeatureFlagEnabled(featureFlagName: FeatureFlagName): boolean {
    return this.isFeatureFlagEnabledByName.get(featureFlagName) ?? false
  }

  public async initialize(packages: RNPackage[]) {
    const stopTracing = this.logger.clone("initialize").startTracing()
    const { descriptorWrapperFactoryByDescriptorType, turboModuleProvider } = await this.processPackages(packages)
    this.turboModuleProvider = turboModuleProvider
    this.descriptorRegistry = new DescriptorRegistry(
      {
        '1': { ...rootDescriptor },
      },
      this.updateState.bind(this),
      this,
      descriptorWrapperFactoryByDescriptorType,
      this.logger,
    );
    const cppFeatureFlags: CppFeatureFlag[] = []
    if (this.shouldUseCApiArchitecture) {
      cppFeatureFlags.push("C_API_ARCH")
    }
    if (this.shouldUseNDKToMeasureText) {
      cppFeatureFlags.push("ENABLE_NDK_TEXT_MEASURING")
    }
    this.napiBridge.createReactNativeInstance(
      this.id,
      this.turboModuleProvider,
      this.frameNodeFactoryRef,
      (mutations) => {
        try {
          this.descriptorRegistry.applyMutations(mutations)
        } catch (err) {
          if (typeof err === "string") {
            this.logger.error(err)
          } else if (err instanceof Error) {
            this.logger.error(err.message)
            if (err.stack) {
              this.logger.error(err.stack)
            }
          } else {
            this.logger.error("Unexpected error when applying mutations")
          }
        }
      },
      (tag, commandName, args) => {
        this.componentCommandHub.dispatchCommand(tag, commandName, args)
      },
      (type, payload) => {
        this.onCppMessage(type, payload)
      },
      this.shouldEnableDebugger,
      this.shouldEnableBackgroundExecutor,
      cppFeatureFlags,
      this.resourceManager,
      this.arkTsComponentNames,
      this.fontFamilyNameByFontPathRelativeToRawfileDir,
    )
    stopTracing()
  }

  private onCppMessage(type: string, payload: any) {
    try {
      this.cppEventEmitter.emit(type, payload);
      switch (type) {
        case "SCHEDULER_DID_SET_IS_JS_RESPONDER": {
          if (payload.blockNativeResponder) {
            this.responderLockDispatcher.onBlockResponder(payload.tag, "REACT_NATIVE")
          } else {
            this.responderLockDispatcher.onUnblockResponder("REACT_NATIVE")
          }
          break;
        }
        case "RNOH_ERROR": {
          this.logger.error(new RNOHError({
            whatHappened: payload.message,
            howCanItBeFixed: [],
            customStack: payload.stack,
            extraData: payload.nested,
          }));
          break;
        }
      }
    } catch (err) {
      this.logger.error(new RNOHError({
        whatHappened: `Failed to handle CPP Message: ${type}`,
        howCanItBeFixed: [],
        extraData: err
      }))
    }
  }

  public blockComponentsGestures(tag: Tag) {
    this.responderLockDispatcher.onBlockResponder(tag, "EXTERNAL")
    return () => {
      this.responderLockDispatcher.onUnblockResponder("EXTERNAL")
    }
  }

  private async processPackages(packages: RNPackage[]) {
    const logger = this.logger.clone("processPackages")
    const stopTracing = logger.startTracing()
    packages.unshift(new RNOHCorePackage({}));
    const turboModuleContext = this.createRNOHContext(this)
    const result = {
      descriptorWrapperFactoryByDescriptorType: packages.reduce((acc, pkg) => {
        const descriptorWrapperFactoryByDescriptorType = pkg.createDescriptorWrapperFactoryByDescriptorType({})
        for (const [descriptorType, descriptorWrapperFactory]
          of Object.entries(descriptorWrapperFactoryByDescriptorType)) {
          acc.set(descriptorType, descriptorWrapperFactory)
        }
        return acc
      }, new Map<string, DescriptorWrapperFactory>()),
      turboModuleProvider: new TurboModuleProvider(
        await Promise.all(packages.map(async (pkg, idx) => {
          const pkgDebugName = pkg.getDebugName()
          let traceName = `package${idx + 1}`
          if (pkgDebugName) {
            traceName += `: ${pkgDebugName}`
          }
          logger.clone(traceName).debug("")
          const turboModuleFactory = pkg.createTurboModulesFactory(turboModuleContext);
          await turboModuleFactory.prepareEagerTurboModules()
          return turboModuleFactory
        })),
        this.logger
      )
    }
    stopTracing()
    return result
  }

  public subscribeToLifecycleEvents<TEventName extends keyof LifecycleEventArgsByEventName>(type: TEventName,
                                                                                            listener: (...args: LifecycleEventArgsByEventName[TEventName]) => void) {
    return this.lifecycleEventEmitter.subscribe(type, listener)
  }

  public subscribeToStageChangeEvents<TEventName extends keyof StageChangeEventArgsByEventName>(
    eventName: TEventName,
    listener: (...args: StageChangeEventArgsByEventName[TEventName]) => void
  ): (() => void) {
    return this.stageEventEmitter.subscribe(eventName, listener);
  };

  public getLifecycleState(): LifecycleState {
    return this.lifecycleState
  }

  public callRNFunction(moduleName: string, functionName: string, args: unknown[]): void {
    if (this.lifecycleState === LifecycleState.BEFORE_CREATE) {
      // wait until the bundle has been loaded before calling the function
      const cancel = this.subscribeToLifecycleEvents("JS_BUNDLE_EXECUTION_FINISH", () => {
        this.napiBridge.callRNFunction(this.id, moduleName, functionName, args);
        cancel();
      });
      return;
    }
    this.napiBridge.callRNFunction(this.id, moduleName, functionName, args)
  }

  public emitComponentEvent(tag: Tag, eventEmitRequestHandlerName: string, payload: any) {
    this.napiBridge.emitComponentEvent(this.id, tag, eventEmitRequestHandlerName, payload)
  }

  public emitDeviceEvent(eventName: string, params: any) {
    this.logger.clone(`emitDeviceEvent`).debug(eventName)
    this.callRNFunction("RCTDeviceEventEmitter", "emit", [eventName, params]);
  }

  public getBundleExecutionStatus(bundleURL: string): BundleExecutionStatus | undefined {
    return this.bundleExecutionStatusByBundleURL.get(bundleURL)
  }

  public async runJSBundle(jsBundleProvider: JSBundleProvider) {
    let bundleURL: string
    const stopTracing = this.logger.clone("runJSBundle").startTracing()
    const isMetroServer = jsBundleProvider.getHotReloadConfig() !== null
    try {
      this.devToolsController.eventEmitter.emit("SHOW_DEV_LOADING_VIEW", this.id,
        `Loading from ${jsBundleProvider.getHumanFriendlyURL()}...`)
      this.bundleExecutionStatusByBundleURL.set(bundleURL, "RUNNING")
      const jsBundle = await jsBundleProvider.getBundle((progress) => {
        this.devToolsController.eventEmitter.emit("SHOW_DEV_LOADING_VIEW", this.id,
          `Loading from ${jsBundleProvider.getHumanFriendlyURL()} (${Math.round(progress * 100)}%)`)
      })
      bundleURL = jsBundleProvider.getURL()
      this.initialBundleUrl = this.initialBundleUrl ?? bundleURL
      await this.napiBridge.loadScript(this.id, jsBundle, bundleURL)
      this.napiBridge.setBundlePath(this.id, bundleURL);
      this.lifecycleState = LifecycleState.READY
      const hotReloadConfig = jsBundleProvider.getHotReloadConfig()
      if (hotReloadConfig) {
        this.callRNFunction("HMRClient", "setup",
          ["harmony", hotReloadConfig.bundleEntry, hotReloadConfig.host, hotReloadConfig.port, true])
        this.logger.info("Configured hot reloading")
      }
      const isRemoteBundle = bundleURL.startsWith("http")
      if (this.shouldEnableDebugger && isRemoteBundle) {
        DevServerHelper.connectToDevServer(bundleURL, this.logger, this.napiBridge.getInspectorWrapper());
      }
      this.bundleExecutionStatusByBundleURL.set(bundleURL, "DONE")
      this.lifecycleEventEmitter.emit("JS_BUNDLE_EXECUTION_FINISH", {
        jsBundleUrl: bundleURL,
        appKeys: jsBundleProvider.getAppKeys()
      })
    } catch (err) {
      this.bundleExecutionStatusByBundleURL.delete(bundleURL)
      if (err instanceof JSBundleProviderError) {
        this.logger.error(err)
      } else {
        const suggestions: string[] = []
        if (isMetroServer) {
          suggestions.push("Please check your Metro Server console. Likely, the error details you need are displayed there.")
        }
        suggestions.push("Please revise your application code. It may contain syntax errors or unhandled exceptions at the top level that could be causing runtime failures.")
        this.logger.error(new RNOHError({
          whatHappened: "Couldn't run a JS bundle",
          howCanItBeFixed: suggestions,
          extraData: err,
        }))
      }
    } finally {
      this.devToolsController.eventEmitter.emit("HIDE_DEV_LOADING_VIEW", this.id)
      stopTracing()
    }
  }

  public getTurboModule<T extends TurboModule>(name: string): T {
    return this.turboModuleProvider.getModule(name);
  }

  public createSurface(appKey: string): SurfaceHandle {
    const stopTracing = this.logger.clone("createSurface").startTracing()
    const tag = this.getNextSurfaceTag();
    const result = new SurfaceHandle(this, tag, appKey, this.defaultProps, this.napiBridge,
      (handle) => this.surfaceHandles.delete(handle));
    this.surfaceHandles.add(result)
    stopTracing()
    return result
  }

  public updateRTL(isRTL: boolean): void {
    this.surfaceHandles.forEach((SurfaceHandle) => {
      SurfaceHandle.updateRTL(isRTL);
    })
  }

  public updateState(componentName: string, tag: Tag, state: unknown): void {
    const stopTracing = this.logger.clone("updateState").startTracing()
    this.napiBridge.updateState(this.id, componentName, tag, state)
    stopTracing()
  }

  public onBackPress() {
    this.emitDeviceEvent('hardwareBackPress', {})
  }

  public onForeground() {
    this.lifecycleState = LifecycleState.READY
    this.lifecycleEventEmitter.emit("FOREGROUND")
  }

  public onBackground() {
    this.lifecycleState = LifecycleState.PAUSED
    this.lifecycleEventEmitter.emit("BACKGROUND")
  }

  public onConfigurationUpdate(...args: Parameters<UIAbility["onConfigurationUpdate"]>) {
    this.lifecycleEventEmitter.emit("CONFIGURATION_UPDATE", ...args)
  }

  public onWindowSizeChange(windowSize: window.Size) {
    this.lifecycleEventEmitter.emit("WINDOW_SIZE_CHANGE", windowSize);
    this.postMessageToCpp("WINDOW_SIZE_CHANGE", windowSize);
  }

  public onWindowStageChange(windowStageEvent: window.WindowStageEventType) {
    if (windowStageEvent == window.WindowStageEventType.ACTIVE) {
      this.stageEventEmitter.emit("APP_STATE_FOCUS");
    } else if (windowStageEvent == window.WindowStageEventType.INACTIVE) {
      this.stageEventEmitter.emit("APP_STATE_BLUR");
    }
  }

  private getNextSurfaceTag(): Tag {
    // NOTE: this is done to mirror the iOS implementation.
    // For details, see `RCTAllocateRootViewTag` in iOS implementation.
    return (this.surfaceCounter++ * 10) + 1;
  }

  public shouldUIBeUpdated(): boolean {
    return this.lifecycleState === LifecycleState.READY
  }

  public bindComponentNameToDescriptorType(componentName: string, descriptorType: string): void {
    this.componentNameByDescriptorType.set(descriptorType, componentName)
  }

  public getComponentNameFromDescriptorType(descriptorType: string): string {
    if (this.componentNameByDescriptorType.has(descriptorType)) {
      return this.componentNameByDescriptorType.get(descriptorType)!
    }
    return descriptorType
  }

  public getInitialBundleUrl(): string | undefined {
    return this.initialBundleUrl
  }


  private subscribeToDevTools() {
    const emitter = this.devToolsController.eventEmitter;
    emitter.subscribe("TOGGLE_ELEMENT_INSPECTOR", () => this.emitDeviceEvent("toggleElementInspector", {}))
    emitter.subscribe("DEV_MENU_SHOWN", () => this.emitDeviceEvent("RCTDevMenuShown", {}))
    emitter.subscribe("DID_PRESS_MENU_ITEM", (item) => this.emitDeviceEvent("didPressMenuItem", item))
    emitter.subscribe("OPEN_URL", (url, onError) => {
      DevServerHelper.openUrl(url, this.getInitialBundleUrl(), onError);
    })
  }

  public postMessageToCpp(name: string, payload: any): void {
    this.napiBridge.postMessageToCpp(name, { rnInstanceId: this.id, payload });
  }

  public setFrameNodeFactory(frameNodeFactory: FrameNodeFactory | null) {
    this.frameNodeFactoryRef.frameNodeFactory = frameNodeFactory
  }

  public cancelTouches() {
    this.postMessageToCpp("CANCEL_TOUCHES", { rnInstanceId: this.id })
  }
}

