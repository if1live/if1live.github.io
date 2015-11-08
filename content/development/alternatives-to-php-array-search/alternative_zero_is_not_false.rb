#!/usr/bin/env ruby

def check_prime_number(val)
  data = [2, 3, 5, 7, 11];
  found = data.index(val)
  if found
    puts "#{val} is prime number, index=#{found}"
  else
    puts "#{val} is not prime number"
  end
end

check_prime_number 5
check_prime_number 2
check_prime_number -1
