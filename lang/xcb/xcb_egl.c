/* Simple XCB application drawing a box in a window */
/* to compile it use :
	 gcc -Wall x.c -lxcb
 */
#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

///for gGL
#include <GL/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <assert.h>

       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>



#define USE_GL 1
pthread_key_t key_c, key_win,key_depth;

////////==================================
void reshape(int width, int height);
void draw(void);
void do_draw();

void  key_exit( void *arg)
{
	printf(" on_exit\n");
}

void reshape(int width, int height)
{
   GLfloat ar = (GLfloat) width / (GLfloat) height;

	printf("%s\n", __FUNCTION__);

   glViewport(0, 0, (GLint) width, (GLint) height);

	 /*
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-ar, ar, -1, 1, 5.0, 60.0);
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.0, 0.0, -10.0);
   */
}

void do_draw()
{
	xcb_connection_t    *c ; 
	xcb_window_t win;
	xcb_rectangle_t      r = { 20, 20, 60, 60 };
	int root_depth ;
	int psize;
	static char *pbuf = 0 ;
	EGLDisplay dpy;
	EGLSurface surf;

	if(!pbuf){
		pbuf = (char *)malloc( 1024*1024);
		memset(pbuf, 0x18, 1024);
	}
	psize = r.width * r.height * 4; //correct

	c = (xcb_connection_t *)pthread_getspecific(key_c);
	win = (xcb_window_t)pthread_getspecific(key_win);
	root_depth   = (int)pthread_getspecific(key_depth);
	dpy = pthread_getspecific(5);
	surf = pthread_getspecific(6);

#if 1
	{
		xcb_get_image_cookie_t cookie;
		xcb_get_image_reply_t *reply;
		xcb_generic_error_t *error;
		cookie = xcb_get_image (c, XCB_IMAGE_FORMAT_Z_PIXMAP,
				win, 0,0, 1,1, ~0);
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
#if 1
	printf(" c=%x win=%x\n", (int)c, (int)win);
	xcb_gcontext_t       g;
	g = xcb_generate_id(c);
	xcb_create_gc(c, g, win, 0,NULL);
	
	xcb_put_image_checked(c, XCB_IMAGE_FORMAT_Z_PIXMAP,
			win, g,
			r.width, r.height, r.x,r.y,
			0, root_depth,
			psize, pbuf);
	xcb_flush(c);
#endif

#if USE_GL
	printf("%s\n", __FUNCTION__);
   glClearColor(0.8, 0.4, 0.4, 0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   draw();

   eglSwapBuffers(dpy, surf);
#endif
}
static GLfloat view_rotx = 0.0, view_roty = 0.0;
static GLint u_matrix = -1;
static GLint attr_pos = 0, attr_color = 1;
void make_z_rot_matrix(GLfloat angle, GLfloat *m)
{
   float c = cos(angle * M_PI / 180.0);
   float s = sin(angle * M_PI / 180.0);
   int i;
   for (i = 0; i < 16; i++)
      m[i] = 0.0;
   m[0] = m[5] = m[10] = m[15] = 1.0;

   m[0] = c;
   m[1] = s;
   m[4] = -s;
   m[5] = c;
}
void make_scale_matrix(GLfloat xs, GLfloat ys, GLfloat zs, GLfloat *m)
{
   int i;
   for (i = 0; i < 16; i++)
      m[i] = 0.0;
   m[0] = xs;
   m[5] = ys;
   m[10] = zs;
   m[15] = 1.0;
}
void mul_matrix(GLfloat *prod, const GLfloat *a, const GLfloat *b)
{
#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define P(row,col)  p[(col<<2)+row]
   GLfloat p[16];
   GLint i;
   for (i = 0; i < 4; i++) {
      const GLfloat ai0=A(i,0),  ai1=A(i,1),  ai2=A(i,2),  ai3=A(i,3);
      P(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
      P(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
      P(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
      P(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
   }
   memcpy(prod, p, sizeof(p));
#undef A
#undef B
#undef PROD
}
void draw(void)
{
   static const GLfloat verts[3][2] = {
      { -1, -1 },
      {  1, -1 },
      {  0,  1 }
   };
   static const GLfloat colors[3][3] = {
      { 1, 0, 0 },
      { 0, 1, 0 },
      { 0, 0, 1 }
   };
   GLfloat mat[16], rot[16], scale[16];

   /* Set modelview/projection matrix */
   make_z_rot_matrix(view_rotx, rot);
   make_scale_matrix(0.5, 0.5, 0.5, scale);
   mul_matrix(mat, rot, scale);
	 glUniformMatrix4fv(u_matrix, 1, GL_FALSE, mat);

   glClearColor(0.4, 0.4, 0.4, 0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	 glTexImage2D( target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	 glVertexAttribPointer(attr_pos, 2, GL_FLOAT, GL_FALSE, 0, verts);
	 glVertexAttribPointer(attr_color, 3, GL_FLOAT, GL_FALSE, 0, colors);
	 glEnableVertexAttribArray(attr_pos);
	 glEnableVertexAttribArray(attr_color);

	 glDrawArrays(GL_TRIANGLES, 0, 3);

	 glDisableVertexAttribArray(attr_pos);
	 glDisableVertexAttribArray(attr_color);


}
void create_shaders(void)
{
   static const char *fragShaderText =
      "varying vec4 v_color;\n"
      "void main() {\n"
      "   gl_FragColor = v_color;\n"
      "}\n";
   static const char *vertShaderText =
      "uniform mat4 modelviewProjection;\n"
      "attribute vec4 pos;\n"
      "attribute vec4 color;\n"
      "varying vec4 v_color;\n"
      "void main() {\n"
      "   gl_Position = modelviewProjection * pos;\n"
      "   v_color = color;\n"
      "}\n";

   GLuint fragShader, vertShader, program;
   GLint stat;

   fragShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragShader, 1, (const char **) &fragShaderText, NULL);
   glCompileShader(fragShader);
   glGetShaderiv(fragShader, GL_COMPILE_STATUS, &stat);
   if (!stat) {
      printf("Error: fragment shader did not compile!\n");
      exit(1);
   }

   vertShader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertShader, 1, (const char **) &vertShaderText, NULL);
   glCompileShader(vertShader);
   glGetShaderiv(vertShader, GL_COMPILE_STATUS, &stat);
   if (!stat) {
      printf("Error: vertex shader did not compile!\n");
      exit(1);
   }

   program = glCreateProgram();
   glAttachShader(program, fragShader);
   glAttachShader(program, vertShader);
   glLinkProgram(program);

   glGetProgramiv(program, GL_LINK_STATUS, &stat);
   if (!stat) {
      char log[1000];
      GLsizei len;
      glGetProgramInfoLog(program, 1000, &len, log);
      printf("Error: linking:\n%s\n", log);
      exit(1);
   }

   glUseProgram(program);

   if (1) {
      /* test setting attrib locations */
      glBindAttribLocation(program, attr_pos, "pos");
      glBindAttribLocation(program, attr_color, "color");
      glLinkProgram(program);  /* needed to put attribs into effect */
   }
   else {
      /* test automatic attrib locations */
      attr_pos = glGetAttribLocation(program, "pos");
      attr_color = glGetAttribLocation(program, "color");
   }

   u_matrix = glGetUniformLocation(program, "modelviewProjection");
   printf("Uniform modelviewProjection at %d\n", u_matrix);
   printf("Attrib pos at %d\n", attr_pos);
   printf("Attrib color at %d\n", attr_color);
}
void init(void)
{
   typedef void (*proc)();

#if 1 /* test code */
   proc p = eglGetProcAddress("glMapBufferOES");
   assert(p);
#endif

   glClearColor(0.4, 0.4, 0.4, 0.0);

   create_shaders();
}

int load_dri()
{
	void *dlo;
#include <dlfcn.h>
	/*
#include <GL/internal/dri_interface.h>
	dlo=dlopen("/usr/lib/i386-linux-gnu/dri/i915_dri.so", RTLD_NOW);
	assert(dlo!=NULL);
__DRIextension *driext[] = dlsym(dlo, "__driDriverExtensions");
assert(driext!=NULL);
*/
	return 0; 
}

int main(void)
{
	xcb_connection_t    *c;
	xcb_screen_t        *s;
	xcb_window_t         w,w2;
	xcb_gcontext_t       g;
	xcb_generic_event_t *e;
	uint32_t             mask;
	uint32_t             values[2];
	int                  done = 0;
	xcb_rectangle_t      r = { 20, 20, 60, 60 }; //rect
	xcb_rectangle_t      r1 = { 20, 20, 200, 200 }; //window
	int ret;

	printf("=====================================\n");
	/* open connection with the server */
	c = xcb_connect(NULL,NULL);
	if (xcb_connection_has_error(c)) {
		printf("Cannot open display\n");
		exit(1);
	}
	/* get the first screen */
	s = xcb_setup_roots_iterator( xcb_get_setup(c) ).data;

	/* create black graphics context */

	/* create window */
	w = xcb_generate_id(c);
	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = s->white_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
	printf(" root_depth= %x\n", s->root_depth);
	xcb_create_window(c, s->root_depth, w, s->root,
//			10, 10, 100, 100, 1,
			r1.x, r1.y, r1.width, r1.height, 1,
			XCB_WINDOW_CLASS_INPUT_OUTPUT, s->root_visual,
			mask, values);

	/* map (show) the window */
	xcb_map_window(c, w); 	xcb_flush(c);

#if 1
	//xcb_dri2 
#include <xcb/dri2.h>
#include <drm/drm.h>
#include <drm/i915_drm.h>

	xcb_dri2_query_version_cookie_t query_version_cookie;
	xcb_dri2_query_version_reply_t *query_version_reply;
	xcb_dri2_connect_cookie_t connect_cookie;
	xcb_dri2_connect_reply_t *connect_reply;
	xcb_dri2_authenticate_cookie_t auth_cookie;
	xcb_dri2_authenticate_reply_t *auth_reply;

	query_version_cookie = xcb_dri2_query_version(c, 1, 0);
	connect_cookie = xcb_dri2_connect(c, s->root, XCB_DRI2_DRIVER_TYPE_DRI);

	query_version_reply =
		xcb_dri2_query_version_reply(c, query_version_cookie, NULL);
	connect_reply = xcb_dri2_connect_reply(c, connect_cookie, NULL);

	if (!query_version_reply) {
		perror("DRI2 required");
		return -1;
	}

	//xcb_dri2_connect_device_name_length(connect_reply);
	drm_magic_t  magic;
	char *device_name = xcb_dri2_connect_device_name(connect_reply);
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
	free(query_version_reply);
	free(connect_reply);

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

	 int count = 8;
	 int attachments[8];

	 load_dri();

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
   attachments[0] = XCB_DRI2_ATTACHMENT_BUFFER_BACK_LEFT;
   count=1;
   cookie = xcb_dri2_get_buffers_unchecked (c,w,
					    count, count, attachments);
	 assert(cookie.sequence!=0);
   reply = xcb_dri2_get_buffers_reply (c, cookie, NULL);
#if 1
   perror("x");
	 assert(reply!=NULL);
   buffers = xcb_dri2_get_buffers_buffers (reply);
   assert(buffers != NULL);

	 printf(" dri: reply->count= %d\n", reply->count);
	 printf(" (%d x %d) \n", reply->width, reply->height);


   xcb_dri2_dri2_buffer_t b = buffers[0];
      printf(" buffer[%d] attachment=%d name=%d pitch=$d cpp=%d flags=%x\n", b.attachment,
      		b.name, b.pitch, b.cpp, b.flags);

   free(reply);
#endif
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
#endif

  // geneate egl surface
  /* dri2_surf->drawable is only a xid 
     XID represend a drawable target in Xserver
     xcb_create_pixmap(conn, size, xid, root, width, height)
     */


	w2 = xcb_generate_id(c); 	xcb_flush(c);
	printf("=====================================\n");

#if USE_GL
	///init gl context here

	EGLDisplay dpy;
	EGLSurface surf;
	EGLContext ctx;
	EGLConfig  conf;
	EGLImageKHR  image;
	EGLint     color;
	EGLint   attribs[]={
//		EGL_RED_SIZE, 8,
//		EGL_GREEN_SIZE, 8,
//		EGL_BLUE_SIZE, 8,
//		EGL_DEPTH_SIZE, 1,
//		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};
	const EGLint ctx_attribs[] = {
      EGL_CONTEXT_CLIENT_VERSION, 2,   //? why must set  ctx to version 2
      EGL_NONE
   };

	int num_conf,major,minor;

#define PR(x)	printf("=> Point=%p @(%s:%d) \n", (void*)x, __FUNCTION__, __LINE__)
	if(1){
		eglBindAPI(EGL_OPENGL_ES_API);
	}		else{
		eglBindAPI(EGL_OPENGL_API);
	}
	dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	assert(dpy!=NULL);
	ret = eglInitialize(dpy, &major, &minor);
	PR(ret);
	assert(ret!=0);
	ret = eglChooseConfig(dpy, attribs, &conf, 1, &num_conf);
	printf("EGLConfig get(%d)\n",  num_conf);
	assert(ret!=0);
	ctx = eglCreateContext(dpy, conf, EGL_NO_CONTEXT, ctx_attribs);
	assert(ctx!=0);

	printf("create EGLSurface with xid(0x%x)\n", w);
	surf = eglCreateWindowSurface(dpy, conf, w,  attribs);
	if (surf == EGL_NO_SURFACE) {
		printf("eglCreateWindowSurface() failed\n");
		int eglerr = eglGetError();
		printf("EGLError(%x)\n",  eglerr);
	}

	ret = eglMakeCurrent(dpy, surf,surf, ctx);

	char image_buf[512];
	image = (EGLImageKHR)eglCreateImageKHR(dpy, ctx, EGL_GL_RENDERBUFFER_KHR,
			 (EGLClientBuffer) 512, (int *)image_buf);
#endif //GL
pthread_key_t key = 0;
    while(key<8){
			pthread_key_create(&key, key_exit);
		}

pthread_key_create(&key_c, key_exit);
pthread_key_create(&key_win, key_exit);
pthread_key_create(&key_depth, key_exit);
pthread_setspecific(key_c, (void*)c);
pthread_setspecific(key_win, (void*)w);
pthread_setspecific(key_depth, (void*)(long)s->root_depth);
pthread_setspecific(5, (void*)(long)dpy);
pthread_setspecific(6, (void*)(long)surf);
if(0){
int api =0 ; 
int attri=EGL_CONTEXT_CLIENT_VERSION;
int value;
int idx=EGL_CLIENT_APIS;
const char *str ;
api=  eglQueryAPI();
eglQueryContext(dpy, ctx, attri, &value);
str = eglQueryString(dpy,     idx);
attri = EGL_RED_SIZE;
eglQuerySurface(dpy, surf, attri, &value);
printf("EGL_Client_APIS=%s\n", str);
}


  create_shaders();
	reshape(r1.width, r1.height);
	do_draw();

	//=================
	/* event loop */
	while (!done && (e = xcb_wait_for_event(c))) {
		switch (e->response_type & ~0x80) {
			case XCB_EXPOSE:    /* draw or redraw the window */
				do_draw();
				xcb_flush(c);
				break;
      case ConfigureNotify:
				{
					struct xcb_configure_notify_event_t *ep =(struct xcb_configure_notify_event_t*) e;
         reshape(ep->width, ep->height);
         break;
				}
			case XCB_KEY_PRESS:  /* exit on key press */
				{
					struct xcb_key_press_event_t  *ep=(struct xcb_key_press_event_t*) e;
					printf("key=%d\n", (char)ep->detail);
					int code =  ep->detail;
					if(ep->detail == 9)
        		done=1;
					else 
						done = 0 ;
					// update degree
         if(1){
#undef  XK_Left
#undef  XK_Right
#undef  XK_Up
#undef  XK_Down
#define XK_Left  100
#define XK_Right 102
#define XK_Up  104
#define XK_Down  98
            if (code == XK_Left) {
               view_roty += 5.0;
            }
            else if (code == XK_Right) {
               view_roty -= 5.0;
            }
            else if (code == XK_Up) {
               view_rotx += 5.0;
            }
            else if (code == XK_Down) {
               view_rotx -= 5.0;
            }
         }
					do_draw();
          break;
				}
			case XCB_MOTION_NOTIFY:
				printf("Event=>(0x%x)\n", e->response_type);
				break;
			default:
				printf("Event=>(0x%x)\n", e->response_type);
				break;
		}
		free(e);
	}
	/* close connection to server */
	xcb_disconnect(c);


#if USE_GL
	//===== destory EGL context,  not needed offten
	eglDestroySurface(dpy,surf);
	eglDestroyContext(dpy,ctx);
	eglTerminate(dpy);
#endif

	return 0;
}

