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
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "keybindings.h"
#include "geometries.h"
#include "callbacks.h"
#include "config.h"
#include "xactions.h"
#include "tiler.h"

unsigned int modifiers=0;

/**
 * contains the reference table of bindings
 * we need one table like this one for each monitor
 */
const Binding_t bindings_reference[MOVESLEN] = {
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
  {"listwindows", XK_VoidSymbol, listwindows,  NULL},
};

/**
 * real binding data array
 * contains bindings_reference * nb_screens
 */
Binding_t **bindings = NULL;

/**
 * register X listener
 */
void grab(const KeyCode code, const unsigned int mod)
{
    XGrabKey(display, code, mod,
             XDefaultRootWindow(display), 1, GrabModeAsync, GrabModeAsync);
}

/**
 * release X listener
 */
void ungrab(const KeyCode code, const unsigned int mod)
{
    XUngrabKey(display, code, mod,
               XDefaultRootWindow(display));
}

/**
 * setup match between keysym key shortcut and move action
 * @param move      action to be executed
 * @param keysym    shortcut wanted for this action
 * @see Binding_t
 */
void add_binding(Move_t move, KeySym keysym)
{
  if(bindings == NULL)
      FATAL(("bindings structure not initialized"));

  int i;
  
  D(("binding \"%s\" to \"%s\"", 
         bindings_reference[move].name,
         XKeysymToString(keysym) ));

  /* same key shortcut for all monitors */
  for (i=0; i<settings.nb_monitors; i++)
    bindings[i][move].keysym = keysym;

  /* set X listening event */
  grab(XKeysymToKeycode(display, keysym), modifiers);
  /* ignoring annoying NumLock (Mod2Mask) key */
  grab(XKeysymToKeycode(display, keysym), modifiers | Mod2Mask);
}


/**
 * add a modifier to observed key sequence
 * modifiers are keys such as Ctrl, Alt, Super...
 *
 * @todo currently a single modifier for all key shortcuts
 */
void add_modifier(unsigned int modmask)
{
  modifiers |= modmask;
}

/**
 * pre-compute data needed by callback functions
 * setup an array of Binding_t for each monitor
 * geometries and other callback data are differents for each monitor
 * actual data argument computation done in compute_geometries_for_monitor()
 *
 * "bindings" example structure (for two monitors):
 * <pre>
 *   bindings = {
 *      [0] = {
 *        {"top",         XK_VoidSymbol, move,         NULL},
 *        {"topright",    XK_VoidSymbol, move,         NULL},
 *        {"topleft",     XK_VoidSymbol, move,         NULL},
 *        {"bottom",      XK_VoidSymbol, move,         NULL},
 *        {"bottomright", XK_VoidSymbol, move,         NULL},
 *        {"bottomleft",  XK_VoidSymbol, move,         NULL},
 *        {"right",       XK_VoidSymbol, move,         NULL},
 *        {"left",        XK_VoidSymbol, move,         NULL},
 *        {"leftscreen",  XK_VoidSymbol, changescreen, NULL},
 *        {"rightscreen", XK_VoidSymbol, changescreen, NULL},
 *        {"grid",        XK_VoidSymbol, grid,         NULL},
 *        {"sidebyside",  XK_VoidSymbol, sidebyside,   NULL},
 *        {"maximize",    XK_VoidSymbol, maximize,     NULL},
 *        {"listwindows", XK_VoidSymbol, listwindows,  NULL},
 *      },
 *      [1] = {
 *        {"top",         XK_VoidSymbol, move,         NULL},
 *        {"topright",    XK_VoidSymbol, move,         NULL},
 *        {"topleft",     XK_VoidSymbol, move,         NULL},
 *        {"bottom",      XK_VoidSymbol, move,         NULL},
 *        {"bottomright", XK_VoidSymbol, move,         NULL},
 *        {"bottomleft",  XK_VoidSymbol, move,         NULL},
 *        {"right",       XK_VoidSymbol, move,         NULL},
 *        {"left",        XK_VoidSymbol, move,         NULL},
 *        {"leftscreen",  XK_VoidSymbol, changescreen, NULL},
 *        {"rightscreen", XK_VoidSymbol, changescreen, NULL},
 *        {"grid",        XK_VoidSymbol, grid,         NULL},
 *        {"sidebyside",  XK_VoidSymbol, sidebyside,   NULL},
 *        {"maximize",    XK_VoidSymbol, maximize,     NULL},
 *        {"listwindows", XK_VoidSymbol, listwindows,  NULL},
 *      },
 *   }
 * </pre>
 *
 * @see Binding_t, compute_geometries_for_monitor
 */
void setup_bindings_data()
{
    if(settings.monitors == NULL){
        FATAL(("\"settings.monitors\" is not available. (did you call setup_config first ?)"));
    }

    bindings = (Binding_t **)malloc((settings.nb_monitors)*sizeof(Binding_t *));
    if(bindings == NULL)
        FATAL(("Could not allocate memory for binding data"));

    int i=0;
    for(i=0; i<settings.nb_monitors; i++){

        bindings[i] = (Binding_t *)malloc(sizeof(bindings_reference));
        if(bindings[i] == NULL)
            FATAL(("Could not allocate memory for binding data"));

        /* fill structure with default and constant values */
        memcpy(bindings[i], bindings_reference, sizeof(bindings_reference));

        /* compute specific data for each monitor */
        compute_geometries_for_monitor(i, bindings[i]);
    }
}

/**
 * free allocated memory for bindings data and release key shortcut watchpoints
 */
void clear_bindings()
{
    int m=0, i=0;

    for(m=0; m<settings.nb_monitors; m++){
        for(i=0; i<MOVESLEN; i++){
            if(XK_VoidSymbol != bindings[m][i].keysym){
                ungrab(XKeysymToKeycode(display, bindings[m][i].keysym), modifiers);
                ungrab(XKeysymToKeycode(display, bindings[m][i].keysym), modifiers | Mod2Mask);
                bindings[m][i].keysym = XK_VoidSymbol;
            }

            if(bindings[m][i].data != NULL){
                free(bindings[m][i].data);
                bindings[m][i].data = NULL;
            }
        }
    }
}

/**
 * @brief converts XKeyEvent to string and prints it
 * @param event     received event to be printed
 * @param with_modifiers    enable/disable printing of modifiers ("Ctrl"...) keys
 * @ingroup debug
 */
void print_key_event(const XKeyEvent event, const bool with_modifiers)
{
    char keystring[80] = "\0";

    if(with_modifiers){
        if (event.state & ControlMask)
            strcat(keystring, "Ctrl + ");
        if (event.state & ShiftMask)
            strcat(keystring, "Shift + ");
        if (event.state & LockMask)
            strcat(keystring, "CapsLock + ");
        if (event.state & Mod1Mask)
            strcat(keystring, "Alt + ");
//        if (event.state & Mod2Mask)
//            strcat(keystring, "NumLock + ");
        if (event.state & Mod3Mask)
            strcat(keystring, "RAlt + ");
        if (event.state & Mod4Mask)
            strcat(keystring, "Win + ");
        if (event.state & Mod5Mask)
            strcat(keystring, "AltGr + ");
    }

    strcat(keystring, XKeysymToString( XKeycodeToKeysym(event.display, event.keycode, 0) ));

    printf(COLOR_GREEN "received \"%s\" key press" COLOR_CLEAR "\n", keystring);
}

/**
 * If received event is a key sequence, loop over all registered key shortcut
 * until we have a match. Call registered callback (if any)
 *
 * @param event received event to match with
 */
void dispatch(XEvent *event)
{
  int i=0;
  
  if (event->type == KeyPress) {
    XKeyEvent e = event->xkey;
    KeySym keysym = XKeycodeToKeysym(e.display, e.keycode, 0);
    
    int monitor = get_window_monitor(get_active_window());

    if(settings.verbose){
        print_key_event(e, true);
    }
    
    for(i=0; i<MOVESLEN; i++){
      if(keysym == bindings[monitor][i].keysym){
        if(bindings[monitor][i].callback != NULL){
          bindings[monitor][i].callback(bindings[monitor][i].data);
          if(settings.verbose)
              printf(" > calling \"%s\"\n", bindings[monitor][i].name);
        }
      }
    }
  }
}


