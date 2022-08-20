import {
  APIGatewayProxyHandlerV2,
} from 'aws-lambda';
import * as R from 'remeda';

export const hello: APIGatewayProxyHandlerV2 = async (event) => {
  const users = [
    { name: 'john', age: 20, gender: 'm' },
    { name: 'marry', age: 22, gender: 'f' },
    { name: 'samara', age: 24, gender: 'f' },
    { name: 'paula', age: 24, gender: 'f' },
    { name: 'bill', age: 33, gender: 'm' },
  ]

  // Remeda
  const result = R.pipe(
    users,
    R.filter(x => x.gender === 'f'),
    R.groupBy(x => x.age),
  );

  return {
    statusCode: 200,
    body: JSON.stringify({
      message: 'Go Serverless v1.0! Your function executed successfully!',
      input: event,
      remeda_example: result,
    }, null, 2),
  };
};
