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

// [Start resource_leak_advise1_negative]
export default {
  data: {
    timer: null
  },
  onInit() {
    this.timer = setInterval(() => {
      this.updateData();
    }, 1000);
  },
  // 忘记在 onDestroy 中清除定时器
}
// [End resource_leak_advise1_negative]

// [Start resource_leak_advise1_positive]
export default {
  data: {
    timer: null
  },
  onInit() {
    // 设置定时器
    this.timer = setInterval(() => {
      this.updateData();
    }, 1000);
  },
  onDestroy() {
    // 清除定时器
    if (this.timer) {
      clearInterval(this.timer);
      this.timer = null; // 可选：将timer充值为null
    }
  },
  updateData() {
    // 数据更新逻辑
  }
}
// [End resource_leak_advise1_positive]
