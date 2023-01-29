export { };

export async function func_throw(): Promise<number> {
  throw new Error();
}

async function main() {
  try {
    return await func_throw()
  } catch (e) {
    console.log('catch');
  }
}

main().then(console.log).catch(console.error);
