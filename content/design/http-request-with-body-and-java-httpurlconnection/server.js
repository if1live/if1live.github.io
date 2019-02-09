const express = require('express');

const app = express();
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

app.all('/', (req, res) => {
  res.json({ method: req.method, body: req.body });
});

const port = 3100;
app.listen(port, () => {
  console.log(`server listen - ${port}`);
});
