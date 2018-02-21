
#include "proc.h"
#include <sys/syscall.h>
#include <sys/types.h>
#include <fcntl.h>
#include <elf.h>
#include <string.h>


void print_str_tab(long vaddr)
{
	printf("Strtab address : %lx\n", vaddr);
	const char* pStr = (const char*)vaddr;
	for(int i=0;i < 4;i++) {
		printf("%s\n", pStr+i);
	}
	
}

void print_sym_tab(long vaddr) 
{
	printf("Symtab address : %lx\n", vaddr);
	Elf64_Sym *pSymtab = (Elf64_Sym *) vaddr;
	
	for(int i=0;i < 4;i++) { 
		printf("%p Symbol : %x, %x, %x, %x, %x, %x \n", pSymtab, pSymtab[i].st_name, 
		pSymtab[i].st_info, pSymtab[i].st_other, pSymtab[i].st_shndx, 
		pSymtab[i].st_value, pSymtab[i].st_size);

	}
}

void print_phdr_el(long vaddr, long type) 
{
	const char *pt_type = "";
	switch(type) {
		case PT_NULL:
			pt_type = "PT_NULL";
			break;
		case PT_LOAD:
			pt_type = "PT_LOAD";
			break;
		case PT_DYNAMIC:
			pt_type = "PT_DYNAMIC";
			break;
		case PT_INTERP:
			pt_type = "PT_INTERP";
			break;
		case PT_NOTE:
			pt_type = "PT_NOTE";
			break;
		case PT_SHLIB:
			pt_type = "PT_SHLIB";
			break;
		case PT_PHDR:
			pt_type = "PT_PHDR";
			break;
		case PT_TLS:
			pt_type = "PT_TLS";
			break;
		case PT_NUM:
			pt_type = "PT_NUM";
			break;
	}
	printf("Virtual Address : %x, type : %s %x\n", vaddr, pt_type, type);
}

void print_dyn_el(long dtag, long value) 
{
	const char *tag_name = "";
	switch(dtag) {
		case 1:
			tag_name = "DT_NEEDED";
			break;
		case 2:
			tag_name = "DT_PLTRELSZ";
			break;
		case 3:
			tag_name = "DT_PLTGOT";
			break;		
		case 4:
			tag_name = "DT_HASH";
			break;
		case 5:
			tag_name = "DT_STRTAB";
			break;
		case 6:
			tag_name = "DT_SYMTAB";
			break;
		case 7:
			tag_name = "DT_RELA";
			break;
		case 8:
			tag_name = "DT_RELASZ";
			break;
		case 9:
			tag_name = "DT_RELAENT";
			break;
		case 10:
			tag_name = "DT_STRSZ";
			break;
		case 11:
			tag_name = "DT_SYMENT";
			break;
		case 12:
			tag_name = "DT_INIT";
			break;
		case 13:
			tag_name = "DT_FINI";
			break;
		case 14:
			tag_name = "DT_SONAME";
			break;
		case 15:
			tag_name = "DT_RPATH";
			break;
		case 16:
			tag_name = "DT_SYMBOLIC";
			break;
		case 17:
			tag_name = "DT_REL";
			break;
		case 18:
			tag_name = "DT_RELSZ";
			break;
		case 19:
			tag_name = "DT_RELENT";
			break;
		case 20:
			tag_name = "DT_PLTREL";
			break;
		case 21:
			tag_name = "DT_DEBUG";
			break;
		case 22:
			tag_name = "DT_TEXTREL";
			break;
		case 23:
			tag_name = "DT_JMPREL";
			break;
		case 24:
			tag_name = "DT_BIND_NOW";
			break;
		case 25:
			tag_name = "DT_INIT_ARRAY";
			break;
		case 26:
			tag_name = "DT_FINI_ARRAY";
			break;
		case 27:
			tag_name = "DT_INIT_ARRAYSZ";
			break;
		case 28:
			tag_name = "DT_FINI_ARRAYSZ";
			break;
		case 29:
			tag_name = "DT_RUNPATH";
			break;
		case 30:
			tag_name = "DT_FLAGS";
			break;
		case 32:
			tag_name = "DT_PREINIT_ARRAY";
			break;
		case 33:
			tag_name = "DT_PREINIT_ARRAYSZ";
			break;
		case 34:
			tag_name = "DT_NUM";
			break;
	}
	if (dtag > 34) {
		printf("\tDtag : %x  %s, val : %x\n", dtag, tag_name, value);
	} else {	
		printf("\tDtag : %d  %s, val : %x\n", dtag, tag_name, value);
	}
	
}

void get_proc_map(pid_t pid)
{
        FILE* fp;
        char filename[30];
        char line[850];
        long start_addr, end_addr, inode, offset;

        char perms[5], device[32];
        char* modulePath;
        sprintf(filename, "/proc/%d/maps", pid);

        fp = fopen(filename, "r");
        if(fp == NULL)
                exit(1);
        while(fgets(line, 850, fp) != NULL)
        {
                sscanf(line, "%llx-%llx %s %lx %s %lx %*s", 
				&start_addr, &end_addr, &perms, &offset,
				&device, &inode, &filename);
		printf(line);
		printf("%llx-%llx %s %lx %s %lx %s\n", 
			start_addr, end_addr, perms, offset, 
			device, inode, filename);

		static const char* read = "r";
		if (!strstr(perms, read)) {
			continue;
		}
		// at least, address space size must over ELF header size
		if ((end_addr - start_addr) > sizeof(Elf64_Ehdr)) {
			Elf64_Ehdr* pEhdr = (Elf64_Ehdr *) start_addr;
			if (pEhdr->e_ident[EI_MAG0] == 0x7f &&
				pEhdr->e_ident[EI_MAG1] == 'E' &&
				pEhdr->e_ident[EI_MAG2] == 'L' &&
				pEhdr->e_ident[EI_MAG3] == 'F') {
				printf("Found ELF File Header\n");
				
				Elf64_Off phoff = pEhdr->e_phoff;
				
				long relative_phoff = start_addr + phoff;
				printf("Program Header offset : %x\n", relative_phoff);
				
				for (int phidx=0; phidx < pEhdr->e_phnum; phidx++) {
					Elf64_Phdr* pPhdr = (Elf64_Phdr *) (relative_phoff + sizeof(Elf64_Phdr) * phidx);	
					//printf("Virtual Address : %x, type : %x\n", pPhdr->p_vaddr, pPhdr->p_type);
					print_phdr_el(pPhdr->p_vaddr, pPhdr->p_type);
					if (pEhdr->e_type == ET_EXEC) {
						// exec
						if (pPhdr->p_type == PT_DYNAMIC) {
							// PT_DYNAMIC : Dynamic linking information 
							Elf64_Dyn* dyn_seg_rva = (Elf64_Dyn *)(pPhdr->p_vaddr);
							printf("Dynamic linking : %p\n", dyn_seg_rva);
							for(int i=0; dyn_seg_rva[i].d_tag != DT_NULL; i++) {
								Elf64_Sxword d_tag = dyn_seg_rva[i].d_tag;
								Elf64_Xword d_val = dyn_seg_rva[i].d_un.d_val;
								print_dyn_el(d_tag, d_val);
								//printf("\tDtag : %d, val : %x\n", dyn_seg_rva[i].d_tag, dyn_seg_rva[i].d_un.d_val);
								if (d_tag == DT_SYMTAB) { 
									// symtab
									print_sym_tab(d_val);
								} else if (d_tag == DT_STRTAB) {
									print_str_tab(d_val);
								}
							}
						}	
					} else if (pEhdr->e_type == ET_DYN) {
						// dyn 
						if (pPhdr->p_type == PT_DYNAMIC) {
							// PT_DYNAMIC : Dynamic linking information 
							Elf64_Dyn* dyn_seg_rva = (Elf64_Dyn *)(start_addr + pPhdr->p_vaddr);
							printf("Dynamic linking : %p\n", dyn_seg_rva);
							for(int i=0; dyn_seg_rva[i].d_tag != DT_NULL; i++) {
								Elf64_Sxword d_tag = dyn_seg_rva[i].d_tag;
								Elf64_Xword d_val = dyn_seg_rva[i].d_un.d_val;
								print_dyn_el(d_tag, d_val);
								//printf("\tDtag : %d, val : %x\n", dyn_seg_rva[i].d_tag, dyn_seg_rva[i].d_un.d_val);
								if (d_tag == DT_SYMTAB) { 
									// symtab
									print_sym_tab(d_val);
								} else if (d_tag == DT_STRTAB) {
									print_str_tab(d_val);	
								}

							}
						}
					}
					
				}
			}
		}
        }
        fclose(fp);
}

