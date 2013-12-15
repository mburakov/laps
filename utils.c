#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char* read_string(const char* path)
{
  static char data[256];
  FILE* source = fopen(path, "rt");
  fgets(data, sizeof(data), source);
  fclose(source);
  *strchrnul(data, '\r') = 0;
  *strchrnul(data, '\n') = 0;
  return data;
}

int read_int(const char* path)
{
  return atoi(read_string(path));
}

void detach(const char* path)
{
  pid_t pid = vfork();
  if (pid)
  {
    waitpid(pid, NULL, 0);
    return;
  }

  if (!vfork())
    execlp(path, path, NULL);

  exit(0);
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
