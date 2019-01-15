const express = require('express');

const app = express();
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

app.get('/', (req, res) => {
  res.json({ method: 'GET', body: req.body });
});
app.post('/', (req, res) => {
  res.json({ method: 'POST', body: req.body });
});

const port = 3100;
app.listen(port, () => {
  console.log(`server listen - ${port}`);
});
