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
 * $Id: ide.h,v 1.2 1998/03/09 18:38:15 bart Exp $
 *
 */
 
#ifndef	__ide_h__
#define	__ide_h__

#include <sys/types.h>

#define	SECTOR_SIZE	(512)
#define	SECTOR_WORDS	(SECTOR_SIZE>>1)

/*
 * max number of ide interfaces
 */   
#define	MAX_IDE_INTERFACES	(4)

/*
 * possible interrupts
 */
#define	IDE_IRQ_1	(14)
#define	IDE_IRQ_2	(15)
#define	IDE_IRQ_3	(11)
#define	IDE_IRQ_4	(10)
 
/*
 * IO base addresses
 */
#define	IDE_BASEIO_1	(0x1f0)
#define	IDE_BASEIO_2	(0x170)
#define	IDE_BASEIO_3	(0x1e8)
#define	IDE_BASEIO_4	(0x168)

/*
 * register offsets
 */
#define	IDE_DATA	(0x00)
#define	IDE_ERROR	(0x01)
#define	IDE_SCOUNT	(0x02)	/* number of sectors */	
#define	IDE_SECTOR	(0x03)	/* starting sector */
#define	IDE_LCYL	(0x04)	/* cylinder low */
#define	IDE_HCYL	(0x05)	/* cylinder high */
#define	IDE_SELECT	(0x06)	/* 101dhhhh, d=drive, hhhh=head */
#define	IDE_STATUS	(0x07)
#define	IDE_FEATURE	IDE_ERROR
#define	IDE_COMMAND	IDE_STATUS

/*
 * unit value when doing select
 */
#define IDE_UNIT(unit)	(0xa0|((unit)<<4))

/*
 * control port offset
 */
#define	IDE_CONTROL	(0x206)

/*
 * stuff sent to control port
 */
#define	IDE_CTL_IMASK	(0x02)	/* mask interrupts */
#define	IDE_CTL_RESET	(0x04)	/* reset controller */
#define	IDE_CTL_4BIT	(0x08)	/* use 4 bits for head
				   enable device interrupts */

/*
 * status bits
 */
#define	IDE_STAT_ERROR	(0x01)	/* operation caused an error */
#define	IDE_STAT_INDEX	(0x02)	/* index pulse */
#define	IDE_STAT_ECC	(0x04)	/* ECC corrected data */
#define	IDE_STAT_DRQ	(0x08)	/* data request */
#define	IDE_STAT_SEEK	(0x10)	/* seek complete */
#define	IDE_STAT_WRERR	(0x20)	/* write fault */
#define	IDE_STAT_READY	(0x40)	/* drive is ready */
#define	IDE_STAT_BUSY	(0x80)	/* controller is busy */

/*
 * commands
 */
#define	IDE_CMD_RESTORE		(0x10)
#define	IDE_CMD_READ		(0x20)
#define IDE_CMD_WRITE		(0x30)
#define	IDE_CMD_VERIFY		(0x40)
#define	IDE_CMD_FORMAT		(0x50)
#define	IDE_CMD_INIT		(0x60)
#define	IDE_CMD_SEEK		(0x70)
#define	IDE_CMD_DIAG		(0x90)
#define IDE_CMD_SPECIFY		(0x91)
#define	IDE_CMD_IDLE1		(0xe3)
#define	IDE_CMD_IDLE2		(0x97)

/*
 * removable media
 */
#define	IDE_CMD_DLOCK		(0xde)	/* door lock */
#define	IDE_CMD_DUNLOCK		(0xdf)	/* door unlock */
#define	IDE_CMD_ACKMC		(0xdb)	/* acknowledge media change */

#define	IDE_CMD_MULTREAD	(0xc4)	/* multiple mode read */
#define	IDE_CMD_MULTWRITE	(0xc5)	/* multiple mode write */
#define IDE_CMD_SETMULTI	(0xc6)	/* set multi mode */
#define	IDE_CMD_IDENTIFY	(0xec)	/* get drive info */
#define	IDE_CMD_FEATURES	(0xef)	/* set features */
#define	IDE_CMD_DMAREAD		(0xc8)	/* DMA read */
#define	IDE_CMD_DMAWRITE	(0xca)	/* DMA write */

/*
 * ATAPI stuff
 */
#define IDE_CMD_PIDENTIFY	(0xa1)
#define	IDE_CMD_SOFTRESET	(0x08)
#define	IDE_CMD_PACKETCMD	(0xa0)

/*
 * error bits
 */
#define	IDE_ERR_MARK		(0x01)	/* data address mark not found */
#define	IDE_ERR_TRACK0		(0x02)	/* track 0 not found */
#define	IDE_ERR_ABORT		(0x04)	/* command aborted */
#define	IDE_ERR_ID		(0x10)	/* sector not found */
#define	IDE_ERR_MC		(0x20)	/* media changed */
#define	IDE_ERR_ECC		(0x40)	/* ECC can't be corrected */
#define	IDE_ERR_BAD		(0x80)	/* bad block detected */

/*
 * this is what IDE_CMD_IDENTIFY returs
 */
 
#define  SERIAL_SZ	20
#define  REV_SZ      	8
#define  MODEL_SZ	40
 
struct ide_identify_data {
	ushort  config;			/* some flags */
	ushort  cyls;			/* physical cylinders */
   	ushort  reserved3;		/* reserved word */
   	ushort  heads;			/* physical heads */
   	ushort  unf_track_bytes;	/* unformatted bytes per track */
   	ushort  unf_sector_bytes;	/* unformatted bytes per sector */
   	ushort  sectors;		/* physical sectors */
   	ushort  vendor0;
   	ushort  vendor1;
   	ushort  vendor2;
   	char   	serial[SERIAL_SZ];	/* serial number */
   	ushort  buf_type;		
   	ushort  buf_size;		/* 512 byte increments  */
   	ushort  necc;			/* ecc bytes */
   	char   	rev[REV_SZ];		/* firmware revision */
   	char   	model[MODEL_SZ];	/* model name */
   	uchar   max_multsect;		/* max multiple IO size */
   	uchar   vendor3;
   	ushort  dwordio;		/* 1 - implemented, 0 - unsupported */ 
   	uchar	vendor4;
	uchar	capability;		/* bits: 0-DMA, 1-LBA, 
					   2-IORDYsw, 3-IORDYsup */
	ushort	reserved50;
	uchar	vendor5;
	uchar	pio;
	uchar	vendor6;
	uchar	dma;
	ushort  field_valid;		/* bits: 0-cur_ok, 1-eide */
	ushort  cur_cyls;		/* logical cylinders */
	ushort  cur_heads;		/* logical heads */
	ushort	cur_sectors;		/* logical sectors per track */
	ushort	cur_capacity0;		/* total logical sectors on drive */
	ushort	cur_capacity1;		/* (2 words, misaligned ints) */
	uchar	multsect;		/* current multiple sector count */
	uchar	multsect_valid;		/* bit 0 set, mutlisect is valid */
	ulong	lba_capacity;		/* total number of sectors */
	ushort	dma_1word;		/* single word DMA info */
	ushort	dma_mword;		/* multiple word DMA info */
	ushort	eide_pio_modes;		/* bits 0-mode3, 1-mode4 */
	ushort	eide_dma_min;		/* min multword DMA cycle time (ns) */
	ushort	eide_dma_time;		/* recommended DMA cycle time */
	ushort	eide_pio;		/* min cycle time (ns) no IORDY */
	ushort  eide_pio_iordy;		/* min cycle time (ns) IORDY */
	ushort	reserved69;
	ushort	reserved70;
};

#define	DF_CONFIGURED	(0x80)
#define	DF_CDROM	(0x04)
#define DF_TAPE		(0x08)

struct ide_interface;

struct ide_disk {
	ulong 	flags;
	struct ide_interface* iface;
	ulong	unit;
	ulong	select;
	ulong	cyls;
	ulong	sectors;
	ulong	heads;
	
	ulong	cyl_sectors;
	ulong	multi;
	ulong	size;
};


struct ide_interface {
	ulong	io_base;
	ulong	irq;
	struct ide_disk	drives[2];
};

struct ide_disk* ide_get_device(ulong);


#endif /* __ide_h__ */

