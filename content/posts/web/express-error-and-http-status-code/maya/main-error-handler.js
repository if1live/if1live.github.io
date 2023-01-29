const express = require('express');
const createErrors = require('http-errors');
const jwt = require('jsonwebtoken');
const app = express();

app.get('/', (req, res) => {
  throw new jwt.JsonWebTokenError('this is sample error');
});

const newErrorMap = new Map([
  ['JsonWebTokenError', createErrors.BadRequest],
  ['ValidationError', createErrors.BadRequest],
]);

app.use((err, req, res, next) => {
  const newError = newErrorMap.get(err.name);
  if(newError) {
    next(new newError(err.message));
  } else {
    next(err);
  }
});

app.listen(3000, () => { console.log('listen'); });
