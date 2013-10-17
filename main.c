#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <sys/select.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resources/resources.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define bounds(a, b, c) min(c, max(a, b))

volatile sig_atomic_t running = 1;

void handle_term(int signal)
{
  running = 0;
}

char* read_string(const char* path)
{
  char data[256];
  FILE* source = fopen(path, "rt");
  fgets(data, sizeof(data), source);
  fclose(source);
  return strndup(data, min(sizeof(data), strlen(data) - 1));
}

int read_int(const char* path)
{
  char* str = read_string(path);
  int result = atoi(str);
  free(str);
  return result;
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

  Pixmap battery_draining[] = battery_init(d);
  Pixmap battery_charging[] = battery_init(c);
  int bat_images = sizeof(battery_draining) / sizeof(battery_draining[0]);

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
          XNextEvent(display, &event);
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
