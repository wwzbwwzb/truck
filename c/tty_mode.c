/**

  VT_ACTIVATE
  1
  2. cursor control
  3. graphics functions


*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/fb.h>
#include <linux/kd.h>

#define TTYN "/dev/tty10"
#define FBDEVICE "/dev/fb0"

struct FB {
    unsigned short *bits;
    unsigned size;
    int fd;
    struct fb_fix_screeninfo fi;
    struct fb_var_screeninfo vi;
};

#define fb_width(fb) ((fb)->vi.xres)
#define fb_height(fb) ((fb)->vi.yres)
#define fb_size(fb) ((fb)->vi.xres * (fb)->vi.yres * 2)

void android_memset16(void *_ptr, unsigned short val, unsigned count)
{
    unsigned short *ptr = _ptr;
    count >>= 1;
    while(count--)
        *ptr++ = val;
}

int vt_open(void)
{
	//struct vt_consize vtsize;
//	struct vt_stat	  vtstate;
	int ret;


//	ret = ioctl(fd, VT_RESIZEX, &data);
//	perror("RESIZEX");
	
//	ioctl(fd,  VT_WAITEVENT);
//		perror("");


		return 0;
}


void dump_fbinfo( struct FB *fb)
{
    struct fb_fix_screeninfo *fi=&fb->fi;
    struct fb_var_screeninfo *vi=&fb->vi;
	printf("fix info for fb: id=%s \n\r\tsmem(0x%lx@0x%x) line_length=%d accel=0x%x\n", 
			fi->id, fi->smem_start,  fi->smem_len,	fi->line_length, fi->accel );
	printf("var info for fb: res(0x%xx0x%x) res_virtual(0x%xx0x%x) offset(0x%xx0x%x)\n\r\tbpp=%d isgray=%d hxw(%dx%d) margin[%d,%d,%d,%d] sync_len(%d,%d) \n", vi->xres, vi->yres, vi->xres_virtual, vi->yres_virtual,
			vi->xoffset, vi->yoffset, vi->bits_per_pixel, vi->grayscale,
			vi->width, vi->height,
			vi->left_margin, vi->right_margin, vi->upper_margin, vi->lower_margin,
			vi->hsync_len,  vi->vsync_len);
}

static int fb_open(struct FB *fb)
{
	int fd=-1;
	int ret=0;
	void * fbmem;

    fd = open( FBDEVICE, O_RDWR);
    if (fd < 0)
		perror("openfb");

	fb->fd=fd;

    if (ioctl(fd, FBIOGET_FSCREENINFO, &fb->fi) < 0)
		perror("FSCREENINFO");
    if (ioctl(fd, FBIOGET_VSCREENINFO, &fb->vi) < 0)
		perror("VSCREENINFO");

	dump_fbinfo(fb);

	printf(" mmap size=0x%x\n", fb_size(fb));
    fb->bits = mmap(0, fb_size(fb), PROT_READ | PROT_WRITE, 
                    MAP_SHARED, fd, 0);
    if (fb->bits == MAP_FAILED)
		perror("mmap");

#if 0
	getchar();
	printf(" set par\n");
    if (ioctl(fd, FBIOPUT_VSCREENINFO, &fb->vi) < 0)
		perror("VSCREENINFO");
#endif
	while(1){
	getchar();
//	fb->vi.yoffset +=1;
	printf(" set par yoffset=%x\n", fb->vi.yoffset);
    if (ioctl(fd, FBIOPAN_DISPLAY, &fb->vi) < 0)
		perror("VSCREENINFO");
	}
	printf(" press enter to exit\n");
	getchar();

	close(fb->fd);
    return 0;
}

static void fb_close(struct FB *fb)
{
    munmap(fb->bits, fb_size(fb));
    close(fb->fd);
}

/* there's got to be a more portable way to do this ... */
static void fb_update(struct FB *fb)
{
    fb->vi.yoffset = 1;
    ioctl(fb->fd, FBIOPUT_VSCREENINFO, &fb->vi);
    fb->vi.yoffset = 0;
    ioctl(fb->fd, FBIOPUT_VSCREENINFO, &fb->vi);
}

static int vt_set_mode(int graphics)
{
    int fd, r;
    fd = open( TTYN, O_RDWR | O_SYNC);
    if (fd < 0)
        return -1;
    r = ioctl(fd, KDSETMODE, (void*) (graphics ? KD_GRAPHICS : KD_TEXT));
	printf(" KDSETMODE #TTYN ret=%d\n", r);
	perror("KDSETMODE");
//    close(fd);
    return r;
}

/* 565RLE image format: [count(2 bytes), rle(2 bytes)] */

int main(int argc, char *argv[])
{
	char *fn=TTYN;
    struct FB fb;

	printf("==========test framebuffer====\n");
    if (fb_open(&fb))
		perror("fb_open");

}

