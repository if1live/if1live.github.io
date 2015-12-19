#!/usr/bin/env ruby

class Sample
  def public_method
    puts "this is public"
  end

  def call_private
    private_method
  end

  private
  def private_method
    puts "this is private"
  end
end

s = Sample.new
s.public_method
s.call_private
begin
  s.private_method
rescue NoMethodError => e
  puts "Error: #{e}"
end
