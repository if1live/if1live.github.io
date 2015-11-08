<?php
function check_prime_number_buggy($val) {
  $data = array(2, 3, 5, 7, 11);
  $found = array_search($val, $data);
  if($found) {
    printf("%d is prime number, index=%d\n", $val, $found);
  } else {
    printf("%d is not prime number\n", $val);
  }
}

// normal case
check_prime_number_buggy(5);
// first element
check_prime_number_buggy(2);
// not exist
check_prime_number_buggy(-1);

?>
