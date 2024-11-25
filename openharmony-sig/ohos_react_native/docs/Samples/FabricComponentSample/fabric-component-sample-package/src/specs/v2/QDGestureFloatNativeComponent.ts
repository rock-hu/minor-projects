import { ViewProps, HostComponent, ProcessedColorValue } from 'react-native';
import type { ColorValue } from "react-native/Libraries/StyleSheet/StyleSheet";
import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';
import codegenNativeCommands from 'react-native/Libraries/Utilities/codegenNativeCommands';
import type {
  Float,
  DirectEventHandler,
} from 'react-native/Libraries/Types/CodegenTypes';

export interface ScrollEvent {
  offsetY: Float;
}

export interface NativeProps extends ViewProps {
  stopPercent?: Float,
  stopPercentMax?: Float,
  onScroll?: DirectEventHandler<ScrollEvent>;
}

type NativeType = HostComponent<NativeProps>;

interface NativeCommands {
  scrollTo: (
    viewRef: React.ElementRef<NativeType>,
    offsetY: Float,
    animated: boolean,
  ) => void;
}

export const Commands: NativeCommands = codegenNativeCommands<NativeCommands>({
  supportedCommands: ['scrollTo'],
});

export default codegenNativeComponent<NativeProps>(
  'QDGestureFloat',
) as HostComponent<NativeProps>;
