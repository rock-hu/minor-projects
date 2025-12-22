/**
 * 最佳实践：JSCrash问题案例分析
 */

// [Start JSCrashCaseAnalyse12]
import { Animal } from './JSCrashCaseAnalyse11'
export let a = "this is A";
export function A() {
  return new Animal;
}
// [End JSCrashCaseAnalyse12]