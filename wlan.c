#include "utils.h"
#include "widgets.h"

#include <dbus/dbus.h>
#include <stdlib.h>
#include <string.h>

#include "resources/wlan-xx.xbm"
#include "resources/wlan-00.xbm"
#include "resources/wlan-01.xbm"
#include "resources/wlan-02.xbm"
#include "resources/wlan-03.xbm"
#include "resources/wlan-04.xbm"
#include "resources/wlan-05.xbm"

// libs: dbus-1

#define wlan_images 6

Pixmap no_wlan;
Pixmap wlan[wlan_images];

DBusConnection* dbus_connection;
unsigned char signal_strength = 0xff;

static struct command_arg args[] =
{
  { "wlanact", "Call the specified binary when wireless lan widget activated", "twlancfg" }
};

// TODO: This function looks ugly, do smth
DBusHandlerResult signal_filter(DBusConnection* conn, DBusMessage* msg, void* user_data)
{
  if (!dbus_message_is_signal(msg, "org.freedesktop.NetworkManager.AccessPoint", "PropertiesChanged"))
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  DBusMessageIter args;
  if (!dbus_message_iter_init(msg, &args))
    fatal("Invalid signal received from Network Manager", NULL);

  char* signature = dbus_message_iter_get_signature(&args);
  int signature_valid = !strcmp(signature, "a{sv}");
  dbus_free(signature);

  if (!signature_valid)
    fatal("Wrong signal signature", NULL);

  DBusMessageIter map;
  dbus_message_iter_recurse(&args, &map);
  for (; dbus_message_iter_get_arg_type(&map) != DBUS_TYPE_INVALID; dbus_message_iter_next(&map))
  {
    DBusMessageIter entry;
    dbus_message_iter_recurse(&map, &entry);

    char* name;
    dbus_message_iter_get_basic(&entry, &name);
    if (strcmp(name, "Strength"))
      continue;

    DBusMessageIter variant;
    dbus_message_iter_next(&entry);
    dbus_message_iter_recurse(&entry, &variant);
    dbus_message_iter_get_basic(&variant, &signal_strength);
  }

  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

void dbus_handler(int fd, void* data)
{
  DBusWatch* watch = data;

  if (!dbus_watch_handle(watch, DBUS_WATCH_READABLE))
    fatal("Not enough memory for the dbus operation", NULL);

  while (dbus_connection_get_dispatch_status(dbus_connection) == DBUS_DISPATCH_DATA_REMAINS)
    dbus_connection_dispatch(dbus_connection);
}

dbus_bool_t add_watch(DBusWatch* watch, void* data)
{
  struct list_entry** notifiers = data;
  struct notifier* dbus_notifier = malloc(sizeof(struct notifier));
  dbus_notifier->fd = dbus_watch_get_unix_fd(watch);
  dbus_notifier->data = watch;
  dbus_notifier->callback = &dbus_handler;
  list_add(notifiers, dbus_notifier);
  return TRUE;
}

static void on_init(struct context* context, struct list_entry** notifiers)
{
  DBusError error;
  dbus_error_init(&error);

  dbus_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
  if (dbus_error_is_set(&error))
    fatal("Cannot connect to dbus", error.message);

  if (!dbus_connection_set_watch_functions(dbus_connection, &add_watch, NULL, NULL, notifiers, NULL))
    fatal("Cannot register watch handlers for dbus", NULL);

  char* rule = "type='signal',interface='org.freedesktop.NetworkManager.AccessPoint'";
  dbus_bus_add_match(dbus_connection, rule, &error);
  if (dbus_error_is_set(&error))
    fatal("Cannot add match to dbus connection", error.message);

  if (!dbus_connection_add_filter(dbus_connection, &signal_filter, NULL, NULL))
    fatal("Not enough memory for the dbus operation", NULL);

  no_wlan = img_init(wlan_xx);
  wlan[0] = img_init(wlan_00); wlan[1] = img_init(wlan_01);
  wlan[2] = img_init(wlan_02); wlan[3] = img_init(wlan_03);
  wlan[4] = img_init(wlan_04); wlan[5] = img_init(wlan_05);
}

static Pixmap on_refresh()
{
  return signal_strength == 0xff ? no_wlan : wlan[wlan_images * signal_strength / 100];
}

static void on_activate()
{
  detach(arg_value(args, alen(args), "wlanact"));
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
