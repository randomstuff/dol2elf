# DOL 2 ELF

Create a ELF executable file from a [DOL](http://wiibrew.org/wiki/DOL) file.

Usage:

~~~sh
dol2elf foo.dol foo.elf
~~~

The generated ELF file is currently a dummy ELF file. It is not meant to be
executed but to be read by standard tools which do not groke the DOL format
(objdump, gdb, radare2).

Currently what it includes:

 * one ELF segment for each DOL segment;

 * one section for each DOL segment;

 * a `.strtab` section (section names);

 * a copy of the DOL header in a `.dolhdr` section.

In fact, the whole DOL file is copied verbatim at the end of the ELF file.
