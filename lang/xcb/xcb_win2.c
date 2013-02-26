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
	int width;
	int height;
} G;
#include <assert.h>
#include <cairo/cairo.h>

void create_dri_drawable()
{
}

int psize;
static uint8_t *pbuf = NULL;
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

void  draw_text(cairo_t *cr)
{
	cairo_text_extents_t te;
	cairo_set_source_rgb (cr, 1, 0, 0);
	cairo_select_font_face (cr, "Georgia",
			    CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size (cr, 120);
	cairo_text_extents (cr, "a", &te);
if(0)	cairo_move_to (cr, 200 - te.width / 2 - te.x_bearing,
			    200 - te.height / 2 - te.y_bearing);
	cairo_move_to (cr, 100 ,200);
	cairo_show_text (cr, "abcdde");
}
void  draw_fill(cairo_t *cr)
{
	cairo_set_source_rgb (cr, 1, 1, 0);
	cairo_rectangle (cr, 150, 50, 150, 150); //x,y,w,h
	cairo_fill (cr);
}

void  draw_path(cairo_t *cr)
{
	cairo_set_line_width (cr, 10.0);
	cairo_set_source_rgb (cr, 0, 1, 1);
	cairo_rectangle (cr, 0, 0, 150, 50);
	cairo_stroke (cr);
}
void draw_cairo(cairo_surface_t *in)
{

	cairo_t *cr;
	int mid ;

	if(0)printf("%p\n", in);

	if(!psize ) return;
	if(!in) return;

	cr = cairo_create (in);
	draw_path(cr);
	draw_fill(cr);
	draw_text(cr);
	draw_mask(cr);
	return;

	cairo_set_source_rgb (cr, 0, 1, 0);
	cairo_rectangle (cr, 0, 0, 640,480);
	cairo_fill (cr);

	cairo_set_source_rgb (cr, 0, 0, 0.9);
	cairo_rectangle (cr, 0, 0, 64,480);
	cairo_fill (cr);

	//   cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
//	cairo_set_source_rgba (cr, 0, 0, 0, 0);
//	cairo_paint (cr);

	cairo_set_source_rgb (cr, 1, 1, 1);
	cairo_rectangle (cr, 0, 0, 100, 100);
	cairo_paint_with_alpha (cr, 0.5);

	//   /cairo_fill (cr);

	//*surface_inout = cairo_surface_reference (cairo_get_target (cr));
#define M_PI 3.1415
	cairo_arc (cr, 128.0, 128.0, 76.8, 0, 2 * M_PI);
	cairo_clip (cr);


	cairo_new_path (cr);  
	cairo_rectangle (cr, 0, 0, 256, 256);
	cairo_fill (cr);
	cairo_set_source_rgb (cr, 0, 1, 0);
	cairo_move_to (cr, 0, 0);
	cairo_line_to (cr, 256, 256);
	cairo_move_to (cr, 256, 0);
	cairo_line_to (cr, 0, 256);
	cairo_set_line_width (cr, 10.0);
	cairo_stroke (cr);

	cairo_destroy (cr);
}

void draw_gc(xcb_window_t win)
{
	xcb_rectangle_t      r = { 0, 0,  0,0 };
	xcb_connection_t    *c = G.conn;
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

	static cairo_surface_t *cmask;
	if( pbuf == NULL){
		//			psize = geo->width *geo->height*4; //for test
		pbuf = malloc(psize);
		printf("malloc pbuf=0x%x size=%d\n",pbuf, psize);
		memset(pbuf, 0x18, psize);

		cmask = cairo_image_surface_create_for_data ((unsigned char *) pbuf,
				CAIRO_FORMAT_ARGB32, 
				geo->width, geo->height, geo->width*4);
		//	12, 4, 48);
		if(cmask == NULL){
			perror("cairo surface create"); exit(1);
		}

	}
	if(0)printf("width (%dx%d=%d\n",  geo->width, geo->height,  geo->width *geo->height*4);
	draw_cairo(cmask);

	//draw image with xcb_put_image
	// creat gc and draw to parent window
	xcb_gcontext_t       g;
	g = xcb_generate_id(c);
	xcb_create_gc(c, g, win, 0,NULL);

	ck = xcb_put_image_checked(c, XCB_IMAGE_FORMAT_Z_PIXMAP,
			win, g,
			r.width, r.height, r.x,r.y,
			0, G.s->root_depth,
			psize, pbuf);
	xcb_flush(c);

}


#include <signal.h>
void handler(int  num)
{
	//print_trace();
	exit(1);
}

xcb_visualtype_t *
lookup_visual (xcb_screen_t   *s,
	       xcb_visualid_t  visual)
{
    xcb_depth_iterator_t d;

    d = xcb_screen_allowed_depths_iterator (s);
    for (; d.rem; xcb_depth_next (&d)) {
	xcb_visualtype_iterator_t v = xcb_depth_visuals_iterator (d.data);
	for (; v.rem; xcb_visualtype_next (&v)) {
	    if (v.data->visual_id == visual)
		return v.data;
	}
    }

    return 0;
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
  int done =0 ;

	signal(SIGSEGV, handler);
	/* open connection with the server */
	c = xcb_connect(NULL,NULL);
	if (xcb_connection_has_error(c)) {
		printf("Cannot open display\n");
		exit(1);
	}

	/* get the first screen */
	s = xcb_setup_roots_iterator( xcb_get_setup(c) ).data;

  xcb_get_geometry_reply_t *geo;

  
  /* create sub window */
  w= xcb_generate_id(c);
	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = s->black_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
  xcb_void_cookie_t cookie;

  int width =640;
  int height =480;
	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = s->white_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS
    | XCB_EVENT_MASK_VISIBILITY_CHANGE    
    ;
	values[1] = 0xffff;
	printf("  root_depth = 0x%x\n", s->root_depth);
	xcb_create_window(c, s->root_depth, w, s->root,
      10, 10, width, height,
      1,
			XCB_WINDOW_CLASS_INPUT_OUTPUT, s->root_visual,
			mask, values);

  xcb_map_window(c, w); xcb_flush(c);

//  xcb_reparent_window(c, child,  G.s->root,100,100);

  values[1] |= XCB_EVENT_MASK_POINTER_MOTION |
     XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE;
  xcb_change_window_attributes_checked(c, w, mask, values); xcb_flush(c);

	G.conn = c;
	G.s = s;
  create_dri_drawable();

  printf("wait event\n");
  while (!done ) {
		//printf("\re=>(x%x)", e->response_type); fflush(stdout);
    e = xcb_wait_for_event(c);
    if(!e) { printf("wait event error\n");
    	break;}
    else{
      switch (e->response_type & ~0x80) {
        case XCB_EXPOSE:    /* draw or redraw the window */
          printf("XCB_EXPOSE\n");
   //     draw_gc(w);
          break;
        case XCB_KEY_PRESS:  /* exit on key press */
          printf("KEY\n");
        	struct xcb_key_press_event_t  *ep = e;
        	printf("key=%d\n", (char)ep->detail);
        	if(ep->detail == 9)
        		exit(1);
					draw_gc(w);
          //gwj done = 1;
          break;
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
