import {
  ComponentBuilderContext,
} from '@rnoh/react-native-openharmony';

@Builder
export function buildCustomRNComponent(ctx: ComponentBuilderContext) {
  Stack() {}
  .position({x:0, y: 0})
}

export const arkTsComponentNames: Array<string> = [];
