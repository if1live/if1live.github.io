import { A } from './class-a';
console.log(`import A: ${A}`);

export class B {
  public sample() {
    console.log(`function A: ${A}`);
    return new A();
  }
}
