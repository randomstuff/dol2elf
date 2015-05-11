/* The MIT License (MIT)

Copyright (c) 2015 Gabriel Corona

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <elf.h>
#include <arpa/inet.h>

#include "doltool.h"

void strtab_create(struct strtab_info *strtab)
{
  strtab->allocated = 64*1024;
  strtab->data = malloc(sizeof(char) * strtab->allocated);
  strtab->data[0] = '\0';
  strtab->used = 1;
}

void strtab_destroy(struct strtab_info *strtab)
{
  free(strtab->data);
}

size_t strtab_index(struct strtab_info *strtab, const char *name)
{
  size_t count = strtab->used;
  size_t len = strlen(name);

  size_t i = 0;
  while (i < strtab->used) {
    if (strcmp(name, strtab->data + i) == 0)
      return i;
    else
      i += strlen(strtab->data + i) + 1;
  }

  size_t res = strtab->used;
  size_t new_used = strtab->used + len + 1;
  if (new_used > strtab->allocated) {
    size_t new_allocated = strtab->allocated + strtab->allocated/2;
    if (new_allocated < new_used)
      new_allocated = new_used;
    strtab->data = realloc(strtab->data, new_allocated);
    strtab->allocated = new_allocated;
  }

  memcpy(strtab->data + i, name, len + 1);
  strtab->used = new_used;
  return res;
}

void strtab_fill(struct strtab_info *strtab, Dol_Hdr *dhdr)
{
  for (int i=0; i != DOL_TEXT_COUNT; ++i)
    if (dhdr->text_size[i])
      strtab_index(strtab, text_sections[i]);

  for (int i=0; i != DOL_DATA_COUNT; ++i)
    if (dhdr->data_size[i])
      strtab_index(strtab, data_sections[i]);

  if (dhdr->bss_size)
    strtab_index(strtab, ".bss");

  strtab_index(strtab, ".shstrtab");
  strtab_index(strtab, ".dolhdr");
}
