#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <cairo/cairo.h>
#include <cairo/cairo-xcb.h>

#define PI 3.1415926535
#define NUM_DASH 2

typedef struct win {
  xcb_connection_t* connection_ptr;
  // was: xcb_window_t win;
  xcb_drawable_t drawable;
  xcb_gcontext_t gc;
  int width;
  int height;
  xcb_screen_t* screen_ptr;
  int screen_number;
  xcb_visualid_t* visualid_ptr;
  xcb_visualtype_t* visualtype_ptr;
  unsigned int white;
} win_t;

static void triangle(cairo_t *cr);
static void square(cairo_t *cr);
static void bowtie(cairo_t *cr);
static void win_init(win_t *win);
static void win_deinit(win_t *win);
static void win_draw(win_t *win);
static void win_handle_events(win_t *win);

int
main(int argc, char *argv[])
{
  win_t win;

  win.connection_ptr = xcb_connect(NULL, NULL);

  if (win.connection_ptr == NULL) {
    fprintf(stderr, "Failed to open display\n");
    return 1;
  }

  win_init(&win);

  win_draw(&win);

  win_handle_events(&win);

  win_deinit(&win);

  xcb_disconnect(win.connection_ptr);

  return 0;
}

#define SIZE 20
static void
triangle(cairo_t *cr)
{
  cairo_move_to(cr, SIZE, 0);
  cairo_rel_line_to(cr, SIZE,  2*SIZE);
  cairo_rel_line_to(cr, -2*SIZE, 0);
  cairo_close_path(cr);
}

static void
square(cairo_t *cr)
{
  cairo_move_to(cr, 0, 0);
  cairo_rel_line_to(cr,  2*SIZE,   0);
  cairo_rel_line_to(cr,   0,  2*SIZE);
  cairo_rel_line_to(cr, -2*SIZE,   0);
  cairo_close_path(cr);
}

static void
bowtie(cairo_t *cr)
{
  cairo_move_to(cr, 0, 0);
  cairo_rel_line_to(cr,  2*SIZE,  2*SIZE);
  cairo_rel_line_to(cr, -2*SIZE,   0);
  cairo_rel_line_to(cr,  2*SIZE, -2*SIZE);
  cairo_close_path(cr);
}

static void
inf(cairo_t *cr)
{
  cairo_move_to(cr, 0, SIZE);
  cairo_rel_curve_to(cr,
		     0, SIZE,
		     SIZE, SIZE,
		     2*SIZE, 0);
  cairo_rel_curve_to(cr,
		     SIZE, -SIZE,
		     2*SIZE, -SIZE,
		     2*SIZE, 0);
  cairo_rel_curve_to(cr,
		     0, SIZE,
		     -SIZE, SIZE,
		     -2*SIZE, 0);
  cairo_rel_curve_to(cr,
		     -SIZE, -SIZE,
		     -2*SIZE, -SIZE,
		     -2*SIZE, 0);
  cairo_close_path(cr);
}

static void
draw_shapes(cairo_t *cr, int x, int y, int fill)
{
  cairo_save(cr);

  cairo_new_path(cr);
  cairo_translate(cr, x+SIZE, y+SIZE);
  bowtie(cr);
  if (fill)
    cairo_fill(cr);
  else
    cairo_stroke(cr);

  cairo_new_path(cr);
  cairo_translate(cr, 4*SIZE, 0);
  square(cr);
  if (fill)
    cairo_fill(cr);
  else
    cairo_stroke(cr);

  cairo_new_path(cr);
  cairo_translate(cr, 4*SIZE, 0);
  triangle(cr);
  if (fill)
    cairo_fill(cr);
  else
    cairo_stroke(cr);

  cairo_new_path(cr);
  cairo_translate(cr, 4*SIZE, 0);
  inf(cr);
  if (fill)
    cairo_fill(cr);
  else
    cairo_stroke(cr);

  cairo_restore(cr);
}

static void
fill_shapes(cairo_t *cr, int x, int y)
{
  draw_shapes(cr, x, y, 1);
}

static void
stroke_shapes(cairo_t *cr, int x, int y)
{
  draw_shapes(cr, x, y, 0);
}

/*
  static void
  draw_broken_shapes(cairo_t *cr)
  {
  cairo_save(cr);

  cairo_set_line_width(cr, 1);
  cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);
  cairo_set_source_rgb(cr, 1, 1, 1);

  cairo_move_to(cr, 19.153717041015625, 144.93951416015625);
  cairo_line_to(cr, 412.987396240234375, 99.407318115234375);
  cairo_line_to(cr, 412.99383544921875, 99.4071807861328125);
  cairo_line_to(cr, 413.15008544921875, 99.5634307861328125);
  cairo_line_to(cr, 413.082489013671875, 99.6920928955078125);
  cairo_line_to(cr, 413.000274658203125, 99.71954345703125);
  cairo_line_to(cr, 273.852630615234375, 138.1925201416015625);
  cairo_line_to(cr, 273.934844970703125, 138.165069580078125);
  cairo_line_to(cr, 16.463653564453125, 274.753662109375);
  cairo_line_to(cr, 16.286346435546875, 274.496337890625);
  cairo_line_to(cr, 273.757537841796875, 137.907745361328125);
  cairo_line_to(cr, 273.839752197265625, 137.8802947998046875);
  cairo_line_to(cr, 412.987396240234375, 99.407318115234375);
  cairo_line_to(cr, 412.99383544921875, 99.4071807861328125);
  cairo_line_to(cr, 413.15008544921875, 99.5634307861328125);
  cairo_line_to(cr, 413.082489013671875, 99.6920928955078125);
  cairo_line_to(cr, 413.000274658203125, 99.71954345703125);
  cairo_line_to(cr, 19.166595458984375, 145.251739501953125);

  cairo_fill(cr);

  cairo_restore(cr);
  }
*/

static void
win_draw(win_t* window_ptr)
{
  static double dash[NUM_DASH] = {SIZE/4.0, SIZE/4.0};
  cairo_surface_t *surface;
  cairo_t *cr;
  xcb_drawable_t drawable;
  drawable = window_ptr->drawable;

  xcb_clear_area(window_ptr->connection_ptr, 0, window_ptr->drawable, 0, 0, 0, 0);

  /* surface = cairo_xcb_surface_create (win->dpy, drawable, win->visual, */
  /* win->width, win->height); */

surface = cairo_xcb_surface_create(window_ptr->connection_ptr,
				   window_ptr->drawable,
				   window_ptr->visualtype_ptr,
				   window_ptr->width,
				   window_ptr->height);

cr = cairo_create(surface);

cairo_set_source_rgb(cr, 1, 1, 1);

cairo_save(cr);
cairo_set_font_size (cr, 20);
cairo_move_to (cr, 10, 10);
cairo_rotate(cr, PI / 2);
cairo_show_text (cr, "Hello World.");
cairo_restore(cr);

/*
  cairo_scale(cr, 5, 5);
  inf(cr);
  cairo_translate(cr, 0, 2 * SIZE);
  inf(cr);
  cairo_translate(cr, 0, - 2 * SIZE);
  cairo_clip(cr);
  cairo_scale(cr, 1/5.0, 1/5.0);
*/

/* This is handy for examining problems more closely */
/*    cairo_scale(cr, 4, 4);  */

cairo_set_line_width(cr, SIZE / 4);

cairo_set_tolerance(cr, .1);

cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
cairo_set_dash(cr, dash, NUM_DASH, 0);
stroke_shapes(cr, 0, 0);

cairo_translate(cr, 0, 4*SIZE);

cairo_set_dash(cr, NULL, 0, 0);
stroke_shapes(cr, 0, 0);

cairo_translate(cr, 0, 4*SIZE);

cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);
stroke_shapes(cr, 0, 0);

cairo_translate(cr, 0, 4*SIZE);

cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
stroke_shapes(cr, 0, 0);

cairo_translate(cr, 0, 4*SIZE);

fill_shapes(cr, 0, 0);

cairo_translate(cr, 0, 4*SIZE);

cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);
fill_shapes(cr, 0, 0);
cairo_set_source_rgb(cr, 1, 0, 0);
stroke_shapes(cr, 0, 0);
/*
  draw_broken_shapes(cr);
*/
if (cairo_status(cr)) {
  printf("Cairo is unhappy: %s\n",
	 cairo_status_to_string(cairo_status(cr)));
  exit(0);
 }

cairo_destroy(cr);
}

/* static int */
/* id_to_visual(xcb_screen_t *root, xcb_visualtype_t **visual) */
/* { */
/*   xcb_depth_iter di; */
/*   xcb_visualtype_t *v; */

/*   for(di = xcb_screen_tAllowedDepthsIter(root); di.rem; XCBDEPTHNext(&di)) */
/*     { */
/*       int i; */
/*       const int count = XCBDEPTHVisualsLength(di.data); */
/*       v = XCBDEPTHVisuals(di.data); */
/*       for(i = 0; i < count; ++i) */
/* 	if(v->visual_id.id == root->root_visual.id) */
/* 	  { */
/* 	    *visual = v; */
/* 	    return di.data->depth; */
/* 	  } */
/*     } */
/*   return 0; */
/* } */


static void
win_init(win_t *win)
{
  // xcb_screen_t *root = XCBConnSetupSuccessRepRootsIter(XCBGetSetup(win->dpy)).data;
  // CARD32 mask = XCBCWBackPixel | XCBCWEventMask;
  // CARD32 values[] = { root->black_pixel, StructureNotifyMask | ExposureMask };
  int depth;

  win->width = 400;
  win->height = 400;

  win->connection_ptr = xcb_connect(NULL, &win->screen_number);
  xcb_screen_iterator_t iter =
      xcb_setup_roots_iterator(xcb_get_setup(win->connection_ptr));

  // win->win = XCBWINDOWNew(win->dpy);
  win->drawable = xcb_generate_id(win->connection_ptr);
  win->white = win->screen_ptr->white_pixel;


  for (; iter.rem; --win->screen_number, xcb_screen_next(&iter))
    if (win->screen_number == 0) {
      win->screen_ptr = iter.data;
      break;
    }

  xcb_create_window(win->connection_ptr,
		    XCB_COPY_FROM_PARENT,
		    win->drawable,
		    win->screen_ptr->root,
		    0,
		    0,
		    win->width,
		    win->height,
		    5,
		    XCB_WINDOW_CLASS_INPUT_OUTPUT,
		    win->screen_ptr->root_visual,
		    XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK,
		    &win->white);


  // depth = id_to_visual(root, &win->visual);
  /* XCBCreateWindow(win->dpy, depth, win->win, root->root, 0, 0, win->width, */
  /* 		  win->height, 0, InputOutput, win->visual->visual_id, */
  /* 		  mask, values); */
  xcb_map_window(win->connection_ptr, win->drawable);
}

static void
win_deinit(win_t *win)
{
  xcb_destroy_window(win->connection_ptr, win->drawable);
}

static void
win_handle_events(win_t *win)
{
    xcb_generic_event_t* event_ptr;

    while (1) {
	event_ptr = xcb_wait_for_event(win->connection_ptr);
	if(!event_ptr) {
	    return;
	}
	switch(event_ptr->response_type) {
	case XCB_CONFIGURE_NOTIFY:
	{
	    xcb_configure_notify_event_t* configure_notify_event_ptr = (xcb_configure_notify_event_t*) event_ptr;

	    win->width = configure_notify_event_ptr->width;
	    win->height = configure_notify_event_ptr->height;
	}
	break;
	case XCB_EXPOSE:
	{
	    xcb_expose_event_t* expose_event_ptr = (xcb_expose_event_t*) event_ptr;

	    if (expose_event_ptr->count == 0)
		win_draw(win);
	}
	break;
	}
    }
}
