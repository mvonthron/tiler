#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include <stdio.h>
#include <stdlib.h>

#define COLOR_GREEN "\033[92m"
#define COLOR_RED "\033[91m"
#define COLOR_BLUE "\033[94m"
#define COLOR_YELLOW "\033[93m"
#define COLOR_BOLD "\033[1m"
#define COLOR_CLEAR "\033[0m"

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define D(msg) do { \
  printf("%s[%s:%s(%d)]%s ",COLOR_RED, __FILE__, __FUNCTION__, __LINE__, COLOR_CLEAR); \
  printf msg;                                                                          \
  printf("\n");                                                                        \
} while (0)

typedef int bool;
#define false (0)
#define true (1)

Display *display;
Window root;

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

void
get_2int_property(Display *display, Window window, char *property, int *data0, int *data1)
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
     && nitems >= 2
     && actual_type == XA_CARDINAL
     && actual_format == 32){

    *data0 = ((int *)data)[0];
    *data1 = ((int *)data)[1];
  }
}



bool 
wm_is_compiz()
{
  XTextProperty wmname;
  char *wmname2;
  XGetWMName(display, root, &wmname);
  
  D(("WM Name: %s (%d)", wmname.value, wmname.nitems));
  
  XFetchName(display, root, &wmname2);
  D(("WM Name2: %s", wmname2));
  
  return false;
}

int 
compiz_get_active_desktop()
{
  int x, y, w, h;
  get_workarea(display, root, &x, &y, &w, &h);

  D(("workarea : (%d, %d, %d, %d)", x, y, w, h));
  
  D(("_NET_NUMBER_OF_DESKTOPS %d", get_int_property(display, root, "_NET_NUMBER_OF_DESKTOPS")));
  D(("_NET_CURRENT_DESKTOP %d", get_int_property(display, root, "_NET_CURRENT_DESKTOP")));
  
  int vp_x, vp_y;
  get_2int_property(display, root, "_NET_DESKTOP_VIEWPORT", &vp_x, &vp_y);
  
  D(("_NET_DESKTOP_VIEWPORT (%d, %d)", vp_x, vp_y));

  /** need to compute max vp_x */
  D(("Current desktop could be %d (%d+%d)", vp_x/w+vp_y/h, vp_x/w, vp_y/h));
}

bool same_viewport(Window win1, Window win2)
{
  int win1_x, win1_y;
  int win2_x, win2_y;
  
  get_2int_property(display, win1, "_NET_DESKTOP_VIEWPORT", &win1_x, &win1_y);
  get_2int_property(display, win2, "_NET_DESKTOP_VIEWPORT", &win2_x, &win2_y);
  
  return (win1_x == win2_x && win1_y == win2_y);
}

int
main(int argc, char **argv)
{
  display = XOpenDisplay(NULL);
  if(!display)
    return 1;
  root = XDefaultRootWindow(display);
  
  wm_is_compiz();
  
  compiz_get_active_desktop();
  
  return 0;
}
