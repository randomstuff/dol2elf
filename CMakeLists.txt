cmake_minimum_required (VERSION 2.8.11)
project(dol2elf)
set(CMAKE_C_FLAGS "-std=c99")

add_executable(dol2elf
  src/dol2elf.c
  src/util.c
  src/phdr.c
  src/shdr.c
  src/strtab.c
  )
