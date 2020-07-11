const express = require('express');
const delay = require('delay');

const app = express();

app.get('/', async (req, res) => {
  const data = await execute(req.query || {});
  res.json(data);
});

async function execute(input) {
  const id = input.id;
  console.info(`before delay: ${id}`);
  await delay(100);
  console.warn(`after delay: ${id}`);
  return { id };
}

const port = 3000;
app.listen(port, () => console.log(`listen: 127.0.0.1:${port}`));
