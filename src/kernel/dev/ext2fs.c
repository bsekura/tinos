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
 * $Id: ext2fs.c,v 1.5 1998/03/11 12:34:36 bart Exp $
 *
 */
 
#include "blk.h"
#include "buf.h"
#include "ext2fs.h"

#include <i386/page.h>

static int ext2fs_check_desc();

/*
 * our in-core filesystem super block
 * we cache some precalculated data here, mostly filled up
 * when mounting the filesystem
 */
struct ext2fs_meta {
	ulong	dev;			/* block device we're on */
	ulong	block_size;		/* ext2fs block size (bytes) */
	ulong	block_size_bits;	/* bits for quick calculations */
	ulong	blocks_per_page;	/* ext2fs blocks per vm page */
	ulong	groups_count;		/* number of group descriptors */
	ulong	group_blocks_count;	/* how many fs blocks they take */
	ulong	inodes_per_block;	/* how many fs blocks inodes take */
	ulong	itb_per_group;		/* inode tables per group */
	ulong	desc_per_block;		/* group descriptors per block */
	struct ext2_super_block* sb;	/* ext2fs superblock as read from
					   the device */
};

static ulong mounted;
static struct ext2fs_meta fs_meta;

static void
ext2fs_dump_group_desc(struct ext2_group_desc* gd)
{
	if(!gd)
		return;
		
	printf("block_bitmap: %d\n", gd->bg_block_bitmap);
	printf("inode_bitmap: %d\n", gd->bg_inode_bitmap);
	printf("inode_table: %d\n", gd->bg_inode_table);
	printf("free_blocks_count: %d\n", gd->bg_free_blocks_count);
	printf("free_inodes_count: %d\n", gd->bg_free_inodes_count);
	printf("used_dirs_count: %d\n", gd->bg_used_dirs_count);
}

static void
ext2fs_dump_inode(struct ext2_inode* i)
{
	int c;

	if(!i)
		return;

	printf("size: %d\n", i->i_size);
	printf("blocks count: %d\n", i->i_blocks);
	printf("blocks: ");
	for(c = 0; c < EXT2_NDIR_BLOCKS + 1; c++) 
		printf("%d ", i->i_block[c]);
	printf("\n");
}

/*
 * get a group descriptor given its number
 *
 * this can be improved by reading in all group descriptors
 * when mounting and keeping their table handy.
 */
static int
ext2fs_get_group_desc(ulong no, struct ext2_group_desc* gd)
{
	ulong block, index;
	struct ext2fs_meta* fs;
	struct buffer_head* bh;

	fs = &fs_meta;
	block = no / fs->desc_per_block;
	index = no % fs->desc_per_block;

#ifdef _DEBUG	
	printf("ext2fs_get_group_desc(): block %d, index %d\n", 
		block, index);		
#endif	

	block += (fs->sb->s_first_data_block + 1);
	bh = bread(fs->dev, block);
	if(!bh) {
		return 0;
	}
	
	bcopy((((struct ext2_group_desc*) bh->data) + index), gd, 
		sizeof(struct ext2_group_desc));
	brelse(bh);
	
	return 1;
}

/*
 * get an inode given its number
 *
 * this should be improved soon by caching inodes in a hashed table
 */
static int
ext2fs_get_inode(ulong ino, struct ext2_inode* inode)
{
	ulong group, block, i, block_nr;
	struct ext2fs_meta* fs;
	struct ext2_super_block* sb;
	struct ext2_group_desc gd;
	struct buffer_head* bh;

	fs = &fs_meta;
	sb = fs->sb;
	group = (ino - 1) / EXT2_INODES_PER_GROUP(sb);
	block = (ino - 1) % EXT2_INODES_PER_GROUP(sb) /
		fs->inodes_per_block;
	i = ((ino - 1) % EXT2_INODES_PER_GROUP(sb)) %  
		fs->inodes_per_block;

#ifdef _DEBUG
	printf("ext2fs_get_inode(): group = %d, block = %d, i = %d\n",
		group, block, i);
#endif
		
	if(!ext2fs_get_group_desc(group, &gd)) {
		printf("ext2fs_get_inode(): group desc null\n");
		return 0;
	}
	
	block_nr = gd.bg_inode_table + block;
	
#ifdef _DEBUG	
	printf("ext2fs_get_inode(): block_nr = %d\n", block_nr);
#endif
	
	bh = bread(fs->dev, block_nr);
	if(!bh)
		return 0;

	bcopy((((struct ext2_inode*) bh->data) + i), inode,
		sizeof(struct ext2_inode));
	brelse(bh);
	
	return 1;
}

/*
 * mount an ext2fs filesystem
 * read in a super block, sanity check some parameters 
 * check group descriptors and setup buffers
 */
int
ext2fs_mount(ulong dev_no)
{
	struct ext2fs_meta* fs;
	struct ext2_super_block* sb;
	ulong sb_page;
	
	fs = &fs_meta;
	if(mounted) {
		printf("ext2 filesystem already mounted on device %d\n",
			fs->dev);
		return 0;
	}

	sb_page = alloc_page();
	if(!sb_page)
		return 0;
		
	fs->dev = dev_no;
	devblk_set_blksize(fs->dev, EXT2_MIN_BLOCK_SIZE);
	devblk_read(fs->dev, 1, 1, (void*) sb_page);
	sb = (struct ext2_super_block*) sb_page;
	if(sb->s_magic != EXT2_SUPER_MAGIC) {
		printf("ext2fs_mount(): magic not found\n");
		free_page(sb_page);
		fs->sb = (struct ext2_super_block*) 0;
		mounted = 0;
		return 0;
	}
	fs->sb = sb;
	
	printf("first data block = %d\n", sb->s_first_data_block);
	fs->block_size_bits = EXT2_BLOCK_SIZE_BITS(sb);
	//fs->block_size = 1 << (sb->s_log_block_size + 10);
	fs->block_size = 1 << fs->block_size_bits;
	fs->blocks_per_page = PAGESZ / fs->block_size;
	
	printf("block_size = %d\n", fs->block_size);
	
	devblk_set_blksize(fs->dev, fs->block_size);
	
	fs->inodes_per_block = fs->block_size / EXT2_INODE_SIZE(sb);
	fs->itb_per_group = sb->s_inodes_per_group /
			    fs->inodes_per_block;
	fs->desc_per_block = fs->block_size / sizeof(struct ext2_group_desc);			   
	
	fs->groups_count = (sb->s_blocks_count - 
			    sb->s_first_data_block + 
			    EXT2_BLOCKS_PER_GROUP(sb) - 1) / 
			    EXT2_BLOCKS_PER_GROUP(sb);
	fs->group_blocks_count = (fs->groups_count + 
			          EXT2_DESC_PER_BLOCK(sb) - 1) / 
			          EXT2_DESC_PER_BLOCK(sb);

	buf_init(fs->block_size);
	if(ext2fs_check_desc()) {
		mounted = 1;			          
		printf("ext2fs filesystem mounted readonly\n");	
		
		return 1;
	}

	printf("ext2fs_mount(): group descriptors corrupted\n");
	ext2fs_umount();
		
	return 0;
}	

/*
 * check group descriptors
 * read in every group descriptor and see whether its
 * values don't seem to be insane
 *
 * a positive side effect of this routine is that all group
 * descriptors are read into the buffers
 *
 */
static int
ext2fs_check_desc()
{
	int c;
	struct ext2fs_meta* fs;
	struct ext2_super_block* sb;
	ulong block;
	
	fs = &fs_meta;
	sb = fs->sb;
	block = sb->s_first_data_block;

	printf("ext2fs_check_desc(): %d group descriptors\n",
		fs->groups_count);
			
	for(c = 0; c < fs->groups_count; c++) {
		struct ext2_group_desc g;
		struct ext2_group_desc* gd;
		
		if(ext2fs_get_group_desc(c, &g) == 0) {
			printf("ext2fs_get_group_desc(): null\n");
			return 0;
		}
		
		gd = &g;
		//ext2fs_dump_group_desc(gd);
		
		if(gd->bg_block_bitmap < block ||
		   gd->bg_block_bitmap >= block + EXT2_BLOCKS_PER_GROUP(sb)) {
		   	printf("block bitmap not in group\n");
		   	return 0;
		}
		if(gd->bg_inode_bitmap < block ||
		   gd->bg_inode_bitmap >= block + EXT2_BLOCKS_PER_GROUP(sb)) {
		  	printf("inode bitmap not in group\n");
		  	return 0;
		}
		if(gd->bg_inode_table < block ||
		   gd->bg_inode_table + fs->itb_per_group >=
		   block + EXT2_BLOCKS_PER_GROUP(sb)) {
		   	printf("inode table not in group\n");
		   	return 0;
		}		
		
		block += EXT2_BLOCKS_PER_GROUP(sb);
	}
	
	return 1;
}

/*
 * unmount a filesystem
 * release buffers and any other memory
 */
void
ext2fs_umount()
{
	printf("unmounting ext2 filesystem on device %d\n", fs_meta.dev);
	buf_destroy();
	if(fs_meta.sb)
		free_page((ulong) fs_meta.sb);
}


/*
 * given index, get corresponding file data block number;
 * some major caching is possible here, by e.g. keeping
 * a table of block numbers and reading ahead to fill it.
 * files are usually accessed sequentially.
 *
 */
static ulong
ext2fs_get_block(struct ext2_inode* inode, ulong index)
{
	static ulong current_block = -1;
	ulong ind_blocks_max, dind_blocks_max, block;
	struct ext2fs_meta* fs;
	struct buffer_head* bh;

	/*
	 * sanity check
	 */	
	if(!inode || (index >= inode->i_blocks))
		return 0;

	/*
	 * direct blocks
	 */		
	if(index < EXT2_NDIR_BLOCKS) {
		return inode->i_block[index];
	}
	
	fs = &fs_meta;	
	ind_blocks_max = EXT2_NDIR_BLOCKS + (fs->block_size>>2);
	//printf("ind_blocks_max = %d\n", ind_blocks_max);

	/*
	 * indirect blocks
	 */	
	if(index < ind_blocks_max) {
		if((block = inode->i_block[EXT2_IND_BLOCK]) == 0) {
			return 0;
		}
		
		bh = bread(fs->dev, block);
		if(!bh) {
			printf("ext2fs_get_block(): bread() failed\n");
			return 0;
		}
		
		block = *(((ulong*) bh->data) + (index - EXT2_NDIR_BLOCKS));
		brelse(bh);
		
		return (block);
	}
	
	dind_blocks_max = ind_blocks_max + 
		((fs->block_size>>2)*(fs->block_size>>2));		
	//printf("dind_blocks_max = %d\n", dind_blocks_max);
	
	/*
	 * double indirect blocks
	 */
	if(index < dind_blocks_max) {	
		ulong i;
	
		//printf("ext2fs_get_block(): double indirect\n");		
		if((block = inode->i_block[EXT2_DIND_BLOCK]) == 0) {
			return 0;
		}
		
		bh = bread(fs->dev, block);
		if(!bh) {
			printf("ext2fs_get_block(): bread() failed\n");
			return 0;
		}

		/*
		 * get an index within first level block of entries
		 */	
		i = (index - ind_blocks_max) / (fs->block_size>>2);
		block = *(((ulong*) bh->data) + i);
		brelse(bh);

		//printf("ext2fs_get_block(): reading block %d\n", block);		
		bh = bread(fs->dev, block);
		if(!bh) {
			printf("ext2fs_get_block(): bread() failed\n");
			return 0;
		}

		/*
		 * get an index within second level block of entries
		 */
		i = (index - ind_blocks_max) & ((fs->block_size>>2)-1);
		block = *(((ulong*) bh->data) + i);
		brelse(bh);
		
		//printf("ext2fs_get_block(): i = %d, block %d\n", i, block);		
		return (block);

	} else {
		/*
		 * triple indirect blocks
		 */
		 
		ulong i;
	
		//printf("ext2fs_get_block(): triple indirect\n");		
		if((block = inode->i_block[EXT2_TIND_BLOCK]) == 0) {
			return 0;
		}
		
		bh = bread(fs->dev, block);
		if(!bh) {
			printf("ext2fs_get_block(): bread() failed\n");
			return 0;
		}

		/*
		 * get an index within first level block of entries
		 */
		i = (index - dind_blocks_max) / 
			((fs->block_size>>2)*(fs->block_size>>2));
		//printf("ext2fs_get_block(): index = %d, i = %d\n", index, i);
		block = *(((ulong*) bh->data) + i);
		brelse(bh);

		//printf("ext2fs_get_block(): reading block %d\n", block);		
		bh = bread(fs->dev, block);
		if(!bh) {
			printf("ext2fs_get_block(): bread() failed\n");
			return 0;
		}

		/*
		 * get an index within second level block of entries
		 */
		i = (index - dind_blocks_max) / (fs->block_size>>2);
		block = *(((ulong*) bh->data) + i);
		brelse(bh);
		
		bh = bread(fs->dev, block);
		if(!bh) {
			printf("ext2fs_get_block(): bread() failed\n");
			return 0;
		}
		
		/*
		 * get an index within third level block of entries
		 */
		//printf("ext2fs_get_block(): i = %d, block %d\n", i, block);		
		i = (index - dind_blocks_max) & ((fs->block_size>>2)-1);
		block = *(((ulong*) bh->data) + i);
		brelse(bh);
		
		//printf("ext2fs_get_block(): i = %d, block %d\n", i, block);		
		return (block);

	}
	
	return 0;
}

/*
 * read a page given an inode number
 *
 * this read goes on a fs block boundaries
 * for generic file reading, see the next routine
 */
int
ext2fs_readpage(ulong ino, ulong page, void* buf)
{
	struct ext2_inode inode;
	struct ext2fs_meta* fs;
	ulong bcnt, blk, c;

	if(!ext2fs_get_inode(ino, &inode)) {
		printf("ext2fs_readpage(): couldn't get root inode\n");
		return 0;
	}
	
	fs = &fs_meta;
	bcnt = fs->blocks_per_page;
	c = page * fs->blocks_per_page;
	
	printf("ext2fs_readpage(): start blk = %d\n", c);
	printf("ext2fs_readpage(): blocks: ");
	while((blk = ext2fs_get_block(&inode, c))) {
		printf("%d ", blk);
	
		c++;
		bcnt--;
		if(bcnt)
			continue;
			
		break;
	}
	
	printf("\n");	
	if(bcnt) {
		printf("ext2fs_readpage(): zeroing %d %d\n",
			((fs->blocks_per_page - bcnt) * fs->block_size), 
		      	bcnt * fs->block_size);
		/*
		bzero(buf + ((fs->blocks_per_page - bcnt) * fs->block_size), 
		      bcnt * fs->block_size);
		*/
	}
	
	return 1;
}

/*
 * generic read routine
 * read count bytes from a file (inode number)
 * starting at a given offset
 */
int
ext2fs_read(ulong ino, ulong pos, void* buf, ulong count)
{
	struct ext2_inode inode;
	struct ext2fs_meta* fs;
	ulong block, bcnt, res, blk;
	int cb;
	struct buffer_head* bh;
	void* p;

	/*
	 * get an inode
	 */
	if(!ext2fs_get_inode(ino, &inode)) {
		printf("ext2fs_read(): couldn't get root inode\n");
		return -1;
	}
	
	fs = &fs_meta;	
	p = buf;
	
	/*
	 * adjust byte count if it'll go beyond file size
	 */
	cb = (pos + count > inode.i_size) ? (inode.i_size - pos) : count;
	
#if _DEBUG	
	printf("ext2fs_read(): pos %d, count %d, size %d, cb %d\n", 
		pos, count, inode.i_size, cb);
#endif		
	if(cb <= 0)
		return (0);
	
	/*
	 * calc starting block and offset
	 */	
	//block = pos / fs->block_size;
	block = pos >> fs->block_size_bits;
	res = pos & (fs->block_size - 1);
	if(res) {
		ulong left;
	
		left = fs->block_size - res;
		left = left > cb ? cb : left;

#if _DEBUG		
		printf("ext2fs_read(): (@) res = %d\n", res);		
		printf("ext2fs_read(): reading block %d\n", block);
#endif	
		
		if(!(blk = ext2fs_get_block(&inode, block))) {
			return 0;
		}
		
		bh = bread(fs->dev, blk);
		if(!bh) {
			printf("ext2fs_read(): bread() failed\n");
			return -1;
		}
		bcopy(bh->data + res, buf, left);
		brelse(bh);
		
		buf += left;
		cb -= left;
		if(cb <= 0)
			return (buf - p);
			
		block++;
	}
	
	//bcnt = cb / fs->block_size;
	bcnt = cb >> fs->block_size_bits;
	res = cb & (fs->block_size - 1);

#if _DEBUG	
	printf("ext2fs_read(): bcnt = %d, res = %d\n", bcnt, res);
#endif
	
	while(bcnt--) {		
		if(!(blk = ext2fs_get_block(&inode, block++))) {
			printf("ext2fs_read(): no block (while(bcnt--))\n");
			return -1;
		}

		bh = bread(fs->dev, blk);
		if(!bh) {
			printf("ext2fs_read(): bread() failed\n");
			return -1;
		}
		bcopy(bh->data, buf, fs->block_size);
		brelse(bh);
		
		buf += fs->block_size;
		cb -= fs->block_size;
	}
	
	if(res) {
		if(!(blk = ext2fs_get_block(&inode, block))) {
			printf("ext2fs_read(): no block\n");
			return 0;
		}

		bh = bread(fs->dev, blk);
		if(!bh) {
			printf("ext2fs_read(): bread() failed\n");
			return -1;
		}
		bcopy(bh->data, buf, res);
		brelse(bh);
		
		cb -= res;
		buf += res;
	}
	
	return (buf - p);	
}


#ifdef _DEBUG

static void
ext2fs_dump_root()
{
	struct ext2_inode root;

	if(!ext2fs_get_inode(EXT2_ROOT_INO, &root)) {
		printf("ext2fs_dump_root(): couldn't get root inode\n");
		return;
	}
	
	ext2fs_dump_inode(&root);
}

static void
ext2fs_file_blocks(ulong ino)
{
	struct ext2_inode i;
	ulong c, blk;
	
	if(!ext2fs_get_inode(ino, &i)) {
		return;
	}
	
	ext2fs_dump_inode(&i);
	
	printf("file blocks: ");
	c = 0;
	while((blk = ext2fs_get_block(&i, c++)))
		printf("%d ", blk);
	printf("\n");
}

#endif

/*
 * for testing purposes this is our 'ls' routine
 * that dumps directory contents given an inode number.
 *
 * ultimately, it'll go away from this module.
 *
 */
  
struct file_desc {
	ulong	s_len;
	ulong	size;
	ulong	inode;
	char*	name;
};

#define MAX_LS_PAGES	(32)
static ulong ls_pages[MAX_LS_PAGES];
static ulong ls_page_count[MAX_LS_PAGES];

void
ext2fs_ls(ulong ino, int detail)
{	
	struct ext2_inode i;
	struct ext2fs_meta* fs;
	ulong c, x, count, nr_pages, fsz, blk;
	struct file_desc* f;
	ulong* page, *page_count;
	
	fs = &fs_meta;
	if(!mounted)
		return;
		
	if(!ext2fs_get_inode(ino, &i)) {
		printf("ext2fs_ls(): couldn't get inode\n");
		return;
	}
	
	//ext2fs_dump_inode(i);
	c = 0;
	count = 0;
	
	page_count = &ls_page_count[0];
	page = &ls_pages[0];
	if(!*page) {
		if(!(*page = alloc_page())) 
			return;
	}
	nr_pages = 1;
	f = (struct file_desc*) *page;
	fsz = 0;
		
	while((blk = ext2fs_get_block(&i, c++))) {
		ulong ptr;
		struct buffer_head* bh;

		bh = bread(fs->dev, blk);
		if(!bh)
			break;
		
		ptr = (ulong) bh->data;	
		for(;;) {
			ulong len;
			//struct ext2_inode dino;
			struct ext2_dir_entry* dir;
		
			dir = (struct ext2_dir_entry*) ptr;
			if(dir->inode == 0)
				break;

			len = sizeof(struct file_desc) + dir->name_len + 1;
			fsz += len;
			
			if(fsz >= PAGESZ) {
				//printf("allocating a new page\n");
			
				nr_pages++;
				if(nr_pages >= MAX_LS_PAGES) {
					printf("MAX_LS_PAGES\n");
					break;
				}
				
				//page = &ls_pages[nr_pages];
				page++;
				page_count++;
				if(!*page) {
					if(!(*page = alloc_page())) {
						return;
					}
				}
				
				f = (struct file_desc*) *page;
				fsz = len;
			}			

			f->name = (char*) (((ulong) f) 
						+ sizeof(struct file_desc));		
						
			strncpy(f->name, dir->name, dir->name_len);
			f->name[dir->name_len] = 0;
			f->inode = dir->inode;
			f->size = 0;
			f->s_len = len;
			
			//printf("file: %s\n", f->name);
			
			count++;
			(*page_count)++;
			
			f = (struct file_desc*) (*page + fsz);
			
			ptr += dir->rec_len;
			if((ptr - (ulong) bh->data - fs->block_size) < 8)
				break;
		}
		brelse(bh);
	}

	if(detail) {	
		page = &ls_pages[0];
		page_count = &ls_page_count[0];
		for(c = 0; c < nr_pages; c++) {
			f = (struct file_desc*) *page++;
			for(x = 0; x < *page_count; x++) {								
				struct ext2_inode dino;
					
				if(f->inode > 0 && 
				   ext2fs_get_inode(f->inode, &dino)) {
					f->size = dino.i_size;
				} else
					f->size = -1;
			
				f = (struct file_desc*) 
					(((ulong) f) + f->s_len);
			}
			page_count++;
		}					
		
		page = &ls_pages[0];
		page_count = &ls_page_count[0];
		for(c = 0; c < nr_pages; c++) {
			f = (struct file_desc*) *page++;
			for(x = 0; x < *page_count; x++) {
				printf("%-8d %-8d %s\n", 
					f->inode, f->size, f->name);		
				f = (struct file_desc*) 
					(((ulong) f) + f->s_len);
			}
			page_count++;
		}					
		
	} else {
		page = &ls_pages[0];
		page_count = &ls_page_count[0];
		for(c = 0; c < nr_pages; c++) {
			f = (struct file_desc*) *page++;
			for(x = 0; x < *page_count; x++) {
				printf("%-8d %s\n", f->inode, f->name);
				f = (struct file_desc*) 
					(((ulong) f) + f->s_len);					
			}
			page_count++;
		}					
	}

	bzero((void*) &ls_page_count[0], sizeof(ulong) * MAX_LS_PAGES);		
}
