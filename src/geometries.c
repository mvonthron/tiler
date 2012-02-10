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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "tiler.h"
#include "utils.h"
#include "geometries.h"
#include "keybindings.h"
#include "config.h"
#include "xactions.h"


/** Compute data for each callback for a given monitor
 * @li calculates the geometries for top/bottom/... callbacks
 * @li calculates the reference of surrounding monitors (left/right for now)
 * Will allocate memory to store data
 *
 * @param[in]   monitor_id          targeted monitor (as identified by get_monitors_config())
 * @param[out]  monitor_bindings    pointer to arrays of keybindings
 *
 * @see get_monitors_config, Binding_t
 */
void compute_geometries_for_monitor(int monitor_id, Binding_t *monitor_bindings)
{
    if(monitor_id >= settings.nb_monitors || monitor_bindings == NULL)
        FATAL(("bad arguments"));

    D(("Computing data for monitor %d", monitor_id));

    /* recover only the monitor array we are interested in */
    Binding_t *bind = bindings[monitor_id];

    Geometry_t *top      = (Geometry_t *)malloc(sizeof(Geometry_t));
    Geometry_t *topright = (Geometry_t *)malloc(sizeof(Geometry_t));
    Geometry_t *topleft  = (Geometry_t *)malloc(sizeof(Geometry_t));

    Geometry_t *bottom      = (Geometry_t *)malloc(sizeof(Geometry_t));
    Geometry_t *bottomright = (Geometry_t *)malloc(sizeof(Geometry_t));
    Geometry_t *bottomleft  = (Geometry_t *)malloc(sizeof(Geometry_t));

    Geometry_t *right = (Geometry_t *)malloc(sizeof(Geometry_t));
    Geometry_t *left  = (Geometry_t *)malloc(sizeof(Geometry_t));

    Geometry_t *rightscreen = (Geometry_t *)malloc(sizeof(Geometry_t));
    Geometry_t *leftscreen  = (Geometry_t *)malloc(sizeof(Geometry_t));

    /* available space */
    int x = settings.monitors[monitor_id].workarea.x,
        y = settings.monitors[monitor_id].workarea.y,
        w = settings.monitors[monitor_id].workarea.width,
        h = settings.monitors[monitor_id].workarea.height;

    /* top */
    top->x      = x;
    top->y      = y;
    top->width  = w;
    top->height = (h/2);
    bind[TOP].data = top;

    /* topright */
    topright->x      = x+(w/2);
    topright->y      = y;
    topright->width  = (w/2);
    topright->height = (h/2);
    bind[TOPRIGHT].data = topright;

    /* topleft */
    topleft->x      = x;
    topleft->y      = y;
    topleft->width  = (w/2);
    topleft->height = (h/2);
    bind[TOPLEFT].data = topleft;

    /* bottom */
    bottom->x      = x;
    bottom->y      = y+(h/2);
    bottom->width  = w;
    bottom->height = (h/2);
    bind[BOTTOM].data = bottom;

    /* bottomright */
    bottomright->x      = x+(w/2);
    bottomright->y      = y+(h/2);
    bottomright->width  = (w/2);
    bottomright->height = (h/2);
    bind[BOTTOMRIGHT].data = bottomright;

    /* bottomleft */
    bottomleft->x      = x;
    bottomleft->y      = y+(h/2);
    bottomleft->width  = (w/2);
    bottomleft->height = (h/2);
    bind[BOTTOMLEFT].data = bottomleft;

    /* right */
    right->x      = x+(w/2);
    right->y      = y;
    right->width  = (w/2);
    right->height = h;
    bind[RIGHT].data = right;

    /* left */
    left->x      = x;
    left->y      = y;
    left->width  = (w/2);
    left->height = h;
    bind[LEFT].data = left;

    /** screen change data */
    if(settings.nb_monitors > 0){
        int i;
        for(i=0; i<settings.nb_monitors; i++){
            if(i == monitor_id)
                continue;

            if(get_relative_position(settings.monitors[monitor_id].infos, settings.monitors[i].infos) == LEFTOF){
                *leftscreen = settings.monitors[i].workarea;
                bind[LEFTSCREEN].data = leftscreen;
            }else if(get_relative_position(settings.monitors[monitor_id].infos, settings.monitors[i].infos) == RIGHTOF){
                *rightscreen = settings.monitors[i].workarea;
                bind[RIGHTSCREEN].data = rightscreen;
            }
        }
    }
}


/**
 * @brief Custom workarea finder handling multiple screens
 *
 * Standard way is to use <code>_NET_WORKAREA</code> atom. However it
 * is not suited for multi-monitors systems (returns a single rectangle large enough to
 * contains every monitor).
 *
 * The real algorithm would me a Largest empty rectangle problem feeded with all
 * system windows (docks mostly). Currently we simplify a lot by assuming we only have docks
 * on the top or on the bottom and we remove shrink the monitor's size base on the dock's geometries
 *
 * @pre monitor physical size should be available (get_monitors_config())
 * @param[in]   monitor_id  target monitor
 * @param[out]  area        where to put result of calculation
 */
void get_usable_area(int monitor_id, Geometry_t *area)
{
    /* _NET_WORKAREA atom doesn't fit for multiple screen */
    Window *window_list = NULL;
    int i, size;

    *area = settings.monitors[monitor_id].infos;

    size = list_windows(display, root, &window_list, LIST_SYSTEM);

    if(window_list == NULL)
      return;

    /* we simplify greatly the calculation, at least for now
      real problem is the "largest empty rectangle problem" */

    Geometry_t geo;
    int mon;
    for(i=0; i< size; i++){
        if((mon = get_window_monitor(window_list[i])) != monitor_id)
            continue;

        get_window_geometry(display, window_list[i], &geo);

        if(geo.width >= settings.monitors[monitor_id].infos.width
                && geo.height >= settings.monitors[monitor_id].infos.height)
            continue;

        if(geo.y < settings.monitors[monitor_id].infos.height/2){
            area->y = geo.y + geo.height;
            area->height -= geo.height;
        }else{
            area->height -= geo.height;
        }
    }

    D(("Usable area found: (%d, %d) (%d, %d) on monitor %d", area->x, area->y, area->width, area->height, monitor_id));

    free(window_list);
}

Position_t
get_relative_position(Geometry_t base, Geometry_t target)
{
    Position_t ret;
    if(base.x >= target.x + target.width)
        ret = LEFTOF;
    else if(base.x + base.width <= target.x)
        ret = RIGHTOF;
    else if(base.y >= target.y + target.height)
        ret = BOTTOMOF;
    else if(base.y + base.height <= target.y)
        ret = TOPOF;
    else
        ret = UNKNOWNPOS;

    return ret;
}

void
print_geometries()
{
    int i=0, monitor_id=0;
    char arg_buffer[64] = "\0";
    char key_buffer[32] = "\0";

    for(monitor_id=0; monitor_id<settings.nb_monitors; monitor_id++){
        printf(COLOR_BOLD"Geometries (monitor %d):\n"COLOR_CLEAR, monitor_id);

        /* screen size */
        int x = settings.monitors[monitor_id].workarea.x,
            y = settings.monitors[monitor_id].workarea.y,
            w = settings.monitors[monitor_id].workarea.width,
            h = settings.monitors[monitor_id].workarea.height;
        printf("  - "COLOR_BOLD"work area"COLOR_CLEAR"        (%d, %d), (%d, %d)\n", x, y, w, h);

        for(i=0; i<MOVESLEN; i++){
            if(bindings[monitor_id][i].data == NULL){
                sprintf(arg_buffer, "(null)");
            }else if(bindings[monitor_id][i].data != NULL && bindings[monitor_id][i].callback == move){
                Geometry_t *data = (Geometry_t *) bindings[monitor_id][i].data;
                sprintf(arg_buffer, "(%d, %d), (%d, %d)", data->x, data->y, data->width, data->height);
            }

            if(bindings[monitor_id][i].keysym == XK_VoidSymbol){
                sprintf(key_buffer, "(null)");
            }else{
                sprintf(key_buffer, "[%s]", XKeysymToString(bindings[monitor_id][i].keysym));
            }

            printf("  - %-16s %-40s %s\n", bindings[monitor_id][i].name, arg_buffer, key_buffer);
        }
    }
}

