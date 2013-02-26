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

#define USE_GL 1
#define USE_SHADE 0
#define GLES 0

#define PR(x)	printf("=> Point=%p @(%s:%d) \n", (void*)x, __FUNCTION__, __LINE__)
#define GE(x)  printf("  GLerr=> %d\n", glGetError());

pthread_key_t key_c, key_win,key_depth;
	int psize;
	static char *pbuf = 0 ;
	GLint tex =1 ;
	GLint vbo =1 ;

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

   glViewport(10, 10, (GLint) width/2, (GLint) height/2);

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
static GLint attr_test;
#define M_PI  3.14
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
#if USE_SHADE
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

	glClearColor(0.4, 0.4, 0.4, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat v1[ ]= { 0,0,0,   1,0,0,    0.5,1,0 }; 
	GLfloat c2[ ]= { 1,0,0,  0,1,0,  0,0,1,   0,1,1,      1,1,0 };
	GLfloat v2[ ]= { -1,-1,0,  1,-1,0,  10,0,0,   1,0,0,    0.5,1,0 }; 

	glUniformMatrix4fv(u_matrix, 1, GL_FALSE, mat);

	glVertexAttribPointer(attr_pos, 2, GL_FLOAT, GL_FALSE, 0, v2); 
	glVertexAttribPointer(attr_pos, 2, GL_FLOAT, GL_FALSE, 0, verts);// will override previous input
	glVertexAttribPointer(attr_color, 3, GL_FLOAT, GL_FALSE, 0, colors);
	glEnableVertexAttribArray(attr_pos);
	glEnableVertexAttribArray(attr_color);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(attr_pos);
	glDisableVertexAttribArray(attr_color);
#endif

#if USE_GL_NO
	memset(pbuf, 0x88, 100*100);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 100, 100, 0, GL_RGBA, GL_UNSIGNED_BYTE, pbuf);

	glClear( GL_COLOR_BUFFER_BIT ); 
	glColor4f( 1, 0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glTexCoord2i(0, 1); glVertex2i(-56, -56);
	glTexCoord2i(0, 0); glVertex2i(-56, 56);
	glTexCoord2i(1, 0); glVertex2i(56, 56);
	glTexCoord2i(1, 1); glVertex2i(56, -56);
	glEnd();
	glFlush(); //? need this ?

	return ;
#endif


#if USE_GL_ARRAY
	GLfloat q3[] = {  -1,-1,	 1,-1,		 1,1,			 -1,1	 };
	GLshort q1[] = {  -1,-1,	 10,-1,		 1,1,			 -1,1	 };
	GLfloat c3[] = {  1,0,0,0,	 0,1,0,1,		 0,0,1,1,    1,0,0,1	};
	GLfloat tex1[] = {	 0,0,	 1,0,		 1,1,			 0,1		 };
	GLshort tex2[] = {	 0,0,	 1,0,		 1,1,			 0,1		 };

	// Bind the texture object
	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, tex );
	//these data have alpha channel.
	memset(pbuf, 0x80, 100*100*4); 
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 100, 100, 0, GL_RGBA, GL_UNSIGNED_BYTE, pbuf);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	//glEnable(GL_BLEND);
	glEnableClientState(GL_VERTEX_ARRAY);
	//glVertexPointer(2, GL_FLOAT, 0, q3);
	glVertexPointer(2, GL_SHORT, 0, q1);

	if(0){
		glColor4f( 1, 0, 0, 0);
	}else {
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4,GL_FLOAT, 0, c3);
	}

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_SHORT, 0, tex2);
	//glTexCoordPointer(2, GL_FLOAT, 0, tex1);

	int t = GL_POINTS; //GL_TRIANGLE_STRIP
	t = GL_QUADS;
	//GL_POINTS	GL_LINES	GL_LINE_LOOP		GL_LINE_STRIP		GL_TRIANGLES	GL_TRIANGLE_STRIP
	//TRIANGLE_FAN  QUADS QUAD_STRIP  POLYGON
	// only QUADS, POLYGON makes texure shown properly

	//glDrawArrays(GL_TRIANGLES,0,3);
	//glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glDrawArrays( t , 0, 4 );
	glDisableClientState(GL_VERTEX_ARRAY);
#endif

//#if USE_VBO_DRAW
#if 1
	GLfloat q3[] = {  -1,-1,	 1,-1,		 1,1,			 -1,1	 };
	GLshort q1[] = {   -1,-1,	 0,-1,		 0,1,			 -1,1,	  -1,-1,	 10,-1,		 1,1,			 -1,1,
  -1,-1,	 1,-1,		 1,1,			 -1,1,	  -1,-1,	 10,-1,		 1,1,			 -1,1,
  -1,-1,	 1,-1,		 1,1,			 -1,1,	  -1,-1,	 10,-1,		 1,1,			 -1,1
	};
	glColor4f( 1, 0, 0, 0);

	//GL_ARRAY_BUFFER,  GL_ELEMENT_ARRAY_BUFFER 两种buffer类型
	glBindBuffer(GL_ARRAY_BUFFER, vbo); 
	//提示数据是否变化，不变，几帧变一次，一帧变一次，
	glBufferData(GL_ARRAY_BUFFER, 4*24, q1, GL_STATIC_DRAW_ARB); //usage=static/dynamic/stream=


	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_SHORT, 0, NULL);

	if(1){
		glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
	}else {
	int indices[4] = { 0, 4, 8, 12, 16};
	glDrawElements(GL_TRIANGLE_FAN, 2, GL_UNSIGNED_SHORT, 0);
	}
#endif


}
void special_effort()
{
    glOrtho(100,100,-100,100, 0.0, 1.0);
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, 100,100);
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
      "attribute vec4 test;\n"
      "attribute vec4 color;\n"
      "varying vec4 v_color;\n"
      "void main() {\n"
      //"   gl_Position = modelviewProjection * pos;\n"
      "   gl_Position = pos;\n"
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
      glBindAttribLocation(program, attr_color, "test");
      glLinkProgram(program);  /* needed to put attribs into effect */
   }
   else {
      /* test automatic attrib locations */
      attr_pos = glGetAttribLocation(program, "pos");
      attr_color = glGetAttribLocation(program, "color");
      attr_test= glGetAttribLocation(program, "test");
   }

   u_matrix = glGetUniformLocation(program, "modelviewProjection");
   printf("Uniform modelviewProjection at %d\n", u_matrix);
   printf("Attrib pos at %d\n", attr_pos);
   printf("Attrib color at %d\n", attr_color);
   printf("Attrib test at %d\n", attr_test);
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
	xcb_map_window(c, w);

  // geneate egl surface
  /* dri2_surf->drawable is only a xid 
     XID represend a drawable target in Xserver
     xcb_create_pixmap(conn, size, xid, root, width, height)
     */

	xcb_map_window(c, w); 	xcb_flush(c);

	w2 = xcb_generate_id(c);
	xcb_flush(c);

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
#if GLES
      EGL_CONTEXT_CLIENT_VERSION, 2,   //? why must set  ctx to version 2
#endif
      EGL_NONE
   };

	int num_conf,major,minor,ret;

#if GLES
		eglBindAPI(EGL_OPENGL_ES_API);
#else
		eglBindAPI(EGL_OPENGL_API);
#endif

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

	GE(x);
	GE(x);
	glGenTextures( 1, &tex );
	GE(x);
	glGenBuffers(1, &vbo);
	printf("gen Buffers (%d)\n", vbo);
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
if(1){
int api =0 ; 
int attri=EGL_CONTEXT_CLIENT_VERSION;
int value;
int idx=EGL_CLIENT_APIS;
char *str ;

setenv("LIBGL_DEBUG", "verbose",1);


api=  eglQueryAPI();
eglQueryContext(dpy, ctx, attri, &value);
attri = EGL_RED_SIZE;
eglQuerySurface(dpy, surf, attri, &value);

str = eglQueryString(dpy,     idx);
printf("EGL_Client_APIS=%s\n", str);

idx = EGL_VERSION;
str = eglQueryString(dpy,     idx);
printf("EGL_VERSION=%s\n", str);
}
	//dump();

#if USE_SHADE
  create_shaders();
#endif
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

void dump()
{
   EGLint i;
   EGLConfig *configs;
   EGLint cfgnum;
	 EGLDisplay d;

   configs = malloc((sizeof(EGLConfig) *200));
   cfgnum =200;

	 d = pthread_getspecific(5);
   eglGetConfigs(d, configs, cfgnum, &cfgnum);
   printf("Configurations(%d):\n", cfgnum);
   printf("     bf lv d st colorbuffer dp st   supported \n");
   printf("  id sz  l b ro  r  g  b  a th cl   surfaces  \n");
   printf("----------------------------------------------\n");
   for (i = 0; i < cfgnum; i++) {
      EGLint id, size, level;
      EGLint red, green, blue, alpha;
      EGLint depth, stencil;
      EGLint surfaces;
      EGLint doubleBuf = 1, stereo = 0;
      char surfString[100] = "";

      eglGetConfigAttrib(d, configs[i], EGL_CONFIG_ID, &id);
      eglGetConfigAttrib(d, configs[i], EGL_BUFFER_SIZE, &size);
      eglGetConfigAttrib(d, configs[i], EGL_LEVEL, &level);

      eglGetConfigAttrib(d, configs[i], EGL_RED_SIZE, &red);
      eglGetConfigAttrib(d, configs[i], EGL_GREEN_SIZE, &green);
      eglGetConfigAttrib(d, configs[i], EGL_BLUE_SIZE, &blue);
      eglGetConfigAttrib(d, configs[i], EGL_ALPHA_SIZE, &alpha);
      eglGetConfigAttrib(d, configs[i], EGL_DEPTH_SIZE, &depth);
      eglGetConfigAttrib(d, configs[i], EGL_STENCIL_SIZE, &stencil);
      eglGetConfigAttrib(d, configs[i], EGL_SURFACE_TYPE, &surfaces);

      if (surfaces & EGL_WINDOW_BIT)
         strcat(surfString, "win,");
      if (surfaces & EGL_PBUFFER_BIT)
         strcat(surfString, "pb,");
      if (surfaces & EGL_PIXMAP_BIT)
         strcat(surfString, "pix,");
      if (strlen(surfString) > 0)
         surfString[strlen(surfString) - 1] = 0;

      printf("0x%02x %2d %2d %c  %c %2d %2d %2d %2d %2d %2d   %-12s\n",
             id, size, level,
             doubleBuf ? 'y' : '.',
             stereo ? 'y' : '.',
             red, green, blue, alpha,
             depth, stencil, surfString);
   }

	int wwidth;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &wwidth);
	printf("get texture width %d\n", wwidth);

}
void setview()
{
	glViewport(0, 0, 100,100);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-100, 100, -1.0, 1.0, 1.5, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -2.5);

	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(0.7, 0.7, 0.7);
	glPopMatrix();

   static const GLfloat pos[4] = { 5.0, 5.0, 10.0, 0.0 };
   glLightfv(GL_LIGHT0, GL_POSITION, pos);
   glEnable(GL_CULL_FACE);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_NORMALIZE);

}
