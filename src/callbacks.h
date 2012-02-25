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

#ifndef CALLBACKS_H
#define CALLBACKS_H

/**
  @page Callbacks

  Callbacks are the final actions made available through the configuration file
  They are matched with a registered key shortcut.

  A callback is a pointer to function taking a <code>void *</code> argument. In some
  cases, this argument is not needed and therefore omitted. For instance, maximize()
  takes the active window and ask for its maximization directly through XLib, there is
  no argument needed for this. The move() function however needs a Geometry_t argument
  representing the rectangle shape we want to fill with the active window. Theses arguments
  are pre-calculated at launch with compute_geometries_for_monitor().

  Available callbacks:
  @li dummy()
  @li move()
  @li grid()
  @li sidebyside()
  @li changescreen()
  @li maximize()
  @li listwindows()
  */


void dummy(void *);

void move(void *);

void grid(void *);

void sidebyside(void *);

void changescreen(void *);

void maximize(void *);

void listwindows(void *);

#endif /* CALLBACKS_H */
