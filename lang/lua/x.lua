-- send stdin through unix socket

socket = require"socket"
socket.unix = require"socket.unix"
c = assert(socket.unix())
assert(c.connect("/tmp/.X11-unix/X0"))
while 1 do
  local l = io.read()
  print(l);
  assert(c.send(l .. "\n"))
end


