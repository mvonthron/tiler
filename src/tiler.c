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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <X11/Xlib.h>

#include "utils.h"
#include "config.h"
#include "keybindings.h"
#include "geometries.h"
#include "xactions.h"

/* extern display & root */
Display *display = NULL;
Window root = BadWindow;

void
cleanup()
{
    clear_bindings();

    /* remove pid file */
    unlink(settings.pidfile);

    // freeze on close display
    //XCloseDisplay(display);

    free_config();
}

void
signal_handler(int sig)
{
    if(sig == SIGTERM || sig == SIGINT) {
        D(("Exiting"));

        /* @todo should interrupt listening loop with a XSendEvent */
        cleanup();
        exit(0);
    }
}

/**
  @mainpage Tiler - Window tiling utility for X11

Tiler is a lightweight tiling utility for X Window Managers. It uses keyboard
shortcuts to organize windows on your desktop on demand. This provides a
very convenient way of moving/resizing windows, for instance by showing
side-by-side two windows. This is especially useful for large screens.
 */

int
main(int argc, char **argv)
{
    XEvent event;

    parse_opt(argc, argv);

    /* signal capture */
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    /*
     * daemonize
     *
    if(!settings.foreground){
      int pid = fork();
      if(pid < 0)
        return EXIT_FAILURE;
      if(pid > 0)
        return EXIT_SUCCESS;

      umask(0);

      int sid = setsid();
      if(sid < 0)
        return EXIT_FAILURE;
    }
    */

    /*
     * pid file creation
     * ensuring single instance of tiler
     */
    int pidfile = open(settings.pidfile, O_CREAT | O_EXCL | O_WRONLY);
    if(pidfile == -1) {
        if(errno == EEXIST)
            D(("pid file \"%s\" already exists", settings.pidfile));

        if(!settings.force_run)
            exit(1);
    } else {
        char pidline[32];
        sprintf(pidline, "%d", getpid());
        write(pidfile, pidline, strlen(pidline));
        close(pidfile);
    }

    /** @todo isolate display and root variables in xactions.c */
    display = XOpenDisplay(NULL);
    if(display == NULL) {
        D(("Cannot connect to X server"));
        return EXIT_FAILURE;
    }

    root = XDefaultRootWindow(display);

    check_compiz_wm();

    /* get monitors info */
    get_monitors_config(display, root);

    setup_bindings_data();

    /*
     * configuration parsing
     * keybinding setup
     */
    parse_conf_file(settings.filename);

    if(settings.verbose) {
        print_config();
        print_geometries();
    }

    /**
     * main key event listening loop
     */
    for(;;) {
        XNextEvent(display, &event);
        dispatch(&event);
    }

    cleanup();
    XCloseDisplay(display);

    return EXIT_SUCCESS;
}
