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

#ifndef CONFIG_H
#define CONFIG_H

#include "tiler.h"
#include "utils.h"

struct settings_monitor_t {
    int id;
    Geometry_t infos;
    char *name;
};

extern struct settings_t {
  struct settings_monitor_t *monitors;
  bool verbose;
  bool foreground;
  bool is_compiz;
  bool force_run;
  int nb_monitors;
  char filename[128];
  char pidfile[128];
} settings;

void parse_opt(int, char **);
void parse_conf_file(char *);
void usage();
void version();

void compute_geometries(Display *, Window);
int get_monitors_config(Display *, Window);
void free_config();

void print_config();
void print_geometries();

#endif /* CONFIG_H */
