/**
 * 最佳实践：JSCrash问题案例分析
 */

// [Start xxx_is_not_initialized_fix]
import { Animal } from './JSCrashCaseAnalyse11'
export let a = "this is A";
export function A() {
  return new Animal;
}
// [End xxx_is_not_initialized_fix]