/*
 * $Id: boot.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#define BOOT_MAX_NAME	(8)

struct file_desc {
	char		name[BOOT_MAX_NAME];
	unsigned long	start;
	unsigned long	size;
};

#define DOS_BOOT_MAGIC	 (0xDEEDFAAF)
#define MAX_FILES (32)
struct info_page {
	unsigned long	  magic;
	unsigned long	  file_count;
	struct file_desc  files[MAX_FILES];
};

