import { ArkCommonMethodComponent, ArkFlex, ArkFlexComponent, ArkPageTransitionEnterComponent, ArkPageTransitionExitComponent, ArkStructBase, ArkText, FlexAlign, FlexDirection, FlexOptions, GestureEvent, ItemAlign, stateOf } from "@koalaui/arkts-arkui";
import { MutableState } from "@koalaui/runtime";
import { LocalStorage } from "@koalaui/arkui-common";
import { observableProxy } from "@koalaui/common";
/** @memo:stable */
class ArkLongPressGestureExampleComponent extends ArkStructBase<ArkLongPressGestureExampleComponent, LongPressGestureExampleOptions> {
    private _entry_local_storage_ = new LocalStorage();
    __initializeStruct(/**/
    /** @memo */
    content?: () => void, initializers?: LongPressGestureExampleOptions): void {
        this.__backing_count = stateOf<number>(initializers?.count ?? (0), this);
    }
    private __backing_count?: MutableState<number>;
    private get count(): number {
        return this.__backing_count!.value;
    }
    private set count(value: number) {
        this.__backing_count!.value = observableProxy(value);
    }
    /** @memo */
    __build(/**/
    /** @memo */
    __builder: ((__instance: ArkCommonMethodComponent) => void) | undefined, /**/
    /** @memo */
    content?: () => void, initializers?: LongPressGestureExampleOptions) {
        ArkFlex(__instance => {
            {
                __instance.height(200).width(300).padding(60).border({ width: 1 }).margin(30)
                    .gesture(LongPressGesture({ repeat: true })
                    .onAction((event: GestureEvent) => {
                    if (event.repeat) {
                        this.count++;
                    }
                })
                    .onActionEnd(() => {
                    this.count = 0;
                }));
            }
            __builder?.(__instance);
        }, () => {
            ArkText(undefined, undefined, 'LongPress onAction:' + this.count);
        }, { direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween } as FlexOptions);
    }
}
export {};
/** @memo */
export function LongPressGestureExample(/**/
/** @memo */
style?: (__instance: ArkCommonMethodComponent) => void, /**/
/** @memo */
content?: () => void, initializers?: LongPressGestureExampleOptions): void {
    const updatedInitializers: LongPressGestureExampleOptions = {
        __backing_count: initializers?.__backing_count
    };
    ArkLongPressGestureExampleComponent._instantiate(style, () => new ArkLongPressGestureExampleComponent, content, updatedInitializers);
}
export interface LongPressGestureExampleOptions {
    __backing_count?: MutableState<number>;
    count?: number;
}
