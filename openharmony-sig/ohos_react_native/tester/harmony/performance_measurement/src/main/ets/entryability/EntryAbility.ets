import {RNAbility} from 'rnoh';

export default class EntryAbility extends RNAbility {
  getPagePath() {
    return 'pages/Index';
  }

  getRNOHWorkerScriptUrl() {
    return "performance_measurement/ets/workers/RNOHWorker.ets"
  }
}
