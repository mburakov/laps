#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define bounds(a, b, c) min(c, max(a, b))

// TODO Replace dots with arrows
#define img_init(id) \
  XCreateBitmapFromData(context.display, context.root, (char*)id##_bits, id##_width, id##_height)

static char* read_string(const char* path)
{
  char data[256];
  FILE* source = fopen(path, "rt");
  fgets(data, sizeof(data), source);
  fclose(source);
  return strndup(data, min(sizeof(data), strlen(data) - 1));
}

static int read_int(const char* path)
{
  char* str = read_string(path);
  int result = atoi(str);
  free(str);
  return result;
}
