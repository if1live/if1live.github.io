<?php
function check_prime_number($val) {
  $data = array(2, 3, 5, 7, 11);
  if(in_array($val, $data) === false) {
    printf("%d is not prime number\n", $val);
  } else {
    $found = array_search($val, $data);
    printf("%d is prime number, index=%d\n", $val, $found);
  }
}

check_prime_number(5);
check_prime_number(2);
check_prime_number(-1);
