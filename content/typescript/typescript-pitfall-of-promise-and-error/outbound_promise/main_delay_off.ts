export { };

async function main() {
  const p1 = func_throw();
  const p2 = func_simple();

  try {
    await Promise.all([p1, p2]);
  } catch (e) {
    console.log('catch');
  }
}

main().then(console.log).catch(console.error);

async function func_throw(): Promise<number> {
  throw new Error();
}

async function func_simple(): Promise<number> {
  return 1;
}

