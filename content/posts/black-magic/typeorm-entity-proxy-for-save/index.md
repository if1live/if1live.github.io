---
title: typeorm에서 repository.save() 호출할때 UPDATE 쿼리만 사용하기
subtitle: custom repository + proxy
tags: [typeorm]
slug: typeorm-entity-proxy-for-save
author: if1live
date: 2020-07-10
url: /posts/typeorm-entity-proxy-for-save
---

## repository.save() 의 함정

typeorm으로 아래와 같은 엔티티를 정의했다고 치자.

```ts
@Entity()
export class UserEntity {
  @PrimaryColumn()
  key1: string;

  @PrimaryColumn()
  key2: string;

  @Column()
  data: string;
}
```

엔티티를 살짝 고쳐서 저장하자.`repository.save()`를 이용하면 쉽게 저장할 수 있다.
(`BEGIN TRANSACTION`, `COMMIT`이 쿼리에 들어가는걸 없애려고 transaction을 껏다)

```ts
const repo = conn.manager.getRepository(UserEntity);
user.data = 'save';
await repo.save(user, { transaction: false });
```

데이터 저장하는건 1줄인데 실행되는 SQL은 1개가 아니다.

```
query: SELECT "UserEntity"."key1" AS "UserEntity_key1", "UserEntity"."key2" AS "UserEntity_key2", "UserEntity"."data" AS "UserEntity_data" FROM "user_entity" "UserEntity" WHERE "UserEntity"."key1" = ? AND "UserEntity"."key2" = ? -- PARAMETERS: ["foo","bar"]
query: UPDATE "user_entity" SET "data" = ? WHERE "key1" = ? AND "key2" = ? -- PARAMETERS: ["save","foo","bar"]
```

엔티티 1개의 필드 1개를 고친거니까 UPDATE 쿼리 1개로 충분해보이지만 SELECT가 같이 실행된다.
[알려진 이슈](https://github.com/typeorm/typeorm/issues/3772)인데 고칠 생각은 없는거같다.
insert or update 같은 고수준 기능 밑에서 저수준 SQL 여러개 실행되는건 추상화의 숙명이니까 어쩔 수 없나보다.

```ts
const repo = conn.manager.getRepository(UserEntity);
await repo.update({
  key1: user.key1,
  key2: user.key2,
}, {
  data: 'update',
});
```

```
UPDATE "user_entity" SET "data" = ? WHERE "key1" = ? AND "key2" = ? -- PARAMETERS: ["update","foo","bar"]
```

UPDATE만 실행하고 싶으면 `repository.update()`를 쓰면 된다.
SELECT 쿼리가 실행되지 않지만 `user.data = 'save'`를 쓸수 없는게 아쉽다.

삽질을 잘 하면 `repository.save()`와 비슷하면서 UPDATE 쿼리만 호출되도록 만들수 있지 않을까?
겉보기에는 `save()` 쓰는 것처럼 보이지만 내부에서는 `update()` 호출하게 만들어보자.
`save()`와 동일한 수준의 기능은 필요없다. 엔티티 1개에 대해서만 대충 작동하게 만들자.

```ts
@EntityRepository(UserEntity)
class UserRepository extends MyRepository<UserEntity> { }
```

```ts
const repo = conn.manager.getCustomRepository(UserRepository)
const user = EntityProxy.create(ent);
user.data = 'custom';
await repo.customSave(user);
```

## 의식의 흐름

### MyRepository.customSave()

```ts
export class MyRepository<T extends ObjectLiteral> extends Repository<T> {
  public async customSave(proxy: EntityProxy<T>) {
    const { ent, diff } = proxy;
    if (!diff) { return; }

    const id = this.getId(ent);
    await this.update(id, diff);
  }
}
```

custom repository는 대충 이런 느낌으로 만들면 된다.
update에 필요한 인자인 조건과 데이터를 빼낼수 있으면 된다.
데이터 수정이 없을때는 쿼리 실행을 건너뛰자.

엔티티 1개만 처리할거니까 조건은 PK로 충분할 것이다. 
`repository.getId()`를 쓰면 PK를 얻을수 있다.
https://typeorm.io/#/repository-api

### diff

엔티티와 변경된 필드 목록이 있다면 diff에 해당되는 객체를 만들 수 있을 것이다

```ts
function createPartialEntity<T>(
  ent: T,
  iter: IterableIterator<keyof T>,
): Partial<T> {
  const data: Partial<T> = {};
  for (const f of iter) {
    data[f] = ent[f];
  };
  return data;
}
```

### proxy

변경된 필드 목록은 어떻게 뽑을수 있는가? 
엔티티를 통째로 감싸면 된다.

```ts
class Data { a: number; }

class Proxy {
  constructor(private inner: Data) {}
  public get a() { return this.inner.a; }
  public set a(x: number) {
    console.log('modify');
    this.inner.a = x;
  }
}
```

손으로 엔티티마다 속성을 하나씩 붙이는건 미친짓이다.
`Object.defineProperty()`를 사용하면 노가다를 없앨 수 있다.

```ts
export class EntityProxy<T> {
  private readonly fields = new Set<keyof T>();

  constructor(public readonly ent: T) {
    const columns = extractColumns(ent);
    for (const p of columns) {
      this.defineProperty(p);
    }
  }

  public defineProperty(p: string) {
    Object.defineProperty(this, p, {
      get() { return this.ent[p]; },
      set(newValue) {
        this.fields.add(p);
        this.ent[p] = newValue;
      },
    })
  }
}
```

속성을 자동 생성하려면 컬럼으로 사용된 속성 목록이 필요하다.

### columns

엔티티에는 `@Column` 이외의 속성이 들어있을지 모른다.
`@Column`, `@PrimaryColumn`같은 데코레이터가 붙은 속성 목록이 필요하다.
reflect-metadata를 사용하면 얻을 수 있다.

```ts
function extractColumns<T>(ent: T): string[] {
  return Object.keys(ent).filter(propertyName => {
    const keys = Reflect.getMetadataKeys(ent, propertyName);
    return keys.includes('design:type');
  });
}
```

typeorm 코드를 뒤져보면 `design:type`로 하드코딩된 이유를 알 수 있다.
https://github.com/typeorm/typeorm/blob/0.2.25/src/decorator/columns/Column.ts#L114

### diff

위에서 만든 createPartialEntity와 프록시를 연결한다

```ts
public get diff(): Partial<T> | undefined {
  if (!this.fields.size) { return; }
  const iter = this.fields[Symbol.iterator]();
  const data = createPartialEntity(this.ent, iter);
  this.fields.clear();
  return data;
}
```

### create

만들어진 객체의 타입은 `EntityProxy<T>`이다.
하지만 외부에서 볼때 `T` 타입이어야 `ent.data = 'xxx'`가 가능하다.
`T`에 붙은 속성을 정의했으니까 `T`로 캐스팅해도 대충 돌아가겠지?

```ts
public static create<T>(ent: T): EntityProxy<T> & T {
  const proxy = new EntityProxy<T>(ent);
  return proxy as any;
}
```

## summary

프로덕션 수준으로 올리면서 더 개선해야되는데 귀찮은 관계로 여기에서 멈춘다.
예외처리를 대충해놔서 그대로 쓰면 엔티티 조금 더 복잡해졌을때 터질 것이다.
필요한 사람은 알아서 잘 고쳐서 쓰자.

전체 코드는 [typeorm-entity-proxy-for-save](https://github.com/if1live/typeorm-entity-proxy-for-save)에서 볼수 있다.
