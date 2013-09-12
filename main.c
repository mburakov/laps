#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

volatile sig_atomic_t running = 1;

void handle_term(int signal)
{
  running = 0;
}

int main(int argc, char** argv)
{
  Display* display = XOpenDisplay(NULL);
  if (!display)
  {
    fprintf(stderr, "Couldn't open X\n");
    fflush(stderr);
    exit(1);
  }

  int xfd = ConnectionNumber(display);
  if (xfd == -1) {
    fprintf(stderr, "Couldn't get X filedescriptor\n");
    fflush(stderr);
    exit(1);
  }

  //int screen = DefaultScreen(display);
  //Colormap colormap = DefaultColormap(display, screen);
  Window root = DefaultRootWindow(display);
  //Visual* vis = DefaultVisual(display, screen);
  Window dockapp = XCreateSimpleWindow(display, root, 0, 0, 128, 24, 0, 0, 0);
  //                                                         W    H

  XWMHints wm_hints;
  wm_hints.initial_state = WithdrawnState;
  wm_hints.icon_window = wm_hints.window_group = dockapp;
  wm_hints.flags = StateHint | IconWindowHint;
  XSetWMHints(display, dockapp, &wm_hints);
  XSetCommand(display, dockapp, argv, argc);

  XMapWindow(display, dockapp);
  XFlush(display);

  signal(SIGTERM, &handle_term);
  signal(SIGINT, &handle_term);

  while(running);

  XCloseDisplay(display);
  return 0;
}
