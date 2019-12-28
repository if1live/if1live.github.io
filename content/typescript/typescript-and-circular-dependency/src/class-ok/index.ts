import { A } from './class-a';
import { B } from './class-b';

console.log({ A });
console.log({ B });

const a = new A();
const b = new B();

b.sample();
a.sample();
