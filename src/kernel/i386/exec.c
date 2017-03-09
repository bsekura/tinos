/* TINOS Operating System
 * Copyright (c) 1996, 1997 Bart Sekura
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software
 * is hereby granted, provided that both the copyright notice and 
 * this permission notice appear in all copies of the software, 
 * derivative works or modified versions.
 *
 * THE AUTHOR ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION AND DISCLAIMS ANY LIABILITY OF ANY KIND FOR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 *
 * $Id: exec.c,v 1.2 1998/03/11 12:34:53 bart Exp $
 *
 */

#include <sys/types.h>
#include <i386/elf.h>
#include <i386/hat.h>
#include <i386/page.h>
#include <i386/seg.h>
#include <i386/tss.h>
#include <i386/spl.h>
#include <i386/cpu.h>
#include <thread.h>

#include <i386/exec.h>

static void dump_elf_ehdr(Elf32_Ehdr*);
static void dump_elf_phdr(Elf32_Phdr*);

static int elf_header(struct boot_task_s* b, struct exec_hdr_s* e);

int
exec_header(struct boot_task_s* b, struct exec_hdr_s* e)
{
	if(!elf_header(b, e)) {
		printf("unrecognized executable format\n");
		return 0;
	}
	
	return 1;
}

static int
elf_header(struct boot_task_s* b, struct exec_hdr_s* e)
{
	int c;
	Elf32_Ehdr* h;
	Elf32_Phdr* p, *_p;

#if 0
	h = (Elf32_Ehdr*) b->start;
	dump_elf_ehdr(h); 

	p = (Elf32_Phdr*) (((uchar*) h) + h->e_phoff);
	_p = p;
	for(c = 0; c < h->e_phnum; c++) {
		dump_elf_phdr(p++);

	}			
	printf("text: [%08x:%08x]\n", _p[0].p_vaddr, _p[0].p_filesz);
	printf("data: [%08x:%08x]\n", _p[1].p_vaddr, _p[1].p_filesz);
	printf("bss : [%08x:%08x]\n", 
		_p[1].p_vaddr + _p[1].p_filesz,
		_p[1].p_memsz - _p[1].p_filesz);

#endif

	/*
	 * setup generic exec header
	 */			
	h = (Elf32_Ehdr*) b->start;
	if(!BOOTABLE_I386_ELF((*h)) || h->e_phnum < 2) {
		printf("exec_header(): wierd elf format\n");
		return 0;
	}
	if(h->e_phnum > 2) {
		printf("exec_header(): warning: more than 2 sections\n");
	}
	
	e->entry = h->e_entry;
	p = (Elf32_Phdr*) (b->start + h->e_phoff);

	//dump_elf_phdr(&p[0]);	
	e->text_vaddr = p[0].p_vaddr | USER_BASE;
	e->text_size = p[0].p_filesz;
	e->text_ptr = b->start + p[0].p_offset;
	
	//dump_elf_phdr(&p[1]);
	e->data_vaddr = p[1].p_vaddr | USER_BASE;
	e->data_size = p[1].p_filesz;
	e->data_ptr = b->start + p[1].p_offset;
	
	e->bss_end = (p[1].p_vaddr + p[1].p_memsz) | USER_BASE;
		
	return 1;
}

static void
dump_elf_ehdr(Elf32_Ehdr* eh)
{
	/*printf("e_ident: %s\n", eh->e_ident);*/
	
	printf("--------------------------------------------------\n");
	printf("type mach ver entry      phoff  phnum shoff  flags\n");
	
	printf("%-4d ", eh->e_type);
	printf("%-4d ", eh->e_machine);
	printf("%-3d ", eh->e_version);
	printf("0x%08x ", eh->e_entry);
	printf("%-6d ", eh->e_phoff);
	printf("%-5d ", eh->e_phnum);
	printf("%-6d ", eh->e_shoff);
	printf("0x%08x\n", eh->e_flags);
}

static void
dump_elf_phdr(Elf32_Phdr* ph)
{
	static unsigned heading = 0;

	if(!heading) {
		printf("-----------------------------------------------------------\n");
		printf("type offset vaddr      paddr      filesz memsz  align flags\n");
		heading = 1;
	}

	printf("%-4d ", ph->p_type);
	printf("%-6d ", ph->p_offset);
	printf("0x%08x ", ph->p_vaddr);
	printf("0x%08x ", ph->p_paddr);
	printf("%-6d ", ph->p_filesz);
	printf("%-6d ", ph->p_memsz);
	printf("%-5d ", ph->p_align);
	printf("0x%08x\n", ph->p_flags);
	
}

void
exec_user()
{
	extern struct tss kernel_tss;
	extern struct tcb* cur_thread;
	
	struct tcb* t;

	/*
	 * this is a layout of what
	 * 'iret' pops off the stack
	 */	
	struct {
		ulong	esds;
		ulong	eip;
		ulong	ecs;
		ulong	eflags;
		ulong	esp;
		ulong	ess;
	} f;
	
	/*
	 * setup ring 0 stack
	 */
	splhi();
	t = cur_thread;
	kernel_tss.esp0 = t->kstack;
	
	f.esds = (USER_DSEG << 16) | USER_DSEG;
	f.eip = t->entry;
	f.ecs = USER_CSEG;
	f.eflags = (EFL_IOPL | EFL_INTS);
	f.esp = t->stk;
	f.ess = USER_DSEG;
	
	//printf("going to user mode...\n");	
	/*
	 * goto user mode
	 */
	asm volatile
	(
		"movl	%%eax, %%esp\n\t"
		"popw	%%es\n\t"
		"popw	%%ds\n\t"
		"iret"
		: /* no output */
		: "a" (&f)
	);
}

int
exec_read(ulong inode)
{
	extern int ext2fs_read(ulong, ulong, void*, ulong);
	Elf32_Ehdr h;	
	
	if(ext2fs_read(inode, 0, (void*) &h, sizeof(Elf32_Ehdr)) > 0) {
		int c;
		Elf32_Phdr p[4];
	
		if(!BOOTABLE_I386_ELF((h))) {
			printf("exec_read(): not an elf format\n");
			return 0;
		}
		dump_elf_ehdr(&h);
		if(ext2fs_read(inode, h.e_phoff, (void*) &p[0], 
			       (h.e_phnum > 4 ? 4 : h.e_phnum) * 
			       sizeof(Elf32_Phdr)) <= 0) {
			       
			printf("read failed\n");
			return;			      
		}			      
		for(c = 0; c < (h.e_phnum > 4 ? 4 : h.e_phnum); c++) {
			dump_elf_phdr(&p[c]);	
		}			
	} else 
		printf("read failed\n");
		
	return 0;
}
