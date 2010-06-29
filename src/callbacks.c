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

#include <stdlib.h>
#include <stdio.h>

#include "tiler.h"
#include "utils.h"
#include "callbacks.h"

void
dummy(void *data)
{
  printf("*** dummy ***\n");
}

void
grid(void *data)
{
  printf(COLOR_BOLD "*** Grid ***" COLOR_CLEAR " (not implemented)\n");
}

void
sidebyside(void *data)
{
  printf(COLOR_BOLD "*** Side by side ***" COLOR_CLEAR " (not implemented)\n");
}

void
move(void *data)
{
  if(data == NULL){
    D(("not implemented"));
    return;
  }

  Geometry_t geometry = * (Geometry_t *) data;  
  fill_geometry(display, get_active_window(display), geometry);
}

void
maximize(void *data)
{
  printf(COLOR_BOLD "*** Maximize ***" COLOR_CLEAR " (not implemented)\n");
}

void
changescreen(void *data)
{
  printf(COLOR_BOLD "*** Change Screen ***" COLOR_CLEAR " (not implemented)\n");
}
