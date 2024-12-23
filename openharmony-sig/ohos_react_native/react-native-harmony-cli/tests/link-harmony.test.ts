import { ReactNativeFixture } from './ReactNativeFixture';

it('list link-harmony in the help description', async () => {
  const result = new ReactNativeFixture('.').help();

  expect(result.includes('link-harmony')).toBeTruthy();
});