#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>

#include <sys/select.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

extern char* battery_xpm[];

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

  int screen = DefaultScreen(display);
  //Colormap colormap = DefaultColormap(display, screen);
  Window root = DefaultRootWindow(display);
  //Visual* vis = DefaultVisual(display, screen);
  Window dockapp = XCreateSimpleWindow(display, root, 0, 0, 16, 24, 0, 0, 0);

  XWMHints wm_hints;
  wm_hints.initial_state = WithdrawnState;
  wm_hints.icon_window = wm_hints.window_group = dockapp;
  wm_hints.flags = StateHint | IconWindowHint;
  XSetWMHints(display, dockapp, &wm_hints);
  XSetCommand(display, dockapp, argv, argc);

  Pixmap battery, mask;
  XpmAttributes attributes;
  attributes.valuemask = XpmReturnAllocPixels | XpmReturnExtensions;
  XpmCreatePixmapFromData(display, root, battery_xpm, &battery, &mask, &attributes);

  XSetWindowBackgroundPixmap(display, dockapp, ParentRelative);

  XSelectInput(display, dockapp, ExposureMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask);
  XMapWindow(display, dockapp);
  XFlush(display);

  signal(SIGTERM, &handle_term);
  signal(SIGINT, &handle_term);

  struct timeval timeout;
  timeout.tv_sec = timeout.tv_usec = 0;

  while(running)
  {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(xfd, &fds);
    int selectret = select(xfd + 1, &fds, NULL, NULL, &timeout);

    switch (selectret)
    {
      case -1:
        continue;
      case 0:
        XSetClipMask(display, DefaultGC(display, screen), mask);
        XCopyArea(display, battery, dockapp, DefaultGC(display, screen), 0, 0, 16, 24, 0, 0);
        XFlush(display);
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        break;
      case 1:
        if (FD_ISSET(xfd, &fds))
        {
          XEvent event;
          XNextEvent(display, &event);
        }
    }
  }

  XCloseDisplay(display);
  return 0;
}
