#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define bounds(a, b, c) min(c, max(a, b))

#define img_init(id) \
  XCreateBitmapFromData(context->display, context->root, (char*)id##_bits, id##_width, id##_height)

char* read_string(const char* path);
int read_int(const char* path);
