import { B } from './class-b';
console.log(`import B: ${B}`);

export class A {
  public sample() {
    console.log(`function B: ${B}`);
    return new B();
  }
}
