#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <sys/select.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "widgets.h"
#include "utils.h"

volatile sig_atomic_t running = 1;

void handle_term(int signal)
{
  running = 0;
}

void print_commandline(char** data, char* format)
{
  for (char** item = data; *item; ++item)
    fprintf(stdout, format, *item);
  free(data);
}

int main(int argc, char** argv)
{
  char *bgcolor_name = NULL;

  for (char** it = argv; it < argv + argc; ++it)
  {
    if (!strcmp(*it, "--help"))
    {
      fprintf(stdout, "Usage: %s", argv[0]);
      print_commandline(cmdline_widgets(1), " [%s <>]");
      fprintf(stdout, " [--bgcolor <color>] [--help]\n");
      print_commandline(cmdline_widgets(0), "\t%s\n");
      fprintf(stdout, "\t--bgcolor  Use <color> (i.e. #777777) for the background\n");
      fprintf(stdout, "\t--help     Show this help\n\n");
      exit(0);
    }
    else if (!strcmp(*it, "--bgcolor"))
      bgcolor_name = *(++it);
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
  context.window = XCreateSimpleWindow(context.display, context.root, 0, 0, measure_widgets(), 24, 0, 0, 0);

  XWMHints wm_hints;
  wm_hints.initial_state = WithdrawnState;
  wm_hints.icon_window = wm_hints.window_group = context.window;
  wm_hints.flags = StateHint | IconWindowHint;
  XSetWMHints(context.display, context.window, &wm_hints);
  XSetCommand(context.display, context.window, argv, argc);

  context.gc = DefaultGC(context.display, context.screen);
  init_widgets(&context);

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
          XClearWindow(context.display, context.window);
          refresh_widgets(&context);
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
