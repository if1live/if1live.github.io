import { Sample } from '@src/sample';

test('ok', () => {
  const x = new Sample();
  expect(x.foo()).toBe(1);
});
