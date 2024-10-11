import {
    ViewProps,
    HostComponent,
    requireNativeComponent,  // Fabric
} from 'react-native';
import type { 
    DirectEventHandler
} from "react-native/Libraries/Types/CodegenTypes";
import codegenNativeComponent from "react-native/Libraries/Utilities/codegenNativeComponent";

export type OnStopEventData = Readonly<{
    isStop: boolean,
    type: string,
}>;

export interface MarqueeViewProps extends ViewProps {
    src: string,
    onStop?: DirectEventHandler<OnStopEventData>;
}

export default codegenNativeComponent<MarqueeViewProps>(
    'MarqueeView',
) as HostComponent<MarqueeViewProps>;
