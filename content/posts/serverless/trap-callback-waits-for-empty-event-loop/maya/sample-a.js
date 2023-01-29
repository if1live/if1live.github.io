const delay = (millis) => new Promise(resolve => setTimeout(resolve, millis));

exports.handler = async (event, context) => {
  try {
    return { statusCode: 200, body: 'a' };
  } catch (e) {
    return { statusCode: 200, body: 'b' };

  } finally {
    console.log('finally 1');
    await delay(1);
    console.log('finally 2');
  }
};
