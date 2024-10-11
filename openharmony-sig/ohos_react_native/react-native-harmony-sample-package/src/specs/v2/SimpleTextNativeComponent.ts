import { ViewProps, HostComponent } from 'react-native';
import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';

import type { DirectEventHandler } from 'react-native/Libraries/Types/CodegenTypes';

export interface TextChangeEvent {
  text: string;
}

export interface SimpleTextNativeProps extends ViewProps {
  onTextChange: DirectEventHandler<TextChangeEvent>;
  text: string;
}

export default codegenNativeComponent<SimpleTextNativeProps>(
  'SimpleText'
) as HostComponent<SimpleTextNativeProps>;
