-- send stdin through unix socket

socket = require"socket"
socket.unix = require"socket.unix"
c = assert(socket.unix())
assert(c.connect("/tmp/foo"))
while 1 do
  local l = io.read()
  assert(c.send(l .. "\n"))
end


