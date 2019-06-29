let counter = 0;

function print(message) {
  counter += 1;
  console.log(`${message} : ${counter}`);
}

beforeAll(() => print('before all - global'));
afterAll(() => print('after all - global'));
beforeEach(() => print('before each - global'));
afterEach(() => print('after each - global'));

describe('foo', () => {
  beforeAll(() => print('before all - local'));
  afterAll(() => print('after all - local'));
  beforeEach(() => print('before each - local'));
  afterEach(() => print('after each - local'));

  test('ok', () => {
    expect(1).toBe(1);
    print('ok');
  });
  test('ok2', () => {
    expect(1).toBe(1);
    print('ok2');
  });
  test.skip('ok2', () => expect(1).toBe(1));
});

describe.skip('skip', () => {
  test('ok', () => expect(1).toBe(1));
});
