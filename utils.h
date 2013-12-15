#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define bounds(a, b, c) min(c, max(a, b))

#define img_init(id) \
  XCreateBitmapFromData(context->display, context->root, (char*)id##_bits, id##_width, id##_height)

#define alen(a) (sizeof(a) / sizeof(a[0]))

char* read_string(const char* path);
int read_int(const char* path);
void detach(const char* path);

struct list_entry
{
  struct list_entry* next;
  void* data;
};

void list_add(struct list_entry** head, void* data);

// TODO: Looks ugly, revisit later
#define for_each(type_name, entry, body) \
  for (struct list_entry* copy = entry; copy; copy = copy->next) \
  { \
    type_name = copy->data; \
    body; \
  }
