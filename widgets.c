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

#define each_widget(i_name) \
  for (struct widget* i_name = widgets_list; i_name != NULL; i_name = i_name->next)

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

int measure_widgets()
{
  int result = 0;
  each_widget(item)
  {
    result += item->w;
  }
  return result;
}

char** cmdline_widgets(int brief)
{
  int args_count = 0;
  each_widget(item)
  {
    args_count += item->description->args_count;
  }

  char** result = (char**)malloc(sizeof(char*) * (args_count + 1));
  char** ptr = result;

  each_widget(item)
  {
    char** source = brief ? item->description->args_short : item->description->args_long;
    memcpy(ptr, source, item->description->args_count * sizeof(char*));
    ptr += item->description->args_count;
  }

  ptr = NULL;
  return result;
}

void init_widgets(struct context* context)
{
  struct widget* item = widgets_list;
  for (; item; item = item->next)
    item->description->on_init(context, 0, NULL);
}

void refresh_widgets(struct context* context)
{
  struct widget* item = widgets_list;
  for (; item; item = item->next)
  {
    XSetClipOrigin(context->display, context->gc, item->x, item->y);
    XSetClipMask(context->display, context->gc, item->description->on_refresh());
    XFillRectangle(context->display, context->window, context->gc, item->x, item->y, item->w, item->h);
  }
}

void del_widgets(struct context* context)
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
