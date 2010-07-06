#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>


void 
map_keysym(Display *display, Window win, int keysym, unsigned int modifiers)
{
  XGrabKey(display, XKeysymToKeycode(display, keysym), modifiers, win,
           1, GrabModeAsync, GrabModeAsync);
}

void 
map_keycode(Display *display, Window win, int keycode, unsigned int modifiers)
{
  XGrabKey(display, keycode, modifiers, win, 1, GrabModeAsync, GrabModeAsync);
}

int main(int argc, char **argv)
{
  Display *display;
  XEvent event;
  
  char *display_name = getenv("DISPLAY");
  display = XOpenDisplay(display_name);
  if (display == NULL){
    fprintf(stderr, "%s: cannot connect to X server '%s'\n",
            argv[0], display_name);
    exit(1);
  }
  
  printf("ctrl: %d\n", ControlMask);
  printf("shift: %d\n", ShiftMask);


  /* ctrl + alt + w */
  map_keysym(display, XDefaultRootWindow(display), XK_w, AnyModifier);
  /* ctrl + alt + left */
  map_keycode(display, XDefaultRootWindow(display), 113, ControlMask);
  /* ctrl + alt + q */
  map_keysym(display, XDefaultRootWindow(display), XK_q, ControlMask|Mod1Mask);
  
  for (;;) {
	XNextEvent(display, &event);
	
    if (event.type == KeyPress) {
      printf ("e.xkey.keycode=%d\n", event.xkey.keycode);
      printf ("e.xkey.state=%d\n", event.xkey.state);
      
      if(event.xkey.keycode == (int)XKeysymToKeycode(display, XK_q))
        break;
    }
  }

  return 0;
}
