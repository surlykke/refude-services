#!/usr/bin/env ruby

# ./dynamic_resource(_ZN14org_restfulipc7Service6WorkerclEv+0x412)[0x4185a8]

while line = gets
	if /^(?<executable>.*)\(.*\)\[(?<addr>.*)\]$/ =~ line
		puts `addr2line -e #{executable} #{addr}`
	else
		puts line
	end
end
