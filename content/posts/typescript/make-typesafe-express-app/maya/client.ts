import fetch from 'node-fetch';
import { Req, Resp } from './server-typesafe';
import qs from 'qs';

async function request() {
  const req: Req = { a: 1, b: 2 };
  const url = `http://127.0.0.1:5000/sum?${qs.stringify(req)}`;
  const resp = await fetch(url);
  const json: Resp = await resp.json();
  console.log(`output = ${json.result}`);
}

request();
