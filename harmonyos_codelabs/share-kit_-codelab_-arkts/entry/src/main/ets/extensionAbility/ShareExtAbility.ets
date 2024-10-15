import { ShareExtensionAbility, UIExtensionContentSession, Want } from '@kit.AbilityKit';

interface IStorage {
  ShareExtAbilityWant: Want,
  ShareExtAbilitySession: UIExtensionContentSession
}

export default class ShareExtAbility extends ShareExtensionAbility {
  onSessionCreate(want: Want, session: UIExtensionContentSession) {
    let para: IStorage = { ShareExtAbilityWant: want, ShareExtAbilitySession: session };
    let storage = new LocalStorage(para);
    session.loadContent('pages/ShareExtDialog', storage);
  }
}
