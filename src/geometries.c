/**
 * Copyright (c) 2011 Manuel Vonthron <manuel.vonthron@acadis.org>
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



void compute_geometries_for_monitor(int monitor_id, Binding_t *monitor_bindings)
{
    if(monitor_id >= settings.nb_monitors || monitor_bindings == NULL)
        FATAL(("bad arguments"));

    D(("Computing data for monitor %d", monitor_id));

    Geometry_t area;
    get_usable_area(monitor_id, &area);

    Binding_t *bind = bindings[monitor_id];

    Geometry_t *top      = (Geometry_t *)malloc(sizeof(Geometry_t));
    Geometry_t *topright = (Geometry_t *)malloc(sizeof(Geometry_t));
    Geometry_t *topleft  = (Geometry_t *)malloc(sizeof(Geometry_t));

    Geometry_t *bottom      = (Geometry_t *)malloc(sizeof(Geometry_t));
    Geometry_t *bottomright = (Geometry_t *)malloc(sizeof(Geometry_t));
    Geometry_t *bottomleft  = (Geometry_t *)malloc(sizeof(Geometry_t));

    Geometry_t *right = (Geometry_t *)malloc(sizeof(Geometry_t));
    Geometry_t *left  = (Geometry_t *)malloc(sizeof(Geometry_t));

    /* available space */
    int x = area.x, y=area.y, w=area.width, h=area.height;

    /* top */
    top->x      = 0 /*x*/;
    top->y      = 0 /*y*/;
    top->width  = w;
    top->height = (int)(h/2);
    bind[TOP].data = top;

    /* topright */
    topright->x      = (int)(w/2);
    topright->y      = 0 /*y*/;
    topright->width  = (int)(w/2);
    topright->height = (int)(h/2);
    bind[TOPRIGHT].data = topright;

    /* topleft */
    topleft->x      = 0 /*x*/;
    topleft->y      = 0 /*y*/;
    topleft->width  = (int)(w/2);
    topleft->height = (int)(h/2);
    bind[TOPLEFT].data = topleft;

    /* bottom */
    bottom->x      = 0;
    bottom->y      = (int)(h/2)/*+y*/;
    bottom->width  = w;
    bottom->height = (int)(h/2);
    bind[BOTTOM].data = bottom;

    /* bottomright */
    bottomright->x      = (int)(w/2);
    bottomright->y      = (int)(h/2)/*+y*/;
    bottomright->width  = (int)(w/2);
    bottomright->height = (int)(h/2);
    bind[BOTTOMRIGHT].data = bottomright;

    /* bottomleft */
    bottomleft->x      = x;
    bottomleft->y      = (int)(h/2)/*+y*/;
    bottomleft->width  = (int)(w/2);
    bottomleft->height = (int)(h/2);
    bind[BOTTOMLEFT].data = bottomleft;

    /* right */
    right->x      = (int)(w/2);
    right->y      = 0;
    right->width  = (int)(w/2);
    right->height = h;
    bind[RIGHT].data = right;

    /* left */
    left->x      = 0;
    left->y      = 0;
    left->width  = (int)(w/2);
    left->height = h;
    bind[LEFT].data = left;

}


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
        if(geo.width == settings.monitors[monitor_id].infos.width
                && geo.height == settings.monitors[monitor_id].infos.height)
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
