#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>

#include <sys/select.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "battery.xpm"
#include "battery-000.xbm"
#include "battery-020.xbm"
#include "battery-040.xbm"
#include "battery-060.xbm"
#include "battery-080.xbm"
#include "battery-100.xbm"

volatile sig_atomic_t running = 1;

void handle_term(int signal)
{
  running = 0;
}

int read_value(const char* path)
{
  char s[16];
  FILE* f = fopen(path, "rt");
  fgets(s, 16, f);
  fclose(f);
  return atoi(s);
}

int main(int argc, char** argv)
{
  char *total_file = "/sys/class/power_supply/BAT0/charge_full";
  char *current_file = "/sys/class/power_supply/BAT0/charge_now";

  for (char** it = argv; it < argv + argc; ++it)
  {
    if (!strcmp(*it, "--total"))
      total_file = *(++it);
    else if (!strcmp(*it, "--current"))
      current_file = *(++it);
  }

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

  Pixmap masks[] =
  {
    XCreateBitmapFromData(display, root, (char*)battery_000_bits, 16, 24),
    XCreateBitmapFromData(display, root, (char*)battery_020_bits, 16, 24),
    XCreateBitmapFromData(display, root, (char*)battery_040_bits, 16, 24),
    XCreateBitmapFromData(display, root, (char*)battery_060_bits, 16, 24),
    XCreateBitmapFromData(display, root, (char*)battery_080_bits, 16, 24),
    XCreateBitmapFromData(display, root, (char*)battery_100_bits, 16, 24)
  };

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
      case 1:
        if (FD_ISSET(xfd, &fds))
        {
          XEvent event;
          XNextEvent(display, &event);
        }
        else
        {
          break;
        }
      case 0:
        {
          int total = read_value(total_file);
          int current = read_value(current_file);
          int value = (current * (sizeof(masks) / sizeof(masks[0]) - 1) / total) + 1;
          XClearWindow(display, dockapp);
          XSetClipMask(display, DefaultGC(display, screen), masks[value]);
          XCopyArea(display, battery, dockapp, DefaultGC(display, screen), 0, 0, 16, 24, 0, 0);
          XFlush(display);
          timeout.tv_sec = 60;
          timeout.tv_usec = 0;
          break;
        }
    }
  }

  XCloseDisplay(display);
  return 0;
}
