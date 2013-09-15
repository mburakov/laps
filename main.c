/*#include <X11/Xlib.h>
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

  //int screen = DefaultScreen(display);
  //Colormap colormap = DefaultColormap(display, screen);
  Window root = DefaultRootWindow(display);
  //Visual* vis = DefaultVisual(display, screen);
  Window dockapp = XCreateSimpleWindow(display, root, 0, 0, 16, 24, 0, 0, 0);
  //                                                         W   H

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

  XSetWindowBackgroundPixmap(display, dockapp, battery);

  XMapWindow(display, dockapp);
  XFlush(display);

  signal(SIGTERM, &handle_term);
  signal(SIGINT, &handle_term);

  while(running)
  {
    struct timeval timeout;
    timeout.tv_sec = timeout.tv_usec = 0;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(xfd, &fds);
    int selectret = select(xfd + 1, &fds, NULL, NULL, &timeout);

    switch (selectret)
    {
      case -1:
        continue;
      case 0:
//        XClearWindow(display, dockapp);
//        XCopyArea(display, battery, root, DefaultGC(display, screen), 0, 0, 24, 24, 0, 0);
//        XFlush(display);
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
}*/

#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
  xcb_window_t         w;
  xcb_generic_event_t *e;
  uint32_t             mask;
  uint32_t             values[2];
  int                  done = 0;
  xcb_rectangle_t      r = { 20, 20, 60, 60 };
 
  xcb_connection_t* c = xcb_connect(NULL, NULL);
  if (xcb_connection_has_error(c))
  {
    printf("Cannot open display\n");
    exit(1);
  }
  
  // Get first screen
  xcb_screen_t* s = xcb_setup_roots_iterator(xcb_get_setup(c)).data;

  // Create context
  xcb_gcontext_t g = xcb_generate_id(c);
  w = s->root;
  mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = s->black_pixel;
  values[1] = 0;
  xcb_create_gc(c, g, w, mask, values);

  // Create window
  w = xcb_generate_id(c);
  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  values[0] = s->white_pixel;
  values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
  xcb_create_window(c, s->root_depth, w, s->root,
                    120, 120, 100, 100, 1,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT, s->root_visual,
                    mask, values);
 
  // Show window
  xcb_map_window(c, w);
  xcb_flush(c);
 
  while (!done && (e = xcb_wait_for_event(c)))
  {
    switch (e->response_type & ~0x80)
    {
    case XCB_EXPOSE:
      // Draw or repaint
      xcb_poly_fill_rectangle(c, w, g,  1, &r);
      xcb_flush(c);
      break;
    case XCB_KEY_PRESS:
      // Exit on keypress
      done = 1;
      break;
    }
    free(e);
  }

  xcb_disconnect(c);
  return 0;
}
