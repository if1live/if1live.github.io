const a = -3 % 3;
const b = 3 % 3;

test('==', () => expect(a == b).toBeTruthy());
test('===', () => expect(a === b).toBeTruthy());
test('toEqual', () => expect(a).toEqual(b));
test('toBe', () => expect(a).toBe(b));
test('Object.is', () => expect(Object.is(a, b)).toBeTruthy());
test('toBeCloseTo', () => expect(a).toBeCloseTo(b));
