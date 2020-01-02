function f(a: string | null) {
  if (a === null) { return; }

  const x = () => console.log(a);
  function y() { console.log(a); }
}
