#!/usr/bin/env ruby
# -*- coding: utf-8 -*-
require 'socket'

class PrimalServer

    def initialize(host='localhost', port=8000)
        port = port.to_i if port.is_a? String
        @host = host
        @port = port
        @server = TCPServer.new(@port)
    end

    def start
        loop do
            Thread.start(@server.accept) do |s|
                begin
                    msg = s.gets
                    print(msg)
                ensure
                    s.close
                end
            end
        end
    end

end

port = ARGV.grep(/^\d+$/).fetch(0, "8000").to_i
server = PrimalServer.new(port=port)
server.start
