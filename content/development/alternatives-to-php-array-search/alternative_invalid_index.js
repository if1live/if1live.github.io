function check_prime_number(val) {
  var data = [2, 3, 5, 7, 11];
  var found = data.indexOf(val);
  if(found >= 0) {
    console.log(`${val} is prime number, index=${found}`);
  } else {
    // not found = -1
    console.log(`${val} is not prime number`);
  }
}

check_prime_number(5);
check_prime_number(2);
check_prime_number(-1);
