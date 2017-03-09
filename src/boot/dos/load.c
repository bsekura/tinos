/*
 * $Id: load.c,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */
#include <stdio.h>
#include <alloc.h>
#include <process.h>
#include <string.h>

#include "desc.h"
#include "coff.h"

#include "boot.h"

#define _DEBUG

#ifdef _DEBUG
#include <conio.h>
#endif

#define PAGESZ		(4096)
#define NPTE		(1024)
#define PGSHIFT 	(12)
#define IPD(va) 	(((ulong)va) >> 22)
#define round_page(x)	(((x) + (PAGESZ-1)) & ~(PAGESZ-1))
#define btorp(x)	(((ulong)(x) + (PAGESZ-1)) >> PGSHIFT)
#define linear(x)	(((x) & 0xFFFF) + (((x) >> 16) << 4))
#define segment(x)	((x) >> 16)
#define offset(x)	((x) & 0xFFFF)
#define segmented(x)	((((x) >> 4) << 16) | ((x) & 0xF))

#define TOPMEM		(655360) /* 640K */

extern void jump(void);
extern void a20_on(void);

ulong base_mem;
ulong cur_mem;
ulong start_mem;
ulong npages;

ulong kernel_start;
ulong kernel_size;

struct info_page* pinfo;
ulong cur_file = 0;

ulong page_dir;   /* page dir linear */
struct {
	ushort len;
	ulong  base;
} gdt_desc;

ulong entry_point;


static void
dump_pinfo(void)
{
	int c;
	struct file_desc* curf;

	printf("base_mem	   : %08lx\n", base_mem);
	printf("pinfo->file_count  : %d\n", pinfo->file_count);

	curf = &pinfo->files[0];
	for(c = 0; c < pinfo->file_count; c++) {
		printf("%-8s %08lx %lu\n",
			curf->name, curf->start, curf->size);
		curf++;
	}
}

static void
error(char* p)
{
	printf("error: %s\n", p);
	exit(-1);
}

/*
 * rip a page and setup gdt there
 */
void
setup_gdt(void)
{
	struct sys_desc* gdt;
	ulong lgdt;
	uchar* p;

	p = (uchar*) segmented(cur_mem);
	memset(p, 0, PAGESZ);
	cur_mem += PAGESZ;

	gdt = (struct sys_desc*) p;
	lgdt = linear((ulong) p);
	printf("linear gdt = %08lx\n", lgdt);

	/* first gdt entry is null */
	gdt++;

	/* code */
	gdt->limit = 0xFFFF;
	gdt->base_0_15 = 0x0000;
	gdt->base_16_23 = 0x00;
	gdt->dpl_type = 0x9A;
	gdt->gav_lim = 0xCF;
	gdt->base_24_31 = 0x00;

	/* data */
	gdt++;
	gdt->limit = 0xFFFF;
	gdt->base_0_15 = 0x0000;
	gdt->base_16_23 = 0x00;
	gdt->dpl_type = 0x92;
	gdt->gav_lim = 0xCF;
	gdt->base_24_31 = 0x00;

	/* descriptor */
	gdt_desc.len = 4096; /* guess it won't hurt */
	gdt_desc.base = lgdt;
}

#define CHUNK_SIZE	(32768)
/*
 * this reads the file contents into current
 * memory pointer and advances all pointers ...
 */
ulong
read_file(FILE* f)
{
	ulong at, r;

	if(!f) {
		error("read_file(): f = null\n");
	}

	if(cur_mem >= 0x90000) {
		error("read_file(): no memory.");
	}

	at = cur_mem;
	while(1) {
		size_t r;
		uchar* p;

		p = (void*)segmented(at);
		printf("reading at: %08lx [%04lx:%04lx]...",
			at, segment((ulong)p), offset((ulong)p));

		r = fread(p, 1, CHUNK_SIZE, f);
		printf("%u bytes read\n", r);

		at += r;
		if(r < CHUNK_SIZE)
			break;
	}

	r = at - cur_mem;
	cur_mem = round_page(at);

	return (r);
}

void
read_section(FILE* f, ulong size)
{
	ulong at, left, read;
	size_t r;

	if(!f) {
		error("read_file(): f = null\n");
	}

	if(cur_mem >= 0x90000) {
		error("read_file(): no memory.");
	}

	left = size;
	read = 0;
	at = cur_mem;
	while(1) {
		uchar* p;
		ulong chunk;

		p = (void*) segmented(at);
		printf("reading at: %08lx [%04lx:%04lx]...",
			at, segment((ulong)p), offset((ulong)p));

		chunk = (left > CHUNK_SIZE) ? CHUNK_SIZE : left;
		r = fread(p, 1, chunk, f);
		printf("%04x(%u) bytes read\n", r, r);

		at += r;
		read += r;
		left -= r;
		if(r < chunk || left <= 0)
			break;
	}
}

void
load_coff(char* filename)
{
	FILE* f;
	FILHDR hdr;
	AOUTHDR aout;
	SCNHDR text, data, bss;
	ulong save_mem;
	ulong text_size, data_size;

	f = fopen(filename, "rb");
	if(!f) {
		error("load_coff(): cannot open file.");
	}

	if(!fread(&hdr, sizeof(FILHDR), 1, f)) {
		error("read failed for file header.");
	}
	printf("FILHDR: f_magic: %x, f_nscns: %d, f_flags: %x\n",
		hdr.f_magic, hdr.f_nscns, hdr.f_flags);

	if(!fread(&aout, sizeof(AOUTHDR), 1, f)) {
		error("read failed for a.out header.");
	}

	if(!fread(&text, sizeof(SCNHDR), 1, f)) {
		error("read failed for text section.");
	}
	if(!fread(&data, sizeof(SCNHDR), 1, f)) {
		error("read failed for data section.");
	}
	if(!fread(&bss, sizeof(SCNHDR), 1, f)) {
		error("read failed for bss section.");
	}

#ifdef _DEBUG
	printf("aout: tsize %lu dsize %lu bsize %lu\n",
		aout.tsize, aout.dsize, aout.bsize);
	printf("      entry %08lx text_start %08lx data_start %08lx\n",
		aout.entry, aout.text_start, aout.data_start);
#endif

	printf("text: [%08lx:%08lx:%08lx:%08lx]\n",
		text.s_paddr, text.s_vaddr, text.s_size, text.s_scnptr);
	printf("data: [%08lx:%08lx:%08lx:%08lx]\n",
		data.s_paddr, data.s_vaddr, data.s_size, data.s_scnptr);
	printf("bss : [%08lx:%08lx:%08lx:%08lx]\n",
		bss.s_paddr, bss.s_vaddr, bss.s_size, bss.s_scnptr);

	entry_point = aout.entry;
	kernel_start = cur_mem;

	fseek(f, text.s_scnptr, SEEK_SET);
	read_section(f, text.s_size);
	cur_mem += (data.s_vaddr - text.s_vaddr);
	fseek(f, data.s_scnptr, SEEK_SET);
	read_section(f, data.s_size);
	cur_mem += (bss.s_vaddr - data.s_vaddr) + bss.s_size;
	fclose(f);

	kernel_size = cur_mem - kernel_start;
	cur_mem = round_page(cur_mem);

#ifdef _DEBUG
	printf("cur_mem = %08lx\n", cur_mem);
#endif
}

void
load_file(char* filename)
{
	FILE* f;
	ulong snapshot, size;

	f = fopen(filename, "rb");
	if(!f)
		error("cannot open file.");

	strncpy(pinfo->files[cur_file].name, filename, 8);
	snapshot = cur_mem;
	size = read_file(f);
	fclose(f);

#ifdef _DEBUG
	printf("cur_mem: %08lx\n", cur_mem);
#endif

	pinfo->files[cur_file].size = size;
	pinfo->files[cur_file].start = snapshot;
	cur_file++;
	pinfo->file_count++;
}

void
main(int argc, char** argv)
{
	ulong* ptr;
	void* sptr;
	ulong sbase, lbase;
	int c;

	if(argc < 2) {
		error("specify filename(s).");
	}

	/*
	 * find the beginning of free memory ...
	 */
	sptr = sbrk(0);
	sbase = (ulong)sptr;
	lbase = linear(sbase);
	npages = 0;
	base_mem = cur_mem = round_page(lbase) + PAGESZ;

#ifdef _DEBUG
	printf("DOS base: %08lx [%04lx:%04lx]\n",
		lbase, segment(sbase), offset(sbase));
	printf("linear, aligned base mem: %08lx\n", base_mem);
#endif

	/*
	 * first page is our info page ...
	 */
	printf("info page at %08lx\n", cur_mem);
	pinfo = (struct info_page*) segmented(cur_mem);
	memset(pinfo, 0, PAGESZ);
	cur_file = 0;

	cur_mem += PAGESZ;
	start_mem = cur_mem;

	setup_gdt();

	/*
	 * succum files ...
	 */
	load_coff(argv[1]);
	for(c = 2; c < argc; c++)
		load_file(argv[c]);

	printf("kernel start = %08lx, size = %lu\n",
		kernel_start, kernel_size);

#ifdef _DEBUG
	printf("ready? \n");
	if(getch() == 'q') {
		dump_pinfo();
		return;
	}
#endif

	asm {
		mov ax, 0305h
		xor bx, bx
		int 16h
	}


	a20_on();
	jump();
}
