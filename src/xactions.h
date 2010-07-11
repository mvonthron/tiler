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

#ifndef XACTIONS_H
#define XACTIONS_H


#include <X11/Xlib.h>
#include "tiler.h"
#include "utils.h"

Window get_active_window();
int get_desktop(Display *, Window);
int list_windows(Display*, Window, Window **, int);
bool window_is_maximized(Display *, Window);
void unmaximize_window(Display *, Window);

int get_nb_desktop(Display *);
int get_nb_screens(Display *);
void get_workarea(Display *, Window, int *, int *, int *, int *);
             
void get_window_geometry(Display *, Window, Geometry_t *);
void get_window_frame_extent(Display *, Window, int *, int *, int *, int *);
void print_window(Display *, Window);

void send_xevent(Display *, Window, Atom, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);
void move_window(Display *, Window, Geometry_t);
void move_resize_window(Display *, Window, Geometry_t);
void fill_geometry(Display *, Window, Geometry_t);

#endif /* XACTIONS_H */