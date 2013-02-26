#include <sys/mman.h>



#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/fb.h>

#if HAVE_ANDROID_OS
#include <linux/fb.h>
#endif

#define NUM_BUFFERS  2

short color_plat[] = {0x1f, 0x07e0, 0xf800, 
	0x1f, 0x07e0, 0xf800, 
	0x1f, 0x07e0, 0xf800, 
	0x1f, 0x07e0, 0xf800
};
struct wmt_fb_dev 
{

	void *base;
	int mapsize;
	int stride;
	unsigned char *currentBuffer;
	unsigned char *NextBuffer;
	int framesize;
	int alter_yoffset;
	int fd;
	struct fb_var_screeninfo info;
	struct fb_fix_screeninfo fix;
	int width;
	int height;

} mdev, *m;

int post_fb()
{
	static nframe=0;
	if (!m)
		return -1;

	m->info.activate = FB_ACTIVATE_VBL;

	m->currentBuffer = m->NextBuffer;
	m->NextBuffer= m->base + m->framesize*nframe;

	nframe ++;
	if( nframe >=NUM_BUFFERS ) nframe=0;

	m->info.yoffset = m->info.yres_virtual/2*nframe;
	if (ioctl(m->fd, FBIOPAN_DISPLAY, &m->info) == -1) {
	//gwj if (ioctl(m->fd, FBIOPUT_VSCREENINFO, &m->info) == -1) {
		printf("FBIOPUT_VSCREENINFO failed");
		return -ENODEV;
	}
	 
	return 0;
}

int unmap_fb()
{
	/*
	 * map the framebuffer
	 */

	int err;
	int fd=m->fd;
	struct fb_var_screeninfo *info = &m->info;
	size_t fbSize = m->stride * info->yres_virtual;


	if(m->base){
		err= munmap(m->base, m->mapsize);
		if(err) perror("munmap");
		m->base=NULL;
		m->mapsize=0;
	}
	return 0;
}

int map_fb()
{
	int err;
	int fd=m->fd;
	struct fb_var_screeninfo *info = &m->info;
	size_t fbSize ;
	
	// *** which of the 3 is typical
	//m->stride=m->fix.line_length;
	m->stride=m->info.xres;
	//m->stride=m->info.xres_virtual;
	fbSize= m->stride* info->yres_virtual * info->bits_per_pixel/8;

	void* vaddr = mmap(0, fbSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (vaddr == MAP_FAILED) {
		printf("Error mapping the framebuffer (%s)", strerror(errno));
		return -errno;
	}
	/* test segment fault
	*(short *)(vaddr+1280*1024*2+10) = 0x33ff;
	printf(" try access mmap region\n");
	*/
	m->base = (vaddr);
	m->mapsize=fbSize;
	m->framesize = m->stride * info->yres * 
			(m->info.bits_per_pixel/8);
	printf(" framesize=%d\n", m->framesize);
		m->currentBuffer = m->base ;
		m->NextBuffer = m->base + m->framesize ; 
//		memset(vaddr, 0, fbSize);
    return 0;
}

int change_mode(int x, int y)
{
	struct fb_var_screeninfo *info = &m->info;
	int fd = m->fd ;

	printf(" (dbg) change mode to %dx%d\n", x,y);

	unmap_fb();
	if (ioctl(fd, FBIOGET_VSCREENINFO, &m->info) == -1){
		perror("ioctl");
		return -errno;
	}

	info->reserved[0] = 0;
	info->reserved[1] = 0;
	info->reserved[2] = 0;
	info->xoffset = 0;
	info->yoffset = 0;
	info->activate = FB_ACTIVATE_NOW;

	/*
	 * Explicitly request 5/6/5
	 */
	info->bits_per_pixel = 16;
	info->red.offset     = 11;
	info->red.length     = 5;
	info->green.offset   = 5;
	info->green.length   = 6;
	info->blue.offset    = 0;
	info->blue.length    = 5;
	info->transp.offset  = 0;
	info->transp.length  = 0;
	info->xres=x;
	info->yres=y;

	/*
	 * Request NUM_BUFFERS screens (at lest 2 for page flipping)
	 */
	info->yres_virtual = info->yres * NUM_BUFFERS;


	if (ioctl(fd, FBIOPUT_VSCREENINFO, info) == -1) {
		info->yres_virtual = info->yres;
		printf("FBIOPUT_VSCREENINFO failed, page flipping not supported\n");
	}

	if (info->yres_virtual < info->yres * 2) {
		// we need at least 2 for page-flipping
		info->yres_virtual = info->yres;
		printf("page flipping not supported (yres_virtual=%d, requested=%d)\n",
				info->yres_virtual, info->yres*2);
	}

	if (ioctl(fd, FBIOGET_VSCREENINFO, info) == -1){
		printf(" set Var info errro\n");
		return -errno;
	}

	if(!info->width || !info->height) {
		printf(" error get info\n");
		return ;
	}

	float xdpi = (info->xres * 25.4f) / info->width;
	float ydpi = (info->yres * 25.4f) / info->height;

	printf(   "using (fd=%d)\n"
			"id           = %s\n"
			"xres         = %d px\n"
			"yres         = %d px\n"
			"xres_virtual = %d px\n"
			"yres_virtual = %d px\n"
			"bpp          = %d\n"
			"r            = %2u:%u\n"
			"g            = %2u:%u\n"
			"b            = %2u:%u\n",
			fd,
			m->fix.id,
			info->xres,
			info->yres,
			info->xres_virtual,
			info->yres_virtual,
			info->bits_per_pixel,
			info->red.offset, info->red.length,
			info->green.offset, info->green.length,
			info->blue.offset, info->blue.length
			);

	printf(   "width        = %d mm (%f dpi)\n"
			"height       = %d mm (%f dpi)\n",
			info->width,  xdpi,
			info->height, ydpi
			);


	if (ioctl(fd, FBIOGET_FSCREENINFO, &m->fix) == -1){
		printf(" get fix info error\n");
		return -errno;
	}

	printf("fix.smem_len=%d\n", m->fix.smem_len);
	printf("fix.line_length = %d\n", m->fix.line_length);
	if (m->fix.smem_len <= 0){
		printf(" smem_len is unreasonable\n");
		return -errno;
	}
	map_fb();

	return 0;
}

int init_fb()
{
	char const * const device_template[] = {
            "/dev/graphics/fb%u",
            "/dev/fb%u",
            0 };

    int fd = -1;
    int i=0;
    char name[64];
	struct fb_var_screeninfo *info = &m->info;

    while ((fd==-1) && device_template[i]) {
        snprintf(name, 64, device_template[i], 0);
        fd = open(name, O_RDWR, 0);
        i++;
    }
    if (fd < 0){
		printf(" can't open fb device\n");
        return -errno;
	}

		m = &mdev;
		m->fd = fd;
		if (ioctl(fd, FBIOGET_FSCREENINFO, &m->fix) == -1)
			return -errno;

		if (ioctl(fd, FBIOGET_VSCREENINFO, &m->info) == -1){
			perror("ioctl");
			return -errno;
		}
		info = &m->info;

		int x= m->info.xres;
		int y= m->info.yres;
		change_mode( x,y );
	  
		return 0;
}

int paint_single(short color)
{
	short *buf=NULL;
	int x=m->stride;
	int y=m->info.yres;

	buf = (short *) mdev.base;
	buf = (short *) ((int)mdev.base + mdev.framesize);
	buf = (short *) mdev.NextBuffer;
	buf = (short *) mdev.currentBuffer;

	printf("paint color 0x%x, with 0x%x bytes\n",color, x*y);
	int j=0;
	for(j=0; j< x*y; j++){
		buf[j]= color;
	}
		post_fb();

	return 0;
}

int paint_color(int color)
{
	int i=0;
	int j=0;

	paint_single(color );
	for (i=0; i< 4; i++){
	}
	return 0;
}

int main()
{
	int ret =0;
	int i=0;

	ret = init_fb();
	if(ret){
		return -ENODEV;
	}

//	paint_color();
	//change res for test. watch /sys/class/graphic/fb0/mode[s].
	paint_single(color_plat[0]);
	sleep(1);
	paint_single(color_plat[1]);
	sleep(1);
	paint_single(color_plat[0]);
	sleep(1);
	paint_single(color_plat[1]);
	sleep(1);

	change_mode(800,600);
//	paint_color(color_plat[0]);
	printf("%s : %d\n", __FUNCTION__, __LINE__);
	sleep(2);

	change_mode(1024,768);
//	paint_color(color_plat[1]);
	printf("%s : %d\n", __FUNCTION__, __LINE__);
	sleep(2);

	return 0;
}

	
