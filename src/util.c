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

#include <stdio.h>
#include <inttypes.h>
#include <arpa/inet.h> // byteorder

#include "doltool.h"

int dol_dump(const Dol_Hdr *header, FILE *file)
{
  // Dump text
  for (int i=0; i != DOL_TEXT_COUNT; ++i) {
    uint32_t offset = ntohl(header->text_offset[i]);
    uint32_t address = ntohl(header->text_address[i]);
    uint32_t size = ntohl(header->text_size[i]);
    fprintf(file,
      "text%02i: %08" PRIx32 "-%08" PRIx32 " (%08" PRIx32 "B) @%08" PRIx32 "\n",
      i, address, address + size, size, offset);
  }
  // Dump data
  for (int i=0; i != DOL_DATA_COUNT; ++i) {
    uint32_t offset = ntohl(header->data_offset[i]);
    uint32_t address = ntohl(header->data_address[i]);
    uint32_t size = ntohl(header->data_size[i]);
    fprintf(file,
      "data%02i: %08" PRIx32 "-%08" PRIx32 " (%08" PRIx32 "B) @%08" PRIx32 "\n",
      i, address, address + size, size, offset);
  }

  fprintf(file,
    "bss   : %08" PRIx32 "-%08" PRIx32 " (%08" PRIx32 "B)\n",
    ntohl(header->bss_address),
    ntohl(header->bss_address) + ntohl(header->bss_size),
    ntohl(header->bss_size));
  fprintf(file,
    "Entry point: 0x%" PRIx32 "\n",
    ntohl(header->entry_point));
}

const char* text_sections[DOL_TEXT_COUNT] = {
  ".text.0",
  ".text.1",
  ".text.2",
  ".text.3",
  ".text.4",
  ".text.5",
  ".text.6"
};

const char* data_sections[DOL_DATA_COUNT] = {
  ".data.0",
  ".data.1",
  ".data.2",
  ".data.3",
  ".data.4",
  ".data.5",
  ".data.6",
  ".data.7",
  ".data.8",
  ".data.9",
  ".data.10"
};
