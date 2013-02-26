/*
 * Copyright (C) 1999-2001  Brian Paul   All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * Ported to X/EGL/GLES.   XXX Actually, uses full OpenGL ATM.
 * Brian Paul
 * 30 May 2008
 */

/*
 * Command line options:
 *    -info      print GL implementation information
 *
 */


#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#include <EGL/egl.h>

#include <EGL/eglext.h>


#define BENCHMARK

#ifdef BENCHMARK

/* XXX this probably isn't very portable */

#include <sys/time.h>
#include <unistd.h>

/* return current time (in seconds) */
static double
current_time(void)
{
   struct timeval tv;
#ifdef __VMS
   (void) gettimeofday(&tv, NULL );
#else
   struct timezone tz;
   (void) gettimeofday(&tv, &tz);
#endif
   return (double) tv.tv_sec + tv.tv_usec / 1000000.0;
}

#else /*BENCHMARK*/

/* dummy */
static double
current_time(void)
{
   /* update this function for other platforms! */
   static double t = 0.0;
   static int warn = 1;
   if (warn) {
      fprintf(stderr, "Warning: current_time() not implemented!!\n");
      warn = 0;
   }
   return t += 1.0;
}

#endif /*BENCHMARK*/



#ifndef M_PI
#define M_PI 3.14159265
#endif


static GLfloat view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
static GLint gear1, gear2, gear3;
static GLfloat angle = 0.0;

/*
 *
 *  Draw a gear wheel.  You'll probably want to call this function when
 *  building a display list since we do a lot of trig here.
 * 
 *  Input:  inner_radius - radius of hole at center
 *          outer_radius - radius at center of teeth
 *          width - width of gear
 *          teeth - number of teeth
 *          tooth_depth - depth of tooth
 */
void draw_img();
static void
draw(void)
{
	draw_img();
	return ;

   glClearColor(0.2, 0.2, 0.2, 0.2);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}


/* new window size or exposure */
static void
reshape(int width, int height)
{
   GLfloat ar = (GLfloat) width / (GLfloat) height;

   glViewport(0, 0, (GLint) width, (GLint) height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-ar, ar, -1, 1, 5.0, 60.0);
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.0, 0.0, -40.0);
}
   


struct egl_manager {
   EGLNativeDisplayType xdpy;
   EGLNativeWindowType xwin;
   EGLNativePixmapType xpix;

   EGLDisplay dpy;
   EGLConfig conf;
   EGLContext ctx;

   EGLSurface win;
   EGLSurface pix;
   EGLSurface pbuf;
   EGLImageKHR image;

   EGLBoolean verbose;
   EGLint major, minor;

   GC gc;
   GLuint fbo;
};

static struct egl_manager *
egl_manager_new(EGLNativeDisplayType xdpy, const EGLint *attrib_list,
                EGLBoolean verbose)
{
   struct egl_manager *eman;
   const char *ver;
   EGLint num_conf;

   eman = calloc(1, sizeof(*eman));
   if (!eman)
      return NULL;

   eman->verbose = verbose;
   eman->xdpy = xdpy;

   eman->dpy = eglGetDisplay(eman->xdpy);
   if (eman->dpy == EGL_NO_DISPLAY) {
      printf("eglGetDisplay() failed\n");
      free(eman);
      return NULL;
   }

   if (!eglInitialize(eman->dpy, &eman->major, &eman->minor)) {
      printf("eglInitialize() failed\n");
      free(eman);
      return NULL;
   }

   ver = eglQueryString(eman->dpy, EGL_VERSION);
   printf("EGL_VERSION = %s\n", ver);

   if (!eglChooseConfig(eman->dpy, attrib_list, &eman->conf, 1, &num_conf) ||
       !num_conf) {
      printf("eglChooseConfig() failed\n");
      eglTerminate(eman->dpy);
      free(eman);
      return NULL;
   }

   eman->ctx = eglCreateContext(eman->dpy, eman->conf, EGL_NO_CONTEXT, NULL);
   if (eman->ctx == EGL_NO_CONTEXT) {
      printf("eglCreateContext() failed\n");
      eglTerminate(eman->dpy);
      free(eman);
      return NULL;
   }

   return eman;
}

static EGLBoolean
egl_manager_create_window(struct egl_manager *eman, const char *name,
                          EGLint w, EGLint h, EGLBoolean need_surface,
                          EGLBoolean fullscreen, const EGLint *attrib_list)
{
   XVisualInfo vinfo_template, *vinfo = NULL;
   EGLint val, num_vinfo;
   Window root;
   XSetWindowAttributes attrs;
   unsigned long mask;
   EGLint x = 0, y = 0;

   if (!eglGetConfigAttrib(eman->dpy, eman->conf,
                           EGL_NATIVE_VISUAL_ID, &val)) {
      printf("eglGetConfigAttrib() failed\n");
      return EGL_FALSE;
   }
   if (val) {
      vinfo_template.visualid = (VisualID) val;
      vinfo = XGetVisualInfo(eman->xdpy, VisualIDMask, &vinfo_template, &num_vinfo);
   }
   /* try harder if window surface is not needed */
   if (!vinfo && !need_surface &&
       eglGetConfigAttrib(eman->dpy, eman->conf, EGL_BUFFER_SIZE, &val)) {
      if (val == 32)
         val = 24;
      vinfo_template.depth = val;
      vinfo = XGetVisualInfo(eman->xdpy, VisualDepthMask, &vinfo_template, &num_vinfo);
   }

   if (!vinfo) {
      printf("XGetVisualInfo() failed\n");
      return EGL_FALSE;
   }

   root = DefaultRootWindow(eman->xdpy);
   if (fullscreen) {
      x = y = 0;
      w = DisplayWidth(eman->xdpy, DefaultScreen(eman->xdpy));
      h = DisplayHeight(eman->xdpy, DefaultScreen(eman->xdpy));
   }

   /* window attributes */
   attrs.background_pixel = 0;
   attrs.border_pixel = 0;
   attrs.colormap = XCreateColormap(eman->xdpy, root, vinfo->visual, AllocNone);
   attrs.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
   attrs.override_redirect = fullscreen;
   mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect;

   eman->xwin = XCreateWindow(eman->xdpy, root, x, y, w, h,
                              0, vinfo->depth, InputOutput,
                              vinfo->visual, mask, &attrs);
   XFree(vinfo);

   /* set hints and properties */
   {
      XSizeHints sizehints;
      sizehints.x = x;
      sizehints.y = y;
      sizehints.width  = w;
      sizehints.height = h;
      sizehints.flags = USSize | USPosition;
      XSetNormalHints(eman->xdpy, eman->xwin, &sizehints);
      XSetStandardProperties(eman->xdpy, eman->xwin, name, name,
                             None, (char **)NULL, 0, &sizehints);
   }

   if (need_surface) {
      eman->win = eglCreateWindowSurface(eman->dpy, eman->conf,
                                         eman->xwin, attrib_list);
      if (eman->win == EGL_NO_SURFACE) {
         printf("eglCreateWindowSurface() failed\n");
         XDestroyWindow(eman->xdpy, eman->xwin);
         eman->xwin = None;
         return EGL_FALSE;
      }
   }

   eman->gc = XCreateGC(eman->xdpy, eman->xwin, 0, NULL);

   XMapWindow(eman->xdpy, eman->xwin);

   return EGL_TRUE;
}

static EGLBoolean
egl_manager_create_pixmap(struct egl_manager *eman, EGLNativeWindowType xwin,
                          EGLBoolean need_surface, const EGLint *attrib_list)
{
   XWindowAttributes attrs;

   if (!XGetWindowAttributes(eman->xdpy, xwin, &attrs)) {
      printf("XGetWindowAttributes() failed\n");
      return EGL_FALSE;
   }

   eman->xpix = XCreatePixmap(eman->xdpy, xwin,
                              attrs.width, attrs.height, attrs.depth);

   if (need_surface) {
      eman->pix = eglCreatePixmapSurface(eman->dpy, eman->conf,
                                         eman->xpix, attrib_list);
      if (eman->pix == EGL_NO_SURFACE) {
         printf("eglCreatePixmapSurface() failed\n");
         XFreePixmap(eman->xdpy, eman->xpix);
         eman->xpix = None;
         return EGL_FALSE;
      }
   }

   return EGL_TRUE;
}

static EGLBoolean
egl_manager_create_pbuffer(struct egl_manager *eman, const EGLint *attrib_list)
{
   eman->pbuf = eglCreatePbufferSurface(eman->dpy, eman->conf, attrib_list);
   if (eman->pbuf == EGL_NO_SURFACE) {
      printf("eglCreatePbufferSurface() failed\n");
      return EGL_FALSE;
   }

   return EGL_TRUE;
}

static void
egl_manager_destroy(struct egl_manager *eman)
{
   eglMakeCurrent(eman->dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
   eglTerminate(eman->dpy);

   if (eman->xwin != None)
      XDestroyWindow(eman->xdpy, eman->xwin);
   if (eman->xpix != None)
      XFreePixmap(eman->xdpy, eman->xpix);

   XFreeGC(eman->xdpy, eman->gc);

   free(eman);
}

enum {
   GEARS_WINDOW,
   GEARS_PIXMAP,
   GEARS_PIXMAP_TEXTURE,
   GEARS_PBUFFER,
   GEARS_PBUFFER_TEXTURE,
   GEARS_RENDERBUFFER
};

static void
event_loop(struct egl_manager *eman, EGLint surface_type, EGLint w, EGLint h)
{
   int window_w = w, window_h = h;

	 while (1) {
//		 while (XPending(eman->xdpy) > 0) {
			 XEvent event;
			 XNextEvent(eman->xdpy, &event);
			 printf("dispatch event\n");
			 switch (event.type) {
				 case Expose:
					 /* we'll redraw below */
					 break;
				 case ConfigureNotify:
					 window_w = event.xconfigure.width;
					 window_h = event.xconfigure.height;
					 if (surface_type == EGL_WINDOW_BIT)
						 reshape(window_w, window_h);
					 break;
				 case KeyPress:
					 {
						 char buffer[10];
						 int r, code;
						 code = XLookupKeysym(&event.xkey, 0);
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
						 else {
							 r = XLookupString(&event.xkey, buffer, sizeof(buffer),
									 NULL, NULL);
							 if (buffer[0] == 27) {
								 /* escape */
								 return;
							 }
						 }
					 }
			 }
		 //}

		 {
			 static int frames = 0;
			 static double tRot0 = -1.0, tRate0 = -1.0;
			 double dt, t = current_time();
			 if (tRot0 < 0.0)
				 tRot0 = t;
			 dt = t - tRot0;
			 tRot0 = t;

			 /* advance rotation for next frame */
			 angle += 70.0 * dt;  /* 70 degrees per second */
			 if (angle > 3600.0)
				 angle -= 3600.0;

			 draw();
			 eglSwapBuffers(eman->dpy, eman->win);
			 frames++;

			 if (tRate0 < 0.0)
				 tRate0 = t;
			 if (t - tRate0 >= 5.0) {
				 GLfloat seconds = t - tRate0;
				 GLfloat fps = frames / seconds;
				 printf("%d frames in %3.1f seconds = %6.3f FPS\n", frames, seconds,
						 fps);
				 tRate0 = t;
				 frames = 0;
			 }
		 }
	 }
}


static void
usage(void)
{
   printf("Usage:\n");
   printf("  -display <displayname>  set the display to run on\n");
   printf("  -fullscreen             run in fullscreen mode\n");
   printf("  -info                   display OpenGL renderer info\n");
   printf("  -pixmap                 use pixmap surface\n");
   printf("  -pixmap-texture         use pixmap surface and texture using EGLImage\n");
   printf("  -pbuffer                use pbuffer surface and eglCopyBuffers\n");
   printf("  -pbuffer-texture        use pbuffer surface and eglBindTexImage\n");
   printf("  -renderbuffer           renderbuffer as EGLImage and bind as texture from\n");
}
 
static const char *names[] = {
   "window",
   "pixmap",
   "pixmap_texture",
   "pbuffer",
   "pbuffer_texture",
   "renderbuffer"
};

int
init_sc()
//main(int argc, char *argv[])
{
   const int winWidth = 300, winHeight = 300;
   Display *x_dpy;
   char *dpyName = NULL;
   struct egl_manager *eman;
   EGLint attribs[] = {
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT, /* may be changed later */
      EGL_RED_SIZE, 1,
      EGL_GREEN_SIZE, 1,
      EGL_BLUE_SIZE, 1,
      EGL_DEPTH_SIZE, 1,
      EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
      EGL_NONE
   };
   char win_title[] = "xeglgears (window/pixmap/pbuffer)";
   EGLint surface_type = GEARS_WINDOW;
   GLboolean printInfo = GL_FALSE;
   GLboolean fullscreen = GL_FALSE;
   EGLBoolean ret;
   GLuint texture, color_rb, depth_rb;
   int i;

   x_dpy = XOpenDisplay(dpyName);
   if (!x_dpy) {
      printf("Error: couldn't open display %s\n",
	     dpyName ? dpyName : getenv("DISPLAY"));
      return -1;
   }

   eglBindAPI(EGL_OPENGL_API);

   eman = egl_manager_new(x_dpy, attribs, printInfo);
   if (!eman) {
      XCloseDisplay(x_dpy);
      return -1;
   }

	 ret = egl_manager_create_window(eman, win_title, winWidth, winHeight,
			 EGL_TRUE, fullscreen, NULL);
	 if (!ret)
		 return -1;

	 ret = eglMakeCurrent(eman->dpy, eman->win, eman->win, eman->ctx);

	 eman->image = eglCreateImageKHR(eman->dpy, eman->ctx,
			 EGL_GL_RENDERBUFFER_KHR,
			 (EGLClientBuffer) color_rb, NULL);

	 if(1){
		 /* Setup texturing */
		 glEnable( GL_TEXTURE_2D );
		 glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );


		 glBindTexture( GL_TEXTURE_2D, 0 );
		 glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		 glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		 glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		 glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	 }

	 event_loop(eman, surface_type, winWidth, winHeight);
   return 0;
}

int exit_x(struct egl_manager *eman) {

	//draw();
	//     eglSwapBuffers(eman->dpy, eman->win);

	egl_manager_destroy(eman);
	XCloseDisplay(eman->xdpy);
	return -1;
}

char Image_tex[1024*4];
int width=30;
int height=30;
void draw_img()
{
	int i,j;
   glClearColor(0.2, 0.2, 0.2, 0.2);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glColor3f(1.0, 1.0, 0.2);

	 if(1){
//	 memset(Image_tex, 0x83, sizeof(Image_tex)/4);
//	 memset(Image_tex, 0xff, 30*30);
	 for (i=0; i<30; i++){
		 for (j=0; j< 30; j++){
			 *((short*)Image_tex+i*30+j)=0x33;
		 }
	 }
   glTexImage2D( GL_TEXTURE_2D, 0, 3, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, Image_tex);
	 }

   glBegin( GL_POLYGON );
	 if(0){
   glTexCoord2f( 0.0, 0.0 );   glVertex2f( -1.0, -1.0 );
   glTexCoord2f( 1.0, 0.0 );   glVertex2f(  1.0, -1.0 );
   glTexCoord2f( 1.0, 1.0 );   glVertex2f(  1.0,  1.0 );
   glTexCoord2f( 0.0, 1.0 );   glVertex2f( -1.0,  1.0 );
	 }
   glTexCoord2f( 0.0, 0.0 );   glVertex2f( 0, 0 );
   glTexCoord2f( 1.0, 0.0 );   glVertex2f(  1.0, 0 );
   glTexCoord2f( 1.0, 1.0 );   glVertex2f(  1.0,  1.0 );
   glTexCoord2f( 0.0, 1.0 );   glVertex2f(0,  1.0 );
   glEnd();
   glPopMatrix();
	 printf("draw");

}
//extern int init_sc();
//extern char *Image_tex=0;
int main()
{
	init_sc();
	return 0;
}
