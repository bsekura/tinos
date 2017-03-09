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
 * $Id: ctype.c,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */
/* 
 * derived from:
 * Copyright (C) 1995 DJ Delorie
 *
 */
 
#include <ctype.h>

ushort __ctab[] = {
  0,                       /* CTRL+?, 0xffff */
  _C_CTL,                  /* CTRL+@, 0x00 */
  _C_CTL,                  /* CTRL+A, 0x01 */
  _C_CTL,                  /* CTRL+B, 0x02 */
  _C_CTL,                  /* CTRL+C, 0x03 */
  _C_CTL,                  /* CTRL+D, 0x04 */
  _C_CTL,                  /* CTRL+E, 0x05 */
  _C_CTL,                  /* CTRL+F, 0x06 */
  _C_CTL,                  /* CTRL+G, 0x07 */
  _C_CTL,                  /* CTRL+H, 0x08 */
  _C_CTL | _C_SPC,         /* CTRL+I, 0x09 */
  _C_CTL | _C_SPC,         /* CTRL+J, 0x0a */
  _C_CTL | _C_SPC,         /* CTRL+K, 0x0b */
  _C_CTL | _C_SPC,         /* CTRL+L, 0x0c */
  _C_CTL | _C_SPC,         /* CTRL+M, 0x0d */
  _C_CTL,                  /* CTRL+N, 0x0e */
  _C_CTL,                  /* CTRL+O, 0x0f */
  _C_CTL,                  /* CTRL+P, 0x10 */
  _C_CTL,                  /* CTRL+Q, 0x11 */
  _C_CTL,                  /* CTRL+R, 0x12 */
  _C_CTL,                  /* CTRL+S, 0x13 */
  _C_CTL,                  /* CTRL+T, 0x14 */
  _C_CTL,                  /* CTRL+U, 0x15 */
  _C_CTL,                  /* CTRL+V, 0x16 */
  _C_CTL,                  /* CTRL+W, 0x17 */
  _C_CTL,                  /* CTRL+X, 0x18 */
  _C_CTL,                  /* CTRL+Y, 0x19 */
  _C_CTL,                  /* CTRL+Z, 0x1a */
  _C_CTL,                  /* CTRL+[, 0x1b */
  _C_CTL,                  /* CTRL+\, 0x1c */
  _C_CTL,                  /* CTRL+], 0x1d */
  _C_CTL,                  /* CTRL+^, 0x1e */
  _C_CTL,                  /* CTRL+_, 0x1f */
  _C_PRN | _C_SPC,               /* ` ', 0x20 */
  _C_GRA | _C_PRN | _C_PUN,            /* `!', 0x21 */
  _C_GRA | _C_PRN | _C_PUN,            /* 0x22 */
  _C_GRA | _C_PRN | _C_PUN,            /* `#', 0x23 */
  _C_GRA | _C_PRN | _C_PUN,            /* `$', 0x24 */
  _C_GRA | _C_PRN | _C_PUN,            /* `%', 0x25 */
  _C_GRA | _C_PRN | _C_PUN,            /* `&', 0x26 */
  _C_GRA | _C_PRN | _C_PUN,            /* 0x27 */
  _C_GRA | _C_PRN | _C_PUN,            /* `(', 0x28 */
  _C_GRA | _C_PRN | _C_PUN,            /* `)', 0x29 */
  _C_GRA | _C_PRN | _C_PUN,            /* `*', 0x2a */
  _C_GRA | _C_PRN | _C_PUN,            /* `+', 0x2b */
  _C_GRA | _C_PRN | _C_PUN,            /* `,', 0x2c */
  _C_GRA | _C_PRN | _C_PUN,            /* `-', 0x2d */
  _C_GRA | _C_PRN | _C_PUN,            /* `.', 0x2e */
  _C_GRA | _C_PRN | _C_PUN,            /* `/', 0x2f */
  _C_ALN | _C_DIG | _C_GRA | _C_PRN | _C_HEX,      /* `0', 0x30 */
  _C_ALN | _C_DIG | _C_GRA | _C_PRN | _C_HEX,      /* `1', 0x31 */
  _C_ALN | _C_DIG | _C_GRA | _C_PRN | _C_HEX,      /* `2', 0x32 */
  _C_ALN | _C_DIG | _C_GRA | _C_PRN | _C_HEX,      /* `3', 0x33 */
  _C_ALN | _C_DIG | _C_GRA | _C_PRN | _C_HEX,      /* `4', 0x34 */
  _C_ALN | _C_DIG | _C_GRA | _C_PRN | _C_HEX,      /* `5', 0x35 */
  _C_ALN | _C_DIG | _C_GRA | _C_PRN | _C_HEX,      /* `6', 0x36 */
  _C_ALN | _C_DIG | _C_GRA | _C_PRN | _C_HEX,      /* `7', 0x37 */
  _C_ALN | _C_DIG | _C_GRA | _C_PRN | _C_HEX,      /* `8', 0x38 */
  _C_ALN | _C_DIG | _C_GRA | _C_PRN | _C_HEX,      /* `9', 0x39 */
  _C_GRA | _C_PRN | _C_PUN,            /* `:', 0x3a */
  _C_GRA | _C_PRN | _C_PUN,            /* `;', 0x3b */
  _C_GRA | _C_PRN | _C_PUN,            /* `<', 0x3c */
  _C_GRA | _C_PRN | _C_PUN,            /* `=', 0x3d */
  _C_GRA | _C_PRN | _C_PUN,            /* `>', 0x3e */
  _C_GRA | _C_PRN | _C_PUN,            /* `?', 0x3f */
  _C_GRA | _C_PRN | _C_PUN,            /* `@', 0x40 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE | _C_HEX,   /* `A', 0x41 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE | _C_HEX,   /* `B', 0x42 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE | _C_HEX,   /* `C', 0x43 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE | _C_HEX,   /* `D', 0x44 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE | _C_HEX,   /* `E', 0x45 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE | _C_HEX,   /* `F', 0x46 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `G', 0x47 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `H', 0x48 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `I', 0x49 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `J', 0x4a */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `K', 0x4b */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `L', 0x4c */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `M', 0x4d */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `N', 0x4e */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `O', 0x4f */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `P', 0x50 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `Q', 0x51 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `R', 0x52 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `S', 0x53 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `T', 0x54 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `U', 0x55 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `V', 0x56 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `W', 0x57 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `X', 0x58 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `Y', 0x59 */
  _C_ALN | _C_ALP | _C_GRA | _C_PRN | _C_UPE,      /* `Z', 0x5a */
  _C_GRA | _C_PRN | _C_PUN,            /* `[', 0x5b */
  _C_GRA | _C_PRN | _C_PUN,            /* 0x5c */
  _C_GRA | _C_PRN | _C_PUN,            /* `]', 0x5d */
  _C_GRA | _C_PRN | _C_PUN,            /* `^', 0x5e */
  _C_GRA | _C_PRN | _C_PUN,            /* `_', 0x5f */
  _C_GRA | _C_PRN | _C_PUN,            /* 0x60 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN | _C_HEX,   /* `a', 0x61 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN | _C_HEX,   /* `b', 0x62 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN | _C_HEX,   /* `c', 0x63 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN | _C_HEX,   /* `d', 0x64 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN | _C_HEX,   /* `e', 0x65 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN | _C_HEX,   /* `f', 0x66 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `g', 0x67 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `h', 0x68 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `i', 0x69 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `j', 0x6a */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `k', 0x6b */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `l', 0x6c */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `m', 0x6d */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `n', 0x6e */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `o', 0x6f */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `p', 0x70 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `q', 0x71 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `r', 0x72 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `s', 0x73 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `t', 0x74 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `u', 0x75 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `v', 0x76 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `w', 0x77 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `x', 0x78 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `y', 0x79 */
  _C_ALN | _C_ALP | _C_GRA | _C_LOW | _C_PRN,      /* `z', 0x7a */
  _C_GRA | _C_PRN | _C_PUN,            /* `{', 0x7b */
  _C_GRA | _C_PRN | _C_PUN,            /* `|', 0x7c */
  _C_GRA | _C_PRN | _C_PUN,            /* `}', 0x7d */
  _C_GRA | _C_PRN | _C_PUN,            /* `~', 0x7e */
  _C_CTL,                  /* 0x7f */
  0,                    /* 0x80 */
  0,                    /* 0x81 */
  0,                    /* 0x82 */
  0,                    /* 0x83 */
  0,                    /* 0x84 */
  0,                    /* 0x85 */
  0,                    /* 0x86 */
  0,                    /* 0x87 */
  0,                    /* 0x88 */
  0,                    /* 0x89 */
  0,                    /* 0x8a */
  0,                    /* 0x8b */
  0,                    /* 0x8c */
  0,                    /* 0x8d */
  0,                    /* 0x8e */
  0,                    /* 0x8f */
  0,                    /* 0x90 */
  0,                    /* 0x91 */
  0,                    /* 0x92 */
  0,                    /* 0x93 */
  0,                    /* 0x94 */
  0,                    /* 0x95 */
  0,                    /* 0x96 */
  0,                    /* 0x97 */
  0,                    /* 0x98 */
  0,                    /* 0x99 */
  0,                    /* 0x9a */
  0,                    /* 0x9b */
  0,                    /* 0x9c */
  0,                    /* 0x9d */
  0,                    /* 0x9e */
  0,                    /* 0x9f */
  0,                    /* 0xa0 */
  0,                    /* 0xa1 */
  0,                    /* 0xa2 */
  0,                    /* 0xa3 */
  0,                    /* 0xa4 */
  0,                    /* 0xa5 */
  0,                    /* 0xa6 */
  0,                    /* 0xa7 */
  0,                    /* 0xa8 */
  0,                    /* 0xa9 */
  0,                    /* 0xaa */
  0,                    /* 0xab */
  0,                    /* 0xac */
  0,                    /* 0xad */
  0,                    /* 0xae */
  0,                    /* 0xaf */
  0,                    /* 0xb0 */
  0,                    /* 0xb1 */
  0,                    /* 0xb2 */
  0,                    /* 0xb3 */
  0,                    /* 0xb4 */
  0,                    /* 0xb5 */
  0,                    /* 0xb6 */
  0,                    /* 0xb7 */
  0,                    /* 0xb8 */
  0,                    /* 0xb9 */
  0,                    /* 0xba */
  0,                    /* 0xbb */
  0,                    /* 0xbc */
  0,                    /* 0xbd */
  0,                    /* 0xbe */
  0,                    /* 0xbf */
  0,                    /* 0xc0 */
  0,                    /* 0xc1 */
  0,                    /* 0xc2 */
  0,                    /* 0xc3 */
  0,                    /* 0xc4 */
  0,                    /* 0xc5 */
  0,                    /* 0xc6 */
  0,                    /* 0xc7 */
  0,                    /* 0xc8 */
  0,                    /* 0xc9 */
  0,                    /* 0xca */
  0,                    /* 0xcb */
  0,                    /* 0xcc */
  0,                    /* 0xcd */
  0,                    /* 0xce */
  0,                    /* 0xcf */
  0,                    /* 0xd0 */
  0,                    /* 0xd1 */
  0,                    /* 0xd2 */
  0,                    /* 0xd3 */
  0,                    /* 0xd4 */
  0,                    /* 0xd5 */
  0,                    /* 0xd6 */
  0,                    /* 0xd7 */
  0,                    /* 0xd8 */
  0,                    /* 0xd9 */
  0,                    /* 0xda */
  0,                    /* 0xdb */
  0,                    /* 0xdc */
  0,                    /* 0xdd */
  0,                    /* 0xde */
  0,                    /* 0xdf */
  0,                    /* 0xe0 */
  0,                    /* 0xe1 */
  0,                    /* 0xe2 */
  0,                    /* 0xe3 */
  0,                    /* 0xe4 */
  0,                    /* 0xe5 */
  0,                    /* 0xe6 */
  0,                    /* 0xe7 */
  0,                    /* 0xe8 */
  0,                    /* 0xe9 */
  0,                    /* 0xea */
  0,                    /* 0xeb */
  0,                    /* 0xec */
  0,                    /* 0xed */
  0,                    /* 0xee */
  0,                    /* 0xef */
  0,                    /* 0xf0 */
  0,                    /* 0xf1 */
  0,                    /* 0xf2 */
  0,                    /* 0xf3 */
  0,                    /* 0xf4 */
  0,                    /* 0xf5 */
  0,                    /* 0xf6 */
  0,                    /* 0xf7 */
  0,                    /* 0xf8 */
  0,                    /* 0xf9 */
  0,                    /* 0xfa */
  0,                    /* 0xfb */
  0,                    /* 0xfc */
  0,                    /* 0xfd */
  0,                    /* 0xfe */
  0,                    /* 0xff */
};

uchar __tolower_tab[] = {
  0x00,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
  0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
  0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
  0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
  0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
  0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
  0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
  0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
  0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
  0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
  0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
  0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
  0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
  0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
  0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
  0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
  0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

uchar __toupper_tab[] = {
  0x00,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
  0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
  0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
  0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
  0x58, 0x59, 0x5a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
  0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
  0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
  0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
  0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
  0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
  0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
  0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
  0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
  0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
  0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
  0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};
