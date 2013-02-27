-- [[

meaning of _IOC in ioctl.h 

  dir       type    nr        size
  2bit     8bit    8bit      14bit
  2read     
  1write    [char]  [index]   {sizeof(size)}


--]]

local B_TYPE_LARGE = 0x85;
local Transaction = {

  FIRST_CALL_TRANSACTION  = 0x00000001,
  LAST_CALL_TRANSACTION   = 0x00ffffff,
  PING_TRANSACTION        = B_PACK_CHARS('_','P','N','G'),
  DUMP_TRANSACTION        = B_PACK_CHARS('_','D','M','P'),
  INTERFACE_TRANSACTION   = B_PACK_CHARS('_', 'N', 'T', 'F'),
  SYSPROPS_TRANSACTION    = B_PACK_CHARS('_', 'S', 'P', 'R'),

  BINDER_TYPE_BINDER = B_PACK_CHARS('s', 'b', '*', B_TYPE_LARGE),
  BINDER_TYPE_WEAK_BINDER = B_PACK_CHARS('w', 'b', '*', B_TYPE_LARGE),
  BINDER_TYPE_HANDLE = B_PACK_CHARS('s', 'h', '*', B_TYPE_LARGE),
  BINDER_TYPE_WEAK_HANDLE = B_PACK_CHARS('w', 'h', '*', B_TYPE_LARGE),
  BINDER_TYPE_FD = B_PACK_CHARS('f', 'd', '*', B_TYPE_LARGE),
  BINDER_NONE_CMD = 0
}

print(  Transaction.ping)

  transact(cmd, sendParcel, replyParcel, flags)
  
  ontransact(cmd, sendParcel, replyParcel, flags)

  onBinderDied()

  call (server, func, sendParcel, replyParcel, flags)

  

