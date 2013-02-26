local ffi = require("ffi")
t= ffi.load('./dll.so')
ffi.cdef[[
void print();
typedef struct test_struct { int a ; int b; char * ptr; } st;
void printr(struct test_struct *a);
]]
t.print()

st_c = ffi.new("st[?]", 3);
ss="hello;"
st_c[0]={a=3,b=4}
st_c[0].ptr="'3'3";
t.printr(st_c[0]);


ffi.cdef[[
int printf(const char *fmt, ...);
int sleep(int m);
]]
ffi.C.printf("sleep time %d!\n", 2+1)
ffi.C.sleep(2)
ffi.C.printf("Hello %s!\n", "world")

