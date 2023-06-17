import _ from "lodash";
import Lodash from "lodash";

{
  type Entry = { name: string };
  const list: Entry[] = [
    { name: "B" },
    { name: "A" },
    { name: "D" },
    { name: "C" },
  ];

  // Not correct
  console.log('maxBy: Not correct', _.maxBy(list));

  // Correct
  console.log('maxBy: Correct', _.maxBy(list, x => x.name));
}
