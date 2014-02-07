#include "utils.h"
#include "widgets.h"

#include "resources/bluetooth.xbm"

Pixmap bluetooth;

static struct command_arg args[] =
{
  { "bthact", "Call the specified binary when bluetooth widget activated", "tbluetoothctl" }
};

static void on_init(struct context* context, struct list_entry** notifiers)
{
  bluetooth = img_init(bluetooth);
}

static Pixmap on_refresh()
{
  return bluetooth;
}

static void on_activate()
{
  detach(arg_value(args, alen(args), "bthact"));
}

static void on_del(struct context* context)
{
}

/////////////////// Initialization code ///////////////////

static void __attribute__ ((constructor)) init()
{
  static struct widget_desc description =
  {
    alen(args),
    args,
    &on_init,
    &on_refresh,
    &on_activate,
    &on_del
  };

  add_widget(&description);
}
