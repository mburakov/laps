#include "utils.h"
#include "widgets.h"

#include "resources/pulse.xbm"

Pixmap pulse;

static struct command_arg args[] =
{
  { "plsact", "Call the specified binary when pulseaudio widget activated", "pavucontrol" }
};

static void on_init(struct context* context, struct list_entry** notifiers)
{
  pulse = img_init(pulse);
}

static Pixmap on_refresh()
{
  return pulse;
}

static void on_activate()
{
  detach(arg_value(args, alen(args), "plsact"));
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
