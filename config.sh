#0. editor ,
#0.1. version control tools
apt-get install  gvim-gtk
at-get install  ggit git-svn 

#0.2 enable git diff show color
git config --global color.diff auto
or
git config --global color.diff always
#0.3 let ssh logon more quick
#use ssh -v to debug
add a line in /etc/hosts,   <clientIP> <hostname>

#1

#2. 

#3. install full apache to ubuntu 12, enable statusnet 
#apt-get install phpmyadmin mysql-server  ssmpt  php5-curl 
#wget  tatus
# Error code: ssl_error_rx_record_too_long
	a2ensite default-ssl
	service apache2 reload
#OK

#3.2 change mysql datadir
top mysqld
cp -Rp /var/lib/mysql /var/www/mysql
sudo cp -R -p /var/lib/mysql /newpath
vi /etc/mysql/my.cnf  # modify the datadir
vi /etc/apparmor.d/usr.sbin.mysqld  #modify datadir, symlink is not support ,use dmesg to debu
/etc/init.d/apparmor reload
sudo /etc/init.d/mysql restart

#3.3 config statusnet



#3. enable remote to access DISPLAY on lightdm
install vnc4server
#use tightvncserver or else, you have to face a  "no password configured for vnc auth" issue
edit lightdm.conf, following the example of /usr/share/doc/lightdm
# it seemd it failed, I don't know why . Another way is run vino-preperences


#4. build xbmc

#5. build android


