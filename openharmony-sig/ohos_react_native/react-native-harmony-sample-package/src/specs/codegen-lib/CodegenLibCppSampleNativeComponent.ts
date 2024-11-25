import { ViewProps, HostComponent } from 'react-native';
import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';
import type { DirectEventHandler } from 'react-native/Libraries/Types/CodegenTypes';

export interface MountEvent {
  text: string;
}

export interface CodegenLibCppSampleComponentProps extends ViewProps {
  onMount: DirectEventHandler<MountEvent>;
  text: string;
}

export default codegenNativeComponent<CodegenLibCppSampleComponentProps>(
  'CodegenLibCppSampleComponent'
) as HostComponent<CodegenLibCppSampleComponentProps>;
