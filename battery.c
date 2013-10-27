#include "utils.h"
#include "widgets.h"

#include <stdlib.h>
#include <string.h>

#include "resources/battery-00d.xbm"
#include "resources/battery-01d.xbm"
#include "resources/battery-02d.xbm"
#include "resources/battery-03d.xbm"
#include "resources/battery-04d.xbm"
#include "resources/battery-05d.xbm"
#include "resources/battery-06d.xbm"
#include "resources/battery-07d.xbm"
#include "resources/battery-08d.xbm"
#include "resources/battery-09d.xbm"
#include "resources/battery-10d.xbm"
#include "resources/battery-11d.xbm"
#include "resources/battery-12d.xbm"
#include "resources/battery-13d.xbm"

#include "resources/battery-00c.xbm"
#include "resources/battery-01c.xbm"
#include "resources/battery-02c.xbm"
#include "resources/battery-03c.xbm"
#include "resources/battery-04c.xbm"
#include "resources/battery-05c.xbm"
#include "resources/battery-06c.xbm"
#include "resources/battery-07c.xbm"
#include "resources/battery-08c.xbm"
#include "resources/battery-09c.xbm"
#include "resources/battery-10c.xbm"
#include "resources/battery-11c.xbm"
#include "resources/battery-12c.xbm"
#include "resources/battery-13c.xbm"

#define bat_images 14

Pixmap battery_draining[bat_images];
Pixmap battery_charging[bat_images];

char *total_file = "/sys/class/power_supply/BAT0/charge_full";
char *current_file = "/sys/class/power_supply/BAT0/charge_now";
char *status_file = "/sys/class/power_supply/BAT0/status";

static void on_init(struct context* context, int argc, struct kv_pair* argv)
{
  battery_draining[0]  = img_init(battery_00d); battery_charging[0]  = img_init(battery_00c);
  battery_draining[1]  = img_init(battery_01d); battery_charging[1]  = img_init(battery_01c);
  battery_draining[2]  = img_init(battery_02d); battery_charging[2]  = img_init(battery_02c);
  battery_draining[3]  = img_init(battery_03d); battery_charging[3]  = img_init(battery_03c);
  battery_draining[4]  = img_init(battery_04d); battery_charging[4]  = img_init(battery_04c);
  battery_draining[5]  = img_init(battery_05d); battery_charging[5]  = img_init(battery_05c);
  battery_draining[6]  = img_init(battery_06d); battery_charging[6]  = img_init(battery_06c);
  battery_draining[7]  = img_init(battery_07d); battery_charging[7]  = img_init(battery_07c);
  battery_draining[8]  = img_init(battery_08d); battery_charging[8]  = img_init(battery_08c);
  battery_draining[9]  = img_init(battery_09d); battery_charging[9]  = img_init(battery_09c);
  battery_draining[10] = img_init(battery_10d); battery_charging[10] = img_init(battery_10c);
  battery_draining[11] = img_init(battery_11d); battery_charging[11] = img_init(battery_11c);
  battery_draining[12] = img_init(battery_12d); battery_charging[12] = img_init(battery_12c);
  battery_draining[13] = img_init(battery_13d); battery_charging[13] = img_init(battery_13c);
}

static Pixmap on_refresh()
{
  int total = read_int(total_file);
  int current = read_int(current_file);
  int value = current * bat_images / total;
  value = bounds(value, 0, bat_images - 1);

  Pixmap* masks = NULL;
  char* status = read_string(status_file);
  if (!strcmp(status, "Discharging"))
    masks = battery_draining;
  else if (!strcmp(status, "Full"))
    masks = battery_draining;
  else if (!strcmp(status, "Charging"))
    masks = battery_charging;
  else if (!strcmp(status, "Unknown"))
    masks = battery_draining;
  free(status);

  return masks[value];
}

static void on_activate()
{
}

static void on_del(struct context* context)
{
}

/////////////////// Initialization code ///////////////////

static void init() __attribute__ ((constructor));

static char* short_args[] = { "--total", "--current", "--status" };
static char* long_args[] = {
  "--total    Use specified file as a source for the total battery stat",
  "--current  Use specified file as a source for the current battery stat",
  "--status   Use specified file as a source for the battery status"
};

static struct widget_desc description =
{
  3,
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
