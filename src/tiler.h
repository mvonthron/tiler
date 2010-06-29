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

#ifndef TILER_H
#define TILER_H

#include <X11/Xlib.h>

/**
 * 
 */
typedef struct {
  int x;
  int y;
  int width;
  int height;
} Geometry_t;

/**
 * 
 */
typedef enum {
  TOP,
  TOPRIGHT,
  TOPLEFT,
  BOTTOM,
  BOTTOMRIGHT,
  BOTTOMLEFT,
  RIGHT,
  LEFT,
  
  LEFTSCREEN,
  RIGHTSCREEN,
  
  GRID,
  SIDEBYSIDE,
  MAXIMIZE,
  
  MOVESLEN
} Move_t;

/**
 * 
 */
static const char 
move_names[32][MOVESLEN] = {
  "top",
  "topright",
  "topleft",
  "bottom",
  "bottomright",
  "bottomleft",
  "right",
  "left",
  
  "leftscreen",
  "rightscreen",
  
  "grid",
  "maximize",
};

/**
 * 
 */
static const Geometry_t 
move_geometries[MOVESLEN] = {
  {-1, -1, -1, -1},     /* top */
  {512,   0, 512, 358}, /* topright */
  {  0,   0, 512, 358}, /* topleft */
  {-1, -1, -1, -1},     /* bottom */
  {512, 384, 512, 358}, /* bottomright */
  {  0, 384, 512, 358}, /* bottomleft */
  {-1, -1, -1, -1},     /* right */
  {-1, -1, -1, -1},     /* left */
  
  {-1, -1, -1, -1},
  {-1, -1, -1, -1},
  
  {-1, -1, -1, -1},
  {-1, -1, -1, -1},
};


/**
 * 
 */
extern Display *display;
extern Window root;

#endif /* TILER_H */
