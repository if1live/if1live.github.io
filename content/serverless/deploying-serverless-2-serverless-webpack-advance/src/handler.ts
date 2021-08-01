import {
  APIGatewayProxyHandlerV2,
} from 'aws-lambda';

import 'aws-sdk';
import 'dynamo-types';
import 'lodash';
import 'typeorm';
import 'mysql';

export const hello: APIGatewayProxyHandlerV2 = async (event) => {
  return {
    statusCode: 200,
    body: JSON.stringify({
      message: 'Go Serverless v1.0! Your function executed successfully!',
      input: event,
    }, null, 2),
  };
};
