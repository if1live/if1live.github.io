var JsonWebTokenError = function (message, error) {
  // ...
  this.name = 'JsonWebTokenError';
  this.message = message;
  if (error) this.inner = error;
};
