import { HostComponent, ViewProps } from "react-native";
import codegenNativeComponent from "react-native/Libraries/Utilities/codegenNativeComponent";

export interface NativeContainerViewProps extends ViewProps {}

export default codegenNativeComponent<NativeContainerViewProps>(
  "ContainerView",
) as HostComponent<NativeContainerViewProps>;
