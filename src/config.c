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
#include <getopt.h>
#include <math.h>
#include <assert.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "tiler.h"
#include "utils.h"
#include "xactions.h"
#include "keybindings.h"
#include "config.h"


static const char *optstring = "hvfFc:V";
static const struct option longopts[] = {
  {"foreground",  0, NULL, 'f'},
  {"force",       0, NULL, 'F'},
  {"compiz",      0, NULL, 'C'},
  {"verbose",     0, NULL, 'v'},
  {"version",     0, NULL, 'V'},
  {"help",        0, NULL, 'h'},
  {NULL,          0, NULL, 0},
};

struct settings_t settings = {
  false,            /* verbose */
  false,            /* foreground */
  false,            /* is_compiz */
  false,            /* force_run */
  "",               /* conf filename */
  "/tmp/tiler.pid", /* pid filename */
};

/**
 * 
 */
void 
usage()
{
  printf("usage: tiler [options]\n"
         "options: \n"
         "  -f  --foreground            Run in foreground rather than as a daemon \n"
         "  -F  --force                 Force program to start even if a pid file is detected \n"
         "  -c  --config-file <file>    Use <file> instead of ~/.config/tiler.conf as a configuration file \n"
         "      --compiz                Force Compiz behaviour even if not detected\n"
         "  -v  --verbose               Print various messages \n"
         "  -V  --version               Print version number and exit \n"
         "  -h  --help                  Print this message and exit \n"
        );

  exit(0);
}

/**
 * 
 */
void
version()
{
  printf("tiler v0.1a \t(built "__DATE__")\n");
  
  exit(0);
}

/**
 * 
 */
void
parse_opt(int argc, char **argv)
{
  int opt, index;
  
  while((opt = getopt_long(argc, argv, optstring, longopts, &index)) != -1){
    switch(opt){
    case 'h':
      usage();
      break;
    case 'v':
      settings.verbose = true;
      break;
    case 'c':
      strcpy(settings.filename, optarg);
      break;
    case 'C':
      settings.is_compiz = true;
      break;
    case 'V':
      version();
      break;
    case 'F':
      settings.force_run = true;
      break;
    case 'f':
      settings.foreground = true;
      break;
    case '?':
    default:
      break;
    }
  }
  
  if( STREQ(settings.filename, "") ){
    strcpy(settings.filename, getenv("HOME"));
    strcat(settings.filename, "/.config/tiler.conf");
  }
}

/**
 * 
 */
static void 
parse_line(char *token, char *value)
{
  int keysym=NoSymbol, i=0;
  Move_t move=MOVESLEN;
  
  /**
   * special case for modifier masks
   */
  if( STREQ(token, "modifier") ){
    char *subvalue = strtok(value, "+");
    
    while(subvalue != NULL){
      /* modifier masks */
      if     (STREQ(subvalue, "CTRL"))  add_modifier(ControlMask);
      else if(STREQ(subvalue, "SHIFT")) add_modifier(ShiftMask);
      else if(STREQ(subvalue, "ALT"))   add_modifier(Mod1Mask);
      else if(STREQ(subvalue, "SUPER")) add_modifier(Mod2Mask);
      
      subvalue = strtok(NULL, "+");
    }
    
    return;
  }
  
  /**
   * token parsing
   */
  for(i=0; i<MOVESLEN; i++)
    if( STREQ(token, bindings[i].name) )
      move = (Move_t) i;
  
  if(move == MOVESLEN){
    D(("Unknown token \"%s\"", token));
    return;
  }
    
  /**
   * value parsing 
   */
  if(value[0] == '\0') 
    return;

  keysym = XStringToKeysym(value);
  if(keysym != NoSymbol)
    add_binding(move, keysym); /* actual key binding */
}

/**
 * 
 */
void
parse_conf_file(char *filename)
{
  FILE *fd;
  char buffer[256];
  char token[32];
  char value[32];
  
  fd = fopen(filename, "r");
  
  if(fd == NULL){
    FATAL(("Unable to open \"%s\"", filename));
  }
  
  while (fgets(buffer, sizeof(buffer), fd) != NULL) {
    if(*buffer != '#' && *buffer != '\n' ){
      sscanf(buffer, "%s = %s", token, value);
      parse_line(token, value);
    }
    
    /* reset */
    *buffer = *token = *value = '\0';
  }
  
  fclose(fd);
}

void
compute_geometries(Display *display, Window root)
{
  extern Binding_t bindings[MOVESLEN];
  Geometry_t *top      = (Geometry_t *)malloc(sizeof(Geometry_t));
  Geometry_t *topright = (Geometry_t *)malloc(sizeof(Geometry_t));
  Geometry_t *topleft  = (Geometry_t *)malloc(sizeof(Geometry_t));
  
  Geometry_t *bottom      = (Geometry_t *)malloc(sizeof(Geometry_t));
  Geometry_t *bottomright = (Geometry_t *)malloc(sizeof(Geometry_t));
  Geometry_t *bottomleft  = (Geometry_t *)malloc(sizeof(Geometry_t));
  
  Geometry_t *right = (Geometry_t *)malloc(sizeof(Geometry_t));
  Geometry_t *left  = (Geometry_t *)malloc(sizeof(Geometry_t));

  /* available space */
  int x, y, w, h;
  get_workarea(display, root, &x, &y, &w, &h);

  /* top */
  top->x      = 0 /*x*/;
  top->y      = 0 /*y*/;
  top->width  = w;
  top->height = (int)(h/2);
  bindings[TOP].data = top;
  
  /* topright */
  topright->x      = (int)(w/2);
  topright->y      = 0 /*y*/;
  topright->width  = (int)(w/2);
  topright->height = (int)(h/2);
  bindings[TOPRIGHT].data = topright;
  
  /* topleft */
  topleft->x      = 0 /*x*/;
  topleft->y      = 0 /*y*/;
  topleft->width  = (int)(w/2);
  topleft->height = (int)(h/2);
  bindings[TOPLEFT].data = topleft;

  /* bottom */
  bottom->x      = 0;
  bottom->y      = (int)(h/2)/*+y*/;
  bottom->width  = w;
  bottom->height = (int)(h/2);
  bindings[BOTTOM].data = bottom;
  
  /* bottomright */
  bottomright->x      = (int)(w/2);
  bottomright->y      = (int)(h/2)/*+y*/;
  bottomright->width  = (int)(w/2);
  bottomright->height = (int)(h/2);
  bindings[BOTTOMRIGHT].data = bottomright;
  
  /* bottomleft */
  bottomleft->x      = x;
  bottomleft->y      = (int)(h/2)/*+y*/;
  bottomleft->width  = (int)(w/2);
  bottomleft->height = (int)(h/2);
  bindings[BOTTOMLEFT].data = bottomleft;
  
  /* right */
  right->x      = (int)(w/2);
  right->y      = 0;
  right->width  = (int)(w/2);
  right->height = h;
  bindings[RIGHT].data = right;
  
  /* left */
  left->x      = 0;
  left->y      = 0;
  left->width  = (int)(w/2);
  left->height = h;
  bindings[LEFT].data = left;
}

/**
 *
 */
void
print_config()
{
    printf(COLOR_BOLD"Configuration:\n"COLOR_CLEAR \
           "  - verbose          %s \n"\
           "  - foreground       %s \n"\
           "  - is compiz        %s \n"\
           "  - force run        %s \n"\
           "  - config file      %s \n"\
           "  - pid file         %s \n",
           (settings.verbose ? "true" : "false"),
           (settings.foreground ? "true" : "false"),
           (settings.is_compiz ? "true" : "false"),
           (settings.force_run ? "true" : "false"),
           settings.filename, settings.pidfile
           );

}

void
print_geometries()
{
    int i=0;
    char arg_buffer[64] = "\0";
    char key_buffer[32] = "\0";

    printf(COLOR_BOLD"Geometries:\n"COLOR_CLEAR);

    /* screen size */
    int x, y, w, h;
    get_workarea(display, root, &x, &y, &w, &h);
    printf("  - "COLOR_BOLD"work area"COLOR_CLEAR"        (%d, %d), (%d, %d)\n", x, y, w, h);

    for(i=0; i<MOVESLEN; i++){
        if(bindings[i].data == NULL){
            sprintf(arg_buffer, "(null)");
        }else if(bindings[i].data != NULL && bindings[i].callback == move){
            Geometry_t *data = (Geometry_t *) bindings[i].data;
            sprintf(arg_buffer, "(%d, %d), (%d, %d)", data->x, data->y, data->width, data->height);
        }

        if(bindings[i].keysym == XK_VoidSymbol){
            sprintf(key_buffer, "(null)");
        }else{
            sprintf(key_buffer, "[%s]", XKeysymToString(bindings[i].keysym));
        }

        printf("  - %-16s %-40s %s\n", bindings[i].name, arg_buffer, key_buffer);
    }
}
