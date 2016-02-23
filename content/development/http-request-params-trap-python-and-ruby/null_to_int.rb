#!/usr/bin/env ruby

puts "str -> int"
p "123".to_i
p Integer("123")

puts "nil -> int"
begin
  p nil.to_i
rescue => ex
  p ex
end

begin
  p Integer(nil)
rescue => ex
  p ex
end
