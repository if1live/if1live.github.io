const express = require('express');
const app = express();

app.get('/', (req, res) => {
  res.status(400).json({ text: 'todo' });
});
app.listen(3000, () => { console.log('listen'); });
