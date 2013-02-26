
banner=[[
   ! This file will test basic lua script language
]]
print(banner)
function eraseTerminal ()
  io.write("\27[2J")
end


-- writes an `*' at column `x' , row `y'
function mark (x,y)
  io.write(string.format("\27[%d;%dH*", y, x))
end

func1 = function () 
  print(" in this function")
  
end

-- Terminal size
TermSize = {w = 80, h = 24}


-- plot a function
-- (assume that domain and image are in the range [-1,1])
function plot (f)
  eraseTerminal()
  for i=1,TermSize.w do
     local x = (i/TermSize.w)*2 - 1
     local y = (f(x) + 1)/2 * TermSize.h
     mark(i, y)
  end
  io.read()  -- wait before spoiling the screen
end


-- example
--plot(function (x) return math.sin(x*2*math.pi) end)

print ( 1 or 2)
print ( 1 and  2)
print ( { 1 , 2, 3, 4, 5 })
func1()


