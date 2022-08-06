boxplasmazoomscroll
===================

### Overview

This program's name is a mouthful, but the purpose is really simple. XFCE allows
zooming in the desktop by using `Alt` and scrolling a mouse wheel up or down.
KDE Kwin doesn't have that functionality, and KDE's shortcut system doesn't seem
to be able to watch for a combination of keypress and mouse wheel movement.

This program watches for `Alt` + a mouse wheel movement. Scrolling up zooms in,
and scrolling down zooms out.

### Usage

`boxplasmazoomscroll` does not take any arguments and does not have any
configuration options.

### Why

I have a large monitor and I like being able to quickly zoom in and out when I'm
not sitting close to it.

I didn't want to use `xbindkeys` and `dbus-send`, because I don't want zooming
to create simple, short-lived processes.

### Caveats

* X11 only. I don't use Wayland, so I probably won't change that.

* This makes assumptions about mouse buttons and how `Alt` is configured. It may
  not work for you. Sorry.

### Credit

[This youtube video](https://www.youtube.com/watch?v=0ZBzM0e7KL8) for
documenting how to activate Plasma shortcuts through dbus!
