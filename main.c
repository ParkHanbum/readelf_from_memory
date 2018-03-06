#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "proc.h"
#include "phdr.h"

int main() {
	get_proc_map(getpid());	
	//get_phdrs();
	getchar();
	return 0;
}

