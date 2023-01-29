#!/usr/bin/env ruby

def foo
  puts "foo called"
end

def bar(a = foo())
  puts "bar called"
end
