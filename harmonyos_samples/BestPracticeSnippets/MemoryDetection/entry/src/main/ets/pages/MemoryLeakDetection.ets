/**
 * 最佳实践：内存泄漏检测
 * 场景一：分析Snapshot数据
 */

// [Start check_snapshot]
class People {
  old: number
  name: string
  constructor(old: number, name: string) {
    this.old = old;
    this.name = name;
  }
  printOld() {
    console.log("old = ", this.old);
  }
  printName() {
    console.log("name = ", this.name);
  }
};
let p = new People(20, "Tom");
// [End check_snapshot]