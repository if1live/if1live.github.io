#!/usr/bin/env ruby

foo_list = [1, 2, 3]
begin
  p foo_list.fetch(100)
rescue => ex
  p ex
end

foo_dict = {foo: 2, bar: 4}
begin
  p foo_dict(:spam)
rescue => ex
  p ex
end
