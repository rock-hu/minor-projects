import type {
  AnyThreadTurboModule,
  UITurboModule,
  UITurboModuleContext,
  WorkerTurboModule,
  WorkerTurboModuleContext,
} from './TurboModule';
import type {DescriptorWrapperFactory} from './DescriptorRegistry';
import {AnyThreadTurboModuleContext} from './RNOHContext';

/**
 * @internal
 * @deprecated This class no longer belongs to the native api. Use UITurboModuleFactory or WorkerTurboModuleFactory instead. (latestRNOHVersion: 0.72.31)
 * By default, the TTurboModule and TTurboModuleContext types are UITurboModule and UITurboModuleContext, respectively, for backward compatibility reasons.
 */
export abstract class TurboModulesFactory<
  TTurboModule = UITurboModule,
  TTurboModuleContext = UITurboModuleContext,
> {
  constructor(protected ctx: TTurboModuleContext) {}

  abstract createTurboModule(name: string): TTurboModule | null;

  prepareEagerTurboModules(): Promise<void> {
    return Promise.resolve();
  }

  abstract hasTurboModule(name: string): boolean;
}

class TurboModulesFactoryFromPackageAdapter<
  TTurboModule,
  TTurboModuleContext,
> extends TurboModulesFactory<TTurboModule, TTurboModuleContext> {
  constructor(
    ctx: TTurboModuleContext,
    private factoriesByName: Map<
      string,
      TurboModuleFactory<TTurboModule, TTurboModuleContext>
    >,
    private prepareEagerTurboModulesImpl: () => Promise<
      Map<string, TTurboModule>
    > = async () => new Map(),
  ) {
    super(ctx);
  }

  createTurboModule(name: string) {
    const turboModuleFactory = this.factoriesByName.get(name);
    return turboModuleFactory?.(this.ctx) ?? null;
  }

  hasTurboModule(name: string) {
    return this.factoriesByName.has(name);
  }

  async prepareEagerTurboModules(): Promise<void> {
    const turboModules = await this.prepareEagerTurboModulesImpl();
    for (const [name, turboModule] of turboModules.entries()) {
      this.factoriesByName.set(name, () => turboModule);
    }
  }
}

/**
 * @api
 * This context provides a way to inject dependencies in the future without in non-breaking changes manner.
 */
export type RNPackageContext = {};

export type DescriptorWrapperFactoryByDescriptorTypeCtx = {};

export type DescriptorWrapperFactoryByDescriptorType = Record<
  string,
  DescriptorWrapperFactory
>;

/**
 * @deprecated (latestRNOHVersion: 0.72.38)
 */
export abstract class UITurboModuleFactory extends TurboModulesFactory<
  UITurboModule,
  UITurboModuleContext
> {}

/**
 * @deprecated (latestRNOHVersion: 0.72.38)
 */
export abstract class WorkerTurboModuleFactory extends TurboModulesFactory<
  WorkerTurboModule,
  WorkerTurboModuleContext
> {}

/**
 * @internal
 */
export abstract class AnyThreadTurboModuleFactory extends TurboModulesFactory<
  AnyThreadTurboModule,
  AnyThreadTurboModuleContext
> {}

type TurboModuleFactory<TTurboModule, TTurboModuleContext> = (
  ctx: TTurboModuleContext,
) => TTurboModule | null;

export abstract class RNPackage {
  constructor(protected ctx: RNPackageContext) {}

  /**
   * @deprecated (latestRNOHVersion: 0.72.38)
   * Use createUITurboModuleFactory instead.
   */
  createTurboModulesFactory(ctx: UITurboModuleContext): TurboModulesFactory {
    return new TurboModulesFactoryFromPackageAdapter(ctx, new Map());
  }

  /**
   * @deprecated (latestRNOHVersion: 0.72.38)
   * Use getUITurboModuleFactoryByNameMap and createEagerUITurboModuleByNameMap instead
   */
  createUITurboModuleFactory(ctx: UITurboModuleContext): UITurboModuleFactory {
    return new TurboModulesFactoryFromPackageAdapter(
      ctx,
      this.getUITurboModuleFactoryByNameMap(),
      () => this.createEagerUITurboModuleByNameMap(ctx),
    );
  }

  /**
   * @deprecated (latestRNOHVersion: 0.72.38)
   * Use getAnyThreadTurboModuleFactoryByNameMap and createEagerAnyThreadTurboModuleByNameMap instead
   */
  createWorkerTurboModuleFactory(
    ctx: WorkerTurboModuleContext,
  ): WorkerTurboModuleFactory | null {
    return null;
  }

  /**
   * @internal
   */
  createAnyThreadTurboModuleFactory(
    ctx: AnyThreadTurboModuleContext,
  ): AnyThreadTurboModuleFactory {
    return new TurboModulesFactoryFromPackageAdapter(
      ctx,
      this.getAnyThreadTurboModuleFactoryByNameMap(),
      () => this.createEagerAnyThreadTurboModuleByNameMap(ctx),
    );
  }

  getDebugName(): string | undefined {
    return undefined;
  }

  /**
   * @architecture: ArkTS
   */
  createDescriptorWrapperFactoryByDescriptorType(
    ctx: DescriptorWrapperFactoryByDescriptorTypeCtx,
  ): DescriptorWrapperFactoryByDescriptorType {
    return {};
  }

  getUITurboModuleFactoryByNameMap(): Map<
    string,
    TurboModuleFactory<UITurboModule, UITurboModuleContext>
  > {
    return new Map();
  }

  async createEagerUITurboModuleByNameMap(
    ctx: UITurboModuleContext,
  ): Promise<Map<string, UITurboModule>> {
    return new Map();
  }

  /*
   * TurboModules created by factories returned from this method will be run
   * on WorkerThread if TurboModule worker is enabled by the application developer
   * and on the UI thread otherwise.
   * RNOH tries to load TurboModules in the following order:
   * 1. AnyThreadTurboModule (ArkTS, WorkerThread or UI)
   * 2. UITurboModules (ArkTS, UI)
   * 3. CppTurboModules (JS thread)
   */
  getAnyThreadTurboModuleFactoryByNameMap(): Map<
    string,
    TurboModuleFactory<AnyThreadTurboModule, AnyThreadTurboModuleContext>
  > {
    return new Map();
  }

  async createEagerAnyThreadTurboModuleByNameMap(
    ctx: AnyThreadTurboModuleContext,
  ): Promise<Map<string, AnyThreadTurboModule>> {
    return new Map();
  }
}
