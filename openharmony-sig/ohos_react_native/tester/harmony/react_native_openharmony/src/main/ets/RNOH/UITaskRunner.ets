/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

/**
 * This file location relative to the package root shouldn't change to avoid introducing breaking changes.
 */
import { lang } from '@kit.ArkTS';
import { UITurboModuleContext } from "./TurboModule"
import { RNOHError } from "./RNOHError"

type ISendable = lang.ISendable

/**
 * @api
 */
export type SupportedUITaskParams = ISendable | undefined

/**
 * @api
 */
export interface UITaskRunnable<TParams extends SupportedUITaskParams = undefined, TResult extends ISendable | void = void> extends ISendable {
  run(uiTurboModuleContext: UITurboModuleContext, params: TParams): TResult;
}

/**
 * @internal
 */
@Sendable
export class UITask<TUITaskRunnable extends UITaskRunnable<lang.ISendable | undefined> = UITaskRunnable<lang.ISendable | undefined>, TParams extends lang.ISendable | undefined = lang.ISendable | undefined> {
  id: string
  runnable: TUITaskRunnable;
  params: TParams;

  constructor(id: string, runnable: TUITaskRunnable, params: TParams) {
    this.id = id
    this.runnable = runnable
    this.params = params;
  }
}

/**
 * @internal
 * @thread: WORKER
 */
export class UITaskRunner {
  private taskResultResolverByTaskId =
    new Map<string, (result: lang.ISendable) => void>()
  private id: string
  private nextTaskId = 0;
  private onScheduleUITask: (uiTask: UITask) => void;

  constructor(id: string, onScheduleUITask: (uiTask: UITask) => void) {
    this.id = id
    this.onScheduleUITask = onScheduleUITask
  }

  run<TParams extends SupportedUITaskParams, TResult extends lang.ISendable | void, TRunnable extends UITaskRunnable<TParams, TResult>>(runnable: TRunnable,
    params: TParams): Promise<TResult> {
    const uiTask = new UITask(`${this.id}::${this.nextTaskId++}`, runnable, params);
    this.onScheduleUITask(uiTask)
    const promisedResultAndPromiseResolver = createPromiseAndResolver();
    this.taskResultResolverByTaskId.set(uiTask.id, promisedResultAndPromiseResolver[1])
    return promisedResultAndPromiseResolver[0] as Promise<TResult>;
  }

  resolve(taskId: string, result: lang.ISendable) {
    const resolveResult = this.taskResultResolverByTaskId.get(taskId)
    if (!resolveResult) {
      throw new RNOHError({
        whatHappened: "Tried to resolve unknown task result " +
        JSON.stringify({ taskId, registeredTaskIds: Array.from(this.taskResultResolverByTaskId.keys()) }),
        howCanItBeFixed: []
      })
    }
    resolveResult(result);
    this.taskResultResolverByTaskId.delete(taskId)
  }
}

function createPromiseAndResolver<T = Object | undefined>(): [Promise<T>, ((result: T) => void)] {
  let resolve: ((result: T) => void) = (value: T) => {
  }
  const promise = new Promise<T>((_resolve) => resolve = _resolve)
  return [promise, resolve]
}