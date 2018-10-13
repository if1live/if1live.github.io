import { isReady } from './common';

const main = async () => {
  if (isReady()) {
    console.log('ready');
  } else {
    console.log('not ready');
  }
};
main();
