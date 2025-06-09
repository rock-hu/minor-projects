// [Start for_each1]
interface IDataSource {
  totalCount(): number;
  getData(index: number): ESObject;
  registerDataChangeListener(listener: DataChangeListener): void;
  unregisterDataChangeListener(listener: DataChangeListener): void;
}

interface DataChangeListener {
  onDataReloaded(): void;
  onDataAdd(index: number): void;
  onDataMove(from: number, to: number): void;
  onDataDelete(index: number): void;
  onDataChange(index: number): void;
}
// [End for_each1]
@Entry
@Component
struct test {
// [Start for_each]
  ForEach(
    arr: ESObject[],
    itemGenerator: (item: ESObject, index?: number) => void,
    keyGenerator?: (item: ESObject, index?: number) => string
  ){}
// [End for_each]

// [Start for_each1]
  LazyForEach(
    dataSource: IDataSource,
    itemGenerator: (item: ESObject) => void,
    keyGenerator?: (item: ESObject) => string
  ): void{}
// [End for_each1]
  build() {}
}