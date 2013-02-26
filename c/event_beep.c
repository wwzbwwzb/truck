       #include <curses.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int beep(void);
int flash(void);

int main()
{
	struct input_event ev[64];
	printf("err:%s\n", strerror(2));


	ev[0].type=EV_SND ;
	ev[0].code=SND_BELL ;
	ev[0].value=1000 ;
	ev[1].type=EV_SND ;
	ev[1].code=SND_TONE;
	ev[1].value=2000 ;
	ev[2].type=EV_SND ;
	ev[2].code=SND_BELL ;
	ev[2].value=0 ;
	ev[3].type=EV_SND ;
	ev[3].code=SND_TONE;
	ev[3].value=3000 ;
	int fd = open("/dev/input/event6", O_RDWR);
	perror("open");

	write(fd, &ev[0], sizeof(ev[0]));
	perror("write");

	getchar();
	write(fd, &ev[1], sizeof(ev[0]));
	getchar();
	write(fd, &ev[3], sizeof(ev[0]));
	getchar();
	write(fd, &ev[2], sizeof(ev[0]));

	printf("%s", "DK");


	return 0;

}

