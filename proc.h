
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_sym_tab(long vaddr);
void print_phdr_el(long vaddr, long type);
void print_dyn_el(long dtag, long value);
void get_proc_map(pid_t pid);
