Tiler - Window tiling utility for X11
=====================================

Tiler is a lightweight tiling utility for X Window Managers. It uses keyboard
shortcuts to organize windows on your desktop on demand. This provides a 
very convenient way of moving/resizing windows, for instance by showing
side-by-side two windows. This is especially useful for large screens.

An admitted goal is to provide a comparable tool as the (very great) 
WinSplit Revolution available on Windows.

*Now with multi-screen support!*

Examples
--------

The default configuration file provides keybindings using the keypad
of full sized keyboards.

  * `CTRL+ALT+3` : resize and move active window to fit the bottom-right quadrant of the screen.
  * `CTRL+ALT+8` : resize and move active window to fit the upper half of the screen.
  * `CTRL+ALT+Enter` : organize the two most recently used windows side by side.
  * `CTRL+ALT+Add` : organize the four most recently used windows on a grid.

![Tiler + Openbox with dual screen](http://www.acadis.org/images/tiler/tiler-openbox-xinerama.small.png "Tiler + Openbox with dual screen")
  
Requirements
------------

Your window manager is expected to be EWMH compliant and implementing the whole
window manager standard (http://standards.freedesktop.org/wm-spec/latest/). 
KWin (KDE) and Metacity (Gnome) are ok with that, some workarounds have been
written for the use with Compiz.

The program is written in C with very few dependencies, you will
only need the following (and their headers): `libc`, `libm`, `libx11`


Installation
------------

    $ make
    # make install

Things & stuff
--------------

Please note that this program is in early development. 

  * Layout may sometimes look weird because of window constraints such as 
      "resizing step" (mostly in terminals) or minimum window size. Theses 
      may not allow to fit the desired window geometry and especially affects 
      small resolutions
  * Windows shadowing causes trouble in general (yes Compiz, I'm looking at you)
      

Source code, bug reports, ideas and more at http://www.github.com/gliss/tiler
  Unfortunately, login is required to post on Github, if you do not have
  an account, you can also send an email to tiler-dev@acadis.org


Licensing
---------

This program and its documentation are released under the terms of the
BSD license.

----
2012, Manuel Vonthron <manuel.vonthron@acadis.org>
