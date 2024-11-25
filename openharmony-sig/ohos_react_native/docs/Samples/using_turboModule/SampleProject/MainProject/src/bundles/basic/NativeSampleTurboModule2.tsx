import { TurboModule, TurboModuleRegistry } from "react-native";

export type Fruit = {
    name: string;
};

export interface EnglishNovelModel {
    content: string
    tts: string
    note: string
    dateline: string
    img: string
  }
  
export interface ResultModel {
    code: number
    result: EnglishNovelModel
    msg: string
}

export interface Error {
    name: string;
    message: string;
    stack?: string;
}

export interface SpecSampleTurboModule2 extends TurboModule {  
    checkPwd(param:{}, success: (res: {})=>void, failure: (res: {})=>void): void;
    eatFruit(fruit: Fruit): void;
    test(): void;
    getRequest(): Promise<ResultModel>;
    getObject(obj:{}): void;
}
  
export default TurboModuleRegistry.getEnforcing<SpecSampleTurboModule2>('SampleTurboModule2');