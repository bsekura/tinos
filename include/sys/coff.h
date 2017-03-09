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
 * $Id: coff.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */
/* 
 * these are needed to load coff executable 
 * Copyright (C) 1995 DJ Delorie
 *
 */
#ifndef _COFF_H_
#define _COFF_H_

/*** coff information for Intel 386/486.  */

/********************** FILE HEADER **********************/

struct external_filehdr {
	unsigned short f_magic;		/* magic number			*/
	unsigned short f_nscns;		/* number of sections		*/
	unsigned long f_timdat;	/* time & date stamp		*/
	unsigned long f_symptr;	/* file pointer to symtab	*/
	unsigned long f_nsyms;		/* number of symtab entries	*/
	unsigned short f_opthdr;	/* sizeof(optional hdr)		*/
	unsigned short f_flags;		/* flags			*/
};


/* Bits for f_flags:
 *	F_RELFLG	relocation info stripped from file
 *	F_EXEC		file is executable (no unresolved external references)
 *	F_LNNO		line numbers stripped from file
 *	F_LSYMS		local symbols stripped from file
 *	F_AR32WR	file has byte ordering of an AR32WR machine (e.g. vax)
 */

#define F_RELFLG	(0x0001)
#define F_EXEC		(0x0002)
#define F_LNNO		(0x0004)
#define F_LSYMS		(0x0008)



#define	I386MAGIC	0x14c
#define I386AIXMAGIC	0x175
#define I386BADMAG(x) (((x).f_magic!=I386MAGIC) && (x).f_magic!=I386AIXMAGIC)


#define	FILHDR	struct external_filehdr
#define	FILHSZ	sizeof(FILHDR)


/********************** AOUT "OPTIONAL HEADER" **********************/


typedef struct 
{
  unsigned short 	magic;		/* type of file				*/
  unsigned short	vstamp;		/* version stamp			*/
  unsigned long	tsize;		/* text size in bytes, padded to FW bdry*/
  unsigned long	dsize;		/* initialized data "  "		*/
  unsigned long	bsize;		/* uninitialized data "   "		*/
  unsigned long	entry;		/* entry pt.				*/
  unsigned long 	text_start;	/* base of text used for this file */
  unsigned long 	data_start;	/* base of data used for this file */
}
AOUTHDR;


typedef struct gnu_aout {
	unsigned long info;
	unsigned long tsize;
	unsigned long dsize;
	unsigned long bsize;
	unsigned long symsize;
	unsigned long entry;
	unsigned long txrel;
	unsigned long dtrel;
	} GNU_AOUT;

#define AOUTSZ (sizeof(AOUTHDR))

#define OMAGIC          0404    /* object files, eg as output */
#define ZMAGIC          0413    /* demand load format, eg normal ld output */
#define STMAGIC		0401	/* target shlib */
#define SHMAGIC		0443	/* host   shlib */


/********************** SECTION HEADER **********************/


struct external_scnhdr {
	char		s_name[8];	/* section name			*/
	unsigned long		s_paddr;	/* physical address, aliased s_nlib */
	unsigned long		s_vaddr;	/* virtual address		*/
	unsigned long		s_size;		/* section size			*/
	unsigned long		s_scnptr;	/* file ptr to raw data for section */
	unsigned long		s_relptr;	/* file ptr to relocation	*/
	unsigned long		s_lnnoptr;	/* file ptr to line numbers	*/
	unsigned short		s_nreloc;	/* number of relocation entries	*/
	unsigned short		s_nlnno;	/* number of line number entries*/
	unsigned long		s_flags;	/* flags			*/
};

#define	SCNHDR	struct external_scnhdr
#define	SCNHSZ	sizeof(SCNHDR)

/*
 * names of "special" sections
 */
#define _TEXT	".text"
#define _DATA	".data"
#define _BSS	".bss"
#define _COMMENT ".comment"
#define _LIB ".lib"

/*
 * s_flags "type"
 */
#define STYP_TEXT	 (0x0020)	/* section contains text only */
#define STYP_DATA	 (0x0040)	/* section contains data only */
#define STYP_BSS	 (0x0080)	/* section contains bss only */

/* For new sections we havn't heard of before */
#define DEFAULT_SECTION_ALIGNMENT 4

#endif /* _COFF_H_ */
