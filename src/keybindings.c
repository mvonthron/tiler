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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "keybindings.h"
#include "callbacks.h"
#include "config.h"
#include "xactions.h"
#include "tiler.h"

unsigned int modifiers=0;

Binding_t bindings[MOVESLEN] = {
  {"top",         XK_VoidSymbol, move,         NULL},
  {"topright",    XK_VoidSymbol, move,         NULL},
  {"topleft",     XK_VoidSymbol, move,         NULL},
  {"bottom",      XK_VoidSymbol, move,         NULL},
  {"bottomright", XK_VoidSymbol, move,         NULL},
  {"bottomleft",  XK_VoidSymbol, move,         NULL},
  {"right",       XK_VoidSymbol, move,         NULL},
  {"left",        XK_VoidSymbol, move,         NULL},
  {"leftscreen",  XK_VoidSymbol, changescreen, NULL},
  {"rightscreen", XK_VoidSymbol, changescreen, NULL},
  {"grid",        XK_VoidSymbol, grid,         NULL},
  {"sidebyside",  XK_VoidSymbol, sidebyside,   NULL},
  {"maximize",    XK_VoidSymbol, maximize,     NULL},
};


void add_binding(Move_t move, KeySym keysym)
{
  extern Display *display;
  
  D(("binding \"%s\" to \"%s\"", 
         bindings[move].name, 
         XKeysymToString(keysym) ));
  
  bindings[move].keysym = keysym;
  
  /* set X listener */
  XGrabKey(display, XKeysymToKeycode(display, keysym), modifiers, 
           XDefaultRootWindow(display), 1, GrabModeAsync, GrabModeAsync);
}

void add_modifier(unsigned int modmask)
{
  modifiers |= modmask;
}

void dispatch(XEvent *event)
{
  int i=0;
  
  if (event->type == KeyPress) {
    XKeyEvent e = event->xkey;    
    KeySym keysym = XKeycodeToKeysym(e.display, e.keycode, 0);
    
    D((COLOR_GREEN "received \"%s\" event" COLOR_CLEAR, XKeysymToString(keysym)));
    
    for(i=0; i<MOVESLEN; i++){
      if(keysym == bindings[i].keysym){        
        if(bindings[i].callback != NULL)
          bindings[i].callback(bindings[i].data);
      }
    }
  }
}


