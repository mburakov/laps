#include "battery-00d.xbm"
#include "battery-01d.xbm"
#include "battery-02d.xbm"
#include "battery-03d.xbm"
#include "battery-04d.xbm"
#include "battery-05d.xbm"
#include "battery-06d.xbm"
#include "battery-07d.xbm"
#include "battery-08d.xbm"
#include "battery-09d.xbm"
#include "battery-10d.xbm"
#include "battery-11d.xbm"
#include "battery-12d.xbm"
#include "battery-13d.xbm"

#include "battery-00c.xbm"
#include "battery-01c.xbm"
#include "battery-02c.xbm"
#include "battery-03c.xbm"
#include "battery-04c.xbm"
#include "battery-05c.xbm"
#include "battery-06c.xbm"
#include "battery-07c.xbm"
#include "battery-08c.xbm"
#include "battery-09c.xbm"
#include "battery-10c.xbm"
#include "battery-11c.xbm"
#include "battery-12c.xbm"
#include "battery-13c.xbm"

#define img_init(id) \
  XCreateBitmapFromData(display, root, (char*)id##_bits, id##_width, id##_height)

#define battery_init(state) \
  { \
    img_init(battery_00##state), img_init(battery_01##state), img_init(battery_02##state), \
    img_init(battery_03##state), img_init(battery_04##state), img_init(battery_05##state), \
    img_init(battery_06##state), img_init(battery_07##state), img_init(battery_08##state), \
    img_init(battery_09##state), img_init(battery_10##state), img_init(battery_11##state), \
    img_init(battery_12##state), img_init(battery_13##state) \
  }
