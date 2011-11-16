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

#ifndef KEYBINDINGS_H
#define KEYBINDINGS_H

#include <X11/keysym.h>
#include "tiler.h"
#include "callbacks.h"

typedef struct {
    char *name;
    KeySym keysym;   /* default XK_VoidSymbol => NULL */
    void (*callback)(void*);
    void *data;
} Binding_t;


unsigned int modifiers;
extern Binding_t bindings[MOVESLEN];

void add_binding(Move_t, KeySym);
void clear_bindings();
void add_modifier(unsigned int);

void dispatch(XEvent *);
void print_key_event(const XKeyEvent);


#endif /* KEYBINDINGS_H */
