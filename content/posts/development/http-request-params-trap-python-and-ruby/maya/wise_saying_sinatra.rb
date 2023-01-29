#!/usr/bin/env ruby

require "sinatra"

messages = File.read("db.txt").split("\n")
msg_count = messages.length

get '/' do
  begin
    idx = params["idx"].to_i % msg_count
  rescue
    idx = (Random.rand * msg_count).to_i
  end
  messages[idx]
end
