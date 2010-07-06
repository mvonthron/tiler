#include <X11/Xlib.h>
#include <X11/Xutil.h>
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
printf msg; \
printf("\n"); \
} while (0)

typedef int bool;
#define false (0)
#define true (1)

Display *display;
Window root;


bool wm_is_compiz()
{
  XTextProperty wmname;
  XGetWMName(display, root, &wmname);
  
  D(("WM Name: %s", wmname.value));
  
  return false;
}

int
main(int argc, char **argv)
{
  display = XOpenDisplay(NULL);
  if(!display)
    return 1;
  root = XDefaultRootWindow(display);
  
  wm_is_compiz();
  
  return 0;
}
