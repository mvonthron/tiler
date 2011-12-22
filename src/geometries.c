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

#include "utils.h"
#include "geometries.h"
#include "keybindings.h"
#include "config.h"
#include "tiler.h"


void compute_geometries_for_monitor(int monitor_id, Binding_t *monitor_bindings)
{
    if(monitor_id >= settings.nb_monitors || monitor_bindings == NULL)
        FATAL(("bad arguments"));

    D(("Computing data for monitor %d", monitor_id));

    Geometry_t area;
    get_usable_area(monitor_id, &area);


}


void get_usable_area(int monitor_id, Geometry_t *area)
{
    /* _NET_WORKAREA atom doesn't fit for multiple screen */



}
