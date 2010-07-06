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
#include <unistd.h>
#include <signal.h>
#include <assert.h>

#include <X11/Xlib.h>

#include "utils.h"
#include "config.h"
#include "keybindings.h"
#include "xactions.h"

/* extern display */
Display *display=NULL;
Window root=BadWindow;

void
signal_handler(int sig)
{
  if(sig == SIGTERM || sig == SIGINT){
    D(("Exiting"));
    /* free allocated */
    
    //XCloseDisplay(display);
    
    exit(0);
  }
}

int
main(int argc, char **argv)
{
  XEvent event;
  
  /* signal capture */
  signal(SIGTERM, signal_handler);
  signal(SIGINT, signal_handler);
  
  display = XOpenDisplay(NULL);
  if (display == NULL) {
    fprintf(stderr, "%s: cannot connect to X server\n",
            argv[0]);
    return EXIT_FAILURE;
  }
  
  /* 
   * configuration parsing 
   * keybinding setup 
   */
  parse_conf_file("tiler.conf");

  
  /* 
   * daemonize
   *
  int pid = fork();
  if(pid < 0)
    return EXIT_FAILURE;
  if(pid > 0)
    return EXIT_SUCCESS;
  
  umask(0);
  
  int sid = setsid();
  if(sid < 0)
    return EXIT_FAILURE;  
  */
  
  /** */
  root = XDefaultRootWindow(display);
  compute_geometries(display, root);
  
  /**
   * main key event listening loop
   */
  for (;;) {
    XNextEvent(display, &event);
    dispatch(&event);
  }
  
  
  XCloseDisplay(display);
  
  return EXIT_SUCCESS;
}
