import { ArkCommonMethodComponent, ArkFlex, ArkFlexComponent, ArkPageTransitionEnterComponent, ArkPageTransitionExitComponent, ArkStructBase, ArkText, FlexAlign, FlexDirection, FlexOptions, GestureEvent, ItemAlign, stateOf } from "@koalaui/arkts-arkui";
import { MutableState } from "@koalaui/runtime";
import { LocalStorage } from "@koalaui/arkui-common";
import { observableProxy } from "@koalaui/common";
/** @memo:stable */
class ArkRotationGestureExampleComponent extends ArkStructBase<ArkRotationGestureExampleComponent, RotationGestureExampleOptions> {
    private _entry_local_storage_ = new LocalStorage();
    __initializeStruct(/**/
    /** @memo */
    content?: () => void, initializers?: RotationGestureExampleOptions): void {
        this.__backing_angle = stateOf<number>(initializers?.angle ?? (0), this);
    }
    private __backing_angle?: MutableState<number>;
    private get angle(): number {
        return this.__backing_angle!.value;
    }
    private set angle(value: number) {
        this.__backing_angle!.value = observableProxy(value);
    }
    /** @memo */
    __build(/**/
    /** @memo */
    __builder: ((__instance: ArkCommonMethodComponent) => void) | undefined, /**/
    /** @memo */
    content?: () => void, initializers?: RotationGestureExampleOptions) {
        ArkFlex(__instance => {
            {
                __instance.height(100).width(200).padding(20).border({ width: 1 })
                    .margin(80).rotate({ x: 1, y: 2, z: 3, angle: this.angle })
                    .gesture(RotationGesture()
                    .onActionStart((event: GestureEvent) => {
                    console.log('Rotation start');
                })
                    .onActionUpdate((event: GestureEvent) => {
                    this.angle = event.angle;
                })
                    .onActionEnd(() => {
                    console.log('Rotation end');
                }));
            }
            __builder?.(__instance);
        }, () => {
            ArkText(undefined, undefined, 'RotationGesture angle:' + this.angle);
        }, { direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween } as FlexOptions);
    }
}
export {};
/** @memo */
export function RotationGestureExample(/**/
/** @memo */
style?: (__instance: ArkCommonMethodComponent) => void, /**/
/** @memo */
content?: () => void, initializers?: RotationGestureExampleOptions): void {
    const updatedInitializers: RotationGestureExampleOptions = {
        __backing_angle: initializers?.__backing_angle
    };
    ArkRotationGestureExampleComponent._instantiate(style, () => new ArkRotationGestureExampleComponent, content, updatedInitializers);
}
export interface RotationGestureExampleOptions {
    __backing_angle?: MutableState<number>;
    angle?: number;
}
