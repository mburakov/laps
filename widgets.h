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

struct widget_desc
{
  int args_count;
  char** args_short;
  char** args_long;
  void (*on_init)(struct context*, int, struct kv_pair*);
  Pixmap (*on_refresh)();
  void (*on_acrivate)();
  void (*on_del)(struct context*);
};

void add_widget(struct widget_desc* description);
int measure_widgets();
char** cmdline_widgets(int brief);
void init_widgets(struct context* context);
void refresh_widgets(struct context* context);
void del_widgets(struct context* context);
