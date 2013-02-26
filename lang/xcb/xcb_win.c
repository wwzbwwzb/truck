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

struct g_t{
	xcb_connection_t    *conn;
	xcb_screen_t        *s;
	xcb_window_t         w;
	xcb_gcontext_t       g;
	xcb_generic_event_t *e;
	xcb_window_t         root; // set to root render window,and hook it's event.
} G;

void create_dri_drawable()
{
	//creat a pbuffer and render to it.
  //load dri driver and call CreateNewDrawable function

  //xcb_dri2 create drawable
  

}

void draw_gc(xcb_window_t win)
{
	// the only problem to draw bgr24 it  bps=4 but 3, if depth=24,
	/*
	fmt depth	 bps
	32	  32    4
	bgr24 24    4
	16    16    2
	8      8     1
	0      0      0
		 */
	//draw something in parent window for test
	//w = s->root;
	xcb_gcontext_t       g_solid;
	uint32_t             mask;
	uint32_t             values[2];
	xcb_rectangle_t      r = { 0, 0,  10,10 };
	xcb_rectangle_t      r2 = { 20, 20,  100,100 };
	xcb_connection_t    *c = G.conn;

	if(win==0) {
		win=G.root;
		// atach to root will cause  error=Resource temporary unavailable
		return ;
	}
	//xcb_map_window(c, win); xcb_flush(c);
	g_solid = xcb_generate_id(c);
	mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
	values[0] = G.s->white_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
	xcb_create_gc(c, g_solid, win, 0,NULL);
	xcb_change_gc(c, g_solid,  mask, values);
	if(0){ //use gc to  draw  shape
		xcb_poly_fill_rectangle(c, win, g_solid,  1, &r2); xcb_flush(c);
	}
	xcb_free_gc(c,g_solid);

	/*
		 can use the geometry and attribute
		 xcb_get_window_attributes_reply
		 xcb_get_geometry_reply(G.conn, 
		 */
  xcb_get_geometry_reply_t *geo;
  geo = xcb_get_geometry_reply(c, 
      xcb_get_geometry(c, win), NULL);
  printf("get (%hdx%hd)@(%dx%d)\n",  
      geo->x, geo->y, geo->width, geo->height);

	if(1){ //xcb_put_image
		//getimage 
		xcb_get_image_cookie_t cookie;
		xcb_get_image_reply_t *reply;
		xcb_generic_error_t *error;
		int psize;
		xcb_void_cookie_t ck;
		static uint8_t *pbuf = NULL;

		if(geo!=NULL){
			r.width = geo->width -20;
			r.height=geo->height;
			r.x=geo->x+10;
			r.y=geo->y;
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

		if( pbuf == NULL){
			pbuf = malloc(psize);
			printf("malloc pbuf=0x%x size=%d\n",pbuf, psize);
			memset(pbuf, 0x18, psize);
		}

#if 0
		cookie = xcb_get_image (c, XCB_IMAGE_FORMAT_Z_PIXMAP,
				win, r.x, r.y, r.width, r.height, ~0);
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
			memcpy(pbuf, idata, bytes);
			printf("root_depth=%d\n", G.s->root_depth);
			psize=bytes;
		}
		free(reply);
#endif

		//draw image with xcb_put_image
		// creat gc and draw to parent window
		xcb_gcontext_t       g;
		g = xcb_generate_id(c);
		xcb_create_gc(c, g, win, 0,NULL);

		memset(pbuf, 0xf8, psize);
		uint8_t *p2=pbuf;
		//fmt = bgr24
		static int  shift=0;
		shift = shift++;
		shift = shift%4;
		//bgr0
		int i=0;
		int j;
		for( i=0; i<psize; ){
			for(j=0;j<4;j++)
				if(j==shift){
					*(p2+i)= 0x80;		i++; //??
				}
				else{
					*(p2+i)= 0;		i++; //??
				}
		}
		ck = xcb_put_image_checked(c, XCB_IMAGE_FORMAT_Z_PIXMAP,
				win, g,
				r.width, r.height, r.x,r.y,
				0, G.s->root_depth,
				psize, pbuf);
		xcb_flush(c);

#if 0
		//BadMatch=8 BadLength=16
		xcb_generic_error_t *err;
		err = xcb_request_check (c, ck);
		if (err)
		{
			int code = err->error_code;
			free (err);
			printf("put image error %d\n", code);
			assert (code != 0);
		}   
		xcb_free_gc(c, g);
#endif
	}

}

void draw_byxid(xcb_window_t xid)
{
  xcb_connection_t    *c=G.conn;
  xcb_window_t         w=xid;
  xcb_get_geometry_reply_t *geo;
  uint32_t mask;
  uint32_t values[4];

  geo = xcb_get_geometry_reply(G.conn, 
      xcb_get_geometry(G.conn, w), NULL);
  assert(geo != NULL);

  printf("get (%hdx%hd)@(%dx%d)\n",  
      geo->x, geo->y, geo->width, geo->height);
  // try require event handler, and draw something.

  //start create sub window and draw gc in it.
//  getchar();

  /* create sub window */
  xcb_window_t         child;
  child = xcb_generate_id(c);
	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = G.s->black_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
  xcb_void_cookie_t cookie;

   // create colormap if needed , window pixfmt is diff from root window.

/*	xcb_map_window(c, w); xcb_flush(c);   */

  //creat a half size window
  geo->width /=2;
  geo->height /=2;

	cookie = xcb_create_window_checked(c, G.s->root_depth, child, w,
      10, 10, geo->width, geo->height,
      1,
			XCB_WINDOW_CLASS_INPUT_OUTPUT, G.s->root_visual,
			mask, values);

  //check cookie error
  if(1){
    xcb_generic_error_t *err;
    err = xcb_request_check (c, cookie);
    if (err){
      int code = err->error_code;
      free (err);
      printf("X11 error %d", code);
      assert (code != 0);
    }
  }
  xcb_map_window(c, child); xcb_flush(c);

  // xcb_reparent_window(c, child,  0,100,100);  
  // root=0 return no affected.
  xcb_reparent_window(c, child,  G.s->root,100,100);
//  xcb_map_window(c, child);xcb_flush(c);

  // ? why make a pixmap as attribute of new sub window
  xcb_pixmap_t      pixmap;
  xcb_create_pixmap (G.conn, geo->depth, pixmap,
      child, geo->width, geo->height);

  values[1] |= XCB_EVENT_MASK_POINTER_MOTION |
     XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE;
  xcb_change_window_attributes_checked(c, child, mask, values); xcb_flush(c);

//  xcb_poly_fill_rectangle(c, child, g,  1, &r);
  // both map and flush need to show window up
  //getchar();
  //unmap is minimum window
//  xcb_unmap_window(c, child);
// xcb_destroy_window(c, child);

  create_dri_drawable();


  int done =0 ;
  xcb_generic_event_t *e;
  printf("wait event\n");
  while (!done ) {
		//printf("\re=>(x%x)", e->response_type); fflush(stdout);
    e = xcb_wait_for_event(c);
    if(!e)printf("wait event error\n");
    else{
      switch (e->response_type & ~0x80) {
        case XCB_EXPOSE:    /* draw or redraw the window */
          printf("XCB_EXPOSE\n");
          draw_gc(child);
          break;
        case XCB_KEY_PRESS:  /* exit on key press */
          //gwj done = 1;
          break;
      }
      free(e);
    }
  }
  xcb_unmap_window(c, child);
  xcb_destroy_window(c, child);
}

int main(void)
{
	xcb_connection_t    *c;
	xcb_screen_t        *s;
	xcb_window_t         w;
	xcb_gcontext_t       g;
	xcb_generic_event_t *e;
	uint32_t             mask;
	uint32_t             values[2];
	int                  done = 0;

	/* open connection with the server */
	c = xcb_connect(NULL,NULL);
	if (xcb_connection_has_error(c)) {
		printf("Cannot open display\n");
		exit(1);
	}

	/* get the first screen */
	s = xcb_setup_roots_iterator( xcb_get_setup(c) ).data;

  G.conn = c;
  G.s= s;

  //enable mouse event
  xcb_cursor_t cursor;
  xcb_font_t cursor_font=0;
  short glyph = 0x34; //cross
  xcb_grab_pointer_cookie_t grab_cookie;
  xcb_grab_pointer_reply_t *grab_reply;
  xcb_generic_error_t *err;
  cursor = xcb_generate_id (c);
  if (!cursor_font) {
    cursor_font = xcb_generate_id (c);
    xcb_open_font (c, cursor_font, strlen ("cursor"), "cursor");
  }

  xcb_create_glyph_cursor (c, cursor, cursor_font, cursor_font,
      glyph, glyph + 1,
      0, 0, 0, 0xffff, 0xffff, 0xffff);  /* rgb, rgb */

  grab_cookie = xcb_grab_pointer(c, 0, s->root,
  		XCB_EVENT_MASK_POINTER_MOTION |
     XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE,
     XCB_GRAB_MODE_SYNC, XCB_GRAB_MODE_ASYNC,
     s->root, cursor, XCB_TIME_CURRENT_TIME);

  assert(grab_reply!=NULL);
  grab_reply = xcb_grab_pointer_reply (c, grab_cookie, &err);
  if (grab_reply->status != XCB_GRAB_STATUS_SUCCESS)
    printf("Can't grab the mouse.");

  printf("grab pointer cookie=0x%x\n", grab_cookie);
  xcb_allow_events (c, XCB_ALLOW_ASYNC_POINTER, XCB_TIME_CURRENT_TIME);  xcb_flush (c);
//  xcb_allow_events (c, XCB_ALLOW_SYNC_POINTER, XCB_TIME_CURRENT_TIME);  xcb_flush (c);

	while (!done ) {
 e = xcb_wait_for_event(c);
 if(e==NULL) printf("wait event error\n");
		//printf("\revent=>%d", e->response_type);
		switch (e->response_type & ~0x80) {
			case XCB_EXPOSE:    /* draw or redraw the window */
          printf("XCB_EXPOSE\n");
          draw_gc(w);
				break;
			case XCB_MOTION_NOTIFY:
				{
					static xcb_window_t    oldw;
        xcb_button_press_event_t *bp = (xcb_button_press_event_t *)e;
        w = bp->child; /* window selected */
        if(oldw!=w){
					oldw = w;
					printf("windowid = %x\n", w);
				}
				//draw it 
				draw_gc(w);
					}
				break;
      case XCB_BUTTON_PRESS:
        {
        xcb_button_press_event_t *bp = (xcb_button_press_event_t *)e;
        w = bp->child; /* window selected */
        if (w== XCB_WINDOW_NONE){
          printf("window select is root\n");
          w=s->root;
        }
        else{
          printf("window = %x\n", w);
				draw_gc(w);
				}
        done=1; printf("grab a child\n");
        break;
        }
		}
		free(e);
	}

  xcb_ungrab_pointer(c, XCB_TIME_CURRENT_TIME);

  draw_byxid(w);
  return 0;

#if 0
	/* create window */
	w = xcb_generate_id(c);
	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = s->white_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
	xcb_create_window(c, s->root_depth, w, s->root,
			10, 10, 100, 100, 1,
			XCB_WINDOW_CLASS_INPUT_OUTPUT, s->root_visual,
			mask, values);

	/* map (show) the window */
	xcb_map_window(c, w);

	xcb_flush(c);

	/* event loop */
	while (!done && (e = xcb_wait_for_event(c))) {
		switch (e->response_type & ~0x80) {
			case XCB_EXPOSE:    /* draw or redraw the window */
				xcb_poly_fill_rectangle(c, w, g,  1, &r);
				xcb_flush(c);
				break;
			case XCB_KEY_PRESS:  /* exit on key press */
				//gwj done = 1;
				break;
		}
		free(e);
	}
	/* close connection to server */
	xcb_disconnect(c);

	return 0;
  if(0){
  //draw image with xcb_put_image
  // creat gc and draw to parent window
	xcb_gcontext_t       g;
	g = xcb_generate_id(c);
	xcb_create_gc(c, g, child, 0,NULL);
  //xcb_poly_fill_rectangle(c, w, g,  1, &r); xcb_flush(c);

  int psize=geo->width * geo->height* (geo->depth/8);
  uint8_t *pbuf = malloc(psize);
  xcb_void_cookie_t ck;
  printf("malloc pbuf=0x%x size=%d\n",pbuf, psize);
  memset(pbuf, 0x18, psize);

  ck = xcb_put_image_checked(c, XCB_IMAGE_FORMAT_Z_PIXMAP,
      child, g,
      geo->width, geo->height , 0, 0,
       0, geo->depth,
     psize, pbuf);

	//BadMatch=8 BadLength=16
    xcb_generic_error_t *err;
    err = xcb_request_check (c, ck);
    if (err)
    {
      int code = err->error_code;
      free (err);
      printf("put image error %d\n", code);
      assert (code != 0);
    }   
  }
  printf("depth =%d\n", geo->depth);
	/* create black graphics context */
#endif
}
