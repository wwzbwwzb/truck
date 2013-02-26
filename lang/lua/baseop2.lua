
socket = require"socket"
socket.unix = require"socket.unix"
c = socket.unix()
c:connect("/tmp/foo")
while 1 do
  local l = io.read()
  c:send(l .. "\n")
end


