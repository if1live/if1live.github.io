import { isReady } from './common';

const main = async () => {
  const ready = await isReady();
  if (ready) {
    console.log('ready');
  } else {
    console.log('not ready');
  }
};
main();
