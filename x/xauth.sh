
xauth generate :0
xauth extract - :0  #extract to .Xauthority

HOME=xxx
XAUTHORITY=xxx
xauth extract - :0 >> $HOME/.Xauthority


#command to start Xorg  with auth
Xorg :2 -auth /run/root/:0 

#preparetion for starting a session is as above.

# Redefine Xorg behavier to demonstrate a new X window system
1. Xorg will never die event session exit
2. Xorg start multi DISPLAY at any time
3. Xorg do not depend on VT any more, use Fb instead
4. init should sent "on X" event on received SIGNAL from X, after Xorg can poll event by its socket

