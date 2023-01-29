---
title: TypeORM과 Knex 같이 쓰기
subtitle: 하나의 Mysql 연결을 TypeORM과 Knex에서 공유하기
tags: [typeorm, knex, mysql]
slug: typeorm-knex-integration
author: if1live
date: 2022-09-25
url: /posts/typeorm-knex-integration
---

## 요구사항

데이터베이스가 없는 백엔드는 상상하기 어렵다.
프로젝트에서 데이터베이스, 그중에서도 RDBMS를 쓰게 된다면 query builder, ORM 같은 물건을 피할 수 없다. 이 글에서는 TypeORM과 Knex를 섞어쓴 배경과 이를 구현한 매직을 다룬다.

### TypeORM + Knex?

ORM을 사용하면 엔티티의 형상을 관리하고 간단한 쿼리는 쉽게 처리할 수 있다.
TypeORM을 오래 사용하고 익숙해져서 ORM 중에서 선택했다.

```ts
@Entity({ name: 'user' })
export class User {
    @PrimaryGeneratedColumn()
    id: number

    @Column()
    name: string
}

const firstUser = await userRepository.findOneBy({ id: 1 });
```

하지만 복잡한 쿼리를 사용해야될 경우에는 ORM이 문제를 더 복잡하게 만든다.
TypeORM의 경우에는 문자열로 쿼리를 작성해야되서 타입 정보도 잃어버린다.

```ts
const { sum } = await dataSource
    .getRepository(User)
    .createQueryBuilder("user")
    .select("SUM(user.photosCount)", "sum")
    .where("user.id = :id", { id: 1 })
    .getRawOne()
```

Knex은 SQL query builder이다.
함수를 chaining해서 호출하면 쿼리가 만들어지고 DB로 요청을 보낼 수 도 있다.
목적이 SQL query builder니까 복잡한 쿼리를 구성해도 TypeORM보다 멀쩡하게 나온다.

```ts
knex('users').sum('products')
```

Knex는 SQL query builder답게 엔티티 관리 기능이 없다.
테이블의 최종 형상도 손으로 관리해야되고 마이그레이션 스크립트도 손으로 관리해야된다.
Knex에는 `knex migrate:make migration_name` 같은 명령어가 있는데 빈 마이그레이션 스크립트를 만들뿐이다.
TypeORM의 migration은 DB의 테이블 구조와 코드의 엔티티 구조를 비교해서 바뀐 필드를 찾고 마이그레이션 스크립트를 만들어주는데!

TypeORM와 Knex를 섞어서쓰면 둘의 장점을 동시에 챙길수 있지 않을까?

### 커넥션을 공유할것

무식한 방법은 프로젝트에 TypeORM과 Knex를 동시에 넣는 것이다.

```ts
// typeorm
const dataSource = new DataSource({ ... });
// knex
const knex = require('knex')({ ... });

const x = await dataSource.find(...)
const y = await knex.select(...)
```

겉보기에는 멀쩡하지만 트랜잭션에서 문제을 일으킬 수 있다.
TypeORM 초기화할때 connection pool을 설정한다.
Knex 초기화할때도 connection pool을 설정한다.
두 connection pool 독립적이다.
커넥션을 서로 공유하지 않으니까 하나의 트랜잭션에서 TypeORM, Knex를 동시에 사용하기 어렵다.

커넥션풀을 외부에서 직접 꽂을수 있다면 문제가 간단해질것이다.
하지만 TypeORM, Knex 문서에서 connection pool을 상세 구현을 직접 제어하는 방법은 찾지 못했다.
TypeORM, Knex 초기화 시점에 DB 접속 정보를 나머지는 라이브러리가 알아서 처리한다.

```ts
// knex
const knex = require('knex')({
  client: 'mysql',
  connection: {
    host : '127.0.0.1',
    port : 3306,
    user : 'your_database_user',
    password : 'your_database_password',
    database : 'myapp_test'
  },
  pool: { min: 0, max: 7 }
});

// typeorm
const AppDataSource = new DataSource({
    type: "mysql",
    host: "localhost",
    port: 3306,
    username: "test",
    password: "test",
    database: "test",
});
```

### 타입 정보가 유지되어야한다

Knex는 SQL query builder라서 SQL만 뽑아내는게 가능하다.
Knex로는 SQL만 생성하고 쿼리 보내는걸 TypeORM에서 처리하면 커넥션풀과 관련된 문제가 사라진다.

https://knexjs.org/guide/interfaces.html#tosql

```ts
knex.select('*')
  .from('users')
  .where(knex.raw('id = ?', [1]))
  .toSQL()
// Outputs:
// {
//   bindings: [1],
//   method: 'select',
//   sql: 'select * from "users" where id = ?',
//   options: undefined,
//   toNative: function () {}
// }

knex.select('*')
  .from('users')
  .where(knex.raw('id = ?', [1]))
  .toSQL()
  .toNative()
// Outputs for postgresql dialect:
// {
//   bindings: [1],
//   sql: 'select * from "users" where id = $1',
// }
```

이렇게 할 경우 타입 정보를 잃어버리고 손으로 붙여줘야한다.
```ts
const context = knex.select('*').from('users').where(knex.raw('id = ?', [1])).toSQL().toNative();
const rawData = await manager.query(context.sql, context.bindings); // TypeORM은 knex의 리턴타입을 모른다
const users = rawData as User[];	// casting
```

Knex는 타입스크립트를 지원한다. Knex에서 DB로 직접 요청했다면 타입 정보가 유지된다.

https://knexjs.org/guide/query-builder.html#usage-with-typescript

```ts
interface User {
  id: number;
  name: string;
  age: number;
}

knex<User>('users') // User is the type of row in database
  .where('id', 1) // Your IDE will be able to help with the completion of id
  .first(); // Resolves to User | undefined
```

Knex에서 직접 DB로 쿼리 날리는걸 버리고 싶진 않다.

## 구현 방향

1. TypeORM 기반으로 프로젝트를 구성한다. Mysql 커넥션풀 관리는 TypeORM에서 수행한다.
2. TypeORM의 DataSource나 EntityManager에서 mysql connection에 접근하는 방법을 찾는다.
3. mysql connection을 그대로 꽂을수 있는 Knex custom dialect를 작성한다.
4. `SELECT CONNECTION_ID()` 쿼리를 TypeORM, Knex에서 호출해서 같은 값인지 확인한다.

[if1live/typeorm-knex-integration](https://github.com/if1live/typeorm-knex-integration)
typeorm과 knex에서 mysql connection을 공유하는걸 구현해봤다.

knex에 mysql dialect, sqlite dialect, ... 같은걸 끼워넣을수 있다면 내가 만든 mysql typeorm dialect를 못끼워넣을 이유가 어디에 있는가?
[I would like to add support for new dialect to knex, is it possible?][knex-contributing]

저장소의 코드에서 컨셉만 남겨봤다.

```ts
const MysqlDialect = (await import(`knex/lib/dialects/mysql/index.js`)).default;

// typeorm-knex mapping table
MysqlDialect.prototype.table = {};

MysqlDialect.prototype.acquireRawConnection = async function () {
  const connectionId = this.connectionSettings.database;
  const connection: DataSource | EntityManager = this.table[connectionId];
  const queryRunner = connection.createQueryRunner();
  const inner = await queryRunner.connect();
  return inner;
};

MysqlDialect.prototype.destroyRawConnection = async function (inner) {...}
MysqlDialect.prototype.validateConnection = async () => {...}

// TypeORM 커넥션을 인자로 받아서 knex 객체를 만든다
export function knexInitialize(conn: DataSource | EntityManager): Knex {
  const connectionId = nanoid();
  MysqlDialect.prototype.table[connectionId] = conn;

  const knex = (knexInit as any)({
    client: MysqlDialect,
  });
  return knex;
}

// 사용 예제. typeorm에서 트랜잭션 열어서 typeorm, knex로 쿼리 날리기
await AppDataSource.manager.transaction(async (manager) => {
  const knex = knexInitialize(manager);

  await execute_typeorm(manager, title);
  await execute_knex(knex, title);

  await knex.destroy();
});
```

TypeORM에서 커넥션 접근하는건 [QueryRunner](https://github.com/typeorm/typeorm/blob/0.3.10/src/query-runner/QueryRunner.ts#L73)에서 찾았다.
```
/**
 * Creates/uses database connection from the connection pool to perform further operations.
 * Returns obtained database connection.
 */
connect(): Promise<any>
```

## production-ready?

TypeORM과 Knex를 섞어서 쓰는게 가능하다는걸 검증했을뿐이다. 실제 프로덕션에 넣으면 사고가 터질것이다.
프로덕션에서 사용하려면 예외 처리, 자원 관리와 같은 문제를 해결해야되는데 나는 실력이 없어서 거기까지 하진 않았다.

아직까지는 하나의 트랜잭션 안에서 TypeORM과 Knex를 동시에 써야하는 일이 생기지 않았다.
앞으로도 없으면 좋겠다.

[knex-contributing]: https://github.com/knex/knex/blob/v0.16.2/CONTRIBUTING.md
