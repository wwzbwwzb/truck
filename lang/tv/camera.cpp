
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "camera.h"
#include <sys/ioctl.h>
#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <sys/mman.h>

class Frame {
	int handle;
	void *buf;
	int size;
};
class CameraBase {
	public:
		int handle;
		Frame *frm;

public:
CameraBase(){}; //construct can't be defined as virtual
	virtual ~CameraBase(){};  //if no destruct or virtual is applied, 'undefined vtable ' error will appear
	virtual int stream_on()=0;
	virtual int grab()=0;
	virtual void release()=0;

};

class Setting {
	int fmt;
	int x,y;
	int bpp;
};

// for a V4LCamera
class Camera : public CameraBase {
	public:
		int quality;
		int snrc; //signal noice quality

#define MAX_FORMATS 16
#define MAX_CTRL	16
#define MAX_INPUT 16
#define MAX_NORM  16
		/* device descriptions */
		int                         ninputs,nstds,nfmts;
		int nbufs;
		struct v4l2_capability  cap;
		struct v4l2_streamparm  streamparm;
		struct v4l2_input   inps[MAX_INPUT];
		struct v4l2_standard        stds[MAX_NORM];
		struct v4l2_fmtdesc   fmts[MAX_FORMATS];
		struct v4l2_queryctrl ctls[MAX_CTRL*2];

		// config
		struct v4l2_format fmt;
		struct v4l2_format             fmt_v4l2;
		struct v4l2_requestbuffers     reqbufs;
		struct v4l2_buffer             vbufs[2];
		struct v4l2_framebuffer        ov_fb;
		struct v4l2_clip               ov_clips[2];
		struct v4l2_cropcap cropcap;
		struct v4l2_crop crop;




	public:
		Camera(char * name);
		virtual int stream_on();
		virtual int grab();
		void release(){}
};

Camera::Camera(char *name)
{
	handle=0;
nbufs=2;
}

int Camera::stream_on()
{
	if(handle <0) return -1;
	// use vivi directly first

	int flag = O_RDWR | O_NONBLOCK;
	flag = O_RDWR;
	handle = open("/dev/video0", flag, 0);
	if( handle<0){
		perror("open");
		return -1;
	}

	// VIDIOC_<op>_<name>
	// ENUM(input, std, fmt), 
	// G_PARAM(param, std, fmt,ctrl,input, tuner, freq,fbuf ,cap)
	// QUERYCTRL(ctrl, menu,cap, buf)
	// TRY_FMT
	// REQFUBS, QBUF, DQBUF,
	// STREAMON, STREAMOFF
	int ret= 0;

	ret = ioctl(handle, VIDIOC_QUERYCAP, &cap);
	if(ret ){		perror("ioctl enum fmt");	}
	printf("cap=0x%x\n", cap.capabilities);
	printf("ref: 0x50000001\n");


	{
	int index=0;
	fmts[0].index=index;
	fmts[0].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(handle, VIDIOC_ENUM_FMT, &fmts[0]);
	if(ret ){		perror("ioctl enum fmt");	}
	printf("enum format(%d): \n\tfmt=0x%x \n\tdesc=%s\n", 
			index,
			fmts[0].pixelformat, fmts[0].description); 
	}

	{
		cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		//ret = ioctl(handle, VIDIOC_CROPCAP, &cropcap);
		//error, Invalid param , why
		if(ret ){		perror("query cap");	}

	}

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(handle, VIDIOC_G_FMT, &fmt);
	if(ret ){		perror("ioctl get fmt");	}
	printf("format: \n\tfmt=0x%x \n\tres=(%dx%d)\n",
			fmt.fmt.pix.pixelformat, fmt.fmt.pix.width, fmt.fmt.pix.height); 

	if(0){
		fmt.fmt.pix.width       = 640;
		fmt.fmt.pix.height      = 480;
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
		fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

	ret = ioctl(handle, VIDIOC_S_FMT, &fmt);
	if(ret ){		perror("ioctl set fmt");	}
	}

	{
		reqbufs.count=nbufs=4;
		reqbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		reqbufs.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(handle, VIDIOC_REQBUFS, &reqbufs);
		if(ret ){		perror("request buffers");	}
		if(reqbufs.count <nbufs){
			printf(" request errro, not enough buffer\n");
		}

		for (int i=0; i< nbufs; i++){
			struct v4l2_buffer             buf;
			buf.index=i;
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;

			ret = ioctl(handle, VIDIOC_QUERYBUF, &buf );
			if(ret ){		perror("query buf");	}

			void *p = mmap(0, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, 
					handle, buf.m.offset);
			if(p==MAP_FAILED) {perror("mmap");}

			memset(&buf, 0, sizeof(buf));
			buf.index=i;
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			ret = ioctl(handle, VIDIOC_QBUF, &buf );
			if(ret ){		perror("QBUF");	}
		}
	}

	int type =V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(handle, VIDIOC_STREAMON, &type);
	if(ret ){		perror("streamon");	}

	ret = ioctl(handle, VIDIOC_STREAMON, &type);
	if(ret ){		perror("streamon");	}

	return 0;
}

int Camera::grab()
{
	int ret = 0 ;
	struct v4l2_buffer buf;

	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(handle, VIDIOC_DQBUF, &buf );
	if(ret ){		perror("DQBUF");	}

	printf("get one \n");
	ret = ioctl(handle, VIDIOC_QBUF, &buf );
	if(ret ){		perror("QBUF");	}
	return 0;
}

void draw_YUYV()
{
	printf(" draw texture\n");
}
extern void (*draw_tex)();

int main()
{
	Camera *cam = new Camera( (char *)"vivi");

	if(!cam) {
		perror("new Camera");
		return -1;
	}

	cam->stream_on();
	cam->grab();

	//display one frame in opengl's texture.
//	draw_tex = draw_YUYV;
	extern int init_screen(int x, int y);
	extern int loop();
	init_screen(640,480);
	printf("now\n");
//	loop();
	while(1){
		draw_YUYV();
	}

	return 0;
}

//bug1: QBUF, invalid argument
//if mmap is failed, QBUF will also failed ( for vivi)
