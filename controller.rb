require 'rubygems'
require 'termios'

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

with_unbuffered_input do
  while true do
    c = STDIN.getc
    puts "Got #{c}"

    if c == "w"
      `echo "UP" | ncat 192.168.1.33 8888 -u --send-only`
    elsif c == "s"
      `echo "DOWN" | ncat 192.168.1.33 8888 -u --send-only`
    elsif c == "d"
      `echo "STOP" | ncat 192.168.1.33 8888 -u --send-only`
    end
  end
end
