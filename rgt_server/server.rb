#!/usr/bin/env ruby
# -*- coding: utf-8 -*-
require 'socket'
require './protocol'
require 'json'

class PrimalServer

    def initialize(host='localhost', port=8000, results_file='results.txt')
        port = port.to_i if port.is_a? String
        @host = host
        @port = port
        @clients = {}
        @left_bound = 1
        @step = 100000
        @server = TCPServer.new(@port)
        @results_file = File.new(results_file, "w")

        @get_interval_m = Mutex.new
        @results_file_m = Mutex.new
    end

    def get_interval
        left_bound = 0
        @get_interval_m.synchronize do
            left_bound = @left_bound
            @left_bound += @step
        end
        return left_bound, (left_bound + @step)
    end

    def write_results(results)
        @results_file_m.synchronize do
            for result in results
                @results_file.puts "#{result}\n"
            end
        end
    end

    def start
        loop do
            Thread.start(@server.accept) do |s|
                begin
                    ai = s.remote_address
                    ai_s = "#{ai.ip_address}:#{ai.ip_port}"
                    @clients[ai_s] = {}
                    mh = MessageHandler.new(s)
                    print "Connected #{ai_s}\n"

                    mh.want MSG_HI
                    mh.send MSG_HI
                    mh.send MSG_INFO
                    @clients[ai_s]["info"] = JSON mh.get
                    loop do
                        mh.send MSG_PREPARE
                        mh.want MSG_OK
                        l_bound, r_bound = get_interval
                        @clients[ai_s]["interval"] = (l_bound .. r_bound)
                        mh.send JSON [l_bound, r_bound]
                        primes = JSON mh.get
                        write_results(primes)
                        print "Host #{ai_s} returned #{primes.length} prime numbers\n"
                    end
                rescue ProtocolBreakageError => e
                    print e.to_s(ai_s)
                ensure
                    s.close
                    @clients.delete(ai_s)
                    print "Disconnect #{ai_s}\n"
                end
            end
        end
    end

end

Thread.abort_on_exception = true
port = ARGV.grep(/^\d+$/).fetch(0, "8000").to_i
server = PrimalServer.new(port=port)
server.start
