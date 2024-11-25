import {RNAbility} from 'rnoh';

export default class EntryAbility extends RNAbility {
  override getPagePath() {
    return 'pages/Index';
  }

  override getRNOHWorkerScriptUrl() {
    return "entry/ets/workers/RNOHWorker.ets"
  }
}
