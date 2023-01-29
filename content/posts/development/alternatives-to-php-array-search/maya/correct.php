<?php
function check_prime_number_correct($val) {
  $data = array(2, 3, 5, 7, 11);
  $found = array_search($val, $data);

  // do not use implicit comparison
  if($found !== false) {
    printf("%d is prime number, index=%d\n", $val, $found);
  } else {
    printf("%d is not prime number\n", $val);
  }
}

// normal case
check_prime_number_correct(5);
// first element
check_prime_number_correct(2);
// not exist
check_prime_number_correct(-1);

?>