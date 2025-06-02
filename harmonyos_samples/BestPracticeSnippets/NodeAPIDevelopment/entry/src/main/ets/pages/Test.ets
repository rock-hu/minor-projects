/**
 *Node-API开发规范
 */
// [Start create_my_external]
// application code
import testNapi from 'libentry.so';
import { worker } from '@kit.ArkTS';

const mWorker = new worker.ThreadWorker('../workers/Worker');

{
  const mExternalObj = testNapi.createMyExternal();

  mWorker.postMessage(mExternalObj);

}

// Close the worker thread
// The application may crash at this step or during subsequent engine GC
mWorker.terminate();
// The implementation of Worker is the default template, omitted here
// [End create_my_external]