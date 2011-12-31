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

#ifndef GEOMETRIES_H
#define GEOMETRIES_H


#include <X11/Xlib.h>
#include "tiler.h"
#include "utils.h"
#include "keybindings.h"

typedef enum {
    RIGHTOF,
    LEFTOF,
    TOPOF,
    BOTTOMOF,
    UNKNOWNPOS
} Position_t;

void get_usable_area(int, Geometry_t *);
void compute_geometries_for_monitor(int, Binding_t *);
Position_t get_relative_position(Geometry_t, Geometry_t);

#endif /* GEOMETRIES_H */
