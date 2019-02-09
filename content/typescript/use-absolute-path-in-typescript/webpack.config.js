const path = require('path');
const buildPath = path.resolve(__dirname, './build');

module.exports = {
  entry: path.resolve(__dirname, 'src', 'index.ts'),
  output: {
    path: buildPath,
    filename: 'main.js',
  },
  module: {
    rules: [
      {
        test: /\.tsx?$/,
        use: [
          'ts-loader',
        ],
      },
    ],
  },
  target: 'node',
  node: {
    __dirname: true,
  },
  mode: 'development',
  resolve: {
    extensions: [
      '.ts',
      '.tsx',
      '.js',
      '.json',
    ],
    // -----
    'alias': {
      '@src': path.resolve(__dirname, 'src'),
    },
    // ----
  },
};
