import { $r, $rawfile, AppStorage, ArkButton, ArkButtonComponent, ArkColumn, ArkCommonMethodComponent, ArkCommonMethodInterface, ArkPageTransitionEnterComponent, ArkPageTransitionExitComponent, ArkStructBase, ArkText, ArkTextComponent, CanvasRenderingContext2D, Color, CustomDialogController, DataChangeListener, Environment, ForEach, GestureGroup, IDataSource, LocalStorage, LongPressGesture, PanGesture, PanGestureOptions, PersistentStorage, PinchGesture, RenderingContextSettings, RotationGesture, Scroller, SubscribedAbstractProperty, SwipeGesture, SwiperController, TabsController, TapGesture, TextAreaController, VideoController, animateTo, fp2px, getContext, getInspectorByKey, lpx2px, px2fp, px2lpx, px2vp, stateOf, vp2px } from "@koalaui/arkui";
import { MutableState } from "@koalaui/runtime";
import { observableProxy } from "@koalaui/common";
function fancybut__Button<T extends ArkButtonComponent>(ButtonInstance: T, color: string | Color): T {
    return ButtonInstance.backgroundColor(color)
        .width(200)
        .height(100);
}
/** @memo:stable */
class ArkExtendComponentComponent extends ArkStructBase<ArkExtendComponentComponent, ExtendComponentOptions> {
    private _entry_local_storage_ = new LocalStorage();
    __initializeStruct(/**/
    /** @memo */
    content?: () => void, initializers?: ExtendComponentOptions): void {
        this.__backing_width_value = stateOf<string>(initializers?.width_value ?? ("100%"), this);
    }
    private __backing_width_value?: MutableState<string>;
    private get width_value(): string {
        return this.__backing_width_value!.value;
    }
    private set width_value(value: string) {
        this.__backing_width_value!.value = observableProxy(value);
    }
    /** @memo */
    __build(/**/
    /** @memo */
    __builder: ((__instance: ArkCommonMethodComponent) => void) | undefined, /**/
    /** @memo */
    content?: () => void, initializers?: ExtendComponentOptions) {
        ArkColumn(__builder, () => {
            ArkButton((__instance: ArkButtonComponent) => {
                __instance.__applyStyle(fancybut__Button, Color.Green);
            }, undefined, "Fancy Button");
            ArkButton((__instance: ArkButtonComponent) => {
                __instance.__applyStyle(fancybut__Button, Color.Green).height(100).width(this.width_value);
            }, undefined, "Fancy Button");
        });
    }
}
function globalFancy<T extends ArkCommonMethodInterface<T>>(CommonInstance: T): T {
    return CommonInstance.backgroundColor(Color.Red);
}
/** @memo:stable */
class ArkStylesComponentComponent extends ArkStructBase<ArkStylesComponentComponent, StylesComponentOptions> {
    private _entry_local_storage_ = new LocalStorage();
    __initializeStruct(/**/
    /** @memo */
    content?: () => void, initializers?: StylesComponentOptions): void {
        this.__backing_enable = initializers?.enable ?? (true);
        this.__backing_width_value = stateOf<string>(initializers?.width_value ?? ("100%"), this);
        this.__backing_size_value = stateOf<number>(initializers?.size_value ?? (50), this);
    }
    private __backing_enable?: boolean;
    private get enable(): boolean {
        return this.__backing_enable as boolean;
    }
    private set enable(value: boolean) {
        this.__backing_enable = value;
    }
    private __backing_width_value?: MutableState<string>;
    private get width_value(): string {
        return this.__backing_width_value!.value;
    }
    private set width_value(value: string) {
        this.__backing_width_value!.value = observableProxy(value);
    }
    private __backing_size_value?: MutableState<number>;
    private get size_value(): number {
        return this.__backing_size_value!.value;
    }
    private set size_value(value: number) {
        this.__backing_size_value!.value = observableProxy(value);
    }
    componentFancy<T extends ArkCommonMethodInterface<T>>(CommonInstance: T): T {
        return CommonInstance.backgroundColor(Color.Blue)
            .width(this.width_value);
    }
    /** @memo */
    __build(/**/
    /** @memo */
    __builder: ((__instance: ArkCommonMethodComponent) => void) | undefined, /**/
    /** @memo */
    content?: () => void, initializers?: StylesComponentOptions) {
        ArkColumn(__builder, () => {
            ArkText((__instance: ArkTextComponent) => {
                __instance.__applyStyle(globalFancy)
                    .width(this.width_value)
                    .height(100);
            }, undefined, "Fancy");
            ArkText((__instance: ArkTextComponent) => {
                __instance.__applyStyle(this.componentFancy.bind(this))
                    .fontSize(this.size_value)
                    .height(100);
            }, undefined, "Fancy");
            ArkButton((__instance: ArkButtonComponent) => {
                __instance.enabled(this.enable)
                    .onClick(() => {
                    this.enable = false;
                }).__applyStyle(this.componentFancy.bind(this))
                    .height(100)
                    .stateStyles({
                    normal: {
                        ButtonInstance: ButtonInstance.backgroundColor(Color.Green)
                            .width(this.width_value)
                    },
                    disabled: this.componentFancy,
                    pressed: globalFancy
                });
            }, () => {
                ArkText((__instance: ArkTextComponent) => {
                    __instance.__applyStyle(globalFancy)
                        .fontSize(this.size_value)
                        .height(100);
                }, undefined, "Fancy");
            });
        }, { space: 10 });
    }
}
export {};
/** @memo */
export function ExtendComponent(/**/
/** @memo */
style?: (__instance: ArkCommonMethodComponent) => void, /**/
/** @memo */
content?: () => void, initializers?: ExtendComponentOptions): void {
    const updatedInitializers: ExtendComponentOptions = {
        __backing_width_value: initializers?.__backing_width_value
    };
    ArkExtendComponentComponent._instantiate(style, () => new ArkExtendComponentComponent, content, updatedInitializers);
}
/** @memo */
export function StylesComponent(/**/
/** @memo */
style?: (__instance: ArkCommonMethodComponent) => void, /**/
/** @memo */
content?: () => void, initializers?: StylesComponentOptions): void {
    const updatedInitializers: StylesComponentOptions = {
        enable: initializers?.enable,
        __backing_width_value: initializers?.__backing_width_value,
        __backing_size_value: initializers?.__backing_size_value
    };
    ArkStylesComponentComponent._instantiate(style, () => new ArkStylesComponentComponent, content, updatedInitializers);
}
export interface ExtendComponentOptions {
    __backing_width_value?: MutableState<string>;
    width_value?: string;
}
export interface StylesComponentOptions {
    enable?: boolean;
    __backing_width_value?: MutableState<string>;
    width_value?: string;
    __backing_size_value?: MutableState<number>;
    size_value?: number;
}
