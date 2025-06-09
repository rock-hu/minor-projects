import { ArkColumn, ArkCommonMethodComponent, ArkPageTransitionEnterComponent, ArkPageTransitionExitComponent, ArkReusableStruct, ArkStructBase, Color, SyncedProperty, propState, stateOf } from "@koalaui/arkts-arkui";
import { MutableState } from "@koalaui/runtime";
import { LocalStorage } from "@koalaui/arkui-common";
import { observableProxy } from "@koalaui/common";
let a: string = "aaaaaaaaaa";
/** @memo:stable */
class ArkHomeComponentComponent extends ArkStructBase<ArkHomeComponentComponent, HomeComponentOptions> {
    private _entry_local_storage_ = new LocalStorage();
    __initializeStruct(/**/
    /** @memo */
    content?: () => void, initializers?: HomeComponentOptions): void {
        this.__backing_state_value = stateOf<string>(initializers?.state_value ?? ("100%"), this);
        this.__backing_value = stateOf<number>(initializers?.value ?? (1), this);
    }
    private __backing_state_value?: MutableState<string>;
    private get state_value(): string {
        return this.__backing_state_value!.value;
    }
    private set state_value(value: string) {
        this.__backing_state_value!.value = observableProxy(value);
    }
    private __backing_value?: MutableState<number>;
    private get value(): number {
        return this.__backing_value!.value;
    }
    private set value(value: number) {
        this.__backing_value!.value = observableProxy(value);
    }
    /** @memo */
    __build(/**/
    /** @memo */
    __builder: ((__instance: ArkCommonMethodComponent) => void) | undefined, /**/
    /** @memo */
    content?: () => void, initializers?: HomeComponentOptions) {
        ArkColumn(__builder, () => {
            child((__instance: ArkCommonMethodComponent) => {
                __instance.border({ width: 3, color: Color.Red })
                    .width(this.state_value)
                    .reuseId("reuse_key");
            }, undefined, { propvalue: this.value, linkvalue: this.value } as childOptions);
            child((__instance: ArkCommonMethodComponent) => {
                __instance.border({ width: 3, color: Color.Red })
                    .reuseId(this.state_value);
            }, undefined, { propvalue: this.value, linkvalue: this.value } as childOptions);
            child((__instance: ArkCommonMethodComponent) => {
                __instance.reuseId("reuse_key11111111111");
            }, undefined, { propvalue: this.value, linkvalue: this.value } as childOptions);
            child((__instance: ArkCommonMethodComponent) => {
                __instance.border({ width: 3, color: Color.Red })
                    .width(this.state_value);
            }, undefined, { propvalue: this.value, linkvalue: this.value } as childOptions);
            child((__instance: ArkCommonMethodComponent) => {
                __instance.reuseId(a);
            }, undefined, { propvalue: this.value, linkvalue: this.value } as childOptions);
        });
    }
}
/** @memo:stable */
class ArkchildComponent extends ArkReusableStruct<ArkchildComponent, childOptions> {
    private _entry_local_storage_ = new LocalStorage();
    __initializeStruct(/**/
    /** @memo */
    content?: () => void, initializers?: childOptions): void {
        this.__backing_propvalue = propState<number>();
        this.__backing_linkvalue = initializers!.__backing_linkvalue!;
    }
    private __backing_propvalue?: SyncedProperty<number>;
    private get propvalue(): number {
        return this.__backing_propvalue!.value;
    }
    private set propvalue(value: number) {
        this.__backing_propvalue!.value = observableProxy(value);
    }
    private __backing_linkvalue?: MutableState<number>;
    private get linkvalue(): number {
        return this.__backing_linkvalue!.value;
    }
    private set linkvalue(value: number) {
        this.__backing_linkvalue!.value = observableProxy(value);
    }
    /** @memo */
    __updateStruct(initializers: childOptions | undefined): void {
        this.__backing_propvalue?.update(initializers?.propvalue);
    }
    __rebindStates(initializers?: childOptions): void {
        this.__backing_propvalue?.update(initializers?.propvalue);
        this.__backing_linkvalue = initializers!.__backing_linkvalue!;
    }
    /** @memo */
    __build(/**/
    /** @memo */
    __builder: ((__instance: ArkCommonMethodComponent) => void) | undefined, /**/
    /** @memo */
    content?: () => void, initializers?: childOptions) {
        ArkColumn(__builder, undefined);
    }
}
export {};
/** @memo */
export function HomeComponent(/**/
/** @memo */
style?: (__instance: ArkCommonMethodComponent) => void, /**/
/** @memo */
content?: () => void, initializers?: HomeComponentOptions): void {
    const updatedInitializers: HomeComponentOptions = {
        __backing_state_value: initializers?.__backing_state_value,
        __backing_value: initializers?.__backing_value
    };
    ArkHomeComponentComponent._instantiate(style, () => new ArkHomeComponentComponent, content, updatedInitializers);
}
/** @memo */
export function child(/**/
/** @memo */
style?: (__instance: ArkCommonMethodComponent) => void, /**/
/** @memo */
content?: () => void, initializers?: childOptions): void {
    const updatedInitializers: childOptions = {
        propvalue: initializers?.propvalue,
        __backing_propvalue: initializers?.__backing_propvalue,
        __backing_linkvalue: initializers?.__backing_linkvalue
    };
    ArkchildComponent._instantiate(style, () => new ArkchildComponent, content, updatedInitializers, "ArkchildComponent");
}
export interface HomeComponentOptions {
    __backing_state_value?: MutableState<string>;
    state_value?: string;
    __backing_value?: MutableState<number>;
    value?: number;
}
export interface childOptions {
    __backing_propvalue?: SyncedProperty<number>;
    propvalue?: number;
    __backing_linkvalue?: MutableState<number>;
    linkvalue?: number;
}
