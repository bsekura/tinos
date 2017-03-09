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
 * keyboard driver
 *
 * based on VSTa kbd server
 * Copyright (c) 1996 Andy Valencia
 *
 * initialization code based on Linux's keyboard.c
 * Copyright (c) Linus Torvalds and others
 *
 * $Id: kb.c,v 1.3 1998/03/01 21:07:01 bart Exp $
 *
 */

#include <sys/types.h>
#include <i386/inlines.h>
#include <i386/spl.h>
#include <thread.h>
#include <msg.h>
#include <pc/kb.h>

#define  KB_BUFSZ    (32)

static char kb_buf[KB_BUFSZ] = {0};
static uint kb_count = 0;

static char* pkbin = kb_buf;
static char* pkbout = kb_buf;

static int shift = 0;
static int ctrl = 0;
static int alt = 0;
static int caps = 0;
static int num = 0;

static char normal[] = {
  0x00,0x1B,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t',
'q','w','e','r','t','y','u','i','o','p','[',']',0x0D,0x80,
'a','s','d','f','g','h','j','k','l',';',047,0140,0x80,
0134,'z','x','c','v','b','n','m',',','.','/',0x80,
'*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,'0',0177
};


static char shifted[] = {
  0,033,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t',
'Q','W','E','R','T','Y','U','I','O','P','{','}',015,0x80,
'A','S','D','F','G','H','J','K','L',':',042,'~',0x80,
'|','Z','X','C','V','B','N','M','<','>','?',0x80,
'*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,'7','8','9',0x80,'4','5','6',0x80,
'1','2','3','0',177
};

extern struct tcb* cur_thread;
struct tcb* kb_waiter;

port_t* kb_port;
message_t* kb_isr_msg;

/* 
 * read keyboard and always check status 
 * if there's some stuff for us 
 */
static inline int 
kb_read()
{
	int c = 100000;
	while(c--) {
		int status = inb(KB_STATUS_REG);
		if(!(status & KBS_OUT_DATA))
			continue;

		return (inb(KB_DATA_REG) & 0xFF);
	}

	return (-1);
}

/* 
 * write some stuff to keyboard 
 * busy-wait spinning while it's not ready
 */
static inline void 
kb_write(int port, int data)
{
	int status;

	do {
		status = inb(KB_STATUS_REG);
	} while(status & KBS_IN_DATA);

	outb(port, data);
}

/*
 * initialize keyboard
 */
void
kb_init()
{
	kb_port = alloc_port();
	kb_isr_msg = alloc_msg();
	kb_isr_msg->flags |= MF_INTERRUPT;

	/* flush keyboard */
	while(kb_read() != -1);

	return;

	/* self test - 0x55 means ok */
	kb_write(KB_CMD_REG, KBC_SELF_TEST);
	if(kb_read() != 0x55) {
		printf("kb_init(): wow, self test failed.\n");
	}

	/* kb interface test - 0 is ok */
	kb_write(KB_CMD_REG, KBC_IF_TEST);
	if(kb_read() != 0x00) {
		printf("kb_init(): wow, interface test failed.\n");
	}

	/* enable interface */
	kb_write(KB_CMD_REG, KBC_IF_ENABLE);

	/* enable keyboard */
	kb_write(KB_DATA_REG, KB_ENABLE);
	if(kb_read() != KB_ACK) {
		printf("kb_init(): KB_ENABLE - no ack.\n");
	}
}

static int
kb_special(uchar key)
{
	switch(key) {
   		case 0x36:
   	   	case 0x2A: 
         		shift = 1;
         		break;
      		case 0xB6:
      		case 0xAA:
         		shift = 0;
         		break;
      		case 0x1D:
         		ctrl = 1;
         		break;
      		case 0x9D:
         		ctrl = 0;
         		break;
      		case 0x38:
         		alt = 1;
         		break;
      		case 0xB8:
         		alt = 0;
         		break;
      		case 0x3A:
      		case 0x45:
         		break;
      		case 0xBA:
         		caps = !caps;
         		break;
      		case 0xC5:
         		num = !num;
         		break;
      		case 0xE0:
         		break;
      		default:
         		return(0);
   	}

   	return (1);
}

void
kb_isr(/*ulong stack*/)
{
   	/*struct frame* f = (struct frame*)&stack;*/
   	extern ulong sched_preempt;
   	int code;
   	
#if 0   	
   	do_send(kb_port, kb_isr_msg);
   	sched_preempt++;
   	return;
#endif  
   	
   	code = inb(KB_DATA_REG);

   	/*
   	 * check if special key 
   	 */
   	if(kb_special(code))
   		return;

  	/*
   	 * filter out release codes
  	 */
   	if(code & 0200)
      		return;

   	/* 
   	 * Ctl-C pressed
   	 */
   	if(ctrl && (code == 0x2E))
      		reboot_machine();

   	/*
   	 * enqueue key
   	 */
   	if( kb_count < KB_BUFSZ ) {
      		*pkbin++ = code;
      		kb_count++;
      		if( pkbin >= kb_buf + KB_BUFSZ ) {
         		pkbin = kb_buf;
      		}
   	} 
   	else {
      		pkbin = pkbout = kb_buf;
      		kb_count = 0;
   	}

	/* check for threads waiting for key */
	if(kb_waiter) {
		thread_wakeup(kb_waiter);
		kb_waiter = NULL;
	}
}

static int 
kb_readbuf()
{
	char ch;
	spl_t s;

   	s = splhi_save();
   	if( kb_count == 0 ) {
      		ch = 0;
   	} 
   	else {
      		kb_count--;
   
      		ch = *pkbout++;
      		if( pkbout >= kb_buf + KB_BUFSZ)
         		pkbout = kb_buf;
   	}
   	splx(s);   
   	return ((int)ch);
}

char
kb_getch()
{
   	register int ch;

   	ch = kb_readbuf();
   	if(ch == 0)
   		return (0);

   	if(ch == 72)
		return ('*');

   	if(caps) {
		register char tmp = normal[ch];
		if((tmp >= 'a') && (tmp <= 'z'))
	   		return (shift ? normal[ch] : shifted[ch]);
		else
	   		return (shift ? shifted[ch] : normal[ch]);
   	}
   	return (shift ? shifted[ch] : normal[ch]);
}

char
get_key()
{
	char ch;
	spl_t s;

	ch = kb_getch();
	if(ch)
		return (ch);

	s = splhi_save();
	kb_waiter = cur_thread;
	thread_sleep(cur_thread);
	splx(s);
	schedule();
	return (kb_getch());
}

/*
struct server kb_srv;
	
void
kb_server()
{
	struct message* m;
	struct message* kb_wait = NULL;
	int k;
	enable_irq(1);
	while(1) {
		m = m_listen(&kb_srv);
		switch((ulong)m) {
			case 99:
				//printf("kb_server(): 99\n");
				if(kb_isr2()) {
					if(kb_wait) {
					//printf("kb_server(): kb_wait set\n");
					k = kb_getch();
					if(k) {
						//printf("kb_server(): replying...\n");
						kb_wait->data = k;
						m_reply_data(&kb_srv, kb_wait);
						kb_wait = NULL;
					}
					}
				}
				break;

			default:
				//printf("kb_server(): getkey\n");
				k = kb_getch();
				if(k) {
					//printf("kb_server(): replying directly\n");
					m->data = k;
					m_reply_data(&kb_srv, m);
				} else {
					//printf("kb_server(): waiting\n");
					kb_wait = m;
				}

				break;
		}
	}
}

void
kb_thread()
{
	struct tcb* t;

	init_server(&kb_srv);	
	t = create_thread((ulong) &kb_server);
	if(!t) {
		printf("kb_thread() failed\n");
		return;
	}
}

void
kb_isr()
{
	m_send_interrupt(&kb_srv, 99);
}

int
get_key()
{
	int k = m_send(&kb_srv, 1);
	//printf("get_key(): got %d\n", k);
	return(k);
}

*/
