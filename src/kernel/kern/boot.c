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
 * $Id: boot.c,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

/*
 * here is the first setup we do upon kernel boot. we primarily deal
 * with bootloader information and convert it to our initial format.
 * adding support for a new bootloader would involve fiddling around
 * with this module. all the other modules use our internal boot information
 * format, so they would not be affected.
 * we also find here how much memory we have so we can start organize
 * physical pages soon.
 *
 * boot information structure is curved out right after kernel bss.
 * this module sets up and exports this pointer for the rest of the world.
 */

#include <sys/types.h>
#include <sys/coff.h>
#include <i386/page.h>
#include <pc/nvram.h>
#include <boot.h>
#include <i386/multiboot.h>
#include <stdio.h>
#include <string.h> 

extern char _start[], end[];

ulong top_memory;
struct boot_info_s* boot_info;

/*
 * this routine deals with the boot structures filled up by
 * DOS boot program. It sets up our internal boot information
 * just after the kernel bss and fills it up.
 */
void
dosboot_init(ulong bootloader_info)
{
        ulong base, ext;
        struct file_desc* f;
        struct info_page* i;
        int c;

        /*
         * start boot_info structure right after kernel
         */
        top_memory = (ulong) end;
        boot_info = (struct boot_info_s*) top_memory;
        top_memory += sizeof(struct boot_info_s);

        /*
         * boot_task pointer is initially null
         * this means no boot tasks were loaded or they were
         * somehow screwed up
         */
        boot_info->boot_task = NULL;

        /*
         * fill up boot_info
         */
        boot_info->magic = BOOT_INFO_MAGIC;
        boot_info->kern_start = (ulong) _start;
        boot_info->kern_end = (ulong) end;

        base = (nvread(MEM_BASE_LO) | (nvread(MEM_BASE_HI) << 8));
        ext = (nvread(MEM_EXT_LO) | (nvread(MEM_EXT_HI) << 8));
        boot_info->mem_size = (base + ext) * 1024;
        printf("boot_info->mem_size = %d\n", boot_info->mem_size);

        /*
         * sanity check on what bootloader gave us
         */
        i = (struct info_page*) bootloader_info;
        if(i->file_count > 32 || i->file_count < 0) {
                printf("oops, wierd info page data...bailing out\n");
                return;
        }

        /*
         * iterate through the list of boot tasks
         * and arrange it in a known format
         */
        if(i->file_count) {
                struct boot_task_s* b;

                /*
                 * point boot task list right after boot info structure
                 */
                boot_info->boot_task = (struct boot_task_s*) top_memory;
                b = boot_info->boot_task;
                
                f = &i->files[0];
                boot_info->boot_start = f->start;
                boot_info->boot_count = 0;

                for(c = 0; c < i->file_count; c++) {
                        printf("file[%d]: %s, start %08x, size %d [%08x]\n",
                                c, f->name, f->start, f->size, f->size);

                        b->start = f->start;
                        b->size = f->size;
                        b->flags = 0;

                        b++;
                        f++;

                        boot_info->boot_count++;
                }
                f--;
                boot_info->boot_end = page_round(f->start + f->size);
                /*top_memory = (ulong) b;*/
                top_memory = boot_info->boot_end;
        }

        printf("boot_info at %08x\n", (ulong) boot_info);
        printf("top_memory   %08x\n", top_memory);
}

void
boot_coffdump()
{
        FILHDR* hdr;
        AOUTHDR* aout;
        SCNHDR* text, *data, *bss;
        void* p, *ptr;
	int c;

        if(boot_info->magic != BOOT_INFO_MAGIC) {
                printf("load_boot(): oops, boot_info not set up yet\n");
                return;
        }

        if(!boot_info->boot_task) {
                printf("load_boot(): no boot tasks loaded\n");
                return;
        }

	for(c = 0; c < boot_info->boot_count; c++) {
	        ptr = (void*) boot_info->boot_task[c].start;
        	p = ptr;
	        hdr = p; p += sizeof(*hdr);
        	aout = p; p += sizeof(*aout);
	        text = p; p += sizeof(*text);
        	data = p; p += sizeof(*data);
	        bss = p;

        	printf("aout: tsize %lu dsize %lu bsize %lu\n",
	                aout->tsize, aout->dsize, aout->bsize);
        	printf("      entry %08x text_start %08x data_start %08x\n",
                	aout->entry, aout->text_start, aout->data_start);

	        printf("text: [%08x:%08x:%08x:%08x]\n",
        	        text->s_paddr, text->s_vaddr, 
			text->s_size, text->s_scnptr);
	        printf("data: [%08x:%08x:%08x:%08x]\n",
        	        data->s_paddr, data->s_vaddr, 
			data->s_size, data->s_scnptr);
	        printf("bss : [%08x:%08x:%08x:%08x]\n",
        	        bss->s_paddr, bss->s_vaddr, 
			bss->s_size, bss->s_scnptr);
	}

#if 0
        fseek(f, text.s_scnptr, SEEK_SET);
        read_section(f, text.s_size);
        cur_mem += (data.s_vaddr - text.s_vaddr);
        fseek(f, data.s_scnptr, SEEK_SET);
        read_section(f, data.s_size);
        cur_mem += (bss.s_vaddr - data.s_vaddr) + bss.s_size;
#endif
}

/*
 * multiboot stuff
 *
 */
 

static void multiboot_dump_modules(struct multiboot_info*);

void
multiboot_dump(struct multiboot_info* mbi)
{
	printf("MultiBoot info structure at %08x\n", (ulong)mbi);

	if(mbi->flags & MB_INFO_MEMORY)
		printf("MB_INFO_MEMORY: memory: %dK base, %dK extended\n", 
			mbi->mem_lower, mbi->mem_upper);
			
	if(mbi->flags & MB_INFO_CMDLINE)
		printf("MB_INFO_CMDLINE: cmdline[%08x]: %s\n", 
			mbi->cmdline, (char*)mbi->cmdline);

	if(mbi->flags & MB_INFO_MODS) {
		printf("MB_INFO_MODS: module count: %d; start addr: %08x\n", 
			mbi->mods_count, mbi->mods_addr);
		multiboot_dump_modules(mbi);
	}
	
	if(mbi->flags & MB_INFO_AOUT_SYMS) {
		printf("MB_INFO_AOUT_SYMS:\n");
	}
	if(mbi->flags & MB_INFO_ELF_SHDR) {
		printf("MB_INFO_ELF_SHDR:\n");
	}

	if(mbi->flags & MB_INFO_MEM_MAP) {
		printf("MB_INFO_MEM_MAP:\n");
	}
}

static void
multiboot_dump_modules(struct multiboot_info* mbi)
{
	int c;
	struct mod_list* ml;
	
	ml = (struct mod_list*)mbi->mods_addr;
	for(c = 0; c < mbi->mods_count; c++) {
		printf("module [%d]: <%08x:%08x> cmdline<%08x>:%s\n",
			c, ml->mod_start, ml->mod_end, 
			ml->cmdline, (char*)ml->cmdline);
		ml++;
	}
}


extern void load_font(char*, ulong);
struct multiboot_info* multiboot_relocate(struct multiboot_info*);

/*
 * we relocate multiboot info structure just after
 * the last module loaded. This simplifies memory
 * management, since we don't have to worry about boot 
 * information somewhere in lower memory.
 */
void
multiboot_init_original(struct multiboot_info* orig_mbi)
{
	struct multiboot_info* mbi;
		
	mbi = multiboot_relocate(orig_mbi);
	multiboot_dump(mbi);
	
	if(mbi->flags & MB_INFO_MODS) {
		char* font_addr;
		ulong font_size, unit_size;
		struct mod_list* ml;

		printf("There is %d module(s) loaded...\n",
			mbi->mods_count);

		ml = (struct mod_list*)mbi->mods_addr;
		font_addr = (char*)ml->mod_start;
		font_size = ml->mod_end - ml->mod_start;
		unit_size = font_size/256;
		printf("Loading font: %s size[%d] height[%d]\n", 
			(char*)ml->cmdline, font_size, unit_size);
 
		load_font(font_addr, unit_size);
	}
}

struct multiboot_info*
multiboot_relocate(struct multiboot_info* mbi)
{
	void* p;
	ulong len, c;
	struct multiboot_info* mbi2;
	struct mod_list* last_mod, *ml, *ml2;

	last_mod = ((struct mod_list*)mbi->mods_addr) + mbi->mods_count - 1;
	printf("last_mod: <%08x:%08x:%08x>\n",
		last_mod->mod_start, last_mod->mod_end, last_mod->cmdline);

	/*
	 * we relocate multiboot info structure to page
	 * aligned location just after the last module
	 */
	p = (void*)page_round(last_mod->mod_end);
	mbi2 = (struct multiboot_info*)p;

	printf("copying multiboot info structure to: %08x\n", (ulong)p);
	bcopy(mbi, p, sizeof(*mbi));
	p += sizeof(*mbi);
	printf("copying kernel cmdline to: %08x\n", (ulong)p);
	len = strlen((char*)mbi->cmdline) + 1;
	bcopy((void*)mbi->cmdline, p, len);
	/* adjust cmdline pointer */
	mbi2->cmdline = (ulong)p;
	
	p += len;
	printf("copying mod list to: %08x\n", (ulong)p);
	len = mbi->mods_count * sizeof(struct mod_list);
	bcopy((void*)mbi->mods_addr, p, len);
	/* adjust mod list pointer */
	mbi2->mods_addr = (ulong)p;

	ml = (struct mod_list*)mbi->mods_addr;	
	ml2 = (struct mod_list*)p;
	p += len;
	for(c = 0; c < mbi->mods_count; c++) {
		printf("copying cmdline of mod %d to: %08x\n",
			c, (ulong)p);
		len = strlen((char*)ml->cmdline) + 1;
		bcopy((void*)ml->cmdline, p, len);
		ml2->cmdline = (ulong)p;
		p += len;
		ml++;
		ml2++;
	}

	top_memory = page_round((ulong)p);
	printf("Top memory: %08x\n", top_memory);
	
	return (mbi2);
} 

int
multiboot_init(struct multiboot_info* m)
{
	if(m->flags & MB_INFO_MODS) {
		struct mod_list* last_mod;

		last_mod = ((struct mod_list*) m->mods_addr) 
			+ m->mods_count - 1;
		printf("last_mod: <%08x:%08x:%08x>\n",
			last_mod->mod_start, 
			last_mod->mod_end, 
			last_mod->cmdline);
		
	        /*
        	 * start boot_info structure right after 
	         * the last module loaded
        	 */
	        top_memory = (ulong) last_mod->mod_end;
	} else
		/*
		 * no modules loaded
		 * start boot_info structure after kernel bss
		 */
		top_memory = (ulong) end;	    
        

	/*
	 * setup boot info
	 */        
        boot_info = (struct boot_info_s*) top_memory;
        top_memory += sizeof(struct boot_info_s);

        /*
         * boot_task pointer is initially null
         * this means no boot tasks were loaded or they were
         * somehow screwed up
         */
        boot_info->boot_task = NULL;

        /*
         * fill up boot_info
         */
        boot_info->magic = BOOT_INFO_MAGIC;
        boot_info->kern_start = (ulong) _start;
        boot_info->kern_end = (ulong) end;
        
	boot_info->mem_size = (m->mem_lower + m->mem_upper) * 1024;        
        printf("boot_info->mem_size = %d\n", boot_info->mem_size);
        
        if(m->flags & MB_INFO_MODS) {
        	ulong c;
        	struct mod_list* mod;
        	struct boot_task_s* b;
        	
        	boot_info->boot_task = b = (struct boot_task_s*) top_memory;
        	boot_info->boot_count = 0;
        	
        	mod = (struct mod_list*) m->mods_addr;
        	boot_info->boot_start = mod->mod_start;
        	for(c = 0; c < m->mods_count; c++, mod++, b++) {
			b->start = mod->mod_start;
			b->size = (mod->mod_end - mod->mod_start);
			b->flags = 0;
			
			boot_info->boot_count++;
        	}
        	top_memory = (ulong) b; /* after last boot_task struct */
		boot_info->boot_end = (mod-1)->mod_end;
        	
        	b = boot_info->boot_task;
        	mod = (struct mod_list*) m->mods_addr;
        	for(c = 0; c < m->mods_count; c++, mod++, b++) {
        		int len = strlen((char*) mod->cmdline) + 1;
        		b->cmdline = top_memory;
        		bcopy((void*) mod->cmdline, (void*) b->cmdline, len);
        		top_memory += len;
        	}
        	top_memory = page_round(top_memory);
        }
        
        return 1;
}

void
bootinfo_dump()
{
	int c;

        if(boot_info == 0 || boot_info->magic != BOOT_INFO_MAGIC) {
                printf("bootinfo_dump(): oops, boot_info not set up yet\n");
                return;
        }
	printf("bootinfo_dump(): begin...\n");

	printf("boot_info at: %08x\n", (ulong) boot_info);	
	printf("boot_start: %08x, boot_end: %08x\n",
		boot_info->boot_start, boot_info->boot_end);
        if(boot_info->boot_task) {
        	printf("boot_task at: %08x\n", (ulong) boot_info->boot_task);
		for(c = 0; c < boot_info->boot_count; c++) {
		        struct boot_task_s* b = &boot_info->boot_task[c];
	        	printf("boot_task[%d]: %08x %08x %08x\n",
	        		c, b->start, b->size, b->cmdline);
	        	printf("cmdline: %s\n", (char*) b->cmdline);
		}
	}
	printf("top_memory: %08x\n", top_memory);
	printf("bootinfo_dump(): end...\n");
}


#if 0
	
static void
reserve(ulong start, ulong end)
{
	printf("reserve: (%08x-%08x) %08x-%08x\n",
		start, end,
		(start & 0xFFFFF000), round_page(end));
}

void
multiboot_scanmem(struct multiboot_info* mbi)
{
	int c;
	struct mod_list* ml;

	reserve((ulong)mbi, ((ulong)mbi) + sizeof(struct multiboot_info));
	
	if(mbi->flags & MB_INFO_CMDLINE)
		reserve(mbi->cmdline, mbi->cmdline 
			+ strlen((char*)mbi->cmdline) + 1);

	if(mbi->flags & MB_INFO_MODS) {
		reserve(mbi->mods_addr, mbi->mods_addr 
			+ (mbi->mods_count * sizeof(struct mod_list)));

		ml = (struct mod_list*)mbi->mods_addr;
		for(c = 0; c < mbi->mods_count; c++) {
			reserve(ml->cmdline, ml->cmdline 
				+ strlen((char*)ml->cmdline) + 1);
			ml++;
		}
	}
}

#endif
