/* Simple XCB application drawing a box in a window */
/* to compile it use :
	 gcc -Wall x.c -lxcb
 */
/******
 * Window stuff for server 
 *
 *    CreateRootWindow, CreateWindow, ChangeWindowAttributes,
 *    GetWindowAttributes, DeleteWindow, DestroySubWindows,
 *    HandleSaveSet, ReparentWindow, MapWindow, MapSubWindows,
 *    UnmapWindow, UnmapSubWindows, ConfigureWindow, CirculateWindow,
 *    ChangeWindowDeviceCursor
 ******/

#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <assert.h>
#include <cairo/cairo.h>

       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
       #include <errno.h>

#include <xcb/dri2.h>
#include <drm/drm.h>
#include <drm/i915_drm.h>
#if 0
void  draw_mask(cairo_t *cr)
{
	cairo_pattern_t *linpat, *radpat;
	linpat = cairo_pattern_create_linear (0, 1, 1, 1);
	cairo_pattern_add_color_stop_rgb (linpat, 1, 0, 0, 0); //offset ,r,g,b
	cairo_pattern_add_color_stop_rgb (linpat, 1, 0, 0, 0);
	
	radpat = cairo_pattern_create_radial (300,200, 100, 300,200, 400);
	cairo_pattern_add_color_stop_rgba (radpat, 1, 0, 0, 0, 0.5);
	cairo_pattern_add_color_stop_rgba (radpat, 1, 0, 0, 0, 0.5);
	
	cairo_set_source (cr, linpat);
	cairo_mask (cr, radpat);
}

void draw_gc(xcb_window_t win)
{
	xcb_rectangle_t      r = { 0, 0,  0,0 };
	xcb_connection_t    *c = ;
	xcb_get_geometry_reply_t *geo;

	geo = xcb_get_geometry_reply(c,
			xcb_get_geometry(c, win), NULL);

	if(0)printf("get (%hdx%hd)@(%dx%d)\n",  
			geo->x, geo->y, geo->width, geo->height);

	xcb_get_image_cookie_t cookie;
	xcb_get_image_reply_t *reply;
	xcb_generic_error_t *error;
	xcb_void_cookie_t ck;

	if(geo!=NULL){
		r.width = geo->width ;
		r.height=geo->height;
		r.x=0;
		r.y=0;
		//printf("window depth=%d\n", geo->depth);
		if(geo->depth == 24 |  geo->depth == 32)
			psize = r.width * r.height * 4; //correct
		else if(geo->depth==16)
			psize = r.width * r.height * 2; //correct
		else
			printf(" *** unsupported window depth\n");
	}else
		psize = r.width * r.height * 4; //correct
	free(geo);

}

#endif

char * pbuf=NULL;
void draw_put_image(xcb_window_t win)
{
	xcb_gcontext_t       g;

	xcb_connection_t    *c;
	xcb_screen_t        *s;
	xcb_window_t         w;
	xcb_rectangle_t      r = { 0, 0,  50,50 };

	c = (xcb_connection_t *)pthread_getspecific(1);
	s = (xcb_screen_t *)pthread_getspecific(2);

	g = xcb_generate_id(c);
	xcb_create_gc(c, g, win, 0,NULL);
	xcb_put_image_checked(c, XCB_IMAGE_FORMAT_Z_PIXMAP,
			win, g,
			r.width, r.height, r.x,r.y,
			0, s->root_depth,
			r.width* r.height*4, pbuf);
	xcb_flush(c);
}

void  key_exit( void *arg)
{
	printf(" on_exit\n");
}

void draw_dri2()
{
	int ret;
	xcb_dri2_query_version_cookie_t query_version_cookie;
	xcb_dri2_query_version_reply_t *query_version_reply;

	xcb_dri2_connect_cookie_t cookie;
	xcb_dri2_connect_reply_t *reply;
	xcb_dri2_authenticate_cookie_t auth_cookie;
	xcb_dri2_authenticate_reply_t *auth_reply;

	xcb_connection_t    *c;
	xcb_screen_t        *s;
	xcb_window_t         w;
	xcb_gcontext_t       g;
	char *device_name ="/dev/dri/card0";

	c = (xcb_connection_t *)pthread_getspecific(1);
	s = (xcb_screen_t *)pthread_getspecific(2);
	w= (xcb_window_t)pthread_getspecific(3);


	printf("======== draw with dri2 directoly\n");
	// dri2: connect, open drm,  auth,   get_image, get_buffers
	//xcb_dri2_connect_device_name_length(connect_reply);
	int xvmc_fd = open(device_name, O_RDWR);
	if (xvmc_fd < 0) {
		perror("Failed to open drm device: ");
		return -1;
	}

	drm_magic_t  magic;
	if (drmGetMagic(xvmc_fd, &magic)) {
		perror("Failed to get magic\n");
		return -2;
	}
	auth_reply = xcb_dri2_authenticate_reply(c, xcb_dri2_authenticate(c, s->root, magic), NULL);
	assert(auth_reply != 0);

	// get image ? why  :  resource temporary unavailable
	//get dri2 buffers of this window drawable
	{
   xcb_dri2_dri2_buffer_t *buffers;
   xcb_dri2_get_buffers_reply_t *reply;
   xcb_dri2_get_buffers_cookie_t cookie;

	 int attachments[1];

	 xcb_void_cookie_t  cookie2;
	 cookie2 =  xcb_dri2_create_drawable (c, w);
	 assert(cookie2.sequence !=0);

   attachments[0] = XCB_DRI2_ATTACHMENT_BUFFER_FRONT_LEFT;
   attachments[0] = XCB_DRI2_ATTACHMENT_BUFFER_BACK_LEFT;
   cookie = xcb_dri2_get_buffers_unchecked (c,w,
					    1, 1, attachments);
	 assert(cookie.sequence!=0);
   reply = xcb_dri2_get_buffers_reply (c, cookie, NULL);
	 assert(reply!=NULL);
   buffers = xcb_dri2_get_buffers_buffers (reply);
   assert(buffers != NULL);

	 printf(" dri: reply->count= %d\n", reply->count);
	 printf(" (%d x %d) \n", reply->width, reply->height);


   xcb_dri2_dri2_buffer_t b = buffers[0];
      printf(" buffer[%d] attachment=%d name=%d pitch=%d cpp=%d flags=%x\n",
					0, b.attachment,b.name, b.pitch, b.cpp, b.flags);

   // mmap
	struct drm_gem_open open_arg;
	struct drm_i915_gem_get_tiling get_tiling;
	struct drm_i915_gem_mmap mmap;

  memset(&open_arg, 0, sizeof(open_arg));
	open_arg.name = b.name;
	ret = drmIoctl(xvmc_fd, DRM_IOCTL_GEM_OPEN, &open_arg);
	assert (ret == 0) ;
	printf("GEM_OPEN name=%d handle=%d size=0x%x\n", open_arg.name, open_arg.handle, open_arg.size);

	mmap.handle = open_arg.handle;
	mmap.offset = 0;
	int tsize= b.pitch * reply->height;
	mmap.size =(long long int)tsize;
	ret = ioctl(xvmc_fd, DRM_IOCTL_I915_GEM_MMAP, &mmap);
	assert(ret == 0);
	memset(mmap.addr_ptr, 0xff,  tsize);
	printf("%d\n", tsize);

	free(reply);
	}
	{
		xcb_generic_error_t  *errors;
		xcb_dri2_swap_buffers_reply(c, xcb_dri2_swap_buffers (c, w, 0,0,0,0,0,0), &errors);
	}
}

#include <signal.h>
void handler(int  num)
{
	//print_trace();
	exit(1);
}

int main(void)
{
	xcb_connection_t    *c;
	xcb_screen_t        *s;
	xcb_window_t         w;
	xcb_gcontext_t       g;
	xcb_generic_event_t *e;
	uint32_t             mask;
	uint32_t             values[4];
  xcb_get_geometry_reply_t *geo;
	xcb_rectangle_t      r = { 0, 0,  100,100 };
  int done =0 ;
  int ret, i=0;

	pbuf = malloc(1024*4);
	memset(pbuf, 0xff, 1024*4);
	signal(SIGSEGV, handler);
	/* open connection with the server */

	c = xcb_connect(NULL,NULL);
	if (xcb_connection_has_error(c)) {
		printf("Cannot open display\n");
		exit(1);
	}

   perror("noerror");
	/* get the first screen */
	s = xcb_setup_roots_iterator( xcb_get_setup(c) ).data;

  /* create sub window */
  w= xcb_generate_id(c);
	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = s->black_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
  xcb_void_cookie_t cookie;

	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = s->white_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS
    | XCB_EVENT_MASK_VISIBILITY_CHANGE
    ;
	values[1] = 0xffff;
	printf("  root_depth = 0x%x\n", s->root_depth);
	xcb_create_window(c, s->root_depth, w, s->root,
      10, 10, r.width, r.height,
      1,
			XCB_WINDOW_CLASS_INPUT_OUTPUT, s->root_visual,
			mask, values);

  xcb_map_window(c, w); xcb_flush(c);

//  xcb_reparent_window(c, child,  G.s->root,100,100);

  values[1] |= XCB_EVENT_MASK_POINTER_MOTION |
     XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE;
  xcb_change_window_attributes_checked(c, w, mask, values); xcb_flush(c);

	pthread_key_t key = 0;
	while(key<8){
		pthread_key_create(&key, key_exit);
	}
	//pthread_setspecific(0, (void*)(long)dpy);
	pthread_setspecific(1, (void*)(long)c);
	pthread_setspecific(2, (void*)(long)s);
	pthread_setspecific(3, (void*)(long)w);
	pthread_setspecific(4, (void*)(long)s);

	perror("noerror");
	printf("=====================================\n");
  //create_dri_drawable();
#if 0

	xcb_dri2_query_version_cookie_t query_version_cookie;
	xcb_dri2_query_version_reply_t *query_version_reply;
	xcb_dri2_connect_cookie_t connect_cookie;
	xcb_dri2_connect_reply_t *connect_reply;
	xcb_dri2_authenticate_cookie_t auth_cookie;
	xcb_dri2_authenticate_reply_t *auth_reply;

	connect_cookie = xcb_dri2_connect(c, s->root, XCB_DRI2_DRIVER_TYPE_DRI);

	connect_reply = xcb_dri2_connect_reply(c, connect_cookie, NULL);

	// dri2: connect, open drm,  auth,   get_image, get_buffers
	//xcb_dri2_connect_device_name_length(connect_reply);
	drm_magic_t  magic;
	char *device_name = xcb_dri2_connect_device_name(connect_reply);
	free(connect_reply);
	printf("open dri dev :%s\n", device_name);
	int xvmc_fd = open(device_name, O_RDWR);
	if (xvmc_fd < 0) {
		perror("Failed to open drm device: ");
		return -1;
	}
	if (drmGetMagic(xvmc_fd, &magic)) {
		perror("Failed to get magic\n");
		return -2;
	}
	printf(" drm magic=%d\n", magic);
	
	auth_cookie = xcb_dri2_authenticate(c, s->root, magic);
	auth_reply = xcb_dri2_authenticate_reply(c, auth_cookie, NULL);
	if (!auth_reply) {
		printf("Failed to authenticate magic %d\n", magic);
		return -3;
	}

	// get image ? why  :  resource temporary unavailable
#if 0
	{
		xcb_get_image_cookie_t cookie;
		xcb_get_image_reply_t *reply;
		xcb_generic_error_t *error;
		cookie = xcb_get_image (c, XCB_IMAGE_FORMAT_Z_PIXMAP,
				w, 0,0, 1,1, ~0);
		reply = xcb_get_image_reply (c, cookie, &error);
		if (reply == NULL){
			perror("get_image_replay");
			return;
		}

		if (error != NULL) {
			perror("error in xcb_get_image");
			free(error);
		} else {
			uint32_t bytes = xcb_get_image_data_length(reply);
			uint8_t *idata = xcb_get_image_data(reply);
			printf("get_image size=%d p=%p\n", bytes, idata);
		}
		free(reply);
	}
#endif

	//get dri2 buffers of this window drawable
	{
   xcb_dri2_dri2_buffer_t *buffers;
   xcb_dri2_get_buffers_reply_t *reply;
   xcb_dri2_get_buffers_cookie_t cookie;

	 int count = 11;
	 int attachments[11];


	 xcb_void_cookie_t  cookie2;
	 cookie2 =  xcb_dri2_create_drawable (c, w);
	 assert(cookie2.sequence !=0);

	 /*typedef enum xcb_dri2_attachment_t {
    XCB_DRI2_ATTACHMENT_BUFFER_FRONT_LEFT,
    XCB_DRI2_ATTACHMENT_BUFFER_BACK_LEFT,
    XCB_DRI2_ATTACHMENT_BUFFER_FRONT_RIGHT,
    XCB_DRI2_ATTACHMENT_BUFFER_BACK_RIGHT,
    XCB_DRI2_ATTACHMENT_BUFFER_DEPTH,
    XCB_DRI2_ATTACHMENT_BUFFER_STENCIL,
    XCB_DRI2_ATTACHMENT_BUFFER_ACCUM,
    XCB_DRI2_ATTACHMENT_BUFFER_FAKE_FRONT_LEFT,
    XCB_DRI2_ATTACHMENT_BUFFER_FAKE_FRONT_RIGHT,
    XCB_DRI2_ATTACHMENT_BUFFER_DEPTH_STENCIL,
    XCB_DRI2_ATTACHMENT_BUFFER_HIZ
} xcb_dri2_attachment_t;

	 		*/
   attachments[0] = XCB_DRI2_ATTACHMENT_BUFFER_FRONT_LEFT;
   attachments[1] = XCB_DRI2_ATTACHMENT_BUFFER_BACK_LEFT;
   attachments[2] = XCB_DRI2_ATTACHMENT_BUFFER_FRONT_RIGHT;
   attachments[3] = XCB_DRI2_ATTACHMENT_BUFFER_BACK_RIGHT;
   attachments[4] = XCB_DRI2_ATTACHMENT_BUFFER_DEPTH;
   attachments[5] = XCB_DRI2_ATTACHMENT_BUFFER_STENCIL;
   attachments[6] = XCB_DRI2_ATTACHMENT_BUFFER_ACCUM;
   attachments[7] = XCB_DRI2_ATTACHMENT_BUFFER_FAKE_FRONT_LEFT;
   attachments[8] = XCB_DRI2_ATTACHMENT_BUFFER_FAKE_FRONT_RIGHT;
   attachments[9] = XCB_DRI2_ATTACHMENT_BUFFER_DEPTH_STENCIL;
   attachments[10] = XCB_DRI2_ATTACHMENT_BUFFER_HIZ;
   count=11;
   cookie = xcb_dri2_get_buffers_unchecked (c,w,
					    count, count, attachments);
	 assert(cookie.sequence!=0);
   reply = xcb_dri2_get_buffers_reply (c, cookie, NULL);
#if 1
	 assert(reply!=NULL);
   buffers = xcb_dri2_get_buffers_buffers (reply);
   assert(buffers != NULL);

	 printf(" dri: reply->count= %d\n", reply->count);
	 printf(" (%d x %d) \n", reply->width, reply->height);


   xcb_dri2_dri2_buffer_t b = buffers[7];
	 for ( i=0; i< reply->count; i++){
   xcb_dri2_dri2_buffer_t lb = buffers[i];
      printf(" buffer[%d] attachment=%d name=%d pitch=%d cpp=%d flags=%x\n",
      		i, lb.attachment,lb.name, lb.pitch, lb.cpp, lb.flags);
	 }

   // mmap
	struct drm_gem_open open_arg;
	struct drm_i915_gem_get_tiling get_tiling;
struct drm_i915_gem_mmap mmap;

  memset(&open_arg, 0, sizeof(open_arg));
	open_arg.name = b.name;
	ret = drmIoctl(xvmc_fd, DRM_IOCTL_GEM_OPEN, &open_arg);
	assert (ret == 0) ;
	printf("GEM_OPEN name=%d handle=%d size=0x%x\n", open_arg.name, open_arg.handle, open_arg.size);

	mmap.handle = open_arg.handle;
	mmap.offset = 0;
	int tsize= b.pitch * reply->height;
	mmap.size =(long long int)tsize;
	ret = ioctl(xvmc_fd, DRM_IOCTL_I915_GEM_MMAP, &mmap);
	assert(ret == 0);
	memset(mmap.addr_ptr, 0xff,  tsize);
	printf("%d\n", tsize);

	free(reply);
#endif
	 }
{
	 struct drm_i915_gem_create create;
	struct drm_gem_flink flink;
	struct drm_gem_open open;

	memset(&create, 0, sizeof(create));
	create.size = 16 * 1024;
	ret = ioctl(xvmc_fd, DRM_IOCTL_I915_GEM_CREATE, &create);
	assert(ret == 0);

	flink.handle = create.handle;
	ret = ioctl(xvmc_fd, DRM_IOCTL_GEM_FLINK, &flink);
	assert(ret == 0);
	printf("drm get flink h=%d name=%d\n", flink.handle, flink.name);



	struct drm_gem_open open_arg;
	struct drm_i915_gem_get_tiling get_tiling;
  memset(&open_arg, 0, sizeof(open_arg));
	open_arg.name = flink.name;
	ret = drmIoctl(xvmc_fd, DRM_IOCTL_GEM_OPEN, &open_arg);
	assert (ret == 0) ;
}
{
xcb_generic_error_t  *errors;
xcb_dri2_swap_buffers_cookie_t cookie;
cookie = xcb_dri2_swap_buffers (c, w, 0,0,0,0,0,0);
 xcb_dri2_swap_buffers_reply(c, cookie, &errors);
}
#endif

  // geneate egl surface
	printf("=====================================\n");

  while (!done ) {
    e = xcb_wait_for_event(c);
		if(!e) { printf("wait event error\n");
			break;}
		else{
			switch (e->response_type & ~0x80) {
				case XCB_EXPOSE:    /* draw or redraw the window */
					printf("XCB_EXPOSE\n");
					break;
				case XCB_KEY_PRESS:  /* exit on key press */
					{
					struct xcb_key_press_event_t  *ep = e;

					printf("key=%d\n", (char)ep->detail);
					if(ep->detail == 9)
						exit(1);
	draw_put_image(w);
					draw_dri2();
					break;
					}
				case XCB_MOTION_NOTIFY:
					{
          printf("XCB_MOTION\n");
					}
          break;
			default:
          printf("default e=%d\n",e->response_type);
					break;
      }
      free(e);
    }
  }
  xcb_unmap_window(c, w);
  xcb_destroy_window(c, w);
  return 0;
}
