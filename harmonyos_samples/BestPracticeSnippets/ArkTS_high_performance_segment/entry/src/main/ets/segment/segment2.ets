// [Start Case4]
// app.ets
import { getObject, TestA } from './segment';

let obj: TestA = getObject(123); // 显式引入标注类型
// [End Case4]

// [Start Case]
const PRICE = 10000; // 常量声明为基础类型时，其内容不可改变

function getPrice() {
  return PRICE;
}

class ClassA {
  propA: string = 'propA'
}

// 常量声明为引用类型时，其地址不可改变，但可以改变属性
const classA: ClassA = new ClassA();
classA.propA = 'Property A';
// [End Case]

// [Start Case2]
function calAddSum(addNum: number): number {
  // count预期是int，不要声明成undefined/null或0.0，直接初始化为0
  let count = 0;
  count += addNum;
  return count;
}
// [End Case2]

// [Start Case5]
// 优化后代码
class Time {
  static START: number = 1987;
  static INFO: number[] = [2001, 2002, 2003, 2004, 2005, 2006];
}


function getDay(year: number): number {
  let totalDays: number = 348;
  // 从循环中提取不变量
  const info = Time.INFO[year - Time.START];
  for (let index: number = 0x8000; index > 0x8; index >>= 1) {
    if ((info & index) !== 0) {
      totalDays++;
    }
  }
  return totalDays;
}
// [End Case5]

// [Start Case6]
class Counter {
  // 设置访问修饰符为private
  private count: number = 0;


  public getCount(): number {
    return this.count;
  }
}
// 访问时
const counter:Counter = new Counter();
let res = counter.getCount();
// [End Case6]

// [Start Case7]
const typedArray1 = new Int8Array([1, 2, 3]);  // 针对这一场景，建议不要使用new Array([1, 2, 3])
const typedArray2 = new Int8Array([4, 5, 6]);  // 针对这一场景，建议不要使用new Array([4, 5, 6])
const res1 = new Int8Array(3);
for (let i = 0; i < 3; i++) {
  res1[i] = typedArray1[i] + typedArray2[i];
}
// [End Case7]

// [Start Case9]
let arr: number[] = [0, 1, 2];
// 改为通过参数传递
function foo(array: Array<number>): number {
  array[0] = 1;
  return array[0] + array[1];
}

foo(arr);
// [End Case9]