#include "utils.h"
#include "widgets.h"

#include <stdlib.h>
#include <string.h>

#include "resources/bluetooth.xbm"

Pixmap bluetooth;

static void on_init(struct context* context)
{
  bluetooth = img_init(bluetooth);
}

static void on_notifiers(struct list_entry** notifiers)
{
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

static void __attribute__ ((constructor)) init()
{
  static struct widget_desc description =
  {
    0,
    NULL,
    &on_init,
    &on_notifiers,
    &on_refresh,
    &on_activate,
    &on_del
  };

  add_widget(&description);
}
