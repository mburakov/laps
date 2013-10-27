#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* read_string(const char* path)
{
  char data[256];
  FILE* source = fopen(path, "rt");
  fgets(data, sizeof(data), source);
  fclose(source);
  return strndup(data, min(sizeof(data), strlen(data) - 1));
}

int read_int(const char* path)
{
  char* str = read_string(path);
  int result = atoi(str);
  free(str);
  return result;
}
