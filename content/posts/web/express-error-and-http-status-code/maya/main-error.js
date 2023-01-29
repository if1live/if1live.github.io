const express = require('express');
const app = express();

app.get('/', (req, res) => {
  throw new Error('BadRequest');
});
app.listen(3000, () => { console.log('listen'); });
