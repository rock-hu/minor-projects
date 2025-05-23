/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

// @ts-ignore
import {NodeContent} from '@ohos.arkui.node';
import display from '@ohos.display';
import ohosResourceManager from '@ohos.resourceManager';
import libRNOHApp from 'librnoh_app.so';

import type {DisplayMode} from './CppBridgeUtils'
import type {Tag} from "./DescriptorBase";
import {JsBundle} from './JSBundleProvider';
import type {Mutation} from "./Mutation";
import type {FrameNodeFactory} from "./RNInstance"
import {FatalRNOHError, RNOHError} from "./RNOHError"
import {RNOHLogger} from "./RNOHLogger"
import type {
    AttributedString, LayoutConstrains, ParagraphAttributes} from
    "./TextLayoutManager";
import {measureParagraph} from "./TextLayoutManager"
import {
    AnyThreadTurboModule,
    UITurboModule,
    UITurboModuleContext,
    WorkerTurboModule,
    WorkerTurboModuleContext
} from './TurboModule';
import type {TurboModuleProvider} from "./TurboModuleProvider";
import type {DisplayMetrics, InspectorInstance} from './types'

export type CppFeatureFlag = "ENABLE_NDK_TEXT_MEASURING" | "C_API_ARCH" | "WORKER_THREAD_ENABLED"

type RawRNOHError = {
  message: string,
  stacktrace?: string[],
  suggestions?: string[]
}

type envINFO = {
  isDebugModeEnabled: boolean,
  envId: number
}

type Result<TOK = null> = {
  ok: TOK,
  err: null
} | {
  ok: null,
  err: RawRNOHError
}

export interface ArkTSBridgeHandler {
  getDisplayMetrics: () => DisplayMetrics
  handleError: (rnohError: RNOHError) => void
  getFoldStatus: () => display.FoldStatus
  getIsSplitScreenMode: () => boolean
  getFontSizeScale: () => number
  getMetadata: (name: string) => string
}

export class NapiBridge {
  private logger: RNOHLogger
  private libRNOHApp: any

  constructor(logger: RNOHLogger) {
    this.libRNOHApp = libRNOHApp;
    this.logger = logger.clone("NapiBridge")
  }

  private unwrapResult<TOk = null>(result: Result<TOk>): TOk {
    if (result.err) {
      throw this.unwrapError(result)
    }
    return result.ok
  }

  private unwrapError(result: Result<unknown>): RNOHError {
    if (!result.err) {
      throw new RNOHError({
        whatHappened: "Called unwrapError on result which doesn't have error",
        howCanItBeFixed: []
      })
    }
    return new RNOHError({
      whatHappened: result.err.message,
      howCanItBeFixed: (result.err.suggestions ?? []),
      customStack: (result.err.stacktrace ?? []).join("\n"),
    })
  }


  onInit(shouldCleanUpRNInstances: boolean, arkTSBridgeHandler: ArkTSBridgeHandler) : envINFO {
    if (!this.libRNOHApp) {
      const err = new FatalRNOHError({
        whatHappened: "Couldn't create bindings between ETS and CPP. libRNOHApp is undefined.",
        howCanItBeFixed: []
      })
      this.logger.fatal(err)
      throw err
    }
    return this.libRNOHApp?.onInit(shouldCleanUpRNInstances, {
      handleError: (err: RawRNOHError) => {
        arkTSBridgeHandler.handleError(new RNOHError({
          whatHappened: err.message,
          howCanItBeFixed: (err.suggestions ?? []),
          customStack: (err.stacktrace ?? []).join("\n"),
        }))
      },
      getDisplayMetrics: () => arkTSBridgeHandler.getDisplayMetrics(),
      getFoldStatus: () => arkTSBridgeHandler.getFoldStatus(),
      getIsSplitScreenMode: () => arkTSBridgeHandler.getIsSplitScreenMode(),
      getFontSizeScale: () => arkTSBridgeHandler.getFontSizeScale(),
      getMetadata: (name: string) => arkTSBridgeHandler.getMetadata(name),
    } satisfies ArkTSBridgeHandler)
  }

  registerWorkerTurboModuleProvider(turboModuleProvider: TurboModuleProvider<WorkerTurboModule | AnyThreadTurboModule>,
    rnInstanceId: number) {
    this.libRNOHApp?.registerWorkerTurboModuleProvider(turboModuleProvider, rnInstanceId)
  }

  getNextRNInstanceId(): number {
    return this.libRNOHApp?.getNextRNInstanceId();
  }

  setBundlePath(instanceId: number, path: string): void {
    this.libRNOHApp?.setBundlePath(instanceId, path);
  }

  onCreateRNInstance(
    envId: number,
    instanceId: number,
    turboModuleProvider: TurboModuleProvider<UITurboModule | AnyThreadTurboModule>,
    frameNodeFactoryRef: { frameNodeFactory: FrameNodeFactory | null },
    mutationsListener: (mutations: Mutation[]) => void,
    componentCommandsListener: (tag: Tag,
      commandName: string,
      args: unknown) => void,
    onCppMessage: (type: string, payload: any) => void,
    shouldEnableDebugger: boolean,
    shouldEnableBackgroundExecutor: boolean,
    cppFeatureFlags: CppFeatureFlag[],
    resourceManager: ohosResourceManager.ResourceManager,
    arkTsComponentNames: Array<string>,
    fontPathByFontFamily: Record<string, string>
  ) {
    const cppFeatureFlagStatusByName = cppFeatureFlags.reduce((acc, cppFeatureFlag) => {
      acc[cppFeatureFlag] = true
      return acc
    }, {} as Record<CppFeatureFlag, boolean>)
    this.libRNOHApp?.onCreateRNInstance(
      instanceId,
      turboModuleProvider,
      mutationsListener,
      componentCommandsListener,
      onCppMessage,
      (attributedString: AttributedString, paragraphAttributes: ParagraphAttributes,
       layoutConstraints: LayoutConstrains) => {
        try {
          const stopTracing = this.logger.clone("measureParagraph").startTracing()
          const result = measureParagraph(attributedString, paragraphAttributes, layoutConstraints)
          stopTracing()
          return result
        } catch (err) {
          console.error(err)
          throw err
        }
      },
      shouldEnableDebugger,
      shouldEnableBackgroundExecutor,
      cppFeatureFlagStatusByName,
      frameNodeFactoryRef,
      resourceManager,
      arkTsComponentNames,
      fontPathByFontFamily,
      envId
    );
  }

  onDestroyRNInstance(instanceId: number) {
    this.libRNOHApp?.onDestroyRNInstance(instanceId)
  }

  emitComponentEvent(instanceId: number, tag: Tag, eventEmitRequestHandlerName: string, payload: any) {
    this.libRNOHApp?.emitComponentEvent(instanceId, tag, eventEmitRequestHandlerName,
      payload);
  }

  loadScript(
      instanceId: number,
      bundle: JsBundle,
      sourceURL:
          string): Promise<void>{return new Promise((resolve, reject) => {
      this.libRNOHApp?.loadScript(instanceId, bundle, sourceURL, (errorMsg: string) => {
        errorMsg ? reject(new Error(errorMsg)) : resolve()
      });
  })}

  startSurface(
    instanceId: number,
    surfaceTag: number,
    surfaceMinWidth: number,
    surfaceMinHeight: number,
    surfaceMaxWidth: number,
    surfaceMaxHeight: number,
    surfaceOffsetX: number,
    surfaceOffsetY: number,
    pixelRatio: number,
    isRTL: boolean,
    initialProps: any) {
    this.libRNOHApp?.startSurface(
      instanceId,
      surfaceTag,
      surfaceMinWidth,
      surfaceMinHeight,
      surfaceMaxWidth,
      surfaceMaxHeight,
      surfaceOffsetX,
      surfaceOffsetY,
      pixelRatio,
      isRTL,
      initialProps,
    );
  }

  updateSurfaceConstraints(
    instanceId: number,
    surfaceTag: number,
    surfaceMinWidth: number,
    surfaceMinHeight: number,
    surfaceMaxWidth: number,
    surfaceMaxHeight: number,
    surfaceOffsetX: number,
    surfaceOffsetY: number,
    pixelRatio: number,
    isRTL: boolean,
  ) {
    this.libRNOHApp?.updateSurfaceConstraints(
      instanceId,
      surfaceTag,
      surfaceMinWidth,
      surfaceMinHeight,
      surfaceMaxWidth,
      surfaceMaxHeight,
      surfaceOffsetX,
      surfaceOffsetY,
      pixelRatio,
      isRTL
    );
  }

  measureSurface(
    instanceId: number,
    surfaceTag: number,
    surfaceMinWidth: number,
    surfaceMinHeight: number,
    surfaceMaxWidth: number,
    surfaceMaxHeight: number,
    surfaceOffsetX: number,
    surfaceOffsetY: number,
    pixelRatio: number,
    isRTL: boolean,
  ) {
    const result = this.libRNOHApp?.measureSurface(
      instanceId,
      surfaceTag,
      surfaceMinWidth,
      surfaceMinHeight,
      surfaceMaxWidth,
      surfaceMaxHeight,
      surfaceOffsetX,
      surfaceOffsetY,
      pixelRatio,
      isRTL
    );
    return this.unwrapResult<{ width: number, height: number }>(result)
  }

  createSurface(
    instanceId: number,
    surfaceTag: number,
    appKey: string,
  ) {
    this.libRNOHApp?.createSurface(
      instanceId,
      surfaceTag,
      appKey,
    );
  }

  setSurfaceProps(
    instanceId: number,
    surfaceTag: number,
    props: Record<string, any>,
  ) {
    this.libRNOHApp?.setSurfaceProps(
      instanceId,
      surfaceTag,
      props,
    )
  }

  async stopSurface(
    instanceId: number,
    surfaceTag: number,
  ) {
    let resolveWait = () => {
    }
    const wait = new Promise<void>((resolve) => {
      resolveWait = () => {
        resolve()
      }
    })
    this.libRNOHApp?.stopSurface(
      instanceId,
      surfaceTag,
      () => resolveWait()
    );
    await wait;
  }

  async destroySurface(
    instanceId: number,
    surfaceTag: number,
  ) {
    this.libRNOHApp?.destroySurface(
      instanceId,
      surfaceTag
    );
  }

  setSurfaceDisplayMode(instanceId: number, surfaceTag: Tag, displayMode: DisplayMode): void {
    this.libRNOHApp?.setSurfaceDisplayMode(instanceId, surfaceTag, displayMode);
  }

  callRNFunction(instanceId: number, moduleName: string, functionName: string, args: unknown[]): void {
    this.libRNOHApp?.callRNFunction(instanceId, moduleName, functionName, args);
  }

  onMemoryLevel(level: number): void {
    this.libRNOHApp?.onMemoryLevel(level)
  }

  updateState(instanceId: number, componentName: string, tag: Tag, state: unknown): void {
    this.libRNOHApp?.updateState(instanceId, componentName, tag, state)
  }

  getInspectorWrapper(): InspectorInstance {
    return this.libRNOHApp?.getInspectorWrapper();
  }

  initializeArkTSBridge(handler: ArkTSBridgeHandler) {
    this.libRNOHApp?.initializeArkTSBridge({
      getDisplayMetrics: () => handler.getDisplayMetrics(),
      handleError: (errData: {
        message: string,
        stacktrace?: string[],
        suggestions?: string[]
      }) => {
        handler.handleError(new RNOHError({
          whatHappened: errData.message,
          howCanItBeFixed: (errData.suggestions ?? []),
          customStack: (errData.stacktrace ?? []).join("\n"),
        }))
      },
      getFoldStatus: () => handler.getFoldStatus(),
      getIsSplitScreenMode: () => handler.getIsSplitScreenMode(),
      getFontSizeScale: () => handler.getFontSizeScale(),
      getMetadata: (name: string) => handler.getMetadata(name)
    });
  }

  postMessageToCpp(name: string, payload: any) {
    this.libRNOHApp?.onArkTSMessage(name, payload)
  }

  logMarker(markerId: string, rnInstanceId: number) {
    this.libRNOHApp?.logMarker(markerId, rnInstanceId)
  }

  registerFont(instanceId: number, fontFamily: string, path: string) {
    return this.unwrapResult(this.libRNOHApp?.registerFont(instanceId, fontFamily, path));
  }
  getNativeNodeIdByTag(instanceId: number, tag: Tag): string | undefined {
    const result = this.libRNOHApp?.getNativeNodeIdByTag(instanceId, tag)
    return this.unwrapResult(result);
  }
  attachRootView(instanceId: number, surfaceId: number, nodeContent: NodeContent) {
    const result = this.libRNOHApp?.attachRootView(instanceId, surfaceId, nodeContent)
    return this.unwrapResult(result);
  }

  detachRootView(instanceId: number, surfaceId: number) {
    const result = this.libRNOHApp?.detachRootView(instanceId, surfaceId)
    return this.unwrapResult(result);
  }
}
