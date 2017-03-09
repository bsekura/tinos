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
 * system console
 *
 * $Id: syscon.c,v 1.8 1998/03/11 12:34:59 bart Exp $
 *
 */

#include <sys/types.h>
#include <string.h>
#include <pc/cons.h>
#include <thread.h>
#include <pool.h>
#include <msg.h>

#include <i386/page.h>
#include <i386/inlines.h>

#define ARG1(x) 	(!strcmp(argv[1], x))

#define BACKSPACE	(8)

#define MAX_ARGS	(10)
#define MAX_ARGSZ	(50)

int   argc = 0;
char  argv[MAX_ARGS][MAX_ARGSZ];

struct cmd_tbl {
   char name[20];
   voidfun func;
};

/* forward */
static void help();
static void pgalloc();
static void pgfree();
static void iitova();
static void vatoii();
static void refmem();
static void tdel();
static void tready();
static void tready_del();
static void tready_dump();
static void create_sample();
static void prio();
static void wait();
static void run();

static void server();
static void client();

static void bzero_speed();

static void memlock_test();
static void stupid_test();
static void hat();
static void timeout();

static void pool_init();
static void pool_alloc();
static void pool_free();
static void pool_dump();

static void vas();
static void cache();

static void region_test();
static void ide_test();
static void ide_read();

static void mount();
static void cd();
static void ls();
static void rpg();
static void read();
static void get();
static void exec();

/* external commands */
extern void reboot_machine();
extern void show_time();
extern void pm_dump();
extern void sched_stat();
extern void sched_uptime();
extern void hat_test();
extern void fd_init();
extern void dump_timeout();
extern void dump_pool();
extern void trap();

extern void ide_init();
extern void devblk_init();

/*
 * command table
 */
struct cmd_tbl cmd[] = {
	{ "reboot", reboot_machine },
	{ "time", show_time },
	{ "pgalloc", pgalloc },
	{ "pgfree", pgfree },
	{ "pmdump", pm_dump },
	{ "iitova", iitova },
	{ "vatoii", vatoii },
	{ "refmem", refmem },
	{ "tdel", tdel },
	{ "tready", tready },
	{ "tready_dump", tready_dump },
	{ "ts", tready_dump },
	{ "tready_del", tready_del },
	{ "sample", create_sample },
	{ "stat", sched_stat },
	{ "w", sched_uptime },
	{ "prio", prio },
	{ "wait", wait },
	{ "run", run },
	{ "server", server },
	{ "client", client },
	{ "speed", bzero_speed },
	{ "memlock", memlock_test },
	{ "stupid", stupid_test },
	{ "hat", hat },
	{ "hat_test", hat_test },
	{ "fd", fd_init },
	{ "timeout", timeout },
	{ "dumptm", dump_timeout },
	{ "pinit", pool_init },
	{ "palloc", pool_alloc },
	{ "pfree", pool_free },
	{ "pd", pool_dump },
	{ "trap", trap },
	{ "vas", vas },
	{ "cache", cache },
	{ "rg", region_test },
	{ "ide", ide_init },
	{ "ideio", ide_test },
	{ "ider", ide_read },
	{ "dev", devblk_init },
	{ "mount", mount },
	{ "cd", cd },
	{ "ls", ls },
	{ "rpg", rpg },
	{ "read", read },
	{ "get", get },
	{ "exec", exec },
	{ "?", help },
	{ "help", help }
};

#define NCMD (sizeof(cmd)/sizeof(struct cmd_tbl))

static void
help()
{
	int c;
	printf("Available commands:\n");
	for(c = 0; c < NCMD; c++) {
		printf("%s, ", cmd[c].name);
	}
	printf("\n");
}

static void
pgalloc()
{
	int ntimes = 1;

	if(argc > 1) {
		ntimes = atoi(argv[1]);
	}

	while(ntimes--) {
		ulong addr = alloc_page();
		printf("alloc_page(): %08x, index: %d\n",
			addr, addr >> PGSHIFT);
	}
}

static void
pgfree()
{
	if(argc <= 1) {
		printf("specify page frame address\n");
		return;
	}

	/*free_page(atoi(argv[1]) << PGSHIFT);		*/
	free_page(strtoul(argv[1], 0, 16));
}

static void
iitova()
{
	if(argc <= 2) {
		printf("enter pdir and pgtable index\n");
		return;
	}

	printf("va = %08x\n",
		atoi(argv[1])*0x40000000 + atoi(argv[2])*PAGESZ);
}

static void
vatoii()
{
	ulong va;

	if(argc <= 1) {
		printf("specify virtual address.\n");
		return;
	}
	va = atoi(argv[1]);
	printf("IPD = %d, IPT = %d\n", va2ipd(va), va2ipt(va));
}

static void
refmem()
{
	char ch;
	ulong* va;
	int write;

	if(argc <= 1) {
		printf("usage: refmem pgno [nbytes].\n");
		return;
	}
	if(argc >= 3 && (argv[2][0] == 'w' || argv[2][0] == 'W')) {
		write = 1;
	} else {
		write = 0;
	}

	va = (ulong*)strtoul(argv[1], 0, 16);
	printf("address: %08x\n", (ulong)va);
	printf("continue {%s} ? (y/n): ", write ? "write" : "read");
	while(1) {
		ch = kb_getch();
		if(ch == 'y' || ch == 'Y') {
			if(write) {
				printf("writing...");
				*va = 0xDEADBEEF;
				printf("ok\n");
			} else {
				ulong value = *va;
				printf("read dword: %08x\n", value);
			}
			break;
		}
		if(ch != 0)
			break;
	}
	printf("\n");
}

static void
tdel()
{
	ulong addr;

	if(argc <= 1) {
		printf("specify hex address of tcb.\n");
		return;
	}

	addr = strtoul(argv[1], 0, 16);
	printf("addr: %08x\n", addr);

	delete_thread(addr);
}

static void
tready()
{
	ulong addr;

	if(argc <= 1) {
		printf("specify hex address of tcb.\n");
		return;
	}

	addr = strtoul(argv[1], 0, 16);
	printf("addr: %08x\n", addr);

	thread_set_ready(addr);
}

static void
tready_del()
{
	ulong addr;

	if(argc <= 1) {
		printf("specify hex address of tcb.\n");
		return;
	}

	addr = strtoul(argv[1], 0, 16);
	printf("addr: %08x\n", addr);

	thread_remove_ready(addr);
}

static void
tready_dump()
{
	dump_ready_threads();
}

static int counter = 60;

void
sample_thread()
{
	ushort* p = (ushort*)0xb8000;
	int c = counter++;

	printf("sample_thread...(%d)\n", c);
	while(1) {
		ushort v;
		uint i = c * 100;
		while(i--);

		v = p[c];
		v++;
		p[c] = v;
	}
}


static void
create_sample()
{
	struct tcb* t;

	t = create_thread(0, (ulong)&sample_thread);
	printf("thread created (%08x)...\n", (ulong)t);
}

static void
prio()
{
	struct tcb* t;

	if(argc <= 2) {
		printf("specify hex addr of tcb and prio\n");
		return;
	}

	t = (struct tcb*) strtoul(argv[1], 0, 16);
	t->prio = t->cnt = atoi(argv[2]);
}

static void
wait()
{
	struct tcb* t;

	if(argc <= 1) {
		printf("specify hex addr of tcb\n");
		return;
	}

	t = (struct tcb*)strtoul(argv[1], 0, 16);
	thread_sleep(t);
}

static void
run()
{
	struct tcb* t;

	if(argc <= 1) {
		printf("specify hex addr of tcb\n");
		return;
	}

	t = (struct tcb*)strtoul(argv[1], 0, 16);
	thread_wakeup(t);
}


static port_t* port;

void
server_thread()
{
	int msg;
	ulong count;

	if(!port) {
		port = alloc_port();
		if(!port) {
			printf("server_thread(): oops, alloc_port() failed\n");
			while(1);
		}
	}

	printf("server_thread(): port = %08x\n", (ulong) port);
	printf("server_thread(): entering loop...\n");
	count = 0;
	while(1) {
		char buf[32];
		msg_desc_t msg;
		
		msg.data = buf;
		msg.len = 31;
		m_receive(port, &msg);
		//printf("server_thread(): data = %s\n", msg.data);
		//printf("server_thread(): msg = %d\n", msg.id);
		count++;
		if(msg.id == -1) {
			count--; /* don't count '-1' message */
			printf("server_thread(): got message %d times\n",
				count);
			count = 0;
		}
	}
}

static void
server()
{
	struct tcb* t;

	printf("creating server thread ...\n");

	t = create_thread(0, (ulong) &server_thread);
	if(!t) {
		printf("create_thread() failed \n");
		return;
	}
}

static int n_times;

static void
client_thread()
{
	//message_t* msg;
	msg_desc_t msg;
	char buf[32];
	int n;

	n = n_times;
	//msg = alloc_msg();
#if 0
	if(!msg) {
		 printf("client_thread(): cannot alloc msg\n");
		 while(1);
	}
#endif

	printf("sending message %d times \n", n);
	//msg->id = 0;
	strcpy(buf, "dupka");
	msg.id = 0;
	msg.data = buf;
	msg.len = 32;
	timer_start();
	while(n--) {
		m_send(port, &msg);
		//msg = alloc_msg();
		//msg->id = n;
		msg.id = n;
		//printf("ok, returned...%d\n", ret);
	}
	timer_stop();
	//msg = alloc_msg();
	//msg->id = -1;
	msg.id = -1;
	m_send(port, &msg);
	
	//dump_port(port);

	while(1);
}

static void
client()
{
	//extern void time_snapshot();
	struct tcb* t;
	int n_msg = 1, n_thread = 1;

	if(argc < 3) {
		printf("client <n_thread> <n_msg>\n");
		return;
	}

	if(!port) {
		port = alloc_port();
		if(!port) {
			printf("client(): oops, alloc_port() failed\n");
			while(1);
		}
	}

	n_thread = atoi(argv[1]);
	n_msg = atoi(argv[2]);

	printf("creating %d client threads with %d messages...\n",
		n_thread, n_msg);

	n_times = n_msg;
	while(n_thread--) {
		t = create_thread(0, (ulong) &client_thread);
		if(!t) {
			printf("create_thread() failed \n");
			break;
		}
	}

/*
	timer_start();
	while(n--) {
		m_send(port, 0);
		//printf("ok, returned...%d\n", ret);
	}
	timer_stop();
	m_send(port, 99);
*/
}

static void
bzero_speed()
{
	char page[2048];
	long c;
	long a, b;

	//c = 10000;
	c = 1000000000;

	b = 102;
	timer_start();
	/*
	while(c--) {
		bzero(page, 2048);
	}
	*/
	while(c--) {
		a = b % 50;
	}

	timer_stop();
}

static void
memlock_thread()
{
	int c;

	printf("memlock_thread() starting ...\n");
	while(1) {
		ulong p = (ulong) alloc_page();
		if(p == 0) {
			printf("memlock_thread: alloc_page() gave me null\n");
			while(1);
		}

		free_page(p);

		c = 10000;
		while(c--);
	}
}

static void
memlock_test()
{
	struct tcb* t;

	printf("creating test_server thread ...\n");
	t = create_thread(0, (ulong) &memlock_thread);
	if(!t) {
		printf("create_thread() failed \n");
		return;
	}
	printf("thread created [%08x]\n", (ulong) t);
}


static void
stupid_thread()
{
	int c;

	while(1) {
		c = 1000000;
		while(c--);
		fd_init();
	}
}

static void
stupid_test()
{
	struct tcb* t;

	t = create_thread(0, (ulong) &stupid_thread);
	if(!t) {
		printf("create_thread() failed \n");
		return;
	}
	printf("thread created [%08x]\n", (ulong) t);
}

static void
hat()
{
	int va;
	ulong page, *p;

	if(argc > 1) {
		va = strtoul(argv[1], 0, 16);
	} else {
		printf("specify virtual address\n");
		return;
	}

	page = alloc_page();
	printf("allocated = %08x\n", page);
	hat_kern_insert(va, page, PTE_READONLY);
	printf("page inserted...\n");
	p = (ulong*) va;
	*p = 0xDEADBEEF;
}

static void
timeout()
{
	ulong ticks;
	if(argc < 2) {
		printf("specify ticks\n");
		return;
	}
	ticks = atoi(argv[1]);
	printf("ok, setting timeout...\n");
	set_timeout(ticks);
	printf("ok, timeout expired...\n");
}

static master_pool_t pool;

static void
pool_init()
{
	int size;
	if(argc < 2) {
		printf("specify size\n");
		return;
	}
	size = atoi(argv[1]);
	init_pool(&pool, size);
}

static void
pool_alloc()
{
	ulong addr;

	addr = (ulong) alloc_pool(&pool);
	printf("addr = %08x\n", addr);
}

static void
pool_free()
{
	ulong addr;
	if(argc < 2) {
		printf("specify address\n");
		return;
	}
	addr = strtoul(argv[1], 0, 16);
	free_pool((void*) addr);
}

static void
pool_dump()
{
	dump_pool(&pool);
}

static void
vas()
{
	if(argc < 2) {
		printf("missing args\n");
		return;
	}

	if(!strcmp(argv[1], "-d")) {
		vas_dump();
	} else
	if(!strcmp(argv[1], "-a")) {
		vas_alloc();
	} else {
		printf("unknown switch\n");
	}
}

#include <vm/page.h>

static void
cache()
{
	static cache_desc_t* c;

	if(argc < 2) {
		printf("missing args\n");
		return;
	}

	if(!strcmp(argv[1], "-a")) {
		if(c) {
			printf("cache already created\n");
			return;
		}
		c = cache_create();
		printf("cache created: %08x\n", (ulong) c);
	} else
	if(!strcmp(argv[1], "-i")) {
		ulong page, offset;

		if(!c) {
			printf("first create cache...\n");
			return;
		}

		if(argc < 4) {
			printf("-i option usage: <page_addr> <offset>\n");
			return;
		}

		page = strtoul(argv[2], 0, 16);
		offset = atoi(argv[3]);
		cache_page(c, page, offset);
	} else
	if(!strcmp(argv[1], "-l")) {
		extern cache_desc_t* reserved_cache;
		ulong offset, p;

		/*
		if(!c) {
			printf("first create cache...\n");
			return;
		}
		*/
		c = reserved_cache;

		if(argc < 3) {
			printf("-l option usage: <offset>\n");
			return;
		}

		//offset = atoi(argv[2]);
		offset = strtoul(argv[2], 0, 16);
		p = cache_lookup(c, offset);		
		printf("cache_lookup = %08x\n", p);

	} else {
		printf("unknown switch\n");
	}
}

static void
region_test()
{
	ulong start, size;

	if(argc < 3) {
		printf("usage: <start{x}> <size{d}>\n");
		return;
	}

	start = strtoul(argv[1], 0, 16);
	size = atoi(argv[2]);
	rg_test(start, size);
}

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

static char buf[512*32];

static void
ide_test()
{
	extern ide_io(ulong, ulong, void*, ulong, ulong);
	ulong sect, scnt;
	struct partition_desc part;
	int c;
	void* p;
	ulong* u;

	if(argc < 3) {
		printf("usage: <start_sector> <sector_count>\n");
		return;
	}
	sect = atoi(argv[1]);
	scnt = atoi(argv[2]);

	if(scnt > 32) {
		printf("sector count must not exceed 32\n");
		return;
	}	
	
	ide_io(0, 0, (void*) buf, sect, scnt);
	p = buf + 0x1be;
	for(c = 0; c < 4; c++) {
		bcopy(p, (void*) &part, sizeof(part));
		p += sizeof(part);
		printf("part[%d]: type %x, start %d, len %d\n", 
			c, part.type, part.start, part.len);
	}
	
	u = (ulong*) (buf + ((scnt-1) << 9));
	for(c = 0; c < 8; c++) {
		printf("%08lx ", *u++);
	}
	printf("\n");
}

static void
ide_read()
{
	extern ide_io(ulong, ulong, void*, ulong, ulong);
	ulong sect;
	ulong* u;
	int c;

	if(argc < 2) {
		printf("usage: ider <sector>\n");
		return;
	}
	
	sect = atoi(argv[1]);
	ide_io(0, 0, (void*) buf, sect, 1);
	u = (ulong*) buf;
	for(c = 0; c < 32; c++) {
		printf("%08lx ", *u++);
		if(c % 8 == 0)
			printf("\n");
	}
}

static void
mount()
{
	extern int ext2fs_mount(ulong);

	if(argc < 2) {
		printf("usage: mount <device_number>\n");
		return;
	}
	
	ext2fs_mount((ulong) atoi(argv[1]));
}

static ulong cur_inode = 2;

static void
cd()
{
	if(argc < 2) {
		printf("usage: cd <inode_number>\n");
		return;
	}
	
	cur_inode = atoi(argv[1]);
}

static void
ls()
{
	extern void ext2fs_ls(ulong, int);	
	int detail;	
	
	detail = 0;
	if(argc == 2) {
		if(!strcmp(argv[1], "-l")) {
			detail = 1;
		} else {
			printf("unknown option\n");
			return;
		}
	}
	
	ext2fs_ls(cur_inode, detail);
}

static void
rpg()
{
	extern int ext2fs_readpage(ulong, ulong, void*);	
	ulong inode, page;
	
	if(argc < 3) {
		printf("usage: rpg <inode_number> <page_number>\n");
		return;
	}
	
	inode = atoi(argv[1]);
	page = atoi(argv[2]);
	
	ext2fs_readpage(inode, page, 0);
}

static void
read()
{
	static char tmp[4096];
	extern int ext2fs_read(ulong, ulong, void*, ulong);	
	ulong inode, off, cb, c;
	ulong* p;
	
	if(argc < 4) {
		printf("usage: read <inode_number> <file_offset> <bytes_count>\n");
		return;
	}
	
	inode = atoi(argv[1]);
	off = atoi(argv[2]);
	cb = atoi(argv[3]);

	if(cb > 4096) {
		printf("bytes_count cannot be bigger than 4096\n");
		return;
	}
	
	cb = ext2fs_read(inode, off, tmp, cb);
	printf("%d bytes read\n", cb);
	if(cb) {
		p = (ulong*) &tmp[0];
		for(c = 1; c < (cb > 64 ? 64 : cb) + 1; c++) {
			printf("%08lx ", *p++);
			if((c & 7) == 0)
				printf("\n");
		}
		off--;
		if(c & 7)
			printf("\n");
	}
}

static void
get()
{
#define BUFSZ	(1024)
	static char tmp[BUFSZ];
	extern int ext2fs_read(ulong, ulong, void*, ulong);	
	ulong inode, r, cb, pos;
	
	if(argc < 2) {
		printf("usage: get <inode_number>\n");
		return;
	}
	
	inode = atoi(argv[1]);

	cb = 0;
	pos = 0;
	while(1) {	
		r = ext2fs_read(inode, pos, tmp, BUFSZ);
		if(r == -1) {
			printf("ext2fs_read() failed\n");
			break;
		}
		if(r == 0)
			break;
					
		cb += r;
		pos += r;
	}
	printf("%d bytes read\n", cb);
}

static void
exec()
{
	if(argc < 2) {
		printf("usage: exec <inode_number>\n");
		return;
	}
	
	exec_read(atoi(argv[1]));
}

/*
 * system console stuff
 */
static void
parse(char* p)
{
	char buf[256];
	char* d = buf;

	argc = 0;
	while(1) {
		while(*p++ == ' ');
		p--;
		while((*p != ' ') && (*p != 0))
			*d++ = *p++;
		*d = 0;
		strcpy(argv[argc], buf);
		argc++;
		if( argc > MAX_ARGS ) {
			printf("parse: fatal - argc > MAX_ARGS\n");
			argc--;
			return;
		}
		if(*p == 0)
			return;
		p++;
		d = buf;
	}
}

static void
execute()
{
	int c = NCMD;
	int i = 0;

	while(c--) {
		if( !strcmp(cmd[i].name, argv[0]) ) {
			cmd[i].func();
			break;
		}
		i++;
	}

	if(i == NCMD)
		printf("unknown command.\n");
}

static void
dump_args()
{
	int c;
	for(c = 0; c < argc; c++)
		printf("argv[%d] = %s\n", c, argv[c]);
}

void
syscon()
{
	char  ch;
	char  buf[256];
	int   c;

	printf("\nEntering kernel console.\n# ");
	//for(;;);
	c = 0;
	while(1) {
		ch = get_key();
		if(ch) {
			if(ch == 0x0D) {
				cons_putc('\n');
				if(c == 0) {
					cons_print("# ");
					continue;
				}
				buf[c] = 0;
				parse(buf);
				execute();
				cons_print("# ");
				c = 0;

				continue;
			}
			if(ch == BACKSPACE) {
				if(c) {
					cons_ungetc();
					c--;
				}
				continue;
			}
			cons_putc(ch);
			buf[c] = ch;
			c++;
		}
	}
}

