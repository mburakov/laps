#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "widgets.h"

struct list_entry* widgets_list = NULL;

char* arg_value(struct command_arg* args, int size, const char* name)
{
  for (struct command_arg* ptr = args, *end = args + size; ptr < end; ++ptr)
  {
    if (!strcmp(ptr->name, name))
      return ptr->value;
  }

  return NULL;
}

void add_widget(struct widget_desc* description)
{
  description->x = measure_widgets();
  description->y = 0;
  description->w = 16;
  description->h = 24;
  list_add(&widgets_list, description);
}

void for_each_widget(widget_callback callback, void* data)
{
  for_each(struct widget_desc* wd, widgets_list,
    callback(wd, data));
}

int measure_widgets()
{
  int result = 0;
  for_each(struct widget_desc* wd, widgets_list,
    result += wd->w);
  return result;
}

void init_widgets(struct context* context)
{
  for_each(struct widget_desc* item, widgets_list,
    item->on_init(context));
}

void notifiers_widgets(struct list_entry** notifiers)
{
  for_each(struct widget_desc* item, widgets_list,
    item->on_get_notifiers(notifiers));
}

void refresh_widgets(struct context* context)
{
  for_each(struct widget_desc* item, widgets_list,
  {
    XSetClipOrigin(context->display, context->gc, item->x, item->y);
    XSetClipMask(context->display, context->gc, item->on_refresh(context));
    XFillRectangle(context->display, context->window, context->gc, item->x, item->y, item->w, item->h);
  });
}

void activate_widgets(int x, int y)
{
  for_each(struct widget_desc* item, widgets_list,
  {
    if (item->x > x) continue;
    if (item->x + item->w < x) continue;
    if (item->y > y) continue;
    if (item->y + item->h < y) continue;
    item->on_activate();
  });
}
