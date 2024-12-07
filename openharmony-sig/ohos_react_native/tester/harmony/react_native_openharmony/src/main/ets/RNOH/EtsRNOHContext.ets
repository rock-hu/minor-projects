import type { lang } from '@kit.ArkTS';
import { WorkerTurboModuleContext } from "./RNOHContext"
import type { SupportedUITaskParams, UITaskRunnable, UITaskRunner } from "./UITaskRunner"
import type { WorkerTurboModuleContextDependencies } from "./RNOHContext"
import { RNOHContext } from "./RNOHContext"
import { DescriptorWrapper, Tag } from "./DescriptorBase"
import { RNComponentDataSource } from './RNComponentDataSource'

/**
 * @internal
 */
export interface EtsWorkerTurboModuleContextDependencies {
  workerTurboModuleContextDependencies: WorkerTurboModuleContextDependencies
  uiTaskRunner: UITaskRunner
}

/**
 * @api
 */
export class EtsWorkerTurboModuleContext extends WorkerTurboModuleContext {
  private deps: EtsWorkerTurboModuleContextDependencies

  /**
   * @internal
   */
  constructor(deps: EtsWorkerTurboModuleContextDependencies) {
    super(deps.workerTurboModuleContextDependencies)
    this.deps = deps
  }

  async runOnUIThread<TParams extends SupportedUITaskParams, TResult extends lang.ISendable | void, TRunnable extends UITaskRunnable<TParams, TResult>>(runnable: TRunnable,
    params: TParams): Promise<TResult> {
    return this.deps.uiTaskRunner.run(runnable, params) as Promise<TResult>
  }
}


/**
 * @api
 * @deprecated This will be removed with ComponentBuilderContext::descriptor (latestRNOHVersion: 0.72.27)
 */
interface DescriptorEssence {
  tag: Tag,
  type: string
}

/**
 * @api
 */
export type CustomComponentBuilder = (ctx: ComponentBuilderContext) => void

/**
 * @api
 */
export type WrappedCustomRNComponentBuilder = WrappedBuilder<[ComponentBuilderContext]>

/**
 * @api
 */
export type WrappedRNChildrenBuilder = WrappedBuilder<[RNComponentContext, Tag]>

/**
 * @api
 */
export type WrappedRNComponentBuilder = WrappedBuilder<[RNComponentContext, Tag]>

/**
 * @api
 */
export interface RNComponentDataSourceFactoryContext {
  tag: Tag
}

/**
 * @api
 */
export type RNComponentDataSourceFactory = (ctx: RNComponentDataSourceFactoryContext) => RNComponentDataSource

/**
 * @api
 */
export class RNComponentContext extends RNOHContext {
  public wrappedCustomRNComponentBuilder: WrappedCustomRNComponentBuilder
  public wrappedRNChildrenBuilder: WrappedRNChildrenBuilder
  public wrappedRNComponentBuilder: WrappedRNComponentBuilder
  private dataSourceFactoriesByType: Map<string, RNComponentDataSourceFactory>

  public createComponentDataSource(ctx: RNComponentDataSourceFactoryContext) {
    const descriptorWrapper = this.descriptorRegistry.findDescriptorWrapperByTag(ctx.tag);
    const createComponentDataSource = descriptorWrapper && this.dataSourceFactoriesByType.get(descriptorWrapper.type_);
    return createComponentDataSource ? createComponentDataSource(ctx) : new RNComponentDataSource(this, ctx.tag);
  }

  constructor(
    rnohContext: RNOHContext,
    wrappedCustomRNComponentBuilder: WrappedCustomRNComponentBuilder,
    wrappedRNComponentBuilder: WrappedRNComponentBuilder,
    rnComponentDataSourceFactoriesByDescriptorType: Map<string, RNComponentDataSourceFactory>
  ) {
    super(
      rnohContext._rnohContextDeps
    )
    this.wrappedCustomRNComponentBuilder = wrappedCustomRNComponentBuilder
    this.wrappedRNChildrenBuilder = wrapBuilder(buildRNChildren)
    this.wrappedRNComponentBuilder = wrappedRNComponentBuilder
    this.dataSourceFactoriesByType = rnComponentDataSourceFactoriesByDescriptorType
  }
}

@Builder
function buildRNChildren(ctx: RNComponentContext, parentTag: Tag) {
  Stack() {
    LazyForEach(ctx.createComponentDataSource({ tag: parentTag }),
      (descriptorWrapper: DescriptorWrapper) => {
        ctx.wrappedRNComponentBuilder.builder(ctx, descriptorWrapper.tag)
      },
      (descriptorWrapper: DescriptorWrapper) => descriptorWrapper.tag.toString() + "@" + descriptorWrapper.renderKey
    )
  }
}

/**
 * @api: RN_APP_DEVELOPER
 */
export class ComponentBuilderContext {
  public rnComponentContext: RNComponentContext
  /**
   * @deprecated Use `rnComponentContext` instead. (latestRNOHVersion: 0.72.27)
   */
  public rnohContext: RNOHContext
  public tag: Tag
  public componentName: string
  /**
   * @deprecated: Use tag and componentName instead. (latestRNOHVersion: 0.72.27)
   */
  public descriptor: DescriptorEssence

  /**
   * @internal
   */
  constructor(rnComponentContext: RNComponentContext, rnohContext: RNOHContext, tag: Tag, componentName: string,
    descriptor: DescriptorEssence) {
    this.rnComponentContext = rnComponentContext
    this.rnohContext = rnohContext
    this.tag = tag
    this.componentName = componentName
    this.descriptor = descriptor
  }
}