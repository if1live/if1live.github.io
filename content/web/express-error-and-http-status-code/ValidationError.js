export default function ValidationError(errors, value, field, type) {
  this.name = 'ValidationError';
  this.value = value;
  this.path = field;
  this.type = type;
  this.errors = [];
  this.inner = [];
  // ...
  this.message =
    this.errors.length > 1
      ? `${this.errors.length} errors occurred`
      : this.errors[0];

  if (Error.captureStackTrace) Error.captureStackTrace(this, ValidationError);
}
