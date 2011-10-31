MSG_HI = 'HI'
MSG_PREPARE = 'PREPARE'
MSG_INFO = 'INFO'
MSG_OK = 'OK'

class MessageHandler
    def initialize(socket)
        @socket = socket
    end

    def want(expected)
        received = get
        raise ProtocolBreakageError.new(received, expected) if received != expected
    end

    def get
        @socket.gets.chomp
    end

    def send(msg)
        @socket.puts msg
    end

    def skip
        @socket.gets
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
