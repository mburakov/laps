#include "utils.h"
#include "widgets.h"

#include <stdlib.h>
#include <string.h>

#include "resources/bluetooth.xbm"

Pixmap bluetooth;

static void on_init(struct context* context, int argc, struct kv_pair* argv)
{
  bluetooth = img_init(bluetooth);
}

static Pixmap on_refresh()
{
  return bluetooth;
}

static void on_activate()
{
}

static void on_del(struct context* context)
{
}

/////////////////// Initialization code ///////////////////

static void init() __attribute__ ((constructor));

static char* short_args[] = { };
static char* long_args[] = { };

static struct widget_desc description =
{
  0,
  short_args,
  long_args,
  &on_init,
  &on_refresh,
  &on_activate,
  &on_del
};

static void init()
{
  add_widget(&description);
}
