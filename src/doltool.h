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

#ifndef DOLTOOL_DOLTOOL_H
#define DOLTOOL_DOLTOOL_H

#include <stdint.h>
#include <stdio.h>

#include <elf.h>

#define DOL_TEXT_COUNT 7
#define DOL_DATA_COUNT 11

typedef struct {
  uint32_t text_offset[DOL_TEXT_COUNT];
  uint32_t data_offset[DOL_DATA_COUNT];
  uint32_t text_address[DOL_TEXT_COUNT];
  uint32_t data_address[DOL_DATA_COUNT];
  uint32_t text_size[DOL_TEXT_COUNT];
  uint32_t data_size[DOL_DATA_COUNT];
  uint32_t bss_address;
  uint32_t bss_size;
  uint32_t entry_point;
  uint32_t padding[7];
} __attribute__((packed)) Dol_Hdr;

int dol2elf(const char *dol_filename, const char *elf_filename);
int dol_dump(const Dol_Hdr *header, FILE *output);

extern const char* text_sections[DOL_TEXT_COUNT];
extern const char* data_sections[DOL_DATA_COUNT];

// Strtab:
struct strtab_info {
  size_t allocated;
  char* data;
  size_t used;
};

struct Elf {
  size_t load_count;
  size_t phnum;
  size_t shnum;
  struct strtab_info strtab;
  uint32_t strtab_offset;
  uint32_t dol_offset;
  Elf32_Ehdr ehdr;
  Elf32_Shdr *shdrs;
  Elf32_Phdr *phdrs;
};

void strtab_create(struct strtab_info *strtatb);
void strtab_destroy(struct strtab_info *strtatb);
size_t strtab_index(struct strtab_info *strtan, const char *name);
void strtab_fill(struct strtab_info *strtatb, Dol_Hdr *dhdr);

void create_shdrs(Dol_Hdr *dhdr, struct Elf *elf);
void create_phdrs(Dol_Hdr *dhdr, struct Elf *elf);

#endif
