MSG_HI = 'HI'
MSG_PREPARE = 'PREPARE'
MSG_INFO = 'INFO'
MSG_OK = 'OK'
MSG_COMPLETED = 'COMPLETED'

class MessageHandler
    def initialize(socket)
        @socket = socket
    end

    def want(expected)
        received = get
        raise ProtocolBreakageError.new(received, expected) if received != expected
    end

    def get
        m = @socket.gets.chomp
        print "GET> #{m}\n"
        m
    end

    def get_with_size
        get
        send MSG_OK
        m = get
        send MSG_OK
        m
    end

    def send_with_size(msg)
        send msg.to_s.length
        want MSG_OK
        send msg
        want MSG_OK
    end

    def send(msg)
        @socket.puts msg
        print "SEND> #{msg}\n"
    end
end

class ProtocolBreakageError < StandardError
    def initialize(received, expected)
        @received = received
        @expected = expected
    end

    def to_s(client=nil)
        msg = "Protocol breakage: expected #{@expected}, but received #{@received}\n"
        msg = "[Client #{client}] #{msg}" unless client.nil?
        return msg
    end
end
