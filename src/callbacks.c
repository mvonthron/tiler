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
#include "config.h"
#include "utils.h"
#include "keybindings.h"
#include "xactions.h"
#include "callbacks.h"

void
dummy(void *data)
{
  D(("*** dummy ***"));
}

void
grid(void *data)
{
  Window *window_list=NULL;
  int size=-1, monitor=-1;
  
  size = list_windows(display, root, &window_list, LIST_DEFAULT);
  monitor = get_window_monitor(get_active_window());
  D(("Nb windows on desktop : %d", size));
  
  if(window_list == NULL)
    return;
  
  /* single window on desktop */
  if(size == 1){
    maximize(NULL);
    return;
  }
  
  /* two windows on desktop, youhou party time \o/ */
  if(size == 2){
    sidebyside(NULL);
    return;
  }
  
  /* two windows on desktop, youhou party time \o/ */
  if(size == 3){
    Geometry_t left        = * (Geometry_t *) bindings[monitor][LEFT].data;
    Geometry_t topright    = * (Geometry_t *) bindings[monitor][TOPRIGHT].data;
    Geometry_t bottomright = * (Geometry_t *) bindings[monitor][BOTTOMRIGHT].data;
    
    fill_geometry(display, window_list[size-1], left);
    fill_geometry(display, window_list[size-2], topright);
    fill_geometry(display, window_list[size-3], bottomright);
    return;
  }
  
  /* stop at 4 win on squared grid for now */
  Geometry_t topleft     = * (Geometry_t *) bindings[monitor][TOPLEFT].data;
  Geometry_t topright    = * (Geometry_t *) bindings[monitor][TOPRIGHT].data;
  Geometry_t bottomleft  = * (Geometry_t *) bindings[monitor][BOTTOMLEFT].data;
  Geometry_t bottomright = * (Geometry_t *) bindings[monitor][BOTTOMRIGHT].data;
  
  fill_geometry(display, window_list[size-1], topleft);
  fill_geometry(display, window_list[size-2], topright);
  fill_geometry(display, window_list[size-3], bottomleft);
  fill_geometry(display, window_list[size-4], bottomright);
}

void
sidebyside(void *data)
{
  Window *window_list=NULL;
  int size=-1, monitor=-1;
  
  size = list_windows(display, root, &window_list, LIST_DEFAULT);
  monitor = get_window_monitor(get_active_window());

  if(size < 2 || window_list == NULL)
    return;
  
  Geometry_t left  = * (Geometry_t *) bindings[monitor][LEFT].data;
  Geometry_t right = * (Geometry_t *) bindings[monitor][RIGHT].data;
  
  fill_geometry(display, window_list[size-1], left);
  fill_geometry(display, window_list[size-2], right);
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
  maximize_window(display, get_active_window());
}

void
changescreen_left(void *data)
{
   if(data == NULL)
       return;

    D((COLOR_BOLD "*** Change Screen LEFT ***" COLOR_CLEAR " (not implemented)"));
    Geometry_t monitor = * (Geometry_t *) data;
    Window win = get_active_window(display);
    Geometry_t current_position, new_position;
    get_window_geometry(display, win, &current_position);
    new_position = current_position;

    /* simple version without size checks */
    new_position.x = current_position.x - (monitor.width);
    fill_geometry(display, win, new_position);
}

void
changescreen_right(void *data)
{
    if(data == NULL)
        return;

    D((COLOR_BOLD "*** Change Screen RIGHT ***" COLOR_CLEAR " (not implemented)"));
    Geometry_t monitor = * (Geometry_t *) data;
    Window win = get_active_window(display);
    Geometry_t current_position, new_position;
    get_window_geometry(display, win, &current_position);
    new_position = current_position;

    new_position.x = current_position.x + (monitor.x);
    fill_geometry(display, win, new_position);
}

/** debug & info callbacks */
void
listwindows(void *data)
{
    Window *window_list=NULL;
    int size=-1, i=0;

    size = list_windows(display, root, &window_list, LIST_ALL);
    D(("Nb windows on desktop : %d", size));

    if(window_list == NULL)
      return;

    for(i=0; i< size; i++){
        print_window(display, window_list[i]);
    }
}
