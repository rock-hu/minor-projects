/**
 * 最佳实践：JSCrash类问题案例
 */

// [Start jscrash_case_analysis_6]
function b(a) {
    a.trim(); // 报错 undefined is not callable
}

export function c() {
    let a = 123;
    b(a);
}

let a = 123;
a(); // 123 is not callable
// [End jscrash_case_analysis_6]


// [Start jscrash_case_analysis_7]
// A.ets
import { Animal } from './B'
export let a = "this is A";
export function A() {
    return new Animal;
}

// ------------
// B.ets
import { a } from './A'
export class Animal {
    static {
        console.log("this is in class");
        let str = a; // 报错信息：a is not initialized
    }
}
// [End jscrash_case_analysis_7]

// [Start jscrash_case_analysis_8]
// B.ets
import { a } from './A'
export class Animal {
    static {
        console.log("this is in class");
    }
    str = a; // 修改点
}
// [End jscrash_case_analysis_8]

// [Start jscrash_case_analysis_9]
// module1.ets
import { a, b} from './module2'

export let i = 1;
export let m = a;
export const j = 2;
export const n = b;

// -------------
import { i, j } from './module1'

export let a = i; // 报错信息：Error Message: i is not initialized
export const b = j; // 报错信息：Error Message: j is not initialized
// [End jscrash_case_analysis_9]

// [Start jscrash_case_analysis_10]
// class1.ets
import { b } from './class2'

export class A {
    static a = b;
}

// -------------
// class2.ets
export let b = 1;

const i = A.a; // 报错信息：Error message: A is not initialized
const j = new A(); // 报错信息：Error message: A is not initialized
// [End jscrash_case_analysis_10]

// [Start jscrash_case_analysis_11]
// utils.ets
@Concurrent
export function printArgs(args: number): number {
    return args
}

// Index.ets
@Concurrent
function creatTask(a:number, b:number) {
    let sum = a + b;
    let task:taskpool.Task = new taskpool.Task(printArgs, sum);
    return task;
}
function resultError() {
    let task: taskpool.Task = new taskpool.Task(creatTask, 1, 2);
    taskpool.execute(task).then((res) => {
    }).catch((e:BusinessError)=>{
        console.error("resultError task catch message:" + e.message)
    })
}
// [End jscrash_case_analysis_11]

// [Start jscrash_case_analysis_12]
// utils.ets
@Concurrent
export function printArgs(args: number): number {
    return args
}

// Index.ets
@Concurrent
function creatTask(a:number, b:number) {
    let sum = a + b;
    return sum;
}
function resultError() {
    let task: taskpool.Task = new taskpool.Task(creatTask, 1, 2);
    taskpool.execute(task).then((res) => {
        let task2:taskpool.Task = new taskpool.Task(printArgs, res);
        console.info("resultError task success");
    }).catch((e:BusinessError)=>{
        console.error("resultError task catch message:" + e.message)
    })
}
// [End jscrash_case_analysis_12]

// [Start jscrash_case_analysis_13]
import { PersistenceV2 } from '@kit.ArkUI'
@ObservedV2
export class IntegrationUserInfo {
    userId: string = '';
    deviceId: string = '';
    static setUserInfo(userInfo: IntegrationUserInfo) {
        const us = PersistenceV2.connect(IntegrationUserInfo, 'IntegrationUserInfo' () => userInfo)!;
        us.userId = userInfo.userId;
        us.deviceId = userInfo.deviceId;
        PersistenceV2.save('IntegrationUserInfo');
    }
}
// [End jscrash_case_analysis_13]

// [Start jscrash_case_analysis_14]
@ObservedV2
export class IntegrationUserInfo {
    @Trace userId: string = '000';
    deviceId: string = '000';
}
// [End jscrash_case_analysis_14]
