function check_prime_number_buggy(val) {
  var data = [2, 3, 5, 7, 11];
  var found = data.indexOf(val);
  if(found) {
    console.log(`${val} is prime number, index=${found}`);
  } else {
    console.log(`${val} is not prime number`);
  }
}

check_prime_number_buggy(5);
check_prime_number_buggy(2);
check_prime_number_buggy(-1);
