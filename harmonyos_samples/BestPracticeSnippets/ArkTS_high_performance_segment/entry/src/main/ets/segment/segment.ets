// [Start Counter_example1]
// 该变量在后续过程并未发生改变，建议声明为常量
let PRICE = 10000;

function getPrice() {
  return PRICE;
}

class ClassA {
  propA: string = 'propA'
}

// 该引用类型的变量在后续过程变量地址并未发生改变，仅修改了变量的属性,本例中的let建议改为const
let classA: ClassA = new ClassA();
classA.propA = 'Property A';
// [End Counter_example1]

// [Start Case3]
// lib.ets
export interface TestA {
  sum: number
}

export function getObject(value: number): TestA {
  let obj: TestA = { sum: value };
  return obj;
}
// [End Case3]

// [Start Counter_example2]
class Time {
  static START: number = 1987;
  static INFO: number[] = [2001, 2002, 2003, 2004, 2005, 2006]
}


function getDay(year: number): number {
  let totalDays: number = 348;
  for (let index: number = 0x8000; index > 0x8; index >>= 1) {
    // 此处会多次对Time的INFO及START进行查找，并且每次查找出来的值是相同的
    totalDays += ((Time.INFO[year - Time.START] & index) !== 0) ? 1 : 0;
  }
  return totalDays;
}
// [End Counter_example2]


// [Start Counter_example3]
class Counter {
  // 没有设置访问修饰符默认为public
  count: number = 0;


  getCount(): number {
    return this.count;
  }
}
// 访问时
const counter:Counter = new Counter();
console.info(counter.count.toString()); // 可以通过实例访问
console.info(counter.getCount().toString());
// [End Counter_example3]


// [Start Counter_example4]
const array1 = new Array(1, 2, 3);  // 针对这一场景，建议不要使用new Array(1, 2, 3)
const array2 = new Array(4, 5, 6);  // 针对这一场景，建议不要使用new Array(4, 5, 6)
const res = new Array<number>(3);
for (let i = 0; i < 3; i++) {
  res[i] = array1[i] + array2[i];
}
// [End Counter_example4]

// [Start Counter_example5]
class InfoUtil {
  getInfo(t1: string, t2: string): string {
    if (t1 === t2) {
      return "";
    }
    // 此处使用Record普通对象作为容器
    let info: Record<string, string> = {};
    this.setInfo(info);
    let t3 = info[t2];
    return (t3 != null) ? t3 : "";
  }


  setInfo(info: Record<string, string>) {
    // 接口内部实际上进行的是map的操作
    info.aaa = 'aaa';
    info.bbb = 'bbb';
    info.ccc = 'ccc';
  }
}
// [End Counter_example5]

// [Start Counter_example6]
// 下面情形会变成稀疏数组
// 1. 直接分配100000大小的数组，虚拟机会处理成用hash表来存储元素
let count = 100000;
let result: number[] = new Array(count);
// 2. 分配数组之后直接，在9999处初始化，会变成稀疏数组
result[9999] = 0;
// [End Counter_example6]


// [Start Counter_example7]
const arr:number[] = [0, 1, 2];
function foo() {
  arr[0] = 1;
  // arr 保持对外部变量的使用
  return arr[0] + arr[1];
}
let aFoo = foo;
aFoo();
// [End Counter_example7]