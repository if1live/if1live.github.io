const Redis = require('ioredis');

function makeRedis() {
  const redis = new Redis({ lazyConnect: true });

  const default_sendCommand = redis.sendCommand.bind(redis);
  redis.sendCommand = async (...params) => {
    const command = params[0];
    const { name, args } = command;

    const start = Date.now();
    const result = await default_sendCommand(...params);
    const end = Date.now()

    const elapsed = end - start;
    console.log(`redis: ${name} ${args.join(' ')} [${elapsed}ms]`);

    return result;
  };

  return redis;
}

async function main() {
  const redis = makeRedis();
  await redis.connect();

  await redis.mset('foo', 1, 'bar', 2);
  console.log(await redis.get('foo'));
  await redis.del('foo', 'bar');
  console.log(await redis.get('bar'));
}

main().then(process.exit).catch(console.error);

