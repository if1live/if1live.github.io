import express from 'express';
import fetch from 'node-fetch';

interface Specification<TReq, TResp> {
  method: 'get' | 'post';
  endpoint: string;
}

interface CounterModel {
  id: string;
  value: number;
}

interface Input { id: string; }

const inc: Specification<Input, CounterModel> = {
  method: 'post',
  endpoint: '/inc',
};

const reset: Specification<Input, CounterModel> = {
  method: 'post',
  endpoint: '/reset',
};

const counterSpec = {
  inc,
  reset,
};

type ControllerFunction<T> = T extends Specification<infer Req, infer Resp>
  ? (body: Req) => Resp
  : never;

type Controller<T> = { [P in keyof T]: ControllerFunction<T[P]> };

type CounterStub = Controller<typeof counterSpec>;

class CounterController implements CounterStub {
  private readonly map = new Map<string, number>();

  public inc: CounterStub['inc'] = body => {
    const { id } = body;
    const value = (this.map.get(id) ?? 0) + 1;
    this.map.set(id, value);
    return { id, value };
  }

  public reset: CounterStub['reset'] = body => {
    const { id } = body;
    this.map.set(id, 0);
    return { id, value: 0 };
  }
}

type ClientFunction<T> = T extends Specification<infer Req, infer Resp>
  ? (body: Req) => Promise<Resp>
  : never;

type Client<T> = { [P in keyof T]: ClientFunction<T[P]> };

class BaseClient {
  constructor(protected readonly host: string) { }

  protected handle<Req, Resp>(
    spec: Specification<Req, Resp>,
  ): ClientFunction<Specification<Req, Resp>> {
    return async (req) => {
      const { method, endpoint } = spec;
      const url = `${this.host}${endpoint}`;

      const resp = await fetch(url, {
        method: method,
        body: JSON.stringify(req),
        headers: { 'Content-Type': 'application/json' },
      });
      return await resp.json();
    };
  }
}

class CounterClient extends BaseClient implements Client<typeof counterSpec> {
  public inc = this.handle(counterSpec.inc);
  public reset = this.handle(counterSpec.reset);
}

function registerSpec<Req, Resp>(
  app: express.Application,
  spec: Specification<Req, Resp>,
  handler: ControllerFunction<Specification<Req, Resp>>,
) {
  const { method, endpoint } = spec;
  app[method](endpoint, async (req, res) => {
    const payload = { ...req.query, ...req.body };
    const resp = await handler(payload);
    res.json(resp);
  });
}

function registerController<T>(
  app: express.Application,
  spec: T,
  controller: Controller<T>,
) {
  for (const x of Object.keys(spec)) {
    const key = x as keyof typeof controller;
    const fn = controller[key].bind(controller);
    registerSpec(app, spec[key] as any, fn);
  }
}

const app = express();
app.use(express.json({}));
app.use(express.urlencoded({ extended: true }));

const controller = new CounterController();
registerController(app, counterSpec, controller);

const PORT = 3000;
app.listen(PORT, async () => {
  console.log(`listen 127.0.0.1:${PORT}`);

  const client = new CounterClient(`http://127.0.0.1:${PORT}`);
  const id = 'foo';
  console.log('inc', await client.inc({ id }));
  console.log('inc', await client.inc({ id }));
  console.log('inc', await client.reset({ id }));
  console.log('inc', await client.inc({ id }));
  process.exit();
});
