#!/usr/bin/env ruby

puts "#!/usr/bin/env ruby"
lines = File.read("command.txt").split("\n")
lines.each do |line|
  var_names = line.split("=")[0].split(",").map { |x| x.gsub("*", "").strip }

  puts "puts \"source: #{line}\""
  puts line
  var_names.each do |var|
    puts "puts " + "\"#{var} = \" + " + "(#{var}.nil? ? \"nil\" : #{var}.to_s)"
  end

  puts "puts ''"
end
