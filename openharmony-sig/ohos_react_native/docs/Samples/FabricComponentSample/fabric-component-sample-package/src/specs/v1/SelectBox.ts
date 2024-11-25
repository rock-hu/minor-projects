import React from 'react';
import type { ViewProps } from "react-native/Libraries/Components/View/ViewPropTypes";
import type { HostComponent } from "react-native";
import codegenNativeComponent from "react-native/Libraries/Utilities/codegenNativeComponent";
import codegenNativeCommands from 'react-native/Libraries/Utilities/codegenNativeCommands';
import type { Int32, DirectEventHandler } from 'react-native/Libraries/Types/CodegenTypes';

export interface SelectItems {
  id: Int32;
  text: string;
}

export interface Res {
  value: Int32[];
}

export interface NativeProps extends ViewProps, Res {
  data: SelectItems[];
  onChange: DirectEventHandler<Res>;
  // 添加其它 props
}

type NativeType = HostComponent<NativeProps>;

interface NativeCommands {
  emitNativeEvent: (
    viewRef: React.ElementRef<NativeType>,
    mutil: boolean,
  ) => void;
}

export const Commands: NativeCommands = codegenNativeCommands<NativeCommands>({
  supportedCommands: ['emitNativeEvent'],
});

export default codegenNativeComponent<NativeProps>(
  "SelectBox"
) as HostComponent<NativeProps>;
