import { RNPackage } from "./RNPackage"
import { ComponentBuilderContext } from "./EtsRNOHContext"

/**
 * @api
 */
export class RNOHPackage extends RNPackage {
  createWrappedCustomRNComponentBuilderByComponentNameMap(): Map<string, WrappedBuilder<[ComponentBuilderContext]>> {
    return new Map()
  }
}