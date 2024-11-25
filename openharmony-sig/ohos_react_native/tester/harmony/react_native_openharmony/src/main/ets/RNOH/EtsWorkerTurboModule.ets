/**
 * This file location relative to the package root shouldn't change to avoid introducing breaking changes.
 */
import { WorkerTurboModule } from "./TurboModule"
import { FatalRNOHError } from "./RNOHError"
import { WorkerTurboModuleContext } from "./RNOHContext"
import { EtsWorkerTurboModuleContext } from "./EtsRNOHContext"


/**
 * @thread: WORKER
 * @deprecated (latestRNOHVersion: 0.72.38)
 * use AnyThreadTurboModule instead
 */
export class EtsWorkerTurboModule extends WorkerTurboModule {
  protected ctx: EtsWorkerTurboModuleContext

  constructor(ctx: WorkerTurboModuleContext) {
    super(ctx)
    if (!(ctx instanceof EtsWorkerTurboModuleContext)) {
      throw new FatalRNOHError({
        whatHappened: "Received context not compatible with EtsWorkerTurboModule",
        howCanItBeFixed: []
      });
    }
    this.ctx = ctx as EtsWorkerTurboModuleContext;
  }
}