// [Start Case8]
import HashMap from '@ohos.util.HashMap';

class InfoUtil {
  getInfo(t1: string, t2: string): string {
    if (t1 === t2) {
      return "";
    }
    // 此处改为使用HashMap进行读写操作
    let info: HashMap<string, string> = new HashMap();
    this.setInfo(info);
    let t3 = info.get(t2);
    return (t3 != null) ? t3 : "";
  }
  setInfo(info:HashMap<string, string>) {
    // ...
    info.set('aaa','aaa');
    info.set('bbb','bbb');
    info.set('ccc','ccc');
  }
}
// [End Case8]