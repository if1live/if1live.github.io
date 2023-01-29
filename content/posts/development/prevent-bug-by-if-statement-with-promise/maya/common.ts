export const isReady = async () => {
  const timeout = new Promise((resolve) => {
    setTimeout(resolve, 100)
  });
  await timeout;
  return false;
};
