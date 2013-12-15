#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

char* read_string(const char* path)
{
  static char data[256];
  FILE* source = fopen(path, "rt");
  fgets(data, sizeof(data), source);
  fclose(source);
  return data;
}

int read_int(const char* path)
{
  return atoi(read_string(path));
}

void detach(const char* path)
{
  if (vfork())
  {
    if (vfork()) exit(0);
    execlp(path, path, NULL);
  }
  wait(NULL);
}

void list_add(struct list_entry** head, void* data)
{
  struct list_entry* tail = malloc(sizeof(struct list_entry));
  tail->data = data;
  tail->next = NULL;

  if (!*head)
  {
    *head = tail;
    return;
  }

  struct list_entry* ptr = *head;
  for (; ptr->next; ptr = ptr->next);
  ptr->next = tail;
}
