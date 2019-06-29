const a1 = Date.now();
var express = require('express');
const a2 = Date.now();
var express = require('express');
const a3 = Date.now();

const pid = process.pid;
console.log(`first load: ${(a2 - a1) / 1000} pid=${pid}`);
console.log(`second load: ${(a3 - a2) / 1000} pid=${pid}`);

const expect = require('expect');

// for mocha
if (!global.test) { global.test = it; }
test(__filename, () => expect(1).toBe(1));

