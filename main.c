#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <sys/select.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "battery-00.xbm"
#include "battery-01.xbm"
#include "battery-02.xbm"
#include "battery-03.xbm"
#include "battery-04.xbm"
#include "battery-05.xbm"
#include "battery-06.xbm"
#include "battery-07.xbm"
#include "battery-08.xbm"
#include "battery-09.xbm"
#include "battery-10.xbm"
#include "battery-11.xbm"
#include "battery-12.xbm"
#include "battery-13.xbm"

#define XCBFD(a) XCreateBitmapFromData(display, root, (char*)a, 16, 24)

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
  char *bgcolor_name = NULL;

  for (char** it = argv; it < argv + argc; ++it)
  {
    if (!strcmp(*it, "--total"))
      total_file = *(++it);
    else if (!strcmp(*it, "--current"))
      current_file = *(++it);
    else if (!strcmp(*it, "--bgcolor"))
      bgcolor_name = *(++it);
    else if (!strcmp(*it, "--help"))
    {
      fprintf(stdout, "Usage: %s [--total <path>] [--current <path>] [--bgcolor <color>] [--help]\n", argv[0]);
      fprintf(stdout, "\t--total    Use <path> as a source for the total battery stat\n");
      fprintf(stdout, "\t--current  Use <path> as a source for the current battery stat\n");
      fprintf(stdout, "\t--bgcolor  Use <color> (i.e. #777777) for the background\n");
      fprintf(stdout, "\t--help     Shtow this help\n\n");
      exit(0);
    }
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
  Window root = DefaultRootWindow(display);
  Window dockapp = XCreateSimpleWindow(display, root, 0, 0, 16, 24, 0, 0, 0);

  XWMHints wm_hints;
  wm_hints.initial_state = WithdrawnState;
  wm_hints.icon_window = wm_hints.window_group = dockapp;
  wm_hints.flags = StateHint | IconWindowHint;
  XSetWMHints(display, dockapp, &wm_hints);
  XSetCommand(display, dockapp, argv, argc);

  Pixmap masks[] =
  {
    XCBFD(battery_00_bits), XCBFD(battery_01_bits), XCBFD(battery_02_bits),
    XCBFD(battery_03_bits), XCBFD(battery_04_bits), XCBFD(battery_05_bits),
    XCBFD(battery_06_bits), XCBFD(battery_07_bits), XCBFD(battery_08_bits),
    XCBFD(battery_09_bits), XCBFD(battery_10_bits), XCBFD(battery_11_bits),
    XCBFD(battery_12_bits), XCBFD(battery_13_bits),
  };

  if (bgcolor_name)
  {
    Colormap colormap = DefaultColormap(display, screen);
    XColor bgcolor;
    XParseColor(display, colormap, bgcolor_name, &bgcolor);
    XAllocColor(display, colormap, &bgcolor);
    XSetWindowBackground(display, dockapp, bgcolor.pixel);
  }
  else XSetWindowBackgroundPixmap(display, dockapp, ParentRelative);

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
          int value = (current * (sizeof(masks) / sizeof(masks[0])) / total) + 1;

          XClearWindow(display, dockapp);
          XSetClipMask(display, DefaultGC(display, screen), masks[value]);
          XFillRectangle(display, dockapp, DefaultGC(display, screen), 0, 0, 16, 24);
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
