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

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <arpa/inet.h>

#include <elf.h>

#include "doltool.h"

#define SHSTRNDX 1

static size_t count_loads(const Dol_Hdr *dhdr)
{
  size_t count = 0;
  for (int i=0; i != DOL_TEXT_COUNT; ++i)
    if (dhdr->text_size[i])
      ++count;
  for (int i=0; i != DOL_DATA_COUNT; ++i)
    if (dhdr->data_size[i])
      ++count;
  if (dhdr->bss_size)
    ++count;
  return count;
}

static void fill_elf_header(const Dol_Hdr *dhdr, struct Elf* elf)
{
  Elf32_Ehdr *ehdr = &elf->ehdr;

  memset(ehdr, 0, sizeof(Elf32_Ehdr));
  ehdr->e_ident[EI_MAG0]       = 0x7f;
  ehdr->e_ident[EI_MAG1]       = 'E';
  ehdr->e_ident[EI_MAG2]       = 'L';
  ehdr->e_ident[EI_MAG3]       = 'F';
  ehdr->e_ident[EI_CLASS]      = ELFCLASS32;
  ehdr->e_ident[EI_DATA]       = ELFDATA2MSB;
  ehdr->e_ident[EI_VERSION]    = EV_CURRENT;
  ehdr->e_ident[EI_OSABI]      = ELFOSABI_STANDALONE;
  ehdr->e_ident[EI_ABIVERSION] = 0;
  ehdr->e_type      = htons(ET_EXEC);
  ehdr->e_machine   = htons(EM_PPC);
  ehdr->e_version   = htonl(EV_CURRENT);
  ehdr->e_entry     = dhdr->entry_point;
  ehdr->e_phoff     = htonl(sizeof(Elf32_Ehdr));
  ehdr->e_shoff     = htonl(sizeof(Elf32_Ehdr) + elf->phnum * sizeof(Elf32_Phdr));
  ehdr->e_flags     = 0;
  ehdr->e_ehsize    = htons(sizeof(Elf32_Ehdr));
  ehdr->e_phentsize = htons(sizeof(Elf32_Phdr));
  ehdr->e_phnum     = htons(elf->phnum);
  ehdr->e_shentsize = htons(sizeof(Elf32_Shdr));
  ehdr->e_shnum     = htons(elf->shnum);
  ehdr->e_shstrndx  = htons(SHSTRNDX);
}

int file_copy(FILE *input, FILE* output)
{
  char buffer[1024];
  while (1) {
    size_t read_count = fread(buffer, sizeof(char), 1024, input);
    if (fwrite(buffer, sizeof(char), read_count, output) != read_count) {
      return -1;
    }
    if (read_count != 1024) {
      if (feof(output))
        return -1;
      else
        return 0;
    }
  }
}

// ***** Main code

static void elf_free(struct Elf *elf)
{
  free(elf->strtab.data);
}

int dol2elf(const char *dol_filename, const char *elf_filename)
{
  FILE *dol_file = NULL;
  FILE *elf_file = NULL;
  struct Elf elf;
  memset(&elf, 0, sizeof(struct Elf));

  dol_file = fopen(dol_filename, "rb");
  if (!dol_file) {
    fprintf(stderr, "Could not open %s\n", dol_filename);
    goto err;
  }

  // Read the DOL header:
  Dol_Hdr dhdr;
  if (fread(&dhdr, sizeof(Dol_Hdr), 1, dol_file) != 1) {
    fprintf(stderr, "Could not read DOL header in %s\n", dol_filename);
    goto err;
  }
  dol_dump(&dhdr, stderr);

  // How many program headers:
  elf.load_count = count_loads(&dhdr);
  // One ELF segment per DOL segment:
  elf.phnum      = elf.load_count;
  // One ELF section per DOL segment
  // + NULL section, a .strtab section and a .dolhdr section:
  elf.shnum      = elf.load_count + 3;

  // Create the strtab:
  strtab_create(&elf.strtab);
  strtab_fill(&elf.strtab, &dhdr);

  // Offset added by the ELF data:
  elf.strtab_offset =
    sizeof(Elf32_Ehdr)
    + elf.phnum * sizeof(Elf32_Phdr)
    + elf.shnum * sizeof(Elf32_Shdr);
  elf.dol_offset =
    sizeof(Elf32_Ehdr)
    + elf.phnum * sizeof(Elf32_Phdr)
    + elf.shnum * sizeof(Elf32_Shdr)
    + elf.strtab.used;

  // Initialize the ELF header:
  fill_elf_header(&dhdr, &elf);
  assert(elf.phnum == ntohs(elf.ehdr.e_phnum));

  create_shdrs(&dhdr, &elf);
  create_phdrs(&dhdr, &elf);

  elf_file = fopen(elf_filename, "wb");
  if (!elf_file) {
    fprintf(stderr, "Could not open %s\n", elf_filename);
    goto err;
  }
  if (fwrite(&elf.ehdr, sizeof(Elf32_Ehdr), 1, elf_file) != 1) {
    fputs("Could not write ELF header\n", stderr);
    goto err;
  }
  if (fwrite(elf.phdrs, sizeof(Elf32_Phdr), elf.phnum, elf_file) != elf.phnum) {
    fputs( "Could not write ELF program headers\n", stderr);
    goto err;
  }
  if (fwrite(elf.shdrs, sizeof(Elf32_Shdr), elf.shnum, elf_file) != elf.shnum) {
    fputs("Could not write ELF section headers\n", stderr);
    goto err;
  }
  if (fwrite(elf.strtab.data, elf.strtab.used, 1, elf_file) != 1) {
    fputs("Could not write .strtab section\n", stderr);
    goto err;
  }
  if (fseek(dol_file, 0, SEEK_SET) < 0 || file_copy(dol_file, elf_file) != 0) {
    fputs("Could not copy DOL file into ELF file\n", stderr);
    goto err;
  }

  fclose(dol_file);
  fclose(elf_file);
  elf_free(&elf);
  return 0;

err:
  if (dol_file)
    fclose(dol_file);
  if (elf_file)
    fclose(elf_file);
  elf_free(&elf);
  return 1;
}

int main(int argc, char **argv)
{
  if (argc != 3) {
    fprintf(stderr, "Bad usage: dol2elf foo.dol foo.elf\n");
    return 1;
  }

  return dol2elf(argv[1], argv[2]);
}
