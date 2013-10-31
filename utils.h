#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define bounds(a, b, c) min(c, max(a, b))

#define img_init(id) \
  XCreateBitmapFromData(context->display, context->root, (char*)id##_bits, id##_width, id##_height)

#define alen(a) (sizeof(a) / sizeof(a[0]))

#define detach(args...) \
  { \
    pid_t pid = vfork(); \
    if (!pid) \
    { \
      if (vfork()) exit(0); \
      execlp(args); \
    } \
    int dummy; \
    waitpid(pid, &dummy, 0); \
  }

char* read_string(const char* path);
int read_int(const char* path);
