#include <stdlib.h>
#include <string.h>

#include "widgets.h"

struct widget
{
  int x, y;
  int w, h;
  struct widget_desc* description;
  struct widget* next;
};

struct widget* widgets_list = NULL;

void add_widget(struct widget_desc* description)
{
  if (!widgets_list)
  {
    struct widget head = { 0, 0, 16, 24, description, NULL };
    widgets_list = malloc(sizeof(struct widget));
    memcpy(widgets_list, &head, sizeof(struct widget));
  }
  else
  {
    struct widget* last = widgets_list;
    for (; last->next; last = last->next);
    last->next = malloc(sizeof(struct widget));
    struct widget tail = { last->x + last->w, last->y, 16, 24, description, NULL };
    memcpy(last->next, &tail, sizeof(struct widget));
  }
}

void init_widgets(struct context* context)
{
  struct widget* item = widgets_list;
  for (; item; item = item->next)
    item->description->on_init(context, 0, NULL);
}

void clear_widgets(struct context* context)
{
  struct widget* item = widgets_list;
  while (item)
  {
    struct widget* del = item;
    item = item->next;

    del->description->on_del(context);
    free(del);
  }
}
