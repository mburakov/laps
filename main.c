#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <sys/select.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "widgets.h"
#include "utils.h"

volatile sig_atomic_t running = 1;

void handle_term(int signal)
{
  running = 0;
}

void print_commandline(struct widget_desc* wd, void* data)
{
  static int max_len = 0;
  for (struct command_arg* arg = wd->arguments; arg < wd->arguments + wd->args_count; ++arg)
  {
    if (data) *(int*)data = max_len = max(max_len, strlen(arg->name));
    fprintf(stdout, data ? " [--%*s <>]" : "  --%-*s%s (i.e. %s)\n",
      data ? 0 : max_len + 2, arg->name, arg->description, arg->value);
  }
}

void update_args(struct widget_desc* wd, void* data)
{
  void** args = data;
  int argc = *(int*)args[0];
  char** argv = (char**)args[1];

  for (char** end = argv + argc; argv < end; ++argv)
  {
    if (strncmp(argv[0], "--", 2))
      continue;

    for (struct command_arg* arg = wd->arguments; arg < wd->arguments + wd->args_count; ++arg)
    {
      if (!strcmp(arg->name, argv[0] + 2))
        arg->value = argv[1];
    }
  }
}

int main(int argc, char** argv)
{
  char *bgcolor_name = NULL;

  for (char** it = argv; it < argv + argc; ++it)
  {
    if ((argc - 1) & 0x1 || !strcmp(*it, "--help"))
    {
      int max_arg = 0;
      fprintf(stdout, "Usage: %s", argv[0]);
      for_each_widget(&print_commandline, &max_arg);
      fprintf(stdout, " [--bgcolor <>] [--help]\n");
      for_each_widget(&print_commandline, NULL);
      fprintf(stdout, "  --%-*sUse specified color for the background (i.e. #bebebe)\n", max_arg + 2, "bgcolor");
      fprintf(stdout, "  --%-*sShow this help\n\n", max_arg + 2, "help");
      exit(0);
    }
    else if (!strcmp(*it, "--bgcolor"))
      bgcolor_name = *(++it);
  }

  void* data[] = { &argc, argv };
  for_each_widget(update_args, data);

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

  struct list_entry* notifiers = NULL;
  context.gc = DefaultGC(context.display, context.screen);
  init_widgets(&context, &notifiers);

  if (bgcolor_name)
  {
    Colormap colormap = DefaultColormap(context.display, context.screen);
    XColor bgcolor;
    XParseColor(context.display, colormap, bgcolor_name, &bgcolor);
    XAllocColor(context.display, colormap, &bgcolor);
    XSetWindowBackground(context.display, context.window, bgcolor.pixel);
  }
  else XSetWindowBackgroundPixmap(context.display, context.window, ParentRelative);

  XSelectInput(context.display, context.window, ExposureMask | StructureNotifyMask | ButtonPressMask);
  XMapWindow(context.display, context.window);
  XFlush(context.display);

  signal(SIGTERM, &handle_term);
  signal(SIGINT, &handle_term);

  while(running)
  {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(xfd, &fds);
    for_each(struct notifier* item, notifiers, FD_SET(item->fd, &fds));
    int selectret = select(FD_SETSIZE, &fds, NULL, NULL, NULL);
    if (selectret < 1)
      continue;

    for_each(struct notifier* item, notifiers,
    {
      if (FD_ISSET(item->fd, &fds))
        item->callback(item->fd);
    });

    while (FD_ISSET(xfd, &fds) && XPending(context.display))
    {
      XEvent event;
      XNextEvent(context.display, &event);
      if (event.type == ButtonPress)
        activate_widgets(event.xbutton.x, event.xbutton.y);
    }

    XClearWindow(context.display, context.window);
    refresh_widgets(&context);
    XFlush(context.display);
  }

  XCloseDisplay(context.display);
  return 0;
}
