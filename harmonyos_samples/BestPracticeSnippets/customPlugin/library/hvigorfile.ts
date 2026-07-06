// [Start har_tasks]
import { harTasks } from '@ohos/hvigor-ohos-plugin';
import { renameHarTask } from '@bpta/custom_plugin';

export default {
  system: harTasks, /* Built-in plugin of Hvigor. It cannot be modified. */
  plugins: [renameHarTask()]         /* Custom plugin to extend the functionality of Hvigor. */
}
// [End har_tasks]