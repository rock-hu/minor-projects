/*
 * @Author: chenhaoyue
 * @Date: 2023-12-07 10:13:12
 * @LastEditTime: 2023-12-07 10:13:15
 */
import type {TurboModule} from 'react-native/Libraries/TurboModule/RCTExport';
import {TurboModuleRegistry} from 'react-native';

interface LogTurboModuleProtocol {
  Log(arg: string): void;
}

interface Spec extends TurboModule, LogTurboModuleProtocol {}
const LogTurboModule = TurboModuleRegistry.get<Spec>('RNCLog')!;
export function HMLog(arg: string) {
  LogTurboModule.Log(arg);
}
