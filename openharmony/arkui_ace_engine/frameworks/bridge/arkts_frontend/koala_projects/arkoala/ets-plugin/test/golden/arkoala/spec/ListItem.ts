import { ArkCommonMethodComponent, ArkList, ArkListItem, ArkListItemComponent, ArkPageTransitionEnterComponent, ArkPageTransitionExitComponent, ArkStructBase, ArkText, ArkTextComponent, LocalStorage } from "@koalaui/arkoala-arkui";
import { registerArkuiEntry } from "@koalaui/arkoala-arkui/ohos.router";
/** @memo:stable */
class ArkParentViewComponent extends ArkStructBase<ArkParentViewComponent, ParentViewOptions> {
    private _entry_local_storage_ = new LocalStorage();
    __initializeStruct(/**/
    /** @memo */
    content?: () => void, initializers?: ParentViewOptions): void {
    }
    /** @memo */
    __build(/**/
    /** @memo */
    __builder: ((__instance: ArkCommonMethodComponent) => void) | undefined, /**/
    /** @memo */
    content?: () => void, initializers?: ParentViewOptions) {
        ArkList(__builder, () => {
            ArkListItem((__instance: ArkListItemComponent) => {
                __instance.width(200).height(100);
            }, () => {
                ArkText((__instance: ArkTextComponent) => {
                    __instance.width(100);
                }, undefined, 'xx');
            }, 'true');
        });
    }
}
export {};
/** @memo */
export function ParentView(/**/
/** @memo */
style?: (__instance: ArkCommonMethodComponent) => void, /**/
/** @memo */
content?: () => void, initializers?: ParentViewOptions): void {
    const updatedInitializers: ParentViewOptions = {};
    ArkParentViewComponent._instantiate(style, () => new ArkParentViewComponent, content, updatedInitializers);
}
export interface ParentViewOptions {
}
registerArkuiEntry(ParentView, "ListItem");
export const __Entry = ParentView;
