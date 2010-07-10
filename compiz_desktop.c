#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

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
  int actual_format, status;
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

void
print_window(Display *display, Window win)
{
  char *name;
  Atom *atoms;
  int nitems;

  XFetchName(display, win, &name);
  atoms = XListProperties(display, win, &nitems);
  
  bool same_desktop = compiz_window_in_active_desktop(win);
  
  if(same_desktop)
    printf(COLOR_BOLD COLOR_GREEN "[current]  "COLOR_CLEAR "\"%s\" (Window 0x%x)\t[%d]\n", name, (unsigned int)win, nitems);
  else
    printf(COLOR_BOLD COLOR_RED   "[other]    "COLOR_CLEAR "\"%s\" (Window 0x%x)\t[%d]\n", name, (unsigned int)win, nitems);
}

bool 
wm_is_compiz()
{
  /* all standard get_wm_name-like failed so far... */
  
  /* compiz has some atoms defined */
  /* another way could be to find the "switcher window" instancied by 
   * compiz, which has a "compiz" WM_CLASS */
  Atom actual_type, atom;
  int actual_format, status=-1, i=0, actual_size=0;
  unsigned long nitems, bytes_after;
  unsigned char *data=NULL;
  Window w;
  
  atom = XInternAtom(display, "_COMPIZ_SUPPORTING_DM_CHECK", 0);
  status = XGetWindowProperty(display, root, atom, 0, (~0L), 0,
                                  AnyPropertyType, &actual_type, &actual_format,
                                  &nitems, &bytes_after, &data);
  
  if(status < Success)
    D(("Error checking Compiz has the window manager"));
  
  return (nitems >= 1);
}

/**
 * @return actual list size
 */
int
list_windows(Display* display, Window root, Window **window_list, bool only_curr_desktop)
{
  Atom actual_type, atom;
  int actual_format, status=-1, i=0, actual_size=0;
  unsigned long nitems, bytes_after;
  unsigned char *data=NULL;
  Window w;
  
  assert(*window_list == NULL);
  
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
      *((*window_list)+actual_size++) = *((Window *)data+i);          /* warning: ligne poilue ! */
    }
  }
  
  return actual_size;
}


int 
compiz_get_active_desktop()
{
  int x, y, w, h;
  get_workarea(display, root, &x, &y, &w, &h);

  D(("WORKAREA : (%d, %d, %d, %d)", x, y, w, h));
  
  D(("_NET_NUMBER_OF_DESKTOPS %d", get_int_property(display, root, "_NET_NUMBER_OF_DESKTOPS")));
  D(("_NET_CURRENT_DESKTOP %d", get_int_property(display, root, "_NET_CURRENT_DESKTOP")));
  
  int vp_x, vp_y;
  get_2int_property(display, root, "_NET_DESKTOP_VIEWPORT", &vp_x, &vp_y);
  D(("_NET_DESKTOP_VIEWPORT (%d, %d)", vp_x, vp_y));
  
  int gx, gy;
  get_2int_property(display, root, "_NET_DESKTOP_GEOMETRY", &gx, &gy);
  D(("_NET_DESKTOP_GEOMETRY (%d, %d) => (%d, %d)", gx, gy, gx/w, gy/h));
  

  int desktop = (vp_x/w)+(gx/w)*(vp_y/h);
  D(("Current desktop is %d (%d+%d*%d)", desktop, (vp_x/w), (gx/w), (vp_y/h)));

  return desktop;
}

/**
 * @note not working
 */
int 
compiz_get_desktop_for_window(Window window)
{
  int desktop=-1;
  
  XWindowAttributes attributes;
  XGetWindowAttributes(display, window, &attributes);

  print_window(display, window);
  //D(("  (%d, %d)", attributes.x, attributes.y));

  return desktop;
}

/**
 * immovable window
 * used for some compiz system windows
 */
bool is_sticky(Window window)
{
  Atom actual_type;
  int i, actual_format;
  unsigned long nitems;
  unsigned long bytes_after;
  unsigned long *data;
  
  unsigned long atom_sticky = XInternAtom(display, "_NET_WM_STATE_STICKY", 0);
  
  Atom atom = XInternAtom(display, "_NET_WM_STATE", 0);
  int status = XGetWindowProperty(display, window, atom, 0, (~0L), 0,
                                  XA_ATOM, &actual_type, &actual_format,
                                  &nitems, &bytes_after, &data);
  if(status >= Success 
     && nitems >= 1
     && actual_type == XA_ATOM
     && actual_format == 32){
       
    for(i=0; i<nitems; i++)
      if(data[i] == atom_sticky)
        return true;
  }
  
  return false;
}


bool 
compiz_window_in_active_desktop(Window window)
{
  XWindowAttributes attributes;
  XGetWindowAttributes(display, window, &attributes);

  //D(("(%d, %d)", attributes.x, attributes.y));
  
  /* negative coordinates => desktop before */
  if(attributes.x < 0 || attributes.y < 0)
    return false;
    
  int x=-1, y=-1, w=-1, h=-1;
  get_workarea(display, root, &x, &y, &w, &h);

  /* too far away */
  if(attributes.x > w || attributes.y > h)
    return false;
  
  /* not in workarea : probably a dock or stuff like that */
  if(attributes.x < x || attributes.y < y)
    return false;
  
  if(is_sticky(window))
    return false;
  
  return true;
}



Window 
get_active_window()
{
  //extern Display *display;
  
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
 * @note not working
 */
bool 
same_viewport(Window win1, Window win2)
{
  int win1_x, win1_y;
  int win2_x, win2_y;
  
  get_2int_property(display, win1, "_NET_DESKTOP_VIEWPORT", &win1_x, &win1_y);
  get_2int_property(display, win2, "_NET_DESKTOP_VIEWPORT", &win2_x, &win2_y);
  
  D(("[(%d, %d) (%d, %d)]", win1_x, win1_y, win2_x, win2_y));
  
  return (win1_x == win2_x && win1_y == win2_y);
}

int
main(int argc, char **argv)
{
  display = XOpenDisplay(NULL);
  if(!display)
    return 1;
  root = XDefaultRootWindow(display);
  
  if( wm_is_compiz() )
    printf(COLOR_BOLD COLOR_RED "Running on Compiz" COLOR_CLEAR);
  else
    printf(COLOR_BOLD COLOR_GREEN "Running on a compliant WM (maybe)" COLOR_CLEAR);

  /*
  compiz_get_active_desktop();
  print_window(display, get_active_window());
  compiz_get_desktop_for_window(get_active_window());
  */
  
  Window *window_list=NULL;
  int size = list_windows(display, root, &window_list, 1); 
  int i;
  
  
  for(i=0; i<size; i++){
    print_window(display, window_list[i]);
    //compiz_get_desktop_for_window(window_list[i]);
    //D(("%d", same_viewport(get_active_window(), window_list[i])));
    //compiz_window_in_active_desktop(window_list[i]);
  }
  
 
  return 0;
}
