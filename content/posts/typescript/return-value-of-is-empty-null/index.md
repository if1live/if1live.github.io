---
title: isEmpty(null)의 리턴 값은?
subtitle: true, false, 그리고 제3의 선택
tags: [typescript, lodash, just, ramda, remeda]
slug: return-value-of-is-empty-null
author: if1live
date: 2023-08-01
---

## isEmpty(null)

옛날 옛적에 lodash를 사용해서 아래와 같은 함수를 만들었다.

```ts
/**
 * prev를 next의 부분을 Partial<CharacterModel>로 리턴
 * prev와 next가 똑같다면 null
 */
function compare(
  prev: CharacterModel,
  next: CharacterModel,
): Partial<CharacterModel> | null {
  ...
}

const delta_character = compare(prev, next);
const diff_character = _.isEmpty(delta_character) ? delta_character : null;
```

lodash로 몇년동안 쓰다가 [lodash를 버리고 remeda로 마이그레이션][escape-from-lodash-remeda] 했다.
당시의 remeda에는 `isEmpty()`가 구현되어 있지 않아서 [remeda의 틈을 just][escape-from-lodash-just]로 막았다.
지금은 remeda에 `isEmpty()`가 구현되어서 just-is-empty를 사용하지 않아도 된다.
remeda 버전 올리면서 just-is-empty 지우고 `R.isEmpty()` 쓰게 바꿨더니 문제가 생겼다.

글을 읽기 전에 `isEmpty(null)`는 어떻게 동작하는게 올바른 명세일지 생각해보자.
그렇게 동작해야 된다고 생각한 이유는 무엇인가?

## 라이브러리

몇가지 라이브러리의 `isEmpty()` 명세를 뒤져봤다.
주로 뒤져본 항목은 다음과 같다.

1. 작성일 기준 최신 코드 또는 명세
2. 주석으로 `isEmpty(null)`가 언급된 시점
3. `isEmpty()`가 처음 구현된 시점

### lodash

lodash 4.17.15 `_.isEmpty(value)`의 [문서](https://lodash.com/docs/4.17.15#isEmpty)

> Checks if value is an empty object, collection, map, or set.
> Objects are considered empty if they have no own enumerable string keyed properties.
> Array-like values such as arguments objects, arrays, buffers, strings,
> or jQuery-like collections are considered empty if they have a length of 0.
> Similarly, maps and sets are considered empty if they have a size of 0.

```
_.isEmpty(null);
// => true
```

[52e96c3](https://github.com/lodash/lodash/commit/52e96c38dc52c79e0b73679456c7c6e092ffa775)부터 isEmpty에서 null을 명시적으로 처리했다.
커밋 메세지는 "Exit early from _.isEmpty for nullish values." 커밋 날짜는 2016-10-09

```
     function isEmpty(value) {
+      if (value == null) {
+        return true;
+      }
       if (isArrayLike(value) &&
```

loash에서 `_.isEmpty(null)`를 주석으로 처음 언급한 것은 [8c61878](https://github.com/lodash/lodash/commit/8c61878)으로 추정된다.
JSDoc에서 `_.isEmpty(null)`이 처음으로 등장하더라 (유닛테스트로 isEmpty를 취급하는건 더 나중의 일인거같지만)
커밋 날짜는 2014-05-04

```
     /**
-     * Checks if `value` is empty. Arrays, strings, or `arguments` objects with a
-     * length of `0` and objects with no own enumerable properties are considered
-     * "empty".
+     * Checks if a collection is empty. A value is considered empty unless it is
+     * an array, array-like object, or string with a length greater than `0` or
+     * an object with own properties.
      *
      * @static
      * @memberOf _
      * @category Objects
      * @param {Array|Object|string} value The value to inspect.
-     * @returns {boolean} Returns `true` if the `value` is empty, else `false`.
+     * @returns {boolean} Returns `true` if `value` is empty, else `false`.
      * @example
      *
-     * _.isEmpty([1, 2, 3]);
-     * // => false
+     * _.isEmpty(null);  <----------------------
+     * // => true
      *
-     * _.isEmpty({});
+     * _.isEmpty(true);
      * // => true
      *
-     * _.isEmpty('');
+     * _.isEmpty(1);
      * // => true
+     *
+     * _.isEmpty([1, 2, 3]);
+     * // => false
+     *
+     * _.isEmpty({ 'a': 1 });
+     * // => false
      */
      function isEmpty(value) {
```

[b5920e9](https://github.com/lodash/lodash/commit/b5920e94a3221bbd8c6146d9839408030eb1922a)
"added an isEmpty function that works on arrays and objects"에서 처음으로 `isEmpty`가 구현되었다.
커밋 날짜는 2009-11-08, 수정된 파일은 `underscore.js`이며 코드는 다음과 같다.

```js
// Is a given array or object empty?
_.isEmpty = function(obj) {
  return (_.isArray(obj) ? obj : _.values(obj)).length == 0;
};
```

lodash의 `_.values()`를 호출해보면 다음과 같이 작동한다.

```
> _.values(null)
[]
```

lodash를 뒤져서 얻은걸 정리하자.

* `_.isEmpty(null)`: true
* `_.isEmpty()`를 만들때는 null을 고민하지 않은 것으로 보인다.
  * 함수 만든 시점과 주석으로 언급된 시점이 몇년 차이난다
* `_.isEmpty()`는 처음부터 배열과 객체를 동시에 고려했다.
* `_.values(null) => []`이다. null은 빈 객체와 비슷하게 취급된다.
* lodash의 부모는 underscore.js

### just-is-empty

just-is-empty의 [명세](https://github.com/angus-c/just/tree/d8c5dd18941062d8db7e9310ecc8f53fd607df54#just-is-empty)

> Return true if object has no enumerable key values

```ts
isEmpty(null) // true
```

[12eb3e2](https://github.com/angus-c/just/commit/12eb3e24825b1601be2c9eba7474d1a33c9addf9)
"Inital commit of object empty" 2016-08-03

just의 isEmpty는 처음부터 null을 고려했다.
isEmpty의 초기 구현인 objectEmpty부터 null을 처리했다.

```ts
 ...
 var obj = null;
 objectEmpty(obj} // true
*/

function objectEmpty(obj1, obj2 /*, [objn]*/) {
  if (obj == null) {
    return true;
  }
  ...
```

just를 뒤져서 얻은걸 정리하자.

* `isEmpty(null)`: true
* `isEmpty()`의 시작부터 null을 고려
* `isEmpty()`는 객체를 기준으로 잡았다. 패키지 이름부터 `object-empty`

### ramda

lodash, just의 isEmpty의 동작이 똑같다.
모든 유명한 라이브러리의 isEmpty의 동작이 똑같다면 글을 안쓸거다.
재밌는건 ramda부터 시작이다.

Ramda v0.29.0 isEmpty [문서](https://ramdajs.com/docs/#isEmpty)

> Returns true if the given value is its type's empty value; false otherwise.

```ts
R.isEmpty(null);  //=> false
```

ramda는 lodash, just와는 동작이 반대다!

ramda의 주석으로 `isEmpty(null)`가 처음 언급되는건 다음의 커밋이다.
[c2f3bad](https://github.com/ramda/ramda/commit/c2f3bad990705fbcb0df6cbf33986ea1c3426403#diff-b6bd0c98a2be150ec2d37193cb14a264d8445e4ce39c04d714051e1ab8ea9c51R728)
`isEmpty`의 구현은 고치지 않고 JSDoc만 개선한 커밋이다.
2014-07-14

```
         /**
-         * XXX
+         * Reports whether an array is empty.
+         *
+         * @static
+         * @memberOf R
+         * @category Array
+         * @param {Array} arr The array to consider.
+         * @return {boolean} Returns true if the `arr` argument has a length of 0 or if `arr` is a
+         * falsy value (e.g. undefined).
+         * @example
+         *
+         * ramda.isEmpty([1, 2, 3]); //=> false
+         * ramda.isEmpty([]); //=> true
+         * ramda.isEmpty(); //=> true
+         * ramda.isEmpty(null); //=> true
          */
-        // Boolean function which reports whether a list is empty.
         var isEmpty = R.isEmpty = function (arr) {
             return !arr || !arr.length;
         };
```

잉???
과거의 ramda는 `ramda.isEmpty(null)`가 `true` 였다.
ramda는 `isEmpty()`의 동작을 중간에 바꿨다!

`isEmpty()`의 동작을 바꾼 커밋은 다음과 같다. 
[6c01c22](https://github.com/ramda/ramda/commit/6c01c22b5ed7fec3dd23a30e0f5fe9b9a8c76e3c)
"simplify semantics of R.isEmpty" 2014-11-20

이전: true
```ts
/**
 * Reports whether a value is "empty".
 * Empty values are null, undefined, "", and every object with a length
 * property whose value is 0 (such as an empty array).
 *
 * @func
 * @memberOf R
 * @category Core
 * @sig * -> Boolean
 * @param {*} val
 * @return {Boolean}
 * @example
 *
 *      R.isEmpty([1, 2, 3]); //=> false
 *      R.isEmpty([]); //=> true
 *      R.isEmpty(''); //=> true
 *      R.isEmpty(null); //=> true
 */
var isEmpty = R.isEmpty = function isEmpty(val) {
    return val == null || val.length === 0;
};
```

이후: false
```ts
/**
 * Reports whether the list has zero elements.
 *
 * @func
 * @memberOf R
 * @category Core
 * @sig [a] -> Boolean
 * @param {Array} list
 * @return {Boolean}
 * @example
 *
 *      R.isEmpty([1, 2, 3]); //=> false
 *      R.isEmpty([]); //=> true
 *      R.isEmpty(''); //=> true
 *      R.isEmpty(null); //=> false
 */
R.isEmpty = function isEmpty(list) {
    return Object(list).length === 0;
};
```

ramda를 뒤져서 얻은걸 정리하자.

* 2014-11-20 이전
  * `R.isEmpty(null)`: true
  * `isEmpty()`는 배열을 생각하고 만들었다.
    * `arr.length`
* 2014-11-20 이후
  * `R.isEmpty(null)`: false
  * `isEmpty()`는 배열을 생각하고 만들었다. 
    * JSDoc: Reports whether the list has zero elements.
* `isEmpty()`의 명세는 버전이 올라가면 바뀔 수 있다.

### remeda

just-is-empty를 remeda의 `R.isEmpty()`로 바꾸고 생긴 문제는 런타임 문제가 아니다.
remeda로는 `R.isEmpty(null)`가 컴파일되지 않는다.

remeda isEmpty [문서](https://remedajs.com/docs#isEmpty)

> A function that checks if the passed parameter is empty

```ts
R.isEmpty('') //=> true
R.isEmpty([]) //=> true
R.isEmpty({}) //=> true
R.isEmpty('test') //=> false
R.isEmpty([1, 2, 3]) //=> false
R.isEmpty({ length: 0 }) //=> false
```

remeda는 문서에도 `R.isEmpty(null)`이 없다.
remeda에 isEmpty가 추가된 PR을 따라가면 배경을 알 수 있다.

[feat: add isEmpty util function #314](https://github.com/remeda/remeda/pull/314#issuecomment-1533208805)
PR의 시작에서는 `R.isEmpty(null) //=> false`가 있었지만 lodash와 ramda에서의 `isEmpty(null)`이 다른게 언급된다.

[feat: add isEmpty util function](https://github.com/remeda/remeda/pull/314/commits/16817bc381d7808ac9562f8c281829d6e338ed21)

PR에서 3줄이 사라진다.
```
-   *    R.isEmpty(null) //=> false
-   *    R.isEmpty(1) //=> false
-   *    R.isEmpty(true) //=> false
```

remeda를 뒤져서 얻은걸 정리하자.

* `R.isEmpty(null)`: 컴파일 에러
* true, false 어느쪽도 고를수 없다면 아무것도 고르지 않는다.
  * 타입시스템을 이용해서 컴파일되지 않도록 막는다.

## 요약

* `isEmpty()`는 단순한 함수지만 라이브러리마다 구현이 다르다.
* `isEmpty()`가 객체를 위한 함수인지, 배열을 위한 함수인지는 사람마다 관점이 다르다.
* `isEmpty([])`, `isEmpty({})`의 리턴으로는 논쟁이 없지만 `isEmpty(null)`은 의견이 갈라진다.
  * true, false를 리턴하는 라이브러리를 찾았다.
  * 컴파일 에러를 던지는 라이브러리도 있더라.
  * 예외를 던지는 구현도 누군가 만들지 않았을까?
* 라이브러리의 명세는 언제든 바뀔수 있다.
  * 리턴값을 뒤집은 ramda.isEmpty()
* 어떤 것도 고를수 없을때는 아무것도 안고르면 된다.
  * remeda의 컴파일 에러를 보고 배우자.
* 컴파일 타임에 검증이 된다는건 좋은 것이다.
  * node.js REPL을 쓰면 remeda의 `R.isEmpty(null)`을 호출할 수 있다.
  * REPL을 믿기 vs 컴파일러를 믿기

[escape-from-lodash-remeda]: {{< ref escape-from-lodash-remeda >}}
[escape-from-lodash-just]: {{< ref escape-from-lodash-just >}}
