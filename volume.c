#include "utils.h"
#include "widgets.h"

#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


#include "resources/volume.xbm"

Pixmap volume;

static char* short_args[];

char *vol_action = "pavucontrol";

static void on_init(struct context* context, int argc, struct kv_pair* args)
{
  arg_switch()
  {
    arg_case(short_args[0], vol_action);
  }

  volume = img_init(volume);
}

static Pixmap on_refresh()
{
  return volume;
}

static void on_activate()
{
  detach(vol_action, vol_action, NULL);
}

static void on_del(struct context* context)
{
}

/////////////////// Initialization code ///////////////////

static void init() __attribute__ ((constructor));

static char* short_args[] = { "--volact" };
static char* long_args[] = {
  "--volact   Call the specified binary when volume widget activated"
};

static struct widget_desc description =
{
  alen(short_args),
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
