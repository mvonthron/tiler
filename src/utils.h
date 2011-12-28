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

#ifndef UTILS_H
#define UTILS_H


#define COLOR_GREEN   "\033[92m"
#define COLOR_RED     "\033[91m"
#define COLOR_BLUE    "\033[94m"
#define COLOR_YELLOW  "\033[93m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_CLEAR   "\033[0m"

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define STREQ(str1, str2) (strcmp((str1), (str2)) == 0)

#ifdef DEBUG
#define D(msg) do {                                                     \
    printf("%s[%s:%s(%d)]%s ",COLOR_YELLOW, __FILE__, __FUNCTION__, __LINE__, COLOR_CLEAR); \
    printf msg;                                                         \
    printf("\n");                                                       \
  } while(0);
#else
#define D(msg)
#endif

#define FATAL(msg) do {                                                     \
    printf("%s[%s:%s(%d)]%s ",COLOR_RED, __FILE__, __FUNCTION__, __LINE__, COLOR_CLEAR); \
    printf msg;                                                         \
    printf("\n");                                                       \
    cleanup();                                                          \
    exit(1);                                                            \
  } while (0);

#define TODO(msg) do {                                                     \
    printf("%s[%s:%s(%d)] TODO: %s ",COLOR_BLUE, __FILE__, __FUNCTION__, __LINE__, COLOR_CLEAR); \
    printf msg;                                                         \
    printf("\n");                                                       \
    } while(0);

typedef int bool;
#define false (0)
#define true (1)

#define FREE(ptr) if(ptr != NULL) {     \
    free(ptr);                          \
    ptr = NULL;                         \
};


#endif /* UTILS_H */
