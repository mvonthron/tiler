/*
 * Copyright (c) 2012 Manuel Vonthron <manuel.vonthron@acadis.org>
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
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "tiler.h"
#include "utils.h"
#include "xactions.h"
#include "config.h"

#define MATCH(condition, state) (((condition) && (state)) || (!condition))

/**
 * property getters
 */
static int
get_int_property(Display *display, Window window, char *property)
{
    Atom at, actual_type;
    int actual_format, status, value = 0;
    unsigned long nitems, bytes_after;
    unsigned char *data = NULL;


    at = XInternAtom(display, property, 0);
    status = XGetWindowProperty(display, window, at, 0, (~0L), 0,
                                XA_CARDINAL, &actual_type, &actual_format,
                                &nitems, &bytes_after, &data);

    if(status >= Success
            && nitems >= 1
            && actual_type == XA_CARDINAL
            && actual_format == 32) {

        value = *((int *)data);
    }

    XFree(data);
    return value;
}

static bool
get_2int_property(Display *display, Window window, char *property, int *data0, int *data1)
{
    Atom at, actual_type;
    int actual_format, status, ret = true;
    unsigned long nitems, bytes_after;
    unsigned char *data = NULL;


    at = XInternAtom(display, property, 0);
    status = XGetWindowProperty(display, window, at, 0, (~0L), 0,
                                XA_CARDINAL, &actual_type, &actual_format,
                                &nitems, &bytes_after, &data);
    if(status >= Success
            && nitems >= 2
            && actual_type == XA_CARDINAL
            && actual_format == 32) {

        *data0 = ((int *)data)[0];
        *data1 = ((int *)data)[1];
    } else {
        ret = false;
    }

    XFree(data);
    return ret;
}

static bool
get_4int_property(Display *display, Window window, char *property, int *data0, int *data1, int *data2, int *data3)
{
    Atom at, actual_type;
    int actual_format, status, ret = true;
    unsigned long nitems, bytes_after;
    unsigned char *data = NULL;


    at = XInternAtom(display, property, 0);
    status = XGetWindowProperty(display, window, at, 0, (~0L), 0,
                                XA_CARDINAL, &actual_type, &actual_format,
                                &nitems, &bytes_after, &data);
    if(status >= Success
            && nitems >= 4
            && actual_type == XA_CARDINAL
            && actual_format == 32) {

        *data0 = ((int *)data)[0];
        *data1 = ((int *)data)[1];
        *data2 = ((int *)data)[2];
        *data3 = ((int *)data)[3];
    } else {
        ret = false;
    }

    XFree(data);
    return ret;
}

/**
 * @warning data must be freed by caller with XFree()
 */
static int
get_property(Display *display, Window window, char *property, void *data)
{
    Atom actual_type, atom;
    int actual_format, status = -1;
    unsigned long nitems, bytes_after;

    atom = XInternAtom(display, property, 0);
    status = XGetWindowProperty(display, window, atom, 0, (~0L), 0,
                                AnyPropertyType, &actual_type, &actual_format,
                                &nitems, &bytes_after, (unsigned char **)&data);

    if(status >= Success && nitems > 0)
        return nitems;
    else
        return -1;
}

static Atom
get_atom_property(Window window, char *property)
{
    Atom at, actual_type, value;
    int actual_format, status;
    unsigned long nitems, bytes_after;
    unsigned char *data = NULL;

    at = XInternAtom(display, property, 0);
    status = XGetWindowProperty(display, window, at, 0, (~0L), 0,
                                XA_ATOM, &actual_type, &actual_format,
                                &nitems, &bytes_after, &data);

    if(status >= Success
            && nitems >= 1
            && actual_type == XA_ATOM
            && actual_format == 32) {

        value = *((int *)data);
    }

  XFree(data);
  return value;
}

/**
 * Compiz specific stuff
 */
static int
__compiz_get_active_desktop()
{
    int x, y, w, h;
    int vp_x, vp_y;
    int gx, gy, desktop;

    get_workarea(display, root, &x, &y, &w, &h);
    get_2int_property(display, root, "_NET_DESKTOP_VIEWPORT", &vp_x, &vp_y);
    get_2int_property(display, root, "_NET_DESKTOP_GEOMETRY", &gx, &gy);

    desktop = (vp_x / w) + (gx / w) * (vp_y / h);

    D(("Current desktop is %d (%d+%d*%d)", desktop, (vp_x / w), (gx / w), (vp_y / h)));

    return desktop;
}

static bool
__compiz_window_in_active_desktop(Window window)
{
    Geometry_t geometry;
    get_window_geometry(display, window, &geometry);

    D(("Window is at (%d, %d), size (%d, %d)", geometry.x, geometry.y, geometry.width, geometry.height));

    /* negative coordinates => desktop before */
    if(geometry.x < 0 || geometry.y < 0)
        return false;

    int x = -1, y = -1, w = -1, h = -1;
    get_workarea(display, root, &x, &y, &w, &h);

    /* too far away */
    if(geometry.x > w || geometry.y > h)
        return false;

    /* not in workarea : probably a dock or stuff like that */
    if(geometry.x < x || geometry.y < y)
        return false;

    return true;
}

static void
__compiz_get_window_adjustments(Window window, int *x, int *y, int *width, int *height)
{
    /* default values */
    *x      =  -4; /* NOT taken into account so far! */
    *y      =   0;
    *width  =   5;
    *height =   4;

    get_4int_property(display, window, "_COMPIZ_WM_WINDOW_BLUR_DECOR", x, y, width, height);
}

/*
 */
static bool
is_regular_window(Window window)
{
    Atom state = get_atom_property(window, "_NET_WM_STATE");
    Atom atom_sticky = XInternAtom(display, "_NET_WM_STATE_STICKY", 0);
    if(state == atom_sticky)
        return false;

    Atom type = get_atom_property(window, "_NET_WM_WINDOW_TYPE");
    Atom atom_normal = XInternAtom(display, "_NET_WM_WINDOW_TYPE_NORMAL", 0);
    Atom atom_utility = XInternAtom(display, "_NET_WM_WINDOW_TYPE_UTILITY", 0);
    Atom atom_dialog = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DIALOG", 0);

    if(type == atom_normal || type == atom_utility || type == atom_dialog)
        return true;
    else
        return false;
}


/**
 * non-static stuff
 */
Window
get_active_window()
{
    extern Display *display;

    Atom atom = XInternAtom(display, "_NET_ACTIVE_WINDOW", 0);
    unsigned char *data = NULL;
    Window root = XDefaultRootWindow(display);
    Window ret;

    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long bytes_after;

    int status = XGetWindowProperty(display, root, atom, 0, (~0L), 0,
                                    AnyPropertyType, &actual_type, &actual_format,
                                    &nitems, &bytes_after, &data);

    if(status >= Success && nitems > 0)
        ret = *((Window *)data);
    else {
        ret = 0;
    }

    XFree(data);
    return ret;
}

int
get_active_desktop()
{
    if(settings.is_compiz)
        return __compiz_get_active_desktop();
    else
        return get_window_desktop(display, get_active_window(display));
}

bool
window_in_active_desktop(Display *display, Window window)
{
    if(settings.is_compiz)
        return __compiz_window_in_active_desktop(window);
    else {
        return (get_window_desktop(display, window) == get_window_desktop(display, get_active_window(display)));
    }
}

/**
 * @return actual list size
 */
int
list_windows(Display *display, Window root, Window **window_list, uint options)
{
    Atom actual_type, atom;
    int actual_format, /*curr_desktop=42,*/ status = -1, i = 0, actual_size = 0;
    unsigned long nitems, bytes_after;
    unsigned char *data = NULL;
    Window w;

    assert(*window_list == NULL);

    //curr_desktop = get_desktop(display, get_active_window(display));

    atom = XInternAtom(display, "_NET_CLIENT_LIST_STACKING", 0);
    status = XGetWindowProperty(display, root, atom, 0, (~0L), 0,
                                XA_WINDOW, &actual_type, &actual_format,
                                &nitems, &bytes_after, &data);

    if(status >= Success
            && nitems >= 1
            && actual_type == XA_WINDOW
            && actual_format == 32) {

        *window_list = (Window *) malloc(nitems * sizeof(Window));

        int active_desktop = get_window_desktop(display, get_active_window()); // @todo refactor
        int active_monitor = get_window_monitor(get_active_window());

        for(i = 0; i < nitems; i++) {
            w = *((Window *)data + i);

            if((options & LIST_ALL)
                    || (MATCH((options & LIST_CURR_DESKTOP), active_desktop == get_window_desktop(display, w))
                        && MATCH((options & LIST_CURR_MONITOR), active_monitor == get_window_monitor(w))
                        && MATCH((options & LIST_REGULAR)     , is_regular_window(w))
                        && MATCH((options & LIST_SYSTEM)      , !is_regular_window(w)))) {
                *((*window_list) + actual_size++) = *((Window *)data + i);      /* warning: ligne poilue ! */
            }
        }
    }
  
      XFree(data);
      return actual_size;
}

bool
is_window_maximized(Display *display, Window window)
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
maximize_window(Display *display, Window window)
{
    Atom state = XInternAtom(display, "_NET_WM_STATE", 0);
    unsigned long horz = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", 0);
    unsigned long vert = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", 0);
    unsigned long add = 1;

    send_xevent(display, window, state, add, horz, vert, 0, 0);
}

/** Build and print a string showing properties of a window
 *
 * The string uses the following format
 * <code>
 *  +* Window 0x<hex id> at (<x>, <y>), size (<width>, <height>)     desktop <current>/<total>, monitor <current>/<total>  (<name>)
 * </code>
 * @li "+" marker denotes a window on the current monitor (omited if single monitor)
 * @li "*" marker denotes a window on the current desktop
 * @li yellow lines denote windows tagged as "system" (docks, background...)
 *
 * @todo move to utils.c
 */
void
print_window(Display *display, Window win)
{
    char *name, current_desktop_marker = ' ', current_monitor_marker = ' ', regular_win_marker[8];

    int nitems, monitor_id;
    Geometry_t geometry;

    XFetchName(display, win, &name);
    get_window_geometry(display, win, &geometry);

    /* get number of properties attached to the window */
    XListProperties(display, win, &nitems);

    if(window_in_active_desktop(display, win))
        current_desktop_marker = '*';

    monitor_id = get_window_monitor(win);
    /* no specific marker if nb_monitor is 1 */
    if(settings.nb_monitors > 1 && current_desktop_marker == '*' && get_window_monitor(win) == get_window_monitor(get_active_window()))
        current_monitor_marker = '+';

    if(is_regular_window(win))
        strcpy(regular_win_marker, COLOR_CLEAR);
    else
        strcpy(regular_win_marker, COLOR_YELLOW);

    printf("%c%c %sWindow 0x%x at (%d, %d), size (%d, %d)\tdesktop %d/%d monitor %d/%d (\"%s\")\t[%d]\n" COLOR_CLEAR,
           current_desktop_marker, current_monitor_marker, regular_win_marker, (unsigned int)win,
           geometry.x, geometry.y, geometry.width, geometry.height,
           get_window_desktop(display, win) + 1, 4, monitor_id + 1, settings.nb_monitors, name, nitems);
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
    XWindowAttributes attributes;
    Window retwin;
    int x, y;

    XGetWindowAttributes(display, window, &attributes);

    XTranslateCoordinates(display, window, root, 0, 0, &x, &y, &retwin);

    D(("Window is at (%d, %d), size (%d, %d), border %d",
       x, y,
       attributes.width, attributes.height,
       attributes.border_width));

    if(geometry != NULL) {
        geometry->x = x;
        geometry->y = y;
        geometry->width  = attributes.width;
        geometry->height = attributes.height;
    }
}

void
get_window_relative_geometry(Display *display, Window window, Geometry_t *geometry)
{
    XWindowAttributes attributes;
    Window retwin;
    int x, y;
    int i = get_window_monitor(window);

    XGetWindowAttributes(display, window, &attributes);

    XTranslateCoordinates(display, window, root, 0, 0, &x, &y, &retwin);

    if(geometry != NULL) {
        geometry->x = x - settings.monitors[i].workarea.x;
        geometry->y = y - settings.monitors[i].workarea.y;
        geometry->width  = attributes.width;
        geometry->height = attributes.height;
    }
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
            && actual_format == 32) {

        *left   = ((long *)data)[0];
        *right  = ((long *)data)[1];
        *top    = ((long *)data)[2];
        *bottom = ((long *)data)[3];
    } else {
        *left   = 0;
        *right  = 0;
        *top    = 0;
        *bottom = 0;
    }

    XFree(data);
}

/** Returns geometry of the workarea
 *
 * The "workarea" is the set of coordinates and sizes of
 * @deprecated single monitor only property: replaced by get_usable_area()
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
            && actual_format == 32) {

        *x = ((int *)data)[0];
        *y = ((int *)data)[1];
        *width = ((int *)data)[2];
        *height = ((int *)data)[3];
    } else {
        *x = -1;
        *y = -1;
        *width = -1;
        *height = -1;
    }

    XFree(data);
    return;
}

/** Compute the right x/y/width/height to fill the desired space by taking
 * border thickness into account.
 *
 * This function use to adjust size and position based on "frame extents" and other properties
 * but it didn't lead to sufficient enhancement. It has been removed since the behavior is ok
 * in most cases, Compiz being the notable weird one...
 */
void
fill_geometry(Display *display, Window window, Geometry_t geometry)
{
#if 0
    int right, left, top, bottom;

    D(("Adjusting: (%d, %d), (%d, %d)", geometry.x, geometry.y, geometry.width, geometry.height));

    /* get _NET_FRAME_EXTENTS */
    get_window_frame_extent(display, window, &left, &right, &top, &bottom);

    geometry.x      += left;
    geometry.y      += top;
    geometry.width  -= (left + right);
    geometry.height -= (top + bottom);

    D(("\t=> (%d, %d), (%d, %d)", geometry.x, geometry.y, geometry.width, geometry.height));

    if(settings.is_compiz) {
        int x, y, width, height;
        __compiz_get_window_adjustments(window, &x, &y, &width, &height);
        geometry.x      += x;
        geometry.y      += y;
        geometry.width  += width;
        geometry.height += height;

        D(("\t=> (%d, %d), (%d, %d) [compiz]", geometry.x, geometry.y, geometry.width, geometry.height));
    }
#endif
    move_resize_window(display, window, geometry);
}

/** Find which desktop a specific window belongs to
 * @return  Id of the desktop where the window has been found, defaults to 0 otherwise
 */
int
get_window_desktop(Display *display, Window window)
{
    if(settings.is_compiz)
        return -1;          /* not supported (@todo) */
    else
        return get_int_property(display, window, "_NET_WM_DESKTOP");
}

/** Find which monitor a specific window belongs to
 * @note Not supported in Compiz environment
 * @return  Id of the desktop where the window has been found (-1 if we are running compiz)
 * @todo Implement a workaround for compiz
 */
int
get_window_monitor(const Window window)
{
    /* let's save some time */
    if(settings.nb_monitors == 1)
        return 0;

    /* general case */
    int i = 0;
    Geometry_t w;
    get_window_geometry(display, window, &w);

    for(i = 0; i < settings.nb_monitors; i++) {
        if((w.x >= settings.monitors[i].infos.x && w.x < settings.monitors[i].infos.x + settings.monitors[i].infos.width)
                && (w.y >= settings.monitors[i].infos.y && w.y < settings.monitors[i].infos.y + settings.monitors[i].infos.height))
            return i;
    }

    /* default */
    return 0;
}

/** Check whether we are running in a Compiz environment or not
 *
 * Compiz doesn't behave like most other VMs, EMWH is not fully implemented
 * and shadowing disrupts window placement.
 *
 * Checking is based on:
 * @li forced through <code>--compiz</code> option at launch
 * @li presence of the <code>_COMPIZ_SUPPORTING_DM_CHECK</code> atom on the root window
 *
 */
void
check_compiz_wm()
{
    /* already set by "--compiz" option */
    if(settings.is_compiz)
        return;

    /* all standard get_wm_name-like failed so far... */

    /*
     * compiz has some atoms defined
     * another way could be to find the "switcher window" instancied by
     * compiz, which has a "compiz" WM_CLASS
     */
    Atom actual_type, atom;
    int actual_format, status = -1;
    unsigned long nitems, bytes_after;
    unsigned char *data = NULL;

    atom = XInternAtom(display, "_COMPIZ_SUPPORTING_DM_CHECK", 0);
    status = XGetWindowProperty(display, root, atom, 0, (~0L), 0,
                                AnyPropertyType, &actual_type, &actual_format,
                                &nitems, &bytes_after, &data);

    settings.is_compiz = (status == Success && nitems >= 1);
    XFree(data);
}

