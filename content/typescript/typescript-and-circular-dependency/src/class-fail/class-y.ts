import { X } from './class-x';

console.log({ X });
const x = new X();

export class Y {
  public static sample() { return x; }
}
