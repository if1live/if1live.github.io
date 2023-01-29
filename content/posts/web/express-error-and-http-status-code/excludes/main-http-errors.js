const express = require('express');
const createError = require('http-errors');
const app = express();

app.get('/', (req, res) => {
  throw new createError.BadRequest();
});
app.listen(3000, () => { console.log('listen'); });
