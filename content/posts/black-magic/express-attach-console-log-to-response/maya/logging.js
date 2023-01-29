/**
 * test
 * curl "http://127.0.0.1:3000/?id=1" & curl "http://127.0.0.1:3000/?id=2"
 */

const express = require('express');
const delay = require('delay');
const { getNamespace, createNamespace } = require('cls-hooked');

const NSID = 'writer';
const KEY_LOGS = 'logs';

function set(key, val) {
  const ns = getNamespace(NSID);
  if (ns && ns.active) { ns.set(key, val); }
}

function get(key) {
  const ns = getNamespace(NSID);
  return (ns && ns.active) ? ns.get(key) : undefined;
}

function monkeypatch_console() {
  const propertyNames = ['info', 'warn', 'error']
  for (const property of propertyNames) {
    const default_fn = console[property].bind(console);
    console[property] = (message) => {
      const logs = get(KEY_LOGS) || [];
      logs.push(`[${property.toUpperCase()}] ${message}`);
      set(KEY_LOGS, logs);

      default_fn(message);
    };
  }
}
monkeypatch_console();

const app = express();

app.use((req, res, next) => {
  const default_json = res.json.bind(res);
  res.json = (data) => default_json({ ...data, _logs: get(KEY_LOGS) });

  const ns = getNamespace(NSID) || createNamespace(NSID);
  ns.bindEmitter(req);
  ns.bindEmitter(res);

  ns.run(() => next());
});

app.get('/', async (req, res) => {
  const data = await execute(req.query || {});
  res.json(data);
});

async function execute(input) {
  const id = input.id;
  console.info(`before delay: ${id}`);
  await delay(100);
  console.warn(`after delay: ${id}`);
  return { id };
}

const port = 3000;
app.listen(port, () => console.log(`listen: 127.0.0.1:${port}`));
