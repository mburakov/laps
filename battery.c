#include "utils.h"
#include "widgets.h"

#include <linux/netlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

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

static struct command_arg args[] =
{
  { "total", "Use specified file as a source for the total battery stat", "/sys/class/power_supply/BAT0/charge_full" },
  { "current", "Use specified file as a source for the current battery stat", "/sys/class/power_supply/BAT0/charge_now" },
  { "status", "Use specified file as a source for the battery status", "/sys/class/power_supply/BAT0/status" },
  { "batact", "Call the specified binary when battery widget activated", "stpowertop" }
};

void udev_handler(int fd, void* data)
{
  // TODO: Add normal receiver here
  for (char byte, res = 1; res > 0; res = recv(fd, &byte, 1, MSG_DONTWAIT));
}

static void on_init(struct context* context, struct list_entry** notifiers)
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

  struct sockaddr_nl sa = { AF_NETLINK, 0, 0, ~0 };
  int sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
  if (sock == -1 || bind(sock, (struct sockaddr*)&sa, sizeof(struct sockaddr_nl)))
  {
    perror("Error initializing udev listener for battery widget");
    exit(13);
  }

  static struct notifier udev_notifier;
  udev_notifier.fd = sock;
  udev_notifier.data = NULL;
  udev_notifier.callback = &udev_handler;
  list_add(notifiers, &udev_notifier);
}

static Pixmap on_refresh()
{
  int total = read_int(arg_value(args, alen(args), "total"));
  int current = read_int(arg_value(args, alen(args), "current"));
  int value = current * bat_images / total;
  value = bounds(value, 0, bat_images - 1);

  Pixmap* masks = NULL;
  char* status = read_string(arg_value(args, alen(args), "status"));
  if (!strcmp(status, "Discharging"))
    masks = battery_draining;
  else if (!strcmp(status, "Full"))
    masks = battery_draining;
  else if (!strcmp(status, "Charging"))
    masks = battery_charging;
  else if (!strcmp(status, "Unknown"))
    masks = battery_draining;

  return masks[value];
}

static void on_activate()
{
  detach(arg_value(args, alen(args), "batact"));
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
