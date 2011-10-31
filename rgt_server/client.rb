#!/usr/bin/env ruby
# -*- coding: utf-8 -*-
require 'socket'
require './primes'
require './protocol'
require 'progressbar'
require 'json'

if ARGV.empty?
    print "Usage: client.rb host port\n"
    print "or     client.rb host:port\n"
    exit 0
end

host, port = ARGV
host, port = host.split ":" if port.nil?
port = port.to_i

begin
    socket = TCPSocket.new(host, port)
    mh = MessageHandler.new(socket)
    mh.send MSG_HI
    mh.want MSG_HI
    mh.want MSG_INFO
    loop do
        mh.want MSG_PREPARE
        mh.send MSG_OK
        l_bound, r_bound = JSON mh.get
        nums = (l_bound..r_bound)
        print "Processing block [#{l_bound}..#{r_bound}]...\n"
        pbar = ProgressBar.new("Proccessing", r_bound - l_bound + 1)
        numsp = nums.find_all do |n|
            is_prime = test_primality(n)
            pbar.inc
            is_prime
        end
        pbar.finish
        print "#{numsp.length} prime numbers found\n"
        mh.send (JSON numsp)
    end
ensure
    socket.close
end
