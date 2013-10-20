#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <sys/select.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resources/resources.h"
#include "widgets.h"
#include "utils.h"

volatile sig_atomic_t running = 1;

void handle_term(int signal)
{
  running = 0;
}

int main(int argc, char** argv)
{
  char *total_file = "/sys/class/power_supply/BAT0/charge_full";
  char *current_file = "/sys/class/power_supply/BAT0/charge_now";
  char *status_file = "/sys/class/power_supply/BAT0/status";
  char *bgcolor_name = NULL;

  for (char** it = argv; it < argv + argc; ++it)
  {
    if (!strcmp(*it, "--total"))
      total_file = *(++it);
    else if (!strcmp(*it, "--current"))
      current_file = *(++it);
    else if (!strcmp(*it, "--status"))
      status_file = *(++it);
    else if (!strcmp(*it, "--bgcolor"))
      bgcolor_name = *(++it);
    else if (!strcmp(*it, "--help"))
    {
      fprintf(stdout, "Usage: %s [--total <path>] [--current <path>] [--bgcolor <color>] [--help]\n", argv[0]);
      fprintf(stdout, "\t--total    Use <path> as a source for the total battery stat\n");
      fprintf(stdout, "\t--current  Use <path> as a source for the current battery stat\n");
      fprintf(stdout, "\t--status   Use <path> as a source for the battery status\n");
      fprintf(stdout, "\t--bgcolor  Use <color> (i.e. #777777) for the background\n");
      fprintf(stdout, "\t--help     Show this help\n\n");
      exit(0);
    }
  }

  struct context context;
  context.display = XOpenDisplay(NULL);
  if (!context.display)
  {
    fprintf(stderr, "Couldn't open X\n");
    fflush(stderr);
    exit(1);
  }

  int xfd = ConnectionNumber(context.display);
  if (xfd == -1) {
    fprintf(stderr, "Couldn't get X filedescriptor\n");
    fflush(stderr);
    exit(1);
  }

  context.screen = DefaultScreen(context.display);
  context.root = DefaultRootWindow(context.display);
  context.window = XCreateSimpleWindow(context.display, context.root, 0, 0, 16, 24, 0, 0, 0);

  XWMHints wm_hints;
  wm_hints.initial_state = WithdrawnState;
  wm_hints.icon_window = wm_hints.window_group = context.window;
  wm_hints.flags = StateHint | IconWindowHint;
  XSetWMHints(context.display, context.window, &wm_hints);
  XSetCommand(context.display, context.window, argv, argc);

  context.gc = DefaultGC(context.display, context.screen);

  Pixmap battery_draining[] = battery_init(d);
  Pixmap battery_charging[] = battery_init(c);
  int bat_images = sizeof(battery_draining) / sizeof(battery_draining[0]);

  if (bgcolor_name)
  {
    Colormap colormap = DefaultColormap(context.display, context.screen);
    XColor bgcolor;
    XParseColor(context.display, colormap, bgcolor_name, &bgcolor);
    XAllocColor(context.display, colormap, &bgcolor);
    XSetWindowBackground(context.display, context.window, bgcolor.pixel);
  }
  else XSetWindowBackgroundPixmap(context.display, context.window, ParentRelative);

  XSelectInput(context.display, context.window, ExposureMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask);
  XMapWindow(context.display, context.window);
  XFlush(context.display);

  signal(SIGTERM, &handle_term);
  signal(SIGINT, &handle_term);

  // TODO Monitor battery stats and status change through inotify

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
          XNextEvent(context.display, &event);
        }
        else
        {
          break;
        }
      case 0:
        {
          int total = read_int(total_file);
          int current = read_int(current_file);
          int value = current * bat_images / total;
          value = bounds(value, 0, bat_images - 1);

          Pixmap* masks = NULL;
          char* status = read_string(status_file);
          if (!strcmp(status, "Discharging"))
            masks = battery_draining;
          else if (!strcmp(status, "Full"))
            masks = battery_draining;
          else if (!strcmp(status, "Charging"))
            masks = battery_charging;
          free(status);

          XClearWindow(context.display, context.window);

          XSetClipOrigin(context.display, context.gc, 0, 0);
          XSetClipMask(context.display, context.gc, masks[value]);
          XFillRectangle(context.display, context.window, context.gc, 0, 0, 16, 24);

          XFlush(context.display);

          timeout.tv_sec = 60;
          timeout.tv_usec = 0;
          break;
        }
    }
  }

  XCloseDisplay(context.display);
  return 0;
}
