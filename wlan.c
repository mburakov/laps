#include "utils.h"
#include "widgets.h"

#include "resources/wlan.xbm"

// libs: dbus-1

Pixmap wlan;

static void on_init(struct context* context, struct list_entry** notifiers)
{
  wlan = img_init(wlan);
}

static Pixmap on_refresh()
{
  return wlan;
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
    &on_refresh,
    &on_activate,
    &on_del
  };

  add_widget(&description);
}
