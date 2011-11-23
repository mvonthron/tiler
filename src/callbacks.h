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

#ifndef CALLBACKS_H
#define CALLBACKS_H

/**
 * dummy function for test purpose 
 * @param [in] data unused
 * @ingroup debug
 */
void dummy(void *);

/**
 * standard move window function
 * @param [in] data geometry_t structure
 * @todo rename
 */
void move(void *);

/**
 * organize windows in current desktop on a grid
 * @param [in] data unused
 */
void grid(void *);

/**
 * organize the two first windows in stack side by side
 * @param [in] data unused
 */
void sidebyside(void *);

/**
 * change screen
 * @param [in] data unused
 * @todo unimplemented
 */
void changescreen(void *);

/**
 * maximize active window
 * people may use standard ALT+F10 though
 * @param [in] data unused
 */
void maximize(void *);

/**
 * list windows on current desktop
 * @param [in] data unused
 * @ingroup debug
 */
void listwindows(void *);

#endif /* CALLBACKS_H */
