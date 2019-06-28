const fs = require('fs');
const path = require('path');

fs.copyFileSync('template.js', path.resolve('test', 'a.test.js'));
fs.copyFileSync('template.js', path.resolve('test', 'b.test.js'));
fs.copyFileSync('template.js', path.resolve('test', 'c.test.js'));
