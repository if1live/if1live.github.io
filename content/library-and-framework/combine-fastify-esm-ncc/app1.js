import { default as Fastify } from 'fastify';

const plugin = async (fastify) => {
  fastify.get('/bar', async (request, reply) => {
    return { hello: 'world' }
  });
};

const fastify = Fastify();

fastify.register(plugin, { prefix: '/foo' });

// Run the server!
const start = async () => {
  try {
    await fastify.listen({ port: 3000 })
    console.log('running server...');
  } catch (err) {
    console.error(err);
    fastify.log.error(err)
    process.exit(1)
  }
}
start()
