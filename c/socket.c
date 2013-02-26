
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <poll.h>


#define Assert(x)  if((x)) { printf("%d):%s\n", __LINE__, __FUNCTION__);perror(""); exit(1); }


int test_unix()
{

  int sock ;
  int r;
  struct sockaddr_un addr;

  sock = socket(AF_UNIX, SOCK_STREAM, 0);
  Assert(sock<=0)

  strcpy(addr.sun_path, "/tmp/.X11-unix/X0");
  addr.sun_family= AF_UNIX;

  r= connect (sock, (struct sockaddr *) &addr, sizeof(addr));
  Assert(r<=0);

  struct pollfd fds[1];
  fds[0].fd = sock;
  fds[0].events = POLLIN;
  fds[0].revents = 0;
  r = poll(fds, 1, 1000);
  Assert(r<0);
  if(r>0){
    if( fds[0].revents & POLLIN){
      printf("can read\n");
    }
  }


    

  return 0; 
}


int main()
{
  
	struct sockaddr_nl nladdr;
	int sz = 64 * 1024;
	int on = 1;

	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;
	nladdr.nl_pid = getpid();
	nladdr.nl_groups = 0xffffffff;

	int sock=-1;
	if ((mSock = socket(PF_NETLINK,SOCK_DGRAM,NETLINK_KOBJECT_UEVENT)) < 0) {
		SLOGE("Unable to create uevent socket: %s", strerror(errno));
		return -1;
	}

	if (setsockopt(mSock, SOL_SOCKET, SO_RCVBUFFORCE, &sz, sizeof(sz)) < 0) {
		SLOGE("Unable to set uevent socket SO_RECBUFFORCE option: %s", strerror(errno));
		return -1;
	}

	if (setsockopt(mSock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on)) < 0) {
		SLOGE("Unable to set uevent socket SO_PASSCRED option: %s", strerror(errno));
		return -1;
	}

	if (bind(mSock, (struct sockaddr *) &nladdr, sizeof(nladdr)) < 0) {
		SLOGE("Unable to bind uevent socket: %s", strerror(errno));
		return -1;
	}

	mHandler = new NetlinkHandler(mSock);
	if (mHandler->start()) {
		SLOGE("Unable to start NetlinkHandler: %s", strerror(errno));
		return -1;
	}
	return 0;
}

int NetlinkManager::stop() {
	if (mHandler->stop()) {
		SLOGE("Unable to stop NetlinkHandler: %s", strerror(errno));
		return -1;
	}
	delete mHandler;
	mHandler = NULL;

	close(mSock);
	mSock = -1;

	return 0;
}
return 0;
}
