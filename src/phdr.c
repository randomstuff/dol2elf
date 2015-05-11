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

#include <elf.h>
#include <arpa/inet.h>

#include "doltool.h"

#define ALIGN 0

static void init_load_text_phdr(const Dol_Hdr *dhdr, int i, Elf32_Phdr *phdr, struct Elf *elf)
{
  phdr->p_type    = htonl(PT_LOAD);
  phdr->p_offset  = htonl( ntohl(dhdr->text_offset[i]) + elf->dol_offset );
  phdr->p_vaddr   = dhdr->text_address[i];
  phdr->p_paddr   = dhdr->text_address[i];
  phdr->p_filesz  = dhdr->text_size[i];
  phdr->p_memsz   = dhdr->text_size[i];
  phdr->p_flags   = htonl(PF_X | PF_R);
  phdr->p_align   = htonl(ALIGN);
}

static void init_load_data_phdr(const Dol_Hdr *dhdr, int i, Elf32_Phdr *phdr, struct Elf *elf)
{
  phdr->p_type    = htonl(PT_LOAD);
  phdr->p_offset  = htonl( ntohl(dhdr->data_offset[i]) + elf->dol_offset );
  phdr->p_vaddr   = dhdr->data_address[i];
  phdr->p_paddr   = dhdr->data_address[i];
  phdr->p_filesz  = dhdr->data_size[i];
  phdr->p_memsz   = dhdr->data_size[i];
  phdr->p_flags   = htonl(PF_R | PF_W);
  phdr->p_align   = htonl(ALIGN);
}

static void init_load_bss_phdr(const Dol_Hdr *dhdr, Elf32_Phdr *phdr, struct Elf *elf)
{
  phdr->p_type    = htonl(PT_LOAD);
  phdr->p_offset  = 0;
  phdr->p_vaddr   = dhdr->bss_address;
  phdr->p_paddr   = dhdr->bss_address;
  phdr->p_filesz  = 0;
  phdr->p_memsz   = dhdr->bss_size;
  phdr->p_flags   = htonl(PF_R | PF_W);
  phdr->p_align   = htonl(ALIGN);
}

void create_phdrs(Dol_Hdr *dhdr, struct Elf *elf)
{
  elf->phdrs = malloc(sizeof(Elf32_Phdr) * elf->phnum);

  size_t phindex = 0;

  for (int i=0; i != DOL_TEXT_COUNT; ++i)
    if (dhdr->text_size[i]) {
      init_load_text_phdr(dhdr, i, elf->phdrs + phindex, elf);
      ++phindex;
    }

  for (int i=0; i != DOL_DATA_COUNT; ++i)
    if (dhdr->data_size[i]) {
      init_load_data_phdr(dhdr, i, elf->phdrs + phindex, elf);
      ++phindex;
    }
  if (dhdr->bss_size) {
    init_load_bss_phdr(dhdr, elf->phdrs + phindex, elf);
    ++phindex;
  }
  assert(phindex == elf->phnum);
}
