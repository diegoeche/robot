require 'rubygems'
require 'termios'
require 'timeout'

def with_unbuffered_input
  old_attrs = Termios.tcgetattr(STDOUT)

  new_attrs = old_attrs.dup

  new_attrs.lflag &= ~Termios::ECHO
  new_attrs.lflag &= ~Termios::ICANON

  Termios::tcsetattr(STDOUT, Termios::TCSANOW, new_attrs)

  yield
ensure
  Termios::tcsetattr(STDOUT, Termios::TCSANOW, old_attrs)
end

ip = "192.168.1.41"
with_unbuffered_input do
  while true do
    c = nil
    stopped = false
    begin
      status = Timeout::timeout(0.250) {
        c = STDIN.getc
      }
    rescue Timeout::Error
      puts "Timeout"
    end

    if c == "w"
      `echo "UP" | ncat #{ip} 8888 -u --send-only`
      puts "Up command sent"
      stop = false
    elsif c == "s"
      `echo "DOWN" | ncat #{ip} 8888 -u --send-only`
      puts "Down command sent"
      stop = false
    elsif c == "a"
      `echo "LEFT" | ncat #{ip} 8888 -u --send-only`
      puts "Left command sent"
      stop = false
    elsif c == "d"
      `echo "RIGHT" | ncat #{ip} 8888 -u --send-only`
      puts "Right command sent"
      stop = false
    elsif c == nil
      if !stop
        `echo "STOP" | ncat #{ip} 8888 -u --send-only`
        puts "Stop command sent"
        stop = true
      end
    end
  end
end
