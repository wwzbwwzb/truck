import xcb
import xcb.xproto

authname = "MIT-MAGIC-COOKIE-1"
authdata = "\xa5\xcf\x95\xfa\x19\x49\x03\x60\xaf\xe4\x1e\xcd\xa3\xe2\xad\x47"

authstr = authname + ':' + authdata

conn = xcb.connect(display=":0",auth=authstr)

print conn.get_setup().roots[0].root
