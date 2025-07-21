/**
 * 最佳实践：内存泄漏检测
 * 场景一：分析Snapshot数据
 */

// [Start check_snapshot]
class People {
  old: number;
  name: string;
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

/**
 * 最佳实践：资源泄漏类问题优化建议
 */

// [Start resource_leak_advise1]
export default {
  data: {
    timer: null
  },
  onInit() {
    this.timer = setInterval(() => {
      this.updateData();
    }, 1000);
  },
  // 忘记在 onDestory 中清除定时器
}
// [End resource_leak_advise1]
