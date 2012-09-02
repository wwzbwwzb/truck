2012-08-25
	I found another interesting work models:
		http://www.linuxdeepin.com/

	if modify some config in jb-dev, you can built it under 32bit. :( I found to developt on 64bit machine is a  horrable thing. Becase, you have to let 32bit elf-binary run compatiblly. For this, it mean you have to install twice of files, to compare a 32bit OS.




================
tasks
   1. libfbv4l abstract various driver control & status, categories by uevent/name: module-version

=============
Augest 25 Sat.
  0. Deb:  bittorrnt transmission emacs23 fancontrol amule inotify-tools
  1. How to change unity quick launch panel
   xdg-utils define the action of browser action , such as chrome.
  2. add youtube repo to xbmc
	 ***. sudo cp -f wqy-microhei.ttc /usr/share/xbmc/media/Fonts/arial.ttf
	  But, it seem like arial has no more than 1 cent matter with the apperance

	 ***. convert  default.py charset 
	    #encoding=utf-8

	 *** youku.so is ELF 32-bit shared-object, got a wrong elf class error.
	   google suggest use 32bit python. fuck :(.
				 1. write a python library youku, realize it's request method. In fact,
				    it ask throw socket to youku_xbmc_daemon. 
   				 need python.socket python.plugin  
				 2. realize a daemon youku_xbmc_daemon, which call youku.so.request to gain url :)
		       :) small case.
		     3. from this incident , I found c/C++ binary is fuck not portable , especially on linux system.
				 so, why not choose java or other high-level language to realize arithmatic.

	 build curl:32 under 64bit system , need modify this file to avoid the following error:
	 /usr/include/curl/curlrules.h:152:
#ifdef __LP64__
#define CURL_SIZEOF_CURL_OFF_T 8
#else
#define CURL_SIZEOF_CURL_OFF_T 4
#endif

#ifdef __LP64__
#define CURL_SIZEOF_LONG 8
#else
#define CURL_SIZEOF_LONG 4
#endif

	  size of array ‘__curl_rule_01__’ is negative 
		size of array ‘__curl_rule_02__’ is negative

bug2:
	youku.so: undefined symbol: PyArg_ParseTuple
  youku.so: undefined symbol: curl_easy_init

	sudo apt-get install libcurl3-gnutls:i386
	apt-get download python2.7:i3i6   python-pycurl:i386 




  3. download  macos 10.7.2 lion
  4. Power Debug: hwmon0=acpitz hwmon1=eeepc_wmi 
  5. detect log file has been modifed with inotify-tools 
gg
