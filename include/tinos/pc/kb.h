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
 * $Id: kb.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */


#ifndef __kb_h__
#define __kb_h__

#define KB_STATUS_REG		(0x64)

#define KBS_OUT_DATA		(0x01)
#define KBS_IN_DATA		(0x02)
#define KBS_TX_TIMEOUT		(0x20)
#define KBS_RECV_TIMEOUT	(0x40)
#define KBS_PAR_EVEN		(0x80)

#define KB_CMD_REG		(0x64)

#define KBC_SELF_TEST		(0xAA)
#define KBC_IF_TEST		(0xAB)
#define KBC_IF_DISABLE		(0xAD)
#define KBC_IF_ENABLE		(0xAE)

#define KB_DATA_REG		(0x60)

#define KB_ENABLE		(0xF4)
#define KB_ACK			(0xFA)
#define KB_RESEND		(0xFE)

#define	KB_CTL_REG		(0x61)

#endif /* __kb_h__ */

