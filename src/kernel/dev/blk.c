/* TINOS Operating System
 * Copyright (c) 1996, 1997, 1998 Bart Sekura
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
 * $Id: blk.c,v 1.4 1998/03/10 16:03:18 bart Exp $
 *
 */
 
#include <sys/types.h>
#include <pc/ide.h>
#include <i386/page.h>

#include "blk.h"
#include "ext2fs.h"

/*
 * some known partition types
 */
#define	PT_NULL		(0x00)
#define	PT_FAT12	(0x01)
#define	PT_FATSMALL	(0x04)	/* dos fs 16-bit < 32MB */
#define	PT_EXTENDED	(0x05)
#define	PT_FAT		(0x06)	/* dos fs 16-bit >= 32MB */
#define	PT_HPFS		(0x07)	/* also Windows NT NTFS */
#define	PT_AIX		(0x08)
#define	PT_AIX_BOOT	(0x09)
#define	PT_BOOTMAN	(0x0a)	/* OS/2 boot manager */
#define	PT_HURD		(0x63)
#define	PT_OLD_MINIX	(0x80)
#define	PT_LINUX_MINIX	(0x81)
#define	PT_LINUX_SWAP	(0x82)
#define	PT_LINUX	(0x83)
#define	PT_BSD_386	(0xa5)
#define	PT_BSDI_FS	(0xb7)
#define	PT_BSDI_SWAP	(0xb8)

#define	MAX_PARTITIONS	(4)

struct partition_desc {
	byte	boot;
	byte	head;
	byte	sec;
	byte	cyl;
	byte	type;
	byte	ehead;
	byte	esec;
	byte	ecyl;
	ulong	start;
	ulong	len;
};

struct dev_blk {
	ulong	dev_no;
	ulong	start;
	ulong	end;
	ulong	block_size;
	ulong	block_sectors;
	ulong	type;
};

#define MAX_DEVBLK	(16)
static struct dev_blk devblk_table[MAX_DEVBLK];
static ulong devblk_count;

static void devblk_scan(ulong, ulong, ulong);
static char* devblk_name(ulong);

void
devblk_init()
{
	int c;
	ulong dev_no, dev_size;
	struct ide_disk* ide;


	dev_no = 0;
	while((ide = ide_get_device(dev_no))) {
		if((ide->flags & DF_CDROM) || (ide->flags & DF_TAPE)) {
			dev_no++;
			continue;
		}
	
		devblk_count = 0;	
		devblk_scan(dev_no, ide->size, 0);
		dev_no++;
	}

	for(c = 0; c < devblk_count; c++) {
		struct dev_blk* dev;
	
		dev = &devblk_table[c];
		printf("dev[%d]: %-12d %-12d %-5dMB %-15s [0x%02x]\n",
			c, 
			dev->start, 
			dev->end,
  			(dev->end - dev->start)/2048,
			devblk_name(dev->type),
			dev->type);

	}
}

/*
 * read in the sector containing partition table
 */
static ulong
devblk_get_partition_table(ulong dev_no, 
			   ulong sector, 
			   struct partition_desc* part)
{
	int c;
	ulong count;
	byte buf[SECTOR_SIZE];
	struct partition_desc* p;

	ide_io(dev_no, 0, (void*) buf, sector, 1);
	p = (struct partition_desc*) ((void*) buf + 0x1be);
	count = 0;
	for(c = 0; c < 4; c++, p++, part++) {
		if(p->type == PT_NULL)
			continue;

		bcopy(p, part, sizeof(*p));
		count++;
	}
	
	return (count);
}

/*
 * scan devices and read partitions
 * this routine is called recursively to handle all extended partitions
 */
static void
devblk_scan(ulong dev_no, ulong dev_size, ulong sector)
{
	static ulong base = 0;
	struct partition_desc p[4];
	ulong c, count;

	count = devblk_get_partition_table(dev_no, sector, &p[0]);
	for(c = 0; c < count; c++) {
		struct dev_blk* dev;
	
		if(p[c].type == PT_EXTENDED) {
			if(!base) {
				base = p[c].start;
				devblk_scan(dev_no, dev_size, sector + base);
			} else
				devblk_scan(dev_no, 
					    dev_size, 
					    p[c].start + base);
				
			continue;
		}

#if _DEBUG		
		printf("type[%d]=%x, start=%d, len=%d\n",
			c, p[c].type, p[c].start, p[c].len);	
#endif			

		if(p[c].start > dev_size 
		   || (p[c].start + p[c].len) > dev_size) {
			printf("partition %d has wierd parameters, skipping\n", 
				c);
			continue;
		}
		
		dev = &devblk_table[devblk_count++];
		dev->dev_no = dev_no;
		dev->type = p[c].type;
		dev->start = p[c].start + sector;
		dev->end = dev->start + p[c].len;
		dev->block_size = SECTOR_SIZE;
		dev->block_sectors = 1;
	}
}

static char*
devblk_name(ulong type)
{
	switch(type) {
		case PT_NULL: return "null";
		case PT_FAT12: return "fat12";
		case PT_FATSMALL: return "fat (<32MB)";
		case PT_FAT: return "fat";
		case PT_HPFS: return "hpfs/ntfs";
		case PT_AIX: return "aix";
		case PT_AIX_BOOT: return "aix (bootable)";
		case PT_BOOTMAN: return "OS/2 boot manager";
		case PT_HURD: return "hurd";
		case PT_OLD_MINIX: return "old minix";
		case PT_LINUX_MINIX: return "linux/minix";
		case PT_LINUX_SWAP: return "linux swap";
		case PT_LINUX: return "linux (ex2fs)";
		case PT_BSD_386: return "bsd386";
		case PT_BSDI_FS: return "bsdi fs";
		case PT_BSDI_SWAP: return "bsdi swap";
	}
	return "unknown";
}

static struct dev_blk*
devblk_get(ulong dev_no)
{
	if(dev_no >= devblk_count) {
		printf("devblk_get(): nonexistent device: %d\n", dev_no);
		return 0;
	}
		
	return &devblk_table[dev_no];
}

int
devblk_set_blksize(ulong dev_no, ulong blk_size)
{
	struct dev_blk* dev;

	if(blk_size > PAGESZ)
		return 0;
		
	if(!(dev = devblk_get(dev_no)))
		return 0;
		
	dev->block_size = blk_size;
	dev->block_sectors = dev->block_size >> 9;
	return 1;	
}

int
devblk_read(ulong dev_no, ulong block, ulong count, void* buf)
{
	struct dev_blk* dev;
	
	if((dev = devblk_get(dev_no))) {
		return ide_io(dev->dev_no, 
			      0,
			      buf,
			      dev->start + (block * dev->block_sectors),
			      count * dev->block_sectors);		   
	}
	
	return 0;			   
}
