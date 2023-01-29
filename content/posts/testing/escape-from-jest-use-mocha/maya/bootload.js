var expect = require('expect');
global.expect = expect;

global.test = (name, fn) => global.it(name, fn);
global.test.skip = (name, fn) => global.it.skip(name, fn);

global.beforeAll = (name, fn) => global.before(name, fn);
global.afterAll = (name, fn) => global.after(name, fn);
