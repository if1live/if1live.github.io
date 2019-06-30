import express from 'express';

const app = express();

app.get('/sum', (req, res) => {
  const a = parseInt(req.query.a, 10);
  const b = parseInt(req.query.b, 10);
  if (isNaN(a)) { throw new Error('not a number'); }
  if (isNaN(b)) { throw new Error('not a number'); }

  const result = a + b;
  res.json({ result });
});

const port = 5000;
app.listen(port, () => console.log(`running 127.0.0.1:${port}`));
