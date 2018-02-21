#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <sys/auxv.h>
#include <sys/types.h>
#include <link.h>
#include <dlfcn.h>


int print_phdrs(struct dl_phdr_info*, size_t, void*);
void get_phdrs();

