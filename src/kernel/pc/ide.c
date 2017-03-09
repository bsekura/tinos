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
 * $Id: ide.c,v 1.4 1998/03/10 07:21:32 bart Exp $
 *
 */

#include <sema.h> 
#include <pc/ide.h>
#include <i386/inlines.h>

static struct ide_interface ide[MAX_IDE_INTERFACES] = {
	{ IDE_BASEIO_1, IDE_IRQ_1 },
	{ IDE_BASEIO_2, IDE_IRQ_2 },
	{ IDE_BASEIO_3, IDE_IRQ_3 },
	{ IDE_BASEIO_4, IDE_IRQ_4 }
};

#define MAX_IDE_DEVICES	(MAX_IDE_INTERFACES*2)
static struct ide_disk* ide_dev_table[MAX_IDE_DEVICES];
static ulong ide_dev_count;

static ulong ide_ecc_count;

struct ide_request {
	struct ide_disk* device;
	ulong	op;
	void*	buf;
	ulong	sector;
	ulong	scount;
	ulong	io_scount;
};

static struct ide_interface* cur_iface;
static struct ide_request request;
static semaphore_t ide_lock;
static semaphore_t ide_busy;

static int ide_ready(struct ide_interface*);
static int ide_data(struct ide_interface*);
static int ide_identify(struct ide_interface*, uchar, uchar);
static int ide_diag(struct ide_interface*);

void ide_interrupt();

void
ide_init()
{
	int c;

	for(c = 0; c < 2 /*MAX_IDE_INTERFACES*/; c++) {
		printf("ide%d: ", c);
	
		interrupt_register(ide[c].irq, ide_interrupt);
	 	if(ide_identify(&ide[c], 0, IDE_CMD_IDENTIFY)) 
	 		continue;
	 		
	 	if(!ide_identify(&ide[c], 0, IDE_CMD_PIDENTIFY)) {
 			printf("no interface\n");
 			interrupt_unregister(ide[c].irq);
 		}
 	} 
 	
 	sema_init(&ide_lock);
 	sema_init(&ide_busy);
 	sema_setval(&ide_busy, 0);
}

static int
ide_identify(struct ide_interface* i, uchar unit, uchar cmd)
{
	int c;
	char* p;
	char buf[SECTOR_SIZE];
	struct ide_disk* d;
	struct ide_identify_data* id;

	/*
	 * select unit
	 */
	outb(i->io_base + IDE_SELECT, IDE_UNIT(unit));
	if(!ide_ready(i))
		return 0;

	outb(i->io_base + IDE_CONTROL, 0x08);

	cli();	
	/*
	 * send identify command
	 */
	cur_iface = i;
	outb(i->io_base + IDE_COMMAND, cmd);
	if(!ide_ready(i))
		return 0;

	/*
	 * get data
	 */		
	if(!ide_data(i))
		return 0;
				
	repinsw(i->io_base + IDE_DATA, buf, sizeof(buf)/sizeof(ushort));
	(void) inb(i->io_base + IDE_STATUS);
	id = (struct ide_identify_data*) buf;
	sti();
	
	/*
	 * fix model field (swap)
	 */
	p = id->model;
	for(c = 0; c < MODEL_SZ; c += 2, p += 2) {
		char tmp;
		
		tmp = *p;
		*p = *(p+1);
		*(p+1) = tmp;		
	}
	
	/*
	 * trim trailing spaces
	 */
	p = id->model + MODEL_SZ - 1;
	while(*p-- == ' ');
	*(p+2) = 0;

	printf("%s ", id->model);
	
	d = &i->drives[unit];
	ide_dev_table[ide_dev_count++] = d;
	
	d->iface = i;
	d->flags = 0;
	d->unit = unit;
	d->select = IDE_UNIT(unit);
	
	if(cmd == IDE_CMD_PIDENTIFY) {
		uchar type;
		
		type = (id->config >> 8) & 0x1f;
		if(type == 5) {
			d->flags |= (DF_CDROM | DF_CONFIGURED);
			printf("ATAPI CDROM\n");
		} else
		if(type == 1) {
			d->flags |= (DF_TAPE | DF_CONFIGURED);
			printf("ATAPI TAPE\n");
		} else {
			printf("unknown device\n");
		}
		
		return 1;
	}		

	d = &i->drives[unit];
	d->flags |= DF_CONFIGURED;
	d->cyls = id->cyls;
	d->heads = id->heads;
	d->sectors = id->sectors;	
	d->size = d->cyls * d->sectors * d->heads;
	
	d->cyl_sectors = d->sectors * d->heads;
	d->multi = id->multsect_valid ? id->multsect : 0;
	
	printf("CHS:%d/%d/%d size:%d(%dMB) cache:%dkb multi:%d\n", 
		d->cyls, d->heads, d->sectors,
		d->size, d->size/2048,
		id->buf_size/2, d->multi);

	if(d->multi && ide_ready(i)) {
		outb(i->io_base + IDE_CONTROL, 0x08);
		outb(i->io_base + IDE_SCOUNT, d->multi);
		outb(i->io_base + IDE_COMMAND, IDE_CMD_SETMULTI);
	}
	
	return 1;
}

static int
ide_diag(struct ide_interface* i)
{
	if(!ide_ready(i))
		return 0;
		
	outb(i->io_base + IDE_COMMAND, IDE_CMD_DIAG);
	if(!ide_ready(i))
		return 0;
	
	inb(i->io_base + IDE_ERROR);
	return 1;
}

static int
ide_io_start(struct ide_request* r)
{
	struct ide_interface* i;
	struct ide_disk* d;
	ulong sect, track, head, cyl;
	ulong sector, scount, sleft;

	d = r->device;
	i = d->iface;
	 		
	sector = r->sector;
	scount = r->scount;

	/*
	 * calculate disk parameters for next IO
	 */
	track = sector / d->sectors;
	sect = (sector % d->sectors) + 1;				
	head = track % d->heads;
	cyl = track / d->heads;
	
	/*
	 * see how many sectors left within this cylinder
	 * set the sector count for IO operation
	 */
	sleft = d->cyl_sectors - sect;
	if(scount > sleft)
		r->io_scount = sleft;
	else
		r->io_scount = scount;

#ifdef _DEBUG		
	printf("ide_io_start(): sect %d, cyl %d, head %d\n",
		sect, cyl, head);
	printf("ide_io_start(): sectors left: %d\n", sleft);
#endif

	/*
	 * program controller to fire up IO
	 */		
	if(ide_ready(i)) {
		outb(i->io_base + IDE_SECTOR, sect);
		outb(i->io_base + IDE_SCOUNT, r->io_scount);
		outb(i->io_base + IDE_LCYL, cyl & 0xff);
		outb(i->io_base + IDE_HCYL, (cyl >> 8) & 0xff);
		outb(i->io_base + IDE_SELECT, d->select | head);
		outb(i->io_base + IDE_COMMAND, r->op);
		
		return 1;
	}
	
	printf("ide_io_start(): drive not ready\n");
	return 0;

#if 0		
	{
		ulong secpercyl = d->heads * d->sectors;
			
		cyl = sector / secpercyl;
		sect = sector % secpercyl;
		track = sect / d->sectors;
		sect = (sect % d->sectors) + 1;

		printf("ide_start_io(): sect %d, cyl %d, head %d\n",
			sect, cyl, head);
			
	}
#endif
}

/*
 * IO is done; release locks and clear operation type
 */
void
ide_io_done()
{
	extern ulong sched_preempt;

	sema_v(&ide_busy);
	sema_v(&ide_lock);
	sched_preempt++;
	request.op = 0;
}

/*
 * fancy error dump
 * might come handy sometimes
 */
static void
ide_dump_error(struct ide_interface* i)
{
	ulong err;

	printf("ide_dump_error():\n");	
	err = inb(i->io_base + IDE_ERROR);
	if(err & IDE_ERR_MARK)
		printf("data address mark not found\n");
	if(err & IDE_ERR_TRACK0)
		printf("track 0 not found\n");
	if(err & IDE_ERR_ABORT)
		printf("command aborted\n");
	if(err & IDE_ERR_ID)
		printf("sector not found\n");
	if(err & IDE_ERR_MC)
		printf("media change\n");
	if(err & IDE_ERR_ECC)
		printf("uncorrectable ecc\n");
	if(err & IDE_ERR_BAD)
		printf("bad block detected\n");
}

/*
 * interrupt handler
 */
void
ide_interrupt()
{
	ulong status;
	struct ide_request* r;
	struct ide_interface* i;

	r = &request;
	if((i = cur_iface) == 0) {
		printf("ide_interrupt(): fatal!, no interface\n");
		return;
	}
	
	status = inb(i->io_base + IDE_STATUS);
	if(status & IDE_STAT_ERROR) {
		printf("ide_interrupt(): drive error occurred\n");
		ide_dump_error(i);
		return;
	}
	
	if(status & IDE_STAT_ECC) {
		printf("ide_interrupt(): warning: ecc\n");
		ide_ecc_count++;
		return;
	}

	if(r->op == IDE_CMD_READ || r->op == IDE_CMD_MULTREAD) {
		ulong multi, n;

#if _DEBUG		
		printf("ide_interrupt(): read command\n");
#endif		

		/*
		 * try to optimize as much as possible
		 * we check whether the drive supports multi sector
		 * operations, and we try to read sectors using
		 * max value supported by the drive
		 */		
		multi = r->device->multi;
		while(r->io_scount) {
		
			/*
			 * see how many sectors we can read at once
			 * if it's more than multi max supported,
			 * read max supported or whatever is left to read
			 * if multi is not supported - read 1 sector
			 */
			if(multi) {
				n = r->io_scount;
				if(n > multi)
					n = multi;
				multi -= n;
			} else
				n = 1;
#ifdef _DEBUG
			printf("ide_interrupt(): reading %d sector(s)\n", n);			
#endif
			repinsw(i->io_base + IDE_DATA, 
				r->buf, 
				SECTOR_WORDS * n);
				
			r->buf += (n << 9);
			r->sector += n;
			r->scount -= n;
			r->io_scount -= n;

			/*
			 * are we finished with this IO?
			 */			
			if(r->scount == 0) {
				ide_io_done();
				return;
			}
			
			/*
			 * we've read as many as we could have
			 * quit, hopefully next interrupt will happen
			 */
			if(multi == 0)
				return;
		}
		
		printf("ide_interrupt(): firing up next IO request\n");
		ide_io_start(r);
		
	} else
	if(r->op == IDE_CMD_WRITE || r->op == IDE_CMD_MULTWRITE) {
		printf("ide_interrupt(): write command\n");
	} 
#if 0
	else
		printf("ide_interrupt(): special command\n");
#endif	
}

static int
ide_ready(struct ide_interface* i)
{
	int c, status;
	
	for(c = 0; c < 1000000; c++) {
		status = inb(i->io_base + IDE_STATUS);
		if(!(status & IDE_STAT_BUSY) 
		   && (status & IDE_STAT_READY))
		   	return (status);
	}
	
	return 0;
}

static int
ide_data(struct ide_interface* i)
{
	int c, status;
	
	for(c = 0; c < 1000000; c++) {
		status = inb(i->io_base + IDE_STATUS);
		if(status & IDE_STAT_DRQ)
			return (status);	
			
		if(status & IDE_STAT_ERROR)
			return 0;
	}
}

/*
 * request IO operation from IDE device
 *
 * arguments: 	dev_no	- device number
 *		op	- operation to perform (read, write)
 *		buf	- memory buffer
 *		sector	- starting sector
 *		scount	- sector count
 */
int
ide_io(ulong dev_no, ulong op, void* buf, ulong sector, ulong scount)
{
	struct ide_disk* d;
	struct ide_request* r;

	/*
	 * zillions of sanity checks
	 */	
	if(dev_no >= ide_dev_count || !(d = ide_dev_table[dev_no])) {
		printf("ide_io(): nonexistent device specified\n");
		return 0;
	}
	
	if((d->flags & DF_CONFIGURED) == 0) {
		printf("ide_io(): device %d not configured\n", dev_no);
		return 0;
	}
	
	if(buf == 0 || scount == 0) {
		printf("ide_io(): invalid parameters\n");
		return 0;
	}
		
	if((d->flags & DF_CDROM) || (d->flags & DF_TAPE)) {
		printf("ide_io(): cdrom or tape not supported\n");
		return 0;
	}
	if(sector > d->size || sector + scount > d->size) {
		printf("ide_io(): sector+scount out of bounds\n");
		return 0;
	}

	/*
	 * prepare the request
	 * ide_lock will effectively serialize all IO requests 
	 */
	r = &request;	
	sema_p(&ide_lock);
	r->device = d;
	r->buf = buf;
	r->op = d->multi ? IDE_CMD_MULTREAD : IDE_CMD_READ; /* XXX temp */
	r->sector = sector;
	r->scount = scount;
	cur_iface = d->iface;
	
	if(ide_io_start(r)) {
		sema_p(&ide_busy);
		return 1;
	}
	
	sema_v(&ide_lock);
	return 0;
}

#if 0
ulong
ide_devsize(ulong dev_no)
{
	struct ide_disk* d;

	if(dev_no >= ide_dev_count || !(d = ide_dev_table[dev_no]))
		return 0;
		
	return d->size;
}
#endif

struct ide_disk*
ide_get_device(ulong dev_no)
{
	struct ide_disk* d;

	if(dev_no < ide_dev_count && (d = ide_dev_table[dev_no]))
		return (d);
	
	return (struct ide_disk*) 0;
}
