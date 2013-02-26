
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <assert.h>

/* int gettimeofday(struct timeval *tv, struct timezone *tz);
   / sighandler_t signal(int signum, sighandler_t handler);
   / unsigned int alarm(unsigned int seconds);
   int getitimer(int which, struct itimerval *curr_value);
   int setitimer(int which, const struct itimerval *new_value,
   struct itimerval *old_value);

usage:  How man gettimeofday will be called in one second , 
I test on my i3 4core 3.3GHZ cpu,  count=50M (Hz)
similar test may be poll, epoll, select. :)
how about you ?
 */

int done = 0;
int count=0;

void sighandler( int sig)
{
  printf("signal d\n",sig);
  done=1;
}

int main()
{
  struct timeval tv;
  int sec=0;
  long  time = 0; 
  long time_old = 0; 
  struct itimerval it;
  struct itimerval newit;
  int err;

#ifndef USE_VT
  /* if  use real time 
   */
     signal(SIGALRM,  sighandler);
     alarm(1);
#else
  /* if use virtual time 
  signal(SIGVTALRM,  sighandler);
  getitimer(ITIMER_VIRTUAL, &it);
  it.it_value.tv_sec=1;
  it.it_interval.tv_sec=1;
  err = setitimer(ITIMER_VIRTUAL, &it,NULL);
  assert(err==0);
   */
#endif

		 //计算一秒可以执行多少次gettimeofday()
  gettimeofday(&tv, NULL);
  time_old = tv.tv_sec*1000000 + tv.tv_usec;
  count=0;
  while(1){
    gettimeofday(&tv, NULL);
    //printf("tv:%d %d\n", tv.tv_sec, tv.tv_usec);
    time = tv.tv_sec*1000000 + tv.tv_usec;
    count++;
    if(done){
      printf("tv:%lu\n",time);
      printf("count=%dK\n", count/1000);
      count=0;
      done=0;
#ifndef USE_VT
     alarm(1);
#endif
    }
    //break;
  }

  return 0;
}
