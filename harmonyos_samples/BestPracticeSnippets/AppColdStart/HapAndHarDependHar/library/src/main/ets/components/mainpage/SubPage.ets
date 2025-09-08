// [Start not_recommend_demo]
// library/src/main/ets/components/mainpage/SubPage.ets
// Global time-consuming functions in SubPage
const LARGE_NUMBER = 10000000;
function computeTask(): number {
  let count = 0;
  while (count < LARGE_NUMBER) {
    count++;
  }
  return count;
}
let count = computeTask();
// ...
// [End not_recommend_demo]