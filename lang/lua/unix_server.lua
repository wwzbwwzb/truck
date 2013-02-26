-- send stdin through unix socket
--[[
42  static luaL_reg un[] = {
  43      {"__gc",        meth_close},
  44      {"__tostring",  auxiliar_tostring},
  45      {"accept",      meth_accept},
  46      {"bind",        meth_bind},
  47      {"close",       meth_close},
  48      {"connect",     meth_connect},
  49      {"dirty",       meth_dirty},
  50      {"getfd",       meth_getfd},
  51      {"getstats",    meth_getstats},
  52      {"setstats",    meth_setstats},
  53      {"listen",      meth_listen},
  54      {"receive",     meth_receive},
  55      {"send",        meth_send},
  56      {"setfd",       meth_setfd},
  57      {"setoption",   meth_setoption},
  58      {"setpeername", meth_connect},
  59      {"setsockname", meth_bind},
  60      {"settimeout",  meth_settimeout},
  61      {"shutdown",    meth_shutdown},
  62      {NULL,          NULL}
  63  };
64  
65  /* socket option handlers */
66  static t_opt opt[] = {
  67      {"keepalive",   opt_keepalive},
  68      {"reuseaddr",   opt_reuseaddr},
  69      {"linger",      opt_linger},
  70      {NULL,          NULL}
  71  };
]]

ox = require"posix"
ox.unlink("/tmp/foo")
socket = require"socket"
socket.unix = require"socket.unix"
c = assert(socket.unix())
s= assert(c:bind("/tmp/foo"))
d=assert(c:accept(s))
while 1 do
  local l = io.read()
  assert(c:send(l .. "\n"))
end


