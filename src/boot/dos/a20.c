/*
 * $Id: a20.c,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */
 
void 
a20_on()
{
	asm 
	{
      		cli
      		xor cx, cx
   	}

IBEmm0:

   	asm 
	{
      		in al, 64h
      		test al, 02h
      		loopnz IBEmm0
      		mov al, 0D1h
     		out 64h, al
      		xor cx, cx
   	}

IBEmm1:

   asm {
      in al, 64h
      test al, 02h
      loopnz IBEmm1
      mov al, 0DFh
      out 60h, al
      xor cx, cx
   }

IBEmm2:

   asm {
      in al, 64h
      test al, 02h
      loopnz IBEmm2
   }
}
