#include <X11/Xlib.h>

struct context
{
  Display* display;
  int screen;
  Window root;
  Window window;
  GC gc;
};

struct kv_pair
{
  char* key;
  char* value;
};

#define arg_switch() \
  for (struct kv_pair* it = args; it < args + argc; ++it)

#define arg_case(a, b) \
  if (!strcmp(it->key, a)) b = it->value;

struct widget_desc
{
  int args_count;
  char** args_short;
  char** args_long;
  void (*on_init)(struct context*, int, struct kv_pair*);
  Pixmap (*on_refresh)();
  void (*on_activate)();
  void (*on_del)(struct context*);
};

void add_widget(struct widget_desc* description);
int measure_widgets();
char** cmdline_widgets(int brief);
void init_widgets(struct context* context, int argc, struct kv_pair* args);
void refresh_widgets(struct context* context);
void activate_widgets(int x, int y);
void del_widgets(struct context* context);
