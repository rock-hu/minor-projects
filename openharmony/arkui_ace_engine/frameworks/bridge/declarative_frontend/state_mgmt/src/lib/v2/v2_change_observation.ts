/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/**
 *
 * This file includes only framework internal classes and functions
 * non are part of SDK. Do not access from app.
 *
 *
 * ObserveV2 is the singleton object for observing state variable access and
 * change
 */

// stackOfRenderedComponentsItem[0] and stackOfRenderedComponentsItem[1] is faster than
// the stackOfRenderedComponentsItem.id and the stackOfRenderedComponentsItem.cmp.
// So use the array to keep id and cmp.
type StackOfRenderedComponentsItem = [number, MonitorV2 | ComputedV2 | PersistenceV2Impl | ViewBuildNodeBase];

// in the case of ForEach, Repeat, AND If, two or more UINodes / elementIds can render at the same time
// e.g. ForEach -> ForEach child Text, Repeat -> Nested Repeat, child Text
// Therefore, ObserveV2 needs to keep a stack of currently rendering ids / components
// in the same way as this is also done for PU stateMgmt with ViewPU.currentlyRenderedElmtIdStack_
class StackOfRenderedComponents {
  private stack_: Array<StackOfRenderedComponentsItem> = new Array<StackOfRenderedComponentsItem>();

  public push(id: number, cmp: MonitorV2 | ComputedV2 | PersistenceV2Impl | ViewBuildNodeBase): void {
    this.stack_.push([id, cmp]);
  }

  public pop(): StackOfRenderedComponentsItem | undefined {
    return this.stack_.pop();
  }

  public top(): StackOfRenderedComponentsItem | undefined {
    return this.stack_.length ? this.stack_[this.stack_.length - 1] : undefined;
  }
}

class ObserveV2 {
  // meta data about decorated variable inside prototype
  public static readonly V2_DECO_META = Symbol('__v2_deco_meta__');

  // meta data about decorated accessor and method: @Computed and @Monitor
  public static readonly V2_DECO_METHOD_META = Symbol('__v2_deco_method_meta__');

  public static readonly SYMBOL_REFS = Symbol('__use_refs__');
  public static readonly ID_REFS = Symbol('__id_refs__');
  public static readonly MONITOR_REFS = Symbol('___monitor_refs_');
  public static readonly ADD_MONITOR_REFS = Symbol('___monitor_refs_');
  public static readonly COMPUTED_REFS = Symbol('___computed_refs_');

  public static readonly SYMBOL_PROXY_GET_TARGET = Symbol('__proxy_get_target');

  public static readonly SYMBOL_MAKE_OBSERVED = Symbol('___make_observed__');

  public static readonly OB_PREFIX = '__ob_'; // OB_PREFIX + attrName => backing store attribute name
  public static readonly OB_PREFIX_LEN = 5;
  public static readonly NO_REUSE = -1; // mark no reuse on-going
  // used by array Handler to create dependency on artificial 'length'
  // property of array, mark it as changed when array has changed.
  public static readonly OB_LENGTH = '___obj_length';

  private static setMapProxy: SetMapProxyHandler = new SetMapProxyHandler();
  private static arrayProxy: ArrayProxyHandler = new ArrayProxyHandler();
  private static objectProxy: ObjectProxyHandler = new ObjectProxyHandler();

  // see MonitorV2.observeObjectAccess: bindCmp is the MonitorV2
  // see modified ViewV2 and ViewPU observeComponentCreation, bindCmp is the ViewV2 or ViewPU

  // bindId: UINode elmtId or watchId, depending on what is being observed
  private stackOfRenderedComponents_: StackOfRenderedComponents = new StackOfRenderedComponents();

  // Map bindId to WeakRef<ViewBuildNodeBase>
  public id2cmp_: { number: WeakRef<ViewBuildNodeBase> } = {} as { number: WeakRef<ViewBuildNodeBase> };

  // Map bindId to WeakRef<MonitorV2 | ComputedV2 | PersistenceV2Impl>
  public id2Others_: { number: WeakRef<MonitorV2 | ComputedV2 | PersistenceV2Impl> } = {} as { number: WeakRef<MonitorV2 | ComputedV2 | PersistenceV2Impl> };

  // Map bindId -> Set of @ObservedV2 class objects
  // reverse dependency map for quickly removing all dependencies of a bindId
  private id2targets_: { number: Set<WeakRef<Object>> } = {} as { number: Set<WeakRef<Object>> };

  // Queue of tasks to run in next idle period (used for optimization)
  public idleTasks_: (Array<[(...any: any[]) => any, ...any[]]> & { first: number, end: number }) =
    Object.assign(Array(1000).fill([]), { first: 0, end: 0 });
  public static readonly idleTasksInitLength = 1000;

  // queued up Set of bindId
  // elmtIds of UINodes need re-render
  // @monitor functions that need to execute
  public elmtIdsChanged_: Set<number> = new Set();
  // @Computed id
  private computedPropIdsChanged_: Set<number> = new Set();
  // AddMonitor API
  private monitorIdsChangedForAddMonitor_: Set<number> = new Set();
  // Sync AddMonitor API
  private monitorSyncIdsChangedForAddMonitor_: Set<number> = new Set();
  // @Monitor id
  private monitorIdsChanged_: Set<number> = new Set();
  private persistenceChanged_: Set<number> = new Set();
  // used for Monitor API
  // only store the MonitorV2 id, not the path id
  // to make sure the callback function will be executed only once
  public monitorFuncsToRun_: Set<number> = new Set();

  // ViewV2s Grouped by instance id (container id)
  private viewV2NeedUpdateMap_: Map<number, Map<ViewV2 | ViewPU, Array<number>>> = new Map();

  // To avoid multiple schedules on the same container
  private scheduledContainerIds_: Set<number> = new Set();

  // avoid recursive execution of updateDirty
  // by state changes => fireChange while
  // UINode rerender or @monitor function execution
  private startDirty_: boolean = false;

  // flag to indicate change observation is disabled
  private disabled_: boolean = false;
  // flag to indicate dependency recording is disabled
  private disableRecording_: boolean = false;

  // flag to indicate ComputedV2 calculation is ongoing
  private calculatingComputedProp_: boolean = false;

  // use for mark current reuse id, ObserveV2.NO_REUSE(-1) mean no reuse on-going
  protected currentReuseId_: number = ObserveV2.NO_REUSE;

  // flag to disable nested component optimization if V1 and V2 components are involved in the nested cases.
  public isParentChildOptimizable_ : boolean = true;

  private static obsInstance_: ObserveV2;

  public static getObserve(): ObserveV2 {
    if (!this.obsInstance_) {
      this.obsInstance_ = new ObserveV2();
    }
    return this.obsInstance_;
  }

  // return true given value is @ObservedV2 object
  public static IsObservedObjectV2(value: any): boolean {
    return (value && typeof (value) === 'object' && value[ObserveV2.V2_DECO_META]);
  }

  // return true if given value is proxied observed object, either makeObserved or autoProxyObject
  public static IsProxiedObservedV2(value: any): boolean {
    return (value && typeof value === 'object' && value[ObserveV2.SYMBOL_PROXY_GET_TARGET]);
  }

  // return true given value is the return value of makeObserved
  public static IsMakeObserved(value: any): boolean {
    return (value && typeof (value) === 'object' && value[ObserveV2.SYMBOL_MAKE_OBSERVED]);
  }

  public static getCurrentRecordedId(): number {
    const bound = ObserveV2.getObserve().stackOfRenderedComponents_.top();
    return bound ? bound[0] : -1;
  }

  // At the start of observeComponentCreation or
  // MonitorV2 observeObjectAccess
  public startRecordDependencies(cmp: MonitorV2 | ComputedV2 | PersistenceV2Impl | ViewBuildNodeBase, id: number, doClearBinding: boolean = true): void {
    if (cmp != null) {
      doClearBinding && this.clearBinding(id);
      this.stackOfRenderedComponents_.push(id, cmp);
    }
  }

  // At the start of observeComponentCreation or
  // MonitorV2 observeObjectAccess
  public stopRecordDependencies(): void {
    const bound = this.stackOfRenderedComponents_.pop();
    if (bound === undefined) {
      stateMgmtConsole.error('stopRecordDependencies finds empty stack. Internal error!');
      return;
    }

    // only add IView | MonitorV2 | ComputedV2 if at least one dependency was
    // recorded when rendering this ViewPU/ViewV2/Monitor/ComputedV2
    // ViewPU is the likely case where no dependency gets recorded
    // for others no dependencies are unlikely to happen

    // once set, the value remains unchanged
    let id: number = bound[0];
    let cmp: MonitorV2 | ComputedV2 | PersistenceV2Impl | ViewBuildNodeBase = bound[1];

    // element id can be registered from id2cmp in aboutToBeDeletedInternal and unregisterElmtIdsFromIViews
    // PersistenceV2Impl instance is Singleton
    if (cmp instanceof ViewBuildNodeBase || cmp instanceof PersistenceV2Impl) {
      this.id2cmp_[id] = new WeakRef<ViewBuildNodeBase | PersistenceV2Impl>(cmp);
      return;
    } 
    const weakRef = WeakRefPool.get(cmp);
    // this instance, which maybe MonitorV2/ComputedV2 have been already recorded in id2Others
    if (this.id2Others_[id] === weakRef) {
      return;
    }
    this.id2Others_[id] = weakRef;
    // register MonitorV2/ComputedV2 instance gc-callback func
    WeakRefPool.register(cmp, id, () => {
      delete this.id2Others_[id];
    });

  }

  // clear any previously created dependency view model object to elmtId
  // find these view model objects with the reverse map id2targets_
  public clearBinding(id: number): void {
    if (this.idleTasks_) {
      if (this.idleTasks_.end - this.idleTasks_.first > ObserveV2.idleTasksInitLength) {
        ObserveV2.getObserve().runIdleTasks();
      }
      this.idleTasks_[this.idleTasks_.end++] = [this.clearBindingInternal, id];
    } else {
      this.clearBindingInternal(id);
    }
  }

  private clearBindingInternal(id: number): void {
    this.id2targets_[id]?.forEach((weakRef: WeakRef<Object>) => {
      const target = weakRef.deref();
      const idRefs: Object | undefined = target?.[ObserveV2.ID_REFS];
      const symRefs: Object = target?.[ObserveV2.SYMBOL_REFS];

      if (idRefs) {
        idRefs[id]?.forEach(key =>
          symRefs?.[key]?.delete(id)
        );
        delete idRefs[id];
      } else {
        for (let key in symRefs) {
          symRefs[key]?.delete(id);
        };
      }

      if (target) {
        WeakRefPool.unregister(target, id);
      }
    });

    delete this.id2targets_[id];

    stateMgmtConsole.propertyAccess(`... id2targets_ length=${Object.keys(this.id2targets_).length}, entries=${JSON.stringify(Object.keys(this.id2targets_))} `);
  }

  /**
   *
   * According to JS specifications, it is up to ArlTS runtime GC implementation when to collect unreferences objects.
   * Parameters such as available memory, ArkTS processing load, number and size of all JS objects for GC collection
   * can impact the time delay between an object loosing last reference and GC collecting this object.
   *
   * WeakRef deref() returns the object until GC has collected it.
   * The id2cmp and is2targets cleanup herein depends on WeakRef.deref() to return undefined, i.e. it depends on GC
   * collecting 'cmp' or 'target' objects. Only then the algorithm can remove the entry from id2cmp / from id2target.
   * It is therefore to be expected behavior that these map objects grow and they a contain a larger number of
   * MonitorV2, ComputedV2, and/or view model @Observed class objects that are no longer used / referenced by the application.
   * Only after ArkTS runtime GC has collected them, this function is able to clean up the id2cmp and is2targets.
   *
   */

  // runs idleTasks until empty or deadline is reached
  public runIdleTasks(deadline: number = Infinity): void {
    stateMgmtConsole.debug(`UINodeRegisterProxy.runIdleTasks(${deadline})`);

    // fast check for early return
    if (!this.idleTasks_ || this.idleTasks_.end === 0) {
      return;
    }

    while (this.idleTasks_.first < this.idleTasks_.end) {
      const [func, ...args] = this.idleTasks_[this.idleTasks_.first] || [];
      func?.apply(this, args);
      delete this.idleTasks_[this.idleTasks_.first];
      this.idleTasks_.first++;
      // ensure that there is no accumulation in idleTask leading to oom
      if (this.idleTasks_.end - this.idleTasks_.first < ObserveV2.idleTasksInitLength &&
        this.idleTasks_.first % 100 === 0 && Date.now() >= deadline - 1) {
        return;
      }
    }
    this.idleTasks_.first = 0;
    this.idleTasks_.end = 0;
    this.idleTasks_.length = ObserveV2.idleTasksInitLength;
  }

  /**
   * counts number of WeakRef<Object> entries in id2cmp_ 'map' object
   * @returns total count and count of WeakRefs that can be deref'ed
   * Methods only for testing
   */
  public get id2CompDeRefSize(): [totalCount: number, aliveCount: number] {
    let totalCount = 0;
    let aliveCount = 0;
    let comp: Object;
    for (const id in this.id2cmp_) {
      totalCount++;
      let weakRef: WeakRef<Object> = this.id2cmp_[id];
      if (weakRef && 'deref' in weakRef && (comp = weakRef.deref()) && comp instanceof Object) {
        aliveCount++;
      }
    }
    return [totalCount, aliveCount];
  }

  /**
   * counts number of target WeakRef<object> entries in all the Sets inside id2targets 'map' object
   * @returns total count and those can be dereferenced
   * Methods only for testing
   */
  public get id2TargetsDerefSize(): [totalCount: number, aliveCount: number] {
    let totalCount = 0;
    let aliveCount = 0;
    for (const id in this.id2targets_) {
      const targetSet: Set<WeakRef<Object>> | undefined = this.id2targets_[id];
      if (targetSet && targetSet instanceof Set) {
        for (let weakTarget of targetSet) {
          totalCount++;
          if (weakTarget.deref()) {
            aliveCount++;
          }
        } // for targetSet
      }
    } // for id2targets_
    return [totalCount, aliveCount];
  }

  // add dependency view model object 'target' property 'attrName'
  // to current this.bindId
  public addRef(target: object, attrName: string): void {
    const bound = this.stackOfRenderedComponents_.top();
    if (!bound || this.disableRecording_) {
      return;
    }
    if (bound[0] === UINodeRegisterProxy.monitorIllegalV1V2StateAccess) {
      const error = `${attrName}: ObserveV2.addRef: trying to use V2 state '${attrName}' to init/update child V2 @Component. Application error`;
      stateMgmtConsole.applicationError(error);
      throw new TypeError(error);
    }

    stateMgmtConsole.propertyAccess(`ObserveV2.addRef '${attrName}' for id ${bound[0]}...`);

    // run in idle time or now
    if (this.idleTasks_) {
      this.idleTasks_[this.idleTasks_.end++] =
        [this.addRef4IdInternal, bound[0], target, attrName];
    } else {
      this.addRef4IdInternal(bound[0], target, attrName);
    }
  }

  // add dependency view model object 'target' property 'attrName' to current this.bindId
  // this variation of the addRef function is only used to record read access to V1 observed object with enableV2Compatibility enabled
  // e.g. only from within ObservedObject proxy handler implementations.
  public addRefV2Compatibility(target: object, attrName: string): void {
    const bound = this.stackOfRenderedComponents_.top();
    if (bound && bound[1]) {
      if (!(bound[1] instanceof ViewPU)) {
        if (bound[0] === UINodeRegisterProxy.monitorIllegalV1V2StateAccess) {
          const error = `${attrName}: ObserveV2.addRefV2Compatibility: trying to use V2 state '${attrName}' to init/update child V2 @Component. Application error`;
          stateMgmtConsole.applicationError(error);
          throw new TypeError(error);
        }
        stateMgmtConsole.propertyAccess(`ObserveV2.addRefV2Compatibility '${attrName}' for id ${bound[0]}...`);
        this.addRef4Id(bound[0], target, attrName);
      } else {
        // inside ViewPU
        stateMgmtConsole.propertyAccess(`ObserveV2.addRefV2Compatibility '${attrName}' for id ${bound[0]} -- skip addRef because render/update is inside V1 ViewPU`);
      }
    }
  }

  public addRef4Id(id: number, target: object, attrName: string): void {
    stateMgmtConsole.propertyAccess(`ObserveV2.addRef4Id '${attrName}' for id ${id} ...`);

    // run in idle time or now
    if (this.idleTasks_) {
      this.idleTasks_[this.idleTasks_.end++] =
        [this.addRef4IdInternal, id, target, attrName];
    } else {
      this.addRef4IdInternal(id, target, attrName);
    }
  }

  private addRef4IdInternal(id: number, target: object, attrName: string): void {
    // Map: attribute/symbol -> dependent id
    const symRefs = target[ObserveV2.SYMBOL_REFS] ??= {};
    symRefs[attrName] ??= new Set();
    symRefs[attrName].add(id);

    // Map id -> attribute/symbol
    // optimization for faster clearBinding
    const idRefs = target[ObserveV2.ID_REFS];
    if (idRefs) {
      idRefs[id] ??= new Set();
      idRefs[id].add(attrName);
    }

    const weakRef = WeakRefPool.get(target);
    if (this.id2targets_?.[id]?.has(weakRef)) {
      return;
    }

    this.id2targets_[id] ??= new Set<WeakRef<Object>>();
    this.id2targets_[id].add(weakRef);
    WeakRefPool.register(target, id, () => {
      if (this.id2targets_?.[id]?.delete(weakRef) && this.id2targets_[id].size === 0) {
        delete this.id2targets_[id];
      }
    });
  }

  /**
   *
   * @param target set tracked attribute to new value without notifying the change
   *               !! use with caution !!
   * @param attrName
   * @param newValue
   */
  public setUnmonitored<Z>(target: object, attrName: string, newValue: Z): void {
    const storeProp = ObserveV2.OB_PREFIX + attrName;
    if (storeProp in target) {
      // @Track attrName
      stateMgmtConsole.propertyAccess(`setUnmonitored '${attrName}' - tracked but unchanged. Doing nothing.`);
      target[storeProp] = newValue;
    } else {
      stateMgmtConsole.propertyAccess(`setUnmonitored '${attrName}' - untracked, assigning straight.`);
      // untracked attrName
      target[attrName] = newValue;
    }
  }

  /**
   * Execute given task while state change observation is disabled
   * A state mutation caused by the task will NOT trigger UI rerender
   * and @monitor function execution.
   *
   * !!! Use with Caution !!!
   *
   * @param task a function to execute without monitoring state changes
   * @returns task function return value
   */
  public executeUnobserved<Z>(task: () => Z): Z {
    stateMgmtConsole.propertyAccess(`executeUnobserved - start`);
    this.disabled_ = true;
    let ret: Z;
    try {
      ret = task();
    } catch (e) {
      stateMgmtConsole.applicationError(`executeUnobserved - task execution caused error ${e} !`);
    }
    this.disabled_ = false;
    stateMgmtConsole.propertyAccess(`executeUnobserved - done`);
    return ret;
  }

  /**
   * Execute given task while state dependency recording is disabled
   * Any property read during task execution will not be recorded as
   * a dependency
   * 
   * !!! Use with Caution !!!
   *
   * @param task a function to execute without monitoring state changes
   * @param unobserved flag to disable also change observation
   * @returns task function return value
   */
  public executeUnrecorded<Z>(task: () => Z, unobserved: boolean = false): Z {
    stateMgmtConsole.propertyAccess(`executeUnrecorded - start`);
    this.disableRecording_ = true;
    // store current value of disabled_ so that we can later restore it properly
    const oldDisabledValue = this.disabled_;
    unobserved && (this.disabled_ = true);
    let ret: Z = task();
    this.disableRecording_ = false;
    unobserved && (this.disabled_ = oldDisabledValue);
    stateMgmtConsole.propertyAccess(`executeUnrecorded - done`);
    return ret;
  }



  /**
  * mark view model object 'target' property 'attrName' as changed
  * notify affected watchIds and elmtIds but exclude given elmtIds
  *
  * @param propName ObservedV2 or ViewV2 target
  * @param attrName attrName
  * @param ignoreOnProfile The data reported to the profiler needs to be the changed state data. 
  * If the fireChange is invoked before the data changed, it needs to be ignored on the profiler.
  * The default value is false.
  */
  public fireChange(target: object, attrName: string, excludeElmtIds?: Set<number>,
    ignoreOnProfiler: boolean = false): void {
    // forcibly run idle time tasks if any
    if (this.idleTasks_?.end) {
      this.runIdleTasks();
    }
    // enable to get more fine grained traces
    // including 2 (!) .end calls.

    if (!target[ObserveV2.SYMBOL_REFS] || this.disabled_) {
      return;
    }

    const bound = this.stackOfRenderedComponents_.top();
    if (this.calculatingComputedProp_) {
      const prop = bound ? (bound[1] as ComputedV2).getProp() : 'unknown computed property';
      const error = `Usage of ILLEGAL @Computed function detected for ${prop}! The @Computed function MUST NOT change the state of any observed state variable!`;
      stateMgmtConsole.applicationError(error);
      throw new Error(error);
    }

    // enable this trace marker for more fine grained tracing of the update pipeline
    // note: two (!) end markers need to be enabled
    let changedIdSet = target[ObserveV2.SYMBOL_REFS][attrName];
    if (changedIdSet instanceof Set === false) {
      return;
    }

    stateMgmtConsole.propertyAccess(`ObserveV2.fireChange '${attrName}' dependent ids: ${JSON.stringify(Array.from(changedIdSet))}  ...`);

    for (const id of changedIdSet) {
      // Cannot fireChange the object that is being created.
      if (bound && id === bound[0]) {
        continue;
      }

      // exclude given elementIds
      if (excludeElmtIds?.has(id)) {
        stateMgmtConsole.propertyAccess(`... exclude id ${id}`);
        continue;
      }

      // if this is the first id to be added to any Set of changed ids,
      // schedule an 'updateDirty' task
      // that will run after the current call stack has unwound.
      // purpose of check for startDirty_ is to avoid going into recursion. This could happen if
      // exec a re-render or exec a monitor function changes some state -> calls fireChange -> ...
      const hasPendingChanges = (this.elmtIdsChanged_.size + this.monitorIdsChanged_.size + this.computedPropIdsChanged_.size) > 0;
      const isReuseInProgress = (this.currentReuseId_ !== ObserveV2.NO_REUSE);
      const shouldUpdateDirty = (!hasPendingChanges && !this.startDirty_ && !isReuseInProgress);

      if (shouldUpdateDirty) {
        Promise.resolve().then(this.updateDirty.bind(this))
          .catch(error => {
            stateMgmtConsole.applicationError(`Exception occurred during the update process involving @Computed properties, @Monitor functions or UINode re-rendering`, error);
            _arkUIUncaughtPromiseError(error);
          });
      }

      // add bindId to the correct Set of pending changes.
      if (id < ComputedV2.MIN_COMPUTED_ID) {
        this.elmtIdsChanged_.add(id);
      } else if (id < MonitorV2.MIN_WATCH_ID) {
        this.computedPropIdsChanged_.add(id);
      } else if (id < MonitorV2.MIN_WATCH_FROM_API_ID) {
        this.monitorIdsChanged_.add(id);
      } else if (id < MonitorV2.MIN_SYNC_WATCH_FROM_API_ID) {
        this.monitorIdsChangedForAddMonitor_.add(id);
      } else if (id < PersistenceV2Impl.MIN_PERSISTENCE_ID) {
        this.monitorSyncIdsChangedForAddMonitor_.add(id);
      } else {
        this.persistenceChanged_.add(id);
      }
    } // for

    // execute the AddMonitor synchronous function
    while (this.monitorSyncIdsChangedForAddMonitor_.size + this.monitorFuncsToRun_.size > 0) {
      if (this.monitorSyncIdsChangedForAddMonitor_.size) {
        stateMgmtConsole.debug(`AddMonitor API monitorSyncIdsChangedForAddMonitor_ ${this.monitorSyncIdsChangedForAddMonitor_.size}`)
        const monitorId: Set<number> = this.monitorSyncIdsChangedForAddMonitor_;
        this.monitorSyncIdsChangedForAddMonitor_ = new Set<number>();
        // update the value and dependency for each path and get the MonitorV2 id needs to be execute
        this.updateDirtyMonitorPath(monitorId);
      }
      if (this.monitorFuncsToRun_.size) {
        const monitorFuncs = this.monitorFuncsToRun_;
        this.monitorFuncsToRun_ = new Set<number>();
        this.runMonitorFunctionsForAddMonitor(monitorFuncs);
      }
    }

    // report the stateVar changed when recording the profiler
    if (stateMgmtDFX.enableProfiler && !ignoreOnProfiler) {
      stateMgmtDFX.reportStateInfoToProfilerV2(target, attrName, changedIdSet);
    }
  }

  /**
   * Group elementIds by their containerId (instanceId).
   * Make sure view.getInstanceId() is called only once for each view., not for all elmtIds!!!
   */
  private groupElementIdsByContainer(): void {
    stateMgmtConsole.debug('ObserveV2.groupElementIdsByContainer');

    // Create sorted array and clear set
    const elmtIds = Array.from(this.elmtIdsChanged_, Number).sort((a, b) => a - b);
    this.elmtIdsChanged_.clear();

    // Cache for view -> instanceId
    const viewInstanceIdCache = new Map<ViewV2 | ViewPU, number>();

    for (const elmtId of elmtIds) {
        const view = this.id2cmp_[elmtId]?.deref();

        // Early continue for invalid views
        if (!(view instanceof ViewV2 || view instanceof ViewPU)) {
            continue;
        }

        // Get or cache instanceId
        let instanceId = viewInstanceIdCache.get(view);
        if (instanceId === undefined) {
            instanceId = view.getInstanceId();
            viewInstanceIdCache.set(view, instanceId);
        }

        // Get or create viewMap
        let viewMap = this.viewV2NeedUpdateMap_.get(instanceId);
        if (!viewMap) {
          viewMap = new Map<ViewV2 | ViewPU, Array<number>>();
          this.viewV2NeedUpdateMap_.set(instanceId, viewMap);
        }

        // Get or create view's element array
        let elements = viewMap.get(view);
        if (!elements) {
            elements = [];
            viewMap.set(view, elements);
        }

        elements.push(elmtId);
        stateMgmtConsole.debug(`groupElementIdsByContainer: elmtId=${elmtId}, view=${view.constructor.name}, instanceId=${instanceId}`);
    }
}
  public updateDirty(): void {
    this.startDirty_ = true;
    this.isParentChildOptimizable_ ? this.updateDirty2Optimized(): this.updateDirty2(false);
    this.startDirty_ = false;
  }

  /**
     * Optimized version of updateDirty2
     * execute /update in this order
     * - @Computed variables
     * - @Monitor functions
     * Request a frame update and schedule a callback to trigger on the next VSync update
    */
  public updateDirty2Optimized(): void {
    stateMgmtConsole.debug(`ObservedV2.updateDirty2Optimized() start`);
    // Calling these functions to retain the original behavior
    // Obtain and unregister the removed elmtIds
    UINodeRegisterProxy.obtainDeletedElmtIds();
    UINodeRegisterProxy.unregisterElmtIdsFromIViews();

    this.updateComputedAndMonitors();

    if (this.elmtIdsChanged_.size === 0) {
      stateMgmtConsole.debug(`Vsync request is unnecessary when no elements have changed - returning from updateDirty2Optimized`);
      return;
    }

    // Group elementIds before scheduling update
    this.groupElementIdsByContainer();

    // At this point, we have the viewV2NeedUpdateMap_ populated with the ViewV2/elementIds that need update
    // For each containerId (instance/container) in the map, schedule an update.
    for (const containerId of this.viewV2NeedUpdateMap_.keys()) {
      if (!this.scheduledContainerIds_.has(containerId)) {
        stateMgmtConsole.debug(`  scheduling update for containerId: ${containerId}`);
        this.scheduledContainerIds_.add(containerId);
        ViewStackProcessor.scheduleUpdateOnNextVSync(this.onVSyncUpdate.bind(this), containerId);
      }
    }
    stateMgmtConsole.debug(`ObservedV2.updateDirty2Optimized() end`);
  }
  // Callback from C++ on VSync
  public onVSyncUpdate(containerId: number): boolean {
    stateMgmtConsole.debug(`ObservedV2.flushDirtyViewsOnVSync containerId=${containerId} start`);
    aceDebugTrace.begin(`ObservedV2.onVSyncUpdate`);
    let maxFlushTimes = 3; // Refer PipelineContext::FlushDirtyNodeUpdate()
    // Obtain and unregister the removed elmtIds
    UINodeRegisterProxy.obtainDeletedElmtIds();
    UINodeRegisterProxy.unregisterElmtIdsFromIViews();

    // Process updates in priority order: computed properties, monitors, UI nodes
    do {
      this.updateComputedAndMonitors();
      const viewV2Map = this.viewV2NeedUpdateMap_.get(containerId);

      // Clear the ViewV2 map for the current containerId
      this.viewV2NeedUpdateMap_.delete(containerId);

      if (viewV2Map?.size) {
        // Update elements, generating new elmtIds in elmtIdsChanged_ for nested updates
        viewV2Map.forEach((elmtIds, view) => {
          this.updateUINodesSynchronously(elmtIds, view);
        });

        if (this.elmtIdsChanged_.size) {
          this.groupElementIdsByContainer();
        }
      } else {
        stateMgmtConsole.error(`No views to update for containerId=${containerId}`);
        break; // Exit loop early since no updates are possible
      }
    } while (this.hasPendingUpdates(containerId) && --maxFlushTimes > 0);

    // Check if more updates are needed
    const viewV2Map = this.viewV2NeedUpdateMap_.get(containerId);

    if (!viewV2Map || viewV2Map.size === 0) {
      if (viewV2Map?.size === 0) {
        this.viewV2NeedUpdateMap_.delete(containerId);
      }

      ViewStackProcessor.scheduleUpdateOnNextVSync(null, containerId);

      // After all processing, remove from scheduled set
      this.scheduledContainerIds_.delete(containerId);
      return false;
    }
    aceDebugTrace.end();
    stateMgmtConsole.debug(`ObservedV2.onVSyncUpdate there are still views to be updated for containerId=${containerId}`);
    return true;
  }

  public hasPendingUpdates(containerId: number): boolean {
    const viewV2Map = this.viewV2NeedUpdateMap_.get(containerId);
    let ret = ((viewV2Map && viewV2Map.size > 0) || this.monitorIdsChanged_.size > 0 || this.computedPropIdsChanged_.size > 0);
    stateMgmtConsole.debug(`hasPendingUpdates() containerId: ${containerId}, viewV2Map size: ${viewV2Map?.size}, ret: ${ret}`);
    return ret;
  }

  /**
   * execute /update in this order
   * - @Computed variables
   * - @Monitor functions
   * - UINode re-render
   * three nested loops, means:
   * process @Computed until no more @Computed need update
   * process @Monitor until no more @Computed and @Monitor
   * process UINode update until no more @Computed and @Monitor and UINode rerender
   *
   * @param updateUISynchronously should be set to true if called during VSYNC only
   *
   */

  public updateDirty2(updateUISynchronously: boolean = false, isReuse: boolean = false): void {
    aceDebugTrace.begin('updateDirty2');
    stateMgmtConsole.debug(`ObservedV2.updateDirty2 updateUISynchronously=${updateUISynchronously} ... `);
    // obtain and unregister the removed elmtIds
    UINodeRegisterProxy.obtainDeletedElmtIds();
    UINodeRegisterProxy.unregisterElmtIdsFromIViews();

    // priority order of processing:
    // 1- update computed properties until no more need computed props update
    // 2- update monitors until no more monitors and no more computed props
    // 3- update UINodes until no more monitors, no more computed props, and no more UINodes
    // FIXME prevent infinite loops
    do {
      this.updateComputedAndMonitors();

      if (this.elmtIdsChanged_.size) {
        const elmtIds = Array.from(this.elmtIdsChanged_).sort((elmtId1, elmtId2) => elmtId1 - elmtId2);
        this.elmtIdsChanged_ = new Set<number>();
        updateUISynchronously ? isReuse ? this.updateUINodesForReuse(elmtIds) : this.updateUINodesSynchronously(elmtIds) : this.updateUINodes(elmtIds);
      }
    } while (this.elmtIdsChanged_.size + this.monitorIdsChanged_.size + this.computedPropIdsChanged_.size > 0);

    aceDebugTrace.end();
    stateMgmtConsole.debug(`ObservedV2.updateDirty2 updateUISynchronously=${updateUISynchronously} - DONE `);
  }

  /**
   * execute /update in this order
   * - @Computed variables
   * - @Monitor functions
   * - UINode re-render
   * three nested loops, means:
   * process @Computed until no more @Computed need update
   * process @Monitor until no more @Computed and @Monitor
  */
  private updateComputedAndMonitors(): void {
    do {
      while (this.computedPropIdsChanged_.size) {
        //  sort the ids and update in ascending order
        // If a @Computed property depends on other @Computed properties, their
        // ids will be smaller as they are defined first.
        const computedProps = Array.from(this.computedPropIdsChanged_).sort((id1, id2) => id1 - id2);
        this.computedPropIdsChanged_ = new Set<number>();
        this.updateDirtyComputedProps(computedProps);
      }

      if (this.persistenceChanged_.size) {
        const persistKeys: Array<number> = Array.from(this.persistenceChanged_);
        this.persistenceChanged_ = new Set<number>();
        PersistenceV2Impl.instance().onChangeObserved(persistKeys);
      }

      if (this.monitorIdsChanged_.size) {
        const monitors = this.monitorIdsChanged_;
        this.monitorIdsChanged_ = new Set<number>();
        this.updateDirtyMonitors(monitors);
      }
      // handle the Monitor id from API configured with asynchronous options
      while (this.monitorIdsChangedForAddMonitor_.size + this.monitorFuncsToRun_.size > 0) {
        if (this.monitorIdsChangedForAddMonitor_.size) {
          stateMgmtConsole.debug(`AddMonitor asynchronous ${this.monitorIdsChangedForAddMonitor_.size}`)
          const monitorId: Set<number> = this.monitorIdsChangedForAddMonitor_;
          this.monitorIdsChangedForAddMonitor_ = new Set<number>();
          // update the value and dependency for each path and get the MonitorV2 id needs to be execute
          this.updateDirtyMonitorPath(monitorId);
        }
        if (this.monitorFuncsToRun_.size) {
          const monitorFuncs = this.monitorFuncsToRun_;
          this.monitorFuncsToRun_ = new Set<number>();
          this.runMonitorFunctionsForAddMonitor(monitorFuncs);
        }
      }
    } while (this.monitorIdsChanged_.size + this.persistenceChanged_.size +
    this.computedPropIdsChanged_.size + this.monitorIdsChangedForAddMonitor_.size + this.monitorFuncsToRun_.size > 0);
  }

  public updateDirtyComputedProps(computed: Array<number>): void {
    stateMgmtConsole.debug(`ObservedV2.updateDirtyComputedProps ${computed.length} props: ${JSON.stringify(computed)} ...`);
    aceDebugTrace.begin(`ObservedV2.updateDirtyComputedProps ${computed.length} @Computed`);
    computed.forEach((id) => {
      const comp = this.id2Others_[id]?.deref();
      if (comp instanceof ComputedV2) {
        const target = comp.getTarget();
        if (target instanceof ViewV2 && !target.isViewActive()) {
          // add delayed ComputedIds id
          target.addDelayedComputedIds(id);
        } else {
          comp.fireChange();
        }
      }
    });
    aceDebugTrace.end();
  }
  /**
   * @function resetMonitorValues
   * @description This function ensures that @Monitor function are reset and reinitialized
   *  during the reuse cycle:
   * - Clear and reinitialize monitor IDs and functions to prevent unintended triggers
   * - Reset dirty states to ensure reusabiltiy
   */
  public resetMonitorValues(): void {
    stateMgmtConsole.debug(`resetMonitorValues changed monitorIds count: ${this.monitorIdsChanged_.size}`);
    if (this.monitorIdsChanged_.size) {
      const monitors = this.monitorIdsChanged_;
      this.monitorIdsChanged_ = new Set<number>();
      this.updateDirtyMonitorsOnReuse(monitors);
    }
  }

  public updateDirtyMonitorsOnReuse(monitors: Set<number>): void {
    let monitor: MonitorV2 | undefined;
    monitors.forEach((watchId) => {
      monitor = this.id2Others_[watchId]?.deref();
      if (monitor instanceof MonitorV2) {
        // only update dependency and reset value, no call monitor.
        monitor.notifyChangeOnReuse();
      }
    });
  }

  public updateDirtyMonitors(monitors: Set<number>): void {
    stateMgmtConsole.debug(`ObservedV2.updateDirtyMonitors: ${monitors.size} @monitor funcs: ${JSON.stringify(Array.from(monitors))} ...`);
    aceDebugTrace.begin(`ObservedV2.updateDirtyMonitors: ${monitors.size} @monitor`);

    let monitor: MonitorV2 | undefined;
    let monitorTarget: Object;

    monitors.forEach((watchId) => {
      monitor = this.id2Others_[watchId]?.deref();
      if (monitor instanceof MonitorV2) {
        monitorTarget = monitor.getTarget();
        if (monitorTarget instanceof ViewV2 && !monitorTarget.isViewActive()) {
          // monitor notifyChange delayed if target is a View that is not active
          monitorTarget.addDelayedMonitorIds(watchId);
        } else {
          monitor.notifyChange();
        }
      }
    });
    aceDebugTrace.end();
  }

  public runMonitorFunctionsForAddMonitor(monitors: Set<number>): void {
    stateMgmtConsole.debug(`ObservedV2.runMonitorFunctionsForAddMonitor: ${monitors.size}. AddMonitor funcs: ${JSON.stringify(Array.from(monitors))} ...`);
    aceDebugTrace.begin(`ObservedV2.runMonitorFunctionsForAddMonitor: ${monitors.size}`);

    let monitor: MonitorV2 | undefined;

    monitors.forEach((watchId) => {
      monitor = this.id2Others_[watchId]?.deref();
      if (monitor instanceof MonitorV2) {
        monitor.runMonitorFunction();
      }
    });
    aceDebugTrace.end();
  }


  public updateDirtyMonitorPath(monitors: Set<number>): void {
    stateMgmtConsole.debug(`ObservedV2.updateDirtyMonitorPath: ${monitors.size} addMonitor funcs: ${JSON.stringify(Array.from(monitors))} ...`);
    aceDebugTrace.begin(`ObservedV3.updateDirtyMonitorPath: ${monitors.size} addMonitor`);

    let ret: number = 0;
    monitors.forEach((watchId) => {
      const monitor = this.id2Others_[watchId]?.deref();
      if (monitor instanceof MonitorV2) {
        const monitorTarget = monitor.getTarget();
        if (monitorTarget instanceof ViewV2 && !monitorTarget.isViewActive()) {
          monitorTarget.addDelayedMonitorIds(watchId)
        } else {
          // find the path MonitorValue and record dependency again
          // get path owning MonitorV2 id
          ret = monitor.notifyChangeForEachPath(watchId);
        }
      }

      // Collect AddMonitor functions that need to be executed later
      if (ret > 0) {
        this.monitorFuncsToRun_.add(ret);
      }
    });
    aceDebugTrace.end();
  }


  /**
   * This version of UpdateUINodes does not wait for VSYNC, violates rules
   * calls UpdateElement, thereby avoids the long and frequent code path from
   * FlushDirtyNodesUpdate to CustomNode to ViewV2.updateDirtyElements to UpdateElement
   * Code left here to reproduce benchmark measurements, compare with future optimisation
   * @param elmtIds
   *
   */
  private updateUINodesSynchronously(elmtIds: Array<number>, inView?: ViewPU | ViewV2): void {
    stateMgmtConsole.debug(`ObserveV2.updateUINodesSynchronously: ${elmtIds.length} elmtIds: ${JSON.stringify(elmtIds)} ...`);
    aceDebugTrace.begin(`ObserveV2.updateUINodesSynchronously: ${elmtIds.length} elmtId`);

    elmtIds.forEach((elmtId) => {
      let view = inView ?? this.id2cmp_[elmtId]?.deref();
      if ((view instanceof ViewV2) || (view instanceof ViewPU)) {
        if (view.isViewActive()) {
          // FIXME need to call syncInstanceId before update?
          view.UpdateElement(elmtId);
        } else {
          // schedule delayed update once the view gets active
          view.scheduleDelayedUpdate(elmtId);
        }
      } // if ViewV2 or ViewPU
    });
    aceDebugTrace.end();
  }

  private updateUINodesForReuse(elmtIds: Array<number>): void {
    aceDebugTrace.begin(`ObserveV2.updateUINodesForReuse: ${elmtIds.length} elmtId`);
    let view: Object;
    let weak: any;
    elmtIds.forEach((elmtId) => {
      if ((weak = this.id2cmp_[elmtId]) && weak && ('deref' in weak) &&
        (view = weak.deref()) && ((view instanceof ViewV2) || (view instanceof ViewPU))) {
        if (view.isViewActive()) {
          /* update child element */ this.currentReuseId_ === view.id__() ||
          /* update parameter */ this.currentReuseId_ === elmtId
            ? view.UpdateElement(elmtId)
            : view.uiNodeNeedUpdateV2(elmtId);
        } else {
          // schedule delayed update once the view gets active
          view.scheduleDelayedUpdate(elmtId);
        }
      } // if ViewV2 or ViewPU
    });
    aceDebugTrace.end();
  }

  // This is the code path similar to V2, follows the rule that UI updates on VSYNC.
  // ViewPU/ViewV2 queues the elmtId that need update, marks the CustomNode dirty in RenderContext
  // On next VSYNC runs FlushDirtyNodesUpdate to call rerender to call UpdateElement. Much longer code path
  // much slower
  private updateUINodes(elmtIds: Array<number>): void {
    stateMgmtConsole.debug(`ObserveV2.updateUINodes: ${elmtIds.length} elmtIds need rerender: ${JSON.stringify(elmtIds)} ...`);
    aceDebugTrace.begin(`ObserveV2.updateUINodes: ${elmtIds.length} elmtId`);

    elmtIds.forEach((elmtId) => {
      const view = this.id2cmp_[elmtId]?.deref();
      if ((view instanceof ViewV2) || (view instanceof ViewPU)) {
        if (view.isViewActive()) {
          view.uiNodeNeedUpdateV2(elmtId);
        } else {
          // schedule delayed update once the view gets active
          view.scheduleDelayedUpdate(elmtId);
        }
      }
    });
    aceDebugTrace.end();
  }

  public constructMonitor(owningObject: Object, owningObjectName: string): void {
    let watchProp = Symbol.for(MonitorV2.WATCH_PREFIX + owningObjectName);
    if (owningObject && (typeof owningObject === 'object') && owningObject[watchProp]) {
      Object.entries(owningObject[watchProp]).forEach(([pathString, monitorFunc]) => {
        if (monitorFunc && pathString && typeof monitorFunc === 'function') {
          const monitor = new MonitorV2(owningObject, pathString, monitorFunc as (m: IMonitor) => void, true);
          monitor.InitRun();
          const refs = owningObject[ObserveV2.MONITOR_REFS] ??= {};
          // store a reference inside owningObject
          // thereby MonitorV2 will share lifespan as owning @ComponentV2 or @ObservedV2
          // remember: id2others only has a WeakRef to MonitorV2 obj
          refs[monitorFunc.name] = monitor;
        }
        // FIXME Else handle error
      });
    } // if target[watchProp]
  }

  public AddMonitorPath(target: object, path: string | string[], monitorFunc: MonitorCallback, options?: MonitorOptions): void {
    const funcName = monitorFunc.name;
    const refs = target[ObserveV2.ADD_MONITOR_REFS] ??= {};
    let monitor = refs[funcName];
    const pathsUniqueString = Array.isArray(path) ? path.join(' ') : path;
    const isSync: boolean = options ? options.isSynchronous : false;
    const paths = Array.isArray(path) ? path : [path];
    if (monitor && monitor instanceof MonitorV2) {
      if (isSync !== monitor.isSync()) {
        stateMgmtConsole.applicationError(`addMonitor failed, current function ${funcName} has already register as ${monitor.isSync()? `sync`: `async`}, cannot change to ${isSync? `sync`: `async`} anymore`);
        return;
      }
      paths.forEach(item => {
        monitor.addPath(item);
      });
      monitor.InitRun();
      return;
    }

    monitor = new MonitorV2(target, pathsUniqueString, monitorFunc, false, isSync);
    monitor.InitRun();
    // store a reference inside target
    // thereby MonitorV2 will share lifespan as owning @ComponentV2 or @ObservedV2 to prevent the MonitorV2 is GC
    // remember: id2others only has a WeakRef to MonitorV2 obj
    refs[funcName] = monitor;
  }

  public clearMonitorPath(target: object, path: string | string[], monitorFunc?: MonitorCallback): void {
    const refs = target[ObserveV2.ADD_MONITOR_REFS] ??= {};
    const paths = Array.isArray(path) ? path : [path];
    
    if (monitorFunc) {
      const funcName = monitorFunc.name;
      let monitor = refs[funcName];
      if (monitor && monitor instanceof MonitorV2) {
        paths.forEach(item => {
          if (!monitor.removePath(item)) {
            stateMgmtConsole.applicationError(
              `cannot clear path ${item} for ${funcName} because it was never registered with addMonitor`
            );
          }
        });
      } else {
        const pathsUniqueString = paths.join(' ');
        stateMgmtConsole.applicationError(
          `clearMonitor failed: cannot clear path(s) ${pathsUniqueString} for ${funcName} because no Monitor instance was found for this function`
        );
      }
      return;
    }
    // no monitorFunc passed: try to remove this path for all the monitorV2 instance stored in current target
    const monitors = Object.values(refs).filter((m): m is MonitorV2 => m instanceof MonitorV2);

    paths.forEach(item => {
      let res = false;
      monitors.forEach(monitor => {
        if (monitor.removePath(item)) {
          res = true;
        }
      });
      if (!res) {
        stateMgmtConsole.applicationError(
          `cannot clear path ${item} for current target ${target.constructor.name} because no Monitor function for this path was registered`
        );
      }
    });
  }

  public constructComputed(owningObject: Object, owningObjectName: string): void {
    const computedProp = Symbol.for(ComputedV2.COMPUTED_PREFIX + owningObjectName);
    if (owningObject && (typeof owningObject === 'object') && owningObject[computedProp]) {
      Object.entries(owningObject[computedProp]).forEach(([computedPropertyName, computeFunc]) => {
        stateMgmtConsole.debug(`constructComputed: in ${owningObject?.constructor?.name} found @Computed ${computedPropertyName}`);
        const computed = new ComputedV2(owningObject, computedPropertyName, computeFunc as unknown as () => any);
        computed.InitRun();
        const refs = owningObject[ObserveV2.COMPUTED_REFS] ??= {};
        // store a reference inside owningObject
        // thereby ComputedV2 will share lifespan as owning @ComponentV2 or @ObservedV2
        // remember: id2cmp only has a WeakRef to ComputedV2 obj
        const existingComputed = refs[computedPropertyName];
        if (existingComputed && existingComputed instanceof ComputedV2) {
          // current computed will be override, and will be GC soon
          // to avoid the Computed be triggered anymore, invalidate it
          this.clearBinding(existingComputed.getComputedId());
          stateMgmtConsole.warn(`Check compatibility, ${owningObjectName} has @Computed ${computedPropertyName} and create ${owningObject?.constructor?.name} instance`);
        }
        refs[computedPropertyName] = computed;
      });
    }
  }

  public clearWatch(id: number): void {
    if (id < MonitorV2.MIN_WATCH_FROM_API_ID) {
      this.clearBinding(id);
      return;
    }
    const monitor: MonitorV2 = this.id2Others_[id]?.deref();
    if (monitor instanceof MonitorV2) {
      monitor.getValues().forEach((monitorValueV2: MonitorValueV2<unknown>) => {
        this.clearBinding(monitorValueV2.id);
      })
    }
    this.clearBinding(id);
  }

  public registerMonitor(monitor: MonitorV2, id: number): void {
    const weakRef = WeakRefPool.get(monitor);
    // this instance, which maybe MonitorV2/ComputedV2 have been already recorded in id2Others
    if (this.id2Others_[id] === weakRef) {
      return;
    }
    this.id2Others_[id] = weakRef;
    // register MonitorV2/ComputedV2 instance gc-callback func
    WeakRefPool.register(monitor, id, () => {
      delete this.id2Others_[id];
    });
  }


  public static autoProxyObject(target: Object, key: string | symbol): any {
    let val = target[key];
    // Not an object, not a collection, no proxy required
    if (!val || typeof (val) !== 'object' ||
      !(Array.isArray(val) || val instanceof Set || val instanceof Map || val instanceof Date)) {
      return val;
    }

    // Collections are the only type that require proxy observation. If they have already been observed, no further observation is needed.
    // Prevents double-proxying: checks if the object is already proxied by either V1 or V2 (to avoid conflicts).
    // Prevents V2 proxy creation if the developer uses makeV1Observed and also tries to wrap a V2 proxy with built-in types
    // Handle the case where both V1 and V2 proxies exist (if V1 proxy doesn't trigger enableV2Compatibility).
    // Currently not implemented to avoid compatibility issues with existing apps that may use both V1 and V2 proxies.
    if (!val[ObserveV2.SYMBOL_PROXY_GET_TARGET] && !(ObservedObject.isEnableV2CompatibleInternal(val) || ObservedObject.isMakeV1Observed(val))) {

      if (Array.isArray(val)) {
        target[key] = new Proxy(val, ObserveV2.arrayProxy);
      } else if (val instanceof Set || val instanceof Map) {
        target[key] = new Proxy(val, ObserveV2.setMapProxy);
      } else {
        target[key] = new Proxy(val, ObserveV2.objectProxy);
      }
      val = target[key];
    }

    // If the return value is an Array, Set, Map
    // if (this.arr[0] !== undefined, and similar for Set and Map) will not update in response /
    // to array length/set or map size changing function without addRef on OB_LENGH
    if (!(val instanceof Date)) {
      if (ObservedObject.isEnableV2CompatibleInternal(val)) {
        ObserveV2.getObserve().addRefV2Compatibility(val, ObserveV2.OB_LENGTH);
      } else {
        ObserveV2.getObserve().addRef(ObserveV2.IsMakeObserved(val) ? RefInfo.get(UIUtilsImpl.instance().getTarget(val)) :
          val, ObserveV2.OB_LENGTH);
      }
    }
    return val;
  }

  /**
   * Helper function to add meta data about decorator to ViewPU or ViewV2
   * @param proto prototype object of application class derived from  ViewPU or ViewV2 or `@ObservedV2` class
   * @param varName decorated variable
   * @param deco '@Local', '@Event', etc
   *              Excludes `@Computed` and `@Monitor`
   */
  public static addVariableDecoMeta(proto: Object, varName: string, deco: string): void {
    // add decorator meta data
    const meta = proto[ObserveV2.V2_DECO_META] ??= {};
    meta[varName] = {};
    meta[varName].deco = deco;

    // FIXME
    // when splitting ViewPU and ViewV2
    // use instanceOf. Until then, this is a workaround.
    // any @Local, @Trace, etc V2 event handles this function to return false
    Reflect.defineProperty(proto, 'isViewV2', {
      get() { return true; },
      enumerable: false
    });
  }

    /**
   * Helper function to add meta data about for `@Computed` and `@Monitor`
   * @param proto prototype object of application class derived from  ViewPU or ViewV2 or `@ObservedV2` class
   * @param varName decorated variable
   * @param deco `@Computed` and `@Monitor`
   */
  public static addMethodDecoMeta(proto: Object, varName: string, deco: string): void {
    // add decorator meta data
    const meta = proto[ObserveV2.V2_DECO_METHOD_META] ??= {};
    meta[varName] = {};
    meta[varName].deco = deco;

    Reflect.defineProperty(proto, 'isViewV2', {
      get() { return true; },
      enumerable: false
    });
  }


  public static addParamVariableDecoMeta(proto: Object, varName: string, deco?: string, deco2?: string): void {
    // add decorator meta data
    const meta = proto[ObserveV2.V2_DECO_META] ??= {};
    meta[varName] ??= {};
    if (deco) {
      meta[varName].deco = deco;
    }
    if (deco2) {
      meta[varName].deco2 = deco2;
    }

    // FIXME
    // when splitting ViewPU and ViewV2
    // use instanceOf. Until then, this is a workaround.
    // any @Local, @Trace, etc V2 event handles this function to return false
    Reflect.defineProperty(proto, 'isViewV2', {
      get() { return true; },
      enumerable: false
    }
    );
  }


  public static usesV2Variables(proto: Object): boolean {
    return (proto && typeof proto === 'object' && proto[ObserveV2.V2_DECO_META]);
  }

  /**
   * Get element info according to the elmtId.
   *
   * @param elmtId element id.
   * @param isProfiler need to return ElementType including the id, type and isCustomNode when isProfiler is true.
   *                   The default value is false. 
   */
  public getElementInfoById(elmtId: number, isProfiler: boolean = false): string | ElementType {
    let weak: WeakRef<ViewBuildNodeBase> | undefined = UINodeRegisterProxy.ElementIdToOwningViewPU_.get(elmtId);
    let view;
    return (weak && (view = weak.deref()) && (view instanceof PUV2ViewBase)) ? view.debugInfoElmtId(elmtId, isProfiler) : `unknown component type[${elmtId}]`;
  }

  /**
   * Get attrName decorator info. 
   */
  public getDecoratorInfo(target: object, attrName: string): string {
    const meta = target[ObserveV2.V2_DECO_META];
    const metaMethod = target[ObserveV2.V2_DECO_METHOD_META];
    const decorator = meta?.[attrName] ?? metaMethod?.[attrName];
    return this.parseDecorator(decorator);
  }

  public parseDecorator(decorator: any): string {
    if (!decorator) {
      return '';
    }
    if (typeof decorator !== 'object') {
      return '';
    }
    let decoratorInfo: string = '';
    if ('deco' in decorator) {
      decoratorInfo = decorator.deco;
    }
    if ('aliasName' in decorator) {
      decoratorInfo += `(${decorator.aliasName})`;
    }
    if ('deco2' in decorator) {
      decoratorInfo += decorator.deco2;
    }
    return decoratorInfo;
  }

  public getComputedInfoById(computedId: number): string {
    let weak = this.id2Others_[computedId];
    let computedV2: ComputedV2;
    return (weak && (computedV2 = weak.deref()) && (computedV2 instanceof ComputedV2)) ? computedV2.getComputedFuncName() : '';
  }

  public getMonitorInfoById(computedId: number): string {
    let weak = this.id2Others_[computedId];
    let monitorV2: MonitorV2;
    return (weak && (monitorV2 = weak.deref()) && (monitorV2 instanceof MonitorV2)) ? monitorV2.getMonitorFuncName() : '';
  }

  public setCurrentReuseId(elmtId: number): void {
    this.currentReuseId_ = elmtId;
  }
} // class ObserveV2

const trackInternal = (
  target: any,
  propertyKey: string
): void => {
  if (typeof target === 'function' && !Reflect.has(target, propertyKey)) {
    // dynamic track，and it not a static attribute
    target = target.prototype;
  }
  const storeProp = ObserveV2.OB_PREFIX + propertyKey;
  target[storeProp] = target[propertyKey];
  Reflect.defineProperty(target, propertyKey, {
    get() {
      ObserveV2.getObserve().addRef(this, propertyKey);
      return ObserveV2.autoProxyObject(this, ObserveV2.OB_PREFIX + propertyKey);
    },
    set(val) {
      // If the object has not been observed, you can directly assign a value to it. This improves performance.
      if (val !== this[storeProp]) {
        this[storeProp] = val;

        // the bindings <*, target, propertyKey> might not have been recorded yet (!)
        // fireChange will run idleTasks to record pending bindings, if any
        ObserveV2.getObserve().fireChange(this, propertyKey);
      }
    },
    enumerable: true
  });
  // this marks the proto as having at least one @Trace property inside
  // used by IsObservedObjectV2
  target[ObserveV2.V2_DECO_META] ??= {};
}; // trackInternal

// used to manually mark dirty v2 before animateTo
function __updateDirty2Immediately_V2_Change_Observation(): void {
  ObserveV2.getObserve().updateDirty2();
}