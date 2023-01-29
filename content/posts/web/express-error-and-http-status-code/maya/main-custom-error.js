const express = require('express');
const app = express();

app.get('/', (req, res) => {
  const e = new Error('sample');
  e.status = 400;
  throw e;
});
app.listen(3000, () => { console.log('listen'); });
