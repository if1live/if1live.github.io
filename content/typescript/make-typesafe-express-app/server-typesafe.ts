import express from 'express';
import * as yup from 'yup';

const app = express();

export interface Req {
  a: number;
  b: number;
}

const schema = yup.object().shape<Req>({
  a: yup.number().required(),
  b: yup.number().required(),
});

type YupArg<T> = T extends yup.ObjectSchema<yup.Shape<any, infer Y>> ? Y : never;
type ReqGenereated = YupArg<typeof schema>;

const handleSum = async (req: express.Request) => {
  const ctx = await schema.validate(req.query);
  const { a, b } = ctx;
  const result = a + b;
  return { result };
}

app.get('/sum', async (req, res) => {
  const resp = await handleSum(req);
  res.json(resp);
});

type ThenArg<T> = T extends Promise<infer U> ? U : T;

export type Resp = ThenArg<ReturnType<typeof handleSum>>;

const port = 5000;
app.listen(port, () => console.log(`running 127.0.0.1:${port}`));
