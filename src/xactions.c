/**
 * Copyright (c) 2010 Manuel Vonthron <manuel.vonthron@acadis.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "tiler.h"
#include "utils.h"
#include "xactions.h"



int
get_int_property(Display *display, Window window, char *property)
{
  Atom at, actual_type;
  int actual_format, status, value;
  unsigned long nitems, bytes_after;
  unsigned char *data=NULL;
  
  
  at = XInternAtom(display, property, 0);
  status = XGetWindowProperty(display, window, at, 0, (~0L), 0,
                              XA_CARDINAL, &actual_type, &actual_format,
                              &nitems, &bytes_after, &data);
  if(status >= Success 
     && nitems >= 1
     && actual_type == XA_CARDINAL
     && actual_format == 32){

    value = *((int *)data);
  }
  
  return value;
}

Window 
get_active_window()
{
  extern Display *display;
  
  Atom atom = XInternAtom(display, "_NET_ACTIVE_WINDOW", 0);
  Window root = XDefaultRootWindow(display);

  Atom actual_type;
  int actual_format;
  unsigned long nitems;
  unsigned long bytes_after;
  unsigned char *data=NULL;

  int status = XGetWindowProperty(display, root, atom, 0, (~0L), 0,
                                  AnyPropertyType, &actual_type, &actual_format,
                                  &nitems, &bytes_after, &data);
  
  if(status >= Success && nitems > 0)
    return *((Window*)data);
  else{
    //Xfree(data); //definition problem
    return 0;
  }
}

/**
 * @return actual list size
 */
int
list_windows(Display* display, Window root, Window **window_list, bool only_curr_desktop)
{
  Atom actual_type, atom;
  int actual_format, curr_desktop=42, status=-1, i=0, actual_size=0;
  unsigned long nitems, bytes_after;
  unsigned char *data=NULL;
  Window w;
  
  assert(*window_list == NULL);
  
  curr_desktop = get_desktop(display, get_active_window(display));
  
  atom = XInternAtom(display, "_NET_CLIENT_LIST_STACKING", 0);
  status = XGetWindowProperty(display, root, atom, 0, (~0L), 0,
                                  XA_WINDOW, &actual_type, &actual_format,
                                  &nitems, &bytes_after, &data);
  
  if(status >= Success 
     && nitems >= 1
     && actual_type == XA_WINDOW
     && actual_format == 32){

    *window_list = (Window *) malloc(nitems * sizeof(Window));

    for(i=0; i<nitems; i++){
      w = *((Window *)data+i);
      
      if(!only_curr_desktop || get_desktop(display, w) == curr_desktop){
        print_window(display, w);
        *((*window_list)+actual_size++) = *((Window *)data+i);          /* warning: ligne poilue ! */
      }
    }
  }
  
  return actual_size;
}

bool
window_is_maximized(Display *display, Window window)
{
  return false;
}

void
unmaximize_window(Display *display, Window window)
{
  Atom state = XInternAtom(display, "_NET_WM_STATE", 0);
  unsigned long horz = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", 0);
  unsigned long vert = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", 0);
  unsigned long full = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", 0);
  unsigned long remove = 0;
  
  send_xevent(display, window, state, remove, horz, vert, full, 0);
}


void
print_window(Display *display, Window win)
{
  char *name;
  Atom *atoms;
  int nitems;

  XFetchName(display, win, &name);
  atoms = XListProperties(display, win, &nitems);
  
  printf("Window 0x%x on desktop %d/%d (\"%s\")\t[%d]\n", (unsigned int)win, get_desktop(display, win)+1, 4, name, nitems);
}

void
send_xevent(Display *display, Window window, Atom message_type,
            unsigned long data0, unsigned long data1,
            unsigned long data2, unsigned long data3,
            unsigned long data4)
{
  XEvent e;
  
  e.xclient.type = ClientMessage;
  e.xclient.serial = 0;
  e.xclient.send_event = True;
  e.xclient.message_type = message_type;
  e.xclient.window = window;
  e.xclient.format = 32;
  e.xclient.data.l[0] = data0;
  e.xclient.data.l[1] = data1;
  e.xclient.data.l[2] = data2;
  e.xclient.data.l[3] = data3;
  e.xclient.data.l[4] = data4;
  
  XSendEvent(display, DefaultRootWindow(display), False, 
             SubstructureRedirectMask | SubstructureNotifyMask, &e);
}

void 
move_window(Display *display, Window window, Geometry_t geometry)
{
  unmaximize_window(display, window);
  
  XMoveWindow(display, window, geometry.x, geometry.y);
}

void 
move_resize_window(Display *display, Window window, Geometry_t geometry)
{
  unmaximize_window(display, window);
  
  XMoveResizeWindow(display, window, geometry.x, geometry.y,
                               geometry.width, geometry.height);
}

void
get_window_geometry(Display *display, Window window, Geometry_t *geometry)
{
}

void
get_window_frame_extent(Display *display, Window window, 
                        int *left, int *right, int *top, int *bottom) /* returning values */
{
  Atom actual_type;
  int actual_format;
  unsigned long nitems;
  unsigned long bytes_after;
  unsigned char *data;
  
  Atom atom = XInternAtom(display, "_NET_FRAME_EXTENTS", 0);
  int status = XGetWindowProperty(display, window, atom, 0, (~0L), 0,
                                  XA_CARDINAL, &actual_type, &actual_format,
                                  &nitems, &bytes_after, &data);
  if(status >= Success 
     && nitems >= 4
     && actual_type == XA_CARDINAL
     && actual_format == 32){

    *left   = ((long*)data)[0];
    *right  = ((long*)data)[1];
    *top    = ((long*)data)[2];
    *bottom = ((long*)data)[3];
    
    return;
  }else{
    return;
  }
}

/**
 * @todo factoriser get_property_4int
 */
void
get_workarea(Display *display, Window window, 
             int *x, int *y, int *width, int *height) /* returning values */
{
  Atom actual_type;
  int actual_format;
  unsigned long nitems;
  unsigned long bytes_after;
  unsigned char *data;
  
  Atom atom = XInternAtom(display, "_NET_WORKAREA", 0);
  int status = XGetWindowProperty(display, window, atom, 0, (~0L), 0,
                                  XA_CARDINAL, &actual_type, &actual_format,
                                  &nitems, &bytes_after, &data);
  if(status >= Success 
     && nitems >= 4
     && actual_type == XA_CARDINAL
     && actual_format == 32){

    *x      = ((int*)data)[0];
    *y      = ((int*)data)[1];
    *width  = ((int*)data)[2];
    *height = ((int*)data)[3];
    
    return;
  }else{
    return;
  }
}

/**
 * Compute the right x/y/width/height to fill the desired space
 * by taking border thickness into account
 */
void 
fill_geometry(Display *display, Window window, Geometry_t geometry)
{
  int right, left, top, bottom;
  
  /* get _NET_FRAME_EXTENTS */
  get_window_frame_extent(display, window, &left, &right, &top, &bottom);  
  
  geometry.x      += left;
  geometry.y      += top;
  geometry.width  -= (left + right);
  geometry.height -= (top + bottom);

  move_resize_window(display, window, geometry);
}


/**
 * NOT WORKING ON COMPIZ ?!
 * @todo replace with an xevent ?
 */
int
get_desktop(Display *display, Window window)
{
  return get_int_property(display, window, "_NET_WM_DESKTOP");
}
