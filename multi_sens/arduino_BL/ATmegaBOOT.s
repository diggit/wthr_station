	.file	"ATmegaBOOT.c"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__tmp_reg__ = 0
__zero_reg__ = 1
	.stabs	"/home/diggit/dev/dlouhodobka/multi_sens/arduino_BL/",100,0,2,.Ltext0
	.stabs	"ATmegaBOOT.c",100,0,2,.Ltext0
	.text
.Ltext0:
	.stabs	"gcc2_compiled.",60,0,0,0
	.stabs	"int:t(0,1)=r(0,1);-32768;32767;",128,0,0,0
	.stabs	"char:t(0,2)=r(0,2);0;127;",128,0,0,0
	.stabs	"long int:t(0,3)=@s32;r(0,3);020000000000;017777777777;",128,0,0,0
	.stabs	"unsigned int:t(0,4)=r(0,4);0;0177777;",128,0,0,0
	.stabs	"long unsigned int:t(0,5)=@s32;r(0,5);0;037777777777;",128,0,0,0
	.stabs	"long long int:t(0,6)=@s64;r(0,6);01000000000000000000000;0777777777777777777777;",128,0,0,0
	.stabs	"long long unsigned int:t(0,7)=@s64;r(0,7);0;01777777777777777777777;",128,0,0,0
	.stabs	"short int:t(0,8)=r(0,8);-32768;32767;",128,0,0,0
	.stabs	"short unsigned int:t(0,9)=r(0,9);0;0177777;",128,0,0,0
	.stabs	"signed char:t(0,10)=@s8;r(0,10);-128;127;",128,0,0,0
	.stabs	"unsigned char:t(0,11)=@s8;r(0,11);0;255;",128,0,0,0
	.stabs	"float:t(0,12)=r(0,1);4;0;",128,0,0,0
	.stabs	"double:t(0,13)=r(0,1);4;0;",128,0,0,0
	.stabs	"long double:t(0,14)=r(0,1);4;0;",128,0,0,0
	.stabs	"short _Fract:t(0,15)=r(0,1);1;0;",128,0,0,0
	.stabs	"_Fract:t(0,16)=r(0,1);2;0;",128,0,0,0
	.stabs	"long _Fract:t(0,17)=r(0,1);4;0;",128,0,0,0
	.stabs	"long long _Fract:t(0,18)=r(0,1);8;0;",128,0,0,0
	.stabs	"unsigned short _Fract:t(0,19)=r(0,1);1;0;",128,0,0,0
	.stabs	"unsigned _Fract:t(0,20)=r(0,1);2;0;",128,0,0,0
	.stabs	"unsigned long _Fract:t(0,21)=r(0,1);4;0;",128,0,0,0
	.stabs	"unsigned long long _Fract:t(0,22)=r(0,1);8;0;",128,0,0,0
	.stabs	"_Sat short _Fract:t(0,23)=r(0,1);1;0;",128,0,0,0
	.stabs	"_Sat _Fract:t(0,24)=r(0,1);2;0;",128,0,0,0
	.stabs	"_Sat long _Fract:t(0,25)=r(0,1);4;0;",128,0,0,0
	.stabs	"_Sat long long _Fract:t(0,26)=r(0,1);8;0;",128,0,0,0
	.stabs	"_Sat unsigned short _Fract:t(0,27)=r(0,1);1;0;",128,0,0,0
	.stabs	"_Sat unsigned _Fract:t(0,28)=r(0,1);2;0;",128,0,0,0
	.stabs	"_Sat unsigned long _Fract:t(0,29)=r(0,1);4;0;",128,0,0,0
	.stabs	"_Sat unsigned long long _Fract:t(0,30)=r(0,1);8;0;",128,0,0,0
	.stabs	"short _Accum:t(0,31)=r(0,1);2;0;",128,0,0,0
	.stabs	"_Accum:t(0,32)=r(0,1);4;0;",128,0,0,0
	.stabs	"long _Accum:t(0,33)=r(0,1);8;0;",128,0,0,0
	.stabs	"long long _Accum:t(0,34)=r(0,1);8;0;",128,0,0,0
	.stabs	"unsigned short _Accum:t(0,35)=r(0,1);2;0;",128,0,0,0
	.stabs	"unsigned _Accum:t(0,36)=r(0,1);4;0;",128,0,0,0
	.stabs	"unsigned long _Accum:t(0,37)=r(0,1);8;0;",128,0,0,0
	.stabs	"unsigned long long _Accum:t(0,38)=r(0,1);8;0;",128,0,0,0
	.stabs	"_Sat short _Accum:t(0,39)=r(0,1);2;0;",128,0,0,0
	.stabs	"_Sat _Accum:t(0,40)=r(0,1);4;0;",128,0,0,0
	.stabs	"_Sat long _Accum:t(0,41)=r(0,1);8;0;",128,0,0,0
	.stabs	"_Sat long long _Accum:t(0,42)=r(0,1);8;0;",128,0,0,0
	.stabs	"_Sat unsigned short _Accum:t(0,43)=r(0,1);2;0;",128,0,0,0
	.stabs	"_Sat unsigned _Accum:t(0,44)=r(0,1);4;0;",128,0,0,0
	.stabs	"_Sat unsigned long _Accum:t(0,45)=r(0,1);8;0;",128,0,0,0
	.stabs	"_Sat unsigned long long _Accum:t(0,46)=r(0,1);8;0;",128,0,0,0
	.stabs	"void:t(0,47)=(0,47)",128,0,0,0
	.stabs	"/usr/avr/include/inttypes.h",130,0,0,0
	.stabs	"/usr/lib/gcc/avr/4.9.0/include/stdint.h",130,0,0,0
	.stabs	"/usr/avr/include/stdint.h",130,0,0,0
	.stabs	"int8_t:t(3,1)=(0,10)",128,0,121,0
	.stabs	"uint8_t:t(3,2)=(0,11)",128,0,122,0
	.stabs	"int16_t:t(3,3)=(0,1)",128,0,123,0
	.stabs	"uint16_t:t(3,4)=(0,4)",128,0,124,0
	.stabs	"int32_t:t(3,5)=(0,3)",128,0,125,0
	.stabs	"uint32_t:t(3,6)=(0,5)",128,0,126,0
	.stabs	"int64_t:t(3,7)=(0,6)",128,0,128,0
	.stabs	"uint64_t:t(3,8)=(0,7)",128,0,129,0
	.stabs	"intptr_t:t(3,9)=(3,3)",128,0,142,0
	.stabs	"uintptr_t:t(3,10)=(3,4)",128,0,147,0
	.stabs	"int_least8_t:t(3,11)=(3,1)",128,0,159,0
	.stabs	"uint_least8_t:t(3,12)=(3,2)",128,0,164,0
	.stabs	"int_least16_t:t(3,13)=(3,3)",128,0,169,0
	.stabs	"uint_least16_t:t(3,14)=(3,4)",128,0,174,0
	.stabs	"int_least32_t:t(3,15)=(3,5)",128,0,179,0
	.stabs	"uint_least32_t:t(3,16)=(3,6)",128,0,184,0
	.stabs	"int_least64_t:t(3,17)=(3,7)",128,0,192,0
	.stabs	"uint_least64_t:t(3,18)=(3,8)",128,0,199,0
	.stabs	"int_fast8_t:t(3,19)=(3,1)",128,0,213,0
	.stabs	"uint_fast8_t:t(3,20)=(3,2)",128,0,218,0
	.stabs	"int_fast16_t:t(3,21)=(3,3)",128,0,223,0
	.stabs	"uint_fast16_t:t(3,22)=(3,4)",128,0,228,0
	.stabs	"int_fast32_t:t(3,23)=(3,5)",128,0,233,0
	.stabs	"uint_fast32_t:t(3,24)=(3,6)",128,0,238,0
	.stabs	"int_fast64_t:t(3,25)=(3,7)",128,0,246,0
	.stabs	"uint_fast64_t:t(3,26)=(3,8)",128,0,253,0
	.stabs	"intmax_t:t(3,27)=(3,7)",128,0,273,0
	.stabs	"uintmax_t:t(3,28)=(3,8)",128,0,278,0
	.stabn	162,0,0,0
	.stabn	162,0,0,0
	.stabs	"int_farptr_t:t(1,1)=(3,5)",128,0,77,0
	.stabs	"uint_farptr_t:t(1,2)=(3,6)",128,0,81,0
	.stabn	162,0,0,0
	.stabs	"/usr/avr/include/avr/io.h",130,0,0,0
	.stabs	"/usr/avr/include/avr/fuse.h",130,0,0,0
	.stabs	"__fuse_t:t(5,1)=(5,2)=s2low:(0,11),0,8;high:(0,11),8,8;;",128,0,252,0
	.stabn	162,0,0,0
	.stabn	162,0,0,0
	.stabs	"/usr/avr/include/avr/pgmspace.h",130,0,0,0
	.stabs	"/usr/lib/gcc/avr/4.9.0/include/stddef.h",130,0,0,0
	.stabs	"size_t:t(7,1)=(0,4)",128,0,212,0
	.stabn	162,0,0,0
	.stabn	162,0,0,0
	.stabs	"/usr/avr/include/avr/eeprom.h",130,0,0,0
	.stabs	"/usr/lib/gcc/avr/4.9.0/include/stddef.h",130,0,0,0
	.stabs	"ptrdiff_t:t(9,1)=(0,1)",128,0,147,0
	.stabs	"wchar_t:t(9,2)=(0,1)",128,0,324,0
	.stabn	162,0,0,0
	.stabn	162,0,0,0
	.stabs	"address_union:T(0,48)=u2word:(3,4),0,16;byte:(0,49)=ar(0,50)=r(0,50);0;0177777;;0;1;(3,2),0,16;;",128,0,0,0
	.stabs	"length_union:T(0,51)=u2word:(3,4),0,16;byte:(0,49),0,16;;",128,0,0,0
	.stabs	"flags_struct:T(0,52)=s1eeprom:(0,4),0,1;rampz:(0,4),1,1;;",128,0,0,0
	.stabs	"putch:F(0,47)",36,0,456,putch
	.stabs	"ch:P(0,2)",64,0,456,24
.global	putch
	.type	putch, @function
putch:
	.stabd	46,0,0
	.stabn	68,0,457,.LM0-.LFBB1
.LM0:
.LFBB1:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
.L2:
	.stabn	68,0,459,.LM1-.LFBB1
.LM1:
	sbis 0xb,5
	rjmp .L2
	.stabn	68,0,460,.LM2-.LFBB1
.LM2:
	out 0xc,r24
	ret
	.size	putch, .-putch
.Lscope1:
	.stabs	"",36,0,0,.Lscope1-.LFBB1
	.stabd	78,0,0
	.stabs	"getch:F(0,2)",36,0,463,getch
.global	getch
	.type	getch, @function
getch:
	.stabd	46,0,0
	.stabn	68,0,464,.LM3-.LFBB2
.LM3:
.LFBB2:
	push r12
	push r13
	push r14
	push r15
/* prologue: function */
/* frame size = 0 */
/* stack size = 4 */
.L__stack_usage = 4
	.stabn	68,0,466,.LM4-.LFBB2
.LM4:
	mov r12,__zero_reg__
	mov r13,__zero_reg__
	movw r14,r12
.L7:
	.stabn	68,0,467,.LM5-.LFBB2
.LM5:
	sbic 0xb,7
	rjmp .L10
	.stabn	68,0,469,.LM6-.LFBB2
.LM6:
	ldi r24,-1
	sub r12,r24
	sbc r13,r24
	sbc r14,r24
	sbc r15,r24
	.stabn	68,0,470,.LM7-.LFBB2
.LM7:
	ldi r24,-127
	cp r12,r24
	ldi r24,-115
	cpc r13,r24
	ldi r24,91
	cpc r14,r24
	cpc r15,__zero_reg__
	brlo .L7
	.stabn	68,0,471,.LM8-.LFBB2
.LM8:
	lds r30,app_start
	lds r31,app_start+1
	icall
	rjmp .L7
.L10:
	.stabn	68,0,473,.LM9-.LFBB2
.LM9:
	in r24,0xc
/* epilogue start */
	.stabn	68,0,474,.LM10-.LFBB2
.LM10:
	pop r15
	pop r14
	pop r13
	pop r12
	ret
	.size	getch, .-getch
	.stabs	"count:r(3,6)",64,0,466,12
	.stabn	192,0,0,.LFBB2-.LFBB2
	.stabn	224,0,0,.Lscope2-.LFBB2
.Lscope2:
	.stabs	"",36,0,0,.Lscope2-.LFBB2
	.stabd	78,0,0
	.stabs	"getNch:F(0,47)",36,0,476,getNch
	.stabs	"count:P(3,2)",64,0,476,29
.global	getNch
	.type	getNch, @function
getNch:
	.stabd	46,0,0
	.stabn	68,0,477,.LM11-.LFBB3
.LM11:
.LFBB3:
	push r28
	push r29
/* prologue: function */
/* frame size = 0 */
/* stack size = 2 */
.L__stack_usage = 2
	mov r29,r24
	.stabn	68,0,479,.LM12-.LFBB3
.LM12:
	ldi r28,0
.L12:
	.stabn	68,0,479,.LM13-.LFBB3
.LM13:
	cp r28,r29
	breq .L14
	.stabn	68,0,483,.LM14-.LFBB3
.LM14:
	rcall getch
	.stabn	68,0,479,.LM15-.LFBB3
.LM15:
	subi r28,lo8(-(1))
	rjmp .L12
.L14:
/* epilogue start */
	.stabn	68,0,485,.LM16-.LFBB3
.LM16:
	pop r29
	pop r28
	ret
	.size	getNch, .-getNch
	.stabs	"i:r(3,2)",64,0,478,28
	.stabn	192,0,0,.LFBB3-.LFBB3
	.stabn	224,0,0,.Lscope3-.LFBB3
.Lscope3:
	.stabs	"",36,0,0,.Lscope3-.LFBB3
	.stabd	78,0,0
	.stabs	"byte_response:F(0,47)",36,0,487,byte_response
	.stabs	"val:P(3,2)",64,0,487,28
.global	byte_response
	.type	byte_response, @function
byte_response:
	.stabd	46,0,0
	.stabn	68,0,488,.LM17-.LFBB4
.LM17:
.LFBB4:
	push r28
/* prologue: function */
/* frame size = 0 */
/* stack size = 1 */
.L__stack_usage = 1
	mov r28,r24
	.stabn	68,0,489,.LM18-.LFBB4
.LM18:
	rcall getch
	cpi r24,lo8(32)
	brne .L15
	.stabn	68,0,490,.LM19-.LFBB4
.LM19:
	ldi r24,lo8(20)
	rcall putch
	.stabn	68,0,491,.LM20-.LFBB4
.LM20:
	mov r24,r28
	rcall putch
	.stabn	68,0,492,.LM21-.LFBB4
.LM21:
	ldi r24,lo8(16)
/* epilogue start */
	.stabn	68,0,494,.LM22-.LFBB4
.LM22:
	pop r28
	.stabn	68,0,492,.LM23-.LFBB4
.LM23:
	rjmp putch
.L15:
/* epilogue start */
	.stabn	68,0,494,.LM24-.LFBB4
.LM24:
	pop r28
	ret
	.size	byte_response, .-byte_response
.Lscope4:
	.stabs	"",36,0,0,.Lscope4-.LFBB4
	.stabd	78,0,0
	.stabs	"nothing_response:F(0,47)",36,0,496,nothing_response
.global	nothing_response
	.type	nothing_response, @function
nothing_response:
	.stabd	46,0,0
	.stabn	68,0,497,.LM25-.LFBB5
.LM25:
.LFBB5:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	.stabn	68,0,498,.LM26-.LFBB5
.LM26:
	rcall getch
	cpi r24,lo8(32)
	brne .L17
	.stabn	68,0,499,.LM27-.LFBB5
.LM27:
	ldi r24,lo8(20)
	rcall putch
	.stabn	68,0,500,.LM28-.LFBB5
.LM28:
	ldi r24,lo8(16)
	rjmp putch
.L17:
	ret
	.size	nothing_response, .-nothing_response
.Lscope5:
	.stabs	"",36,0,0,.Lscope5-.LFBB5
	.stabd	78,0,0
	.section	.text.startup,"ax",@progbits
	.stabs	"main:F(0,1)",36,0,128,main
.global	main
	.type	main, @function
main:
	.stabd	46,0,0
	.stabn	68,0,129,.LM29-.LFBB6
.LM29:
.LFBB6:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	.stabn	68,0,136,.LM30-.LFBB6
.LM30:
/* #APP */
 ;  136 "ATmegaBOOT.c" 1
	nop
	
 ;  0 "" 2
	.stabn	68,0,147,.LM31-.LFBB6
.LM31:
/* #NOAPP */
	out 0x20,__zero_reg__
	.stabn	68,0,148,.LM32-.LFBB6
.LM32:
	ldi r24,lo8(51)
	out 0x9,r24
	.stabn	68,0,149,.LM33-.LFBB6
.LM33:
	ldi r24,lo8(24)
	out 0xa,r24
	.stabn	68,0,150,.LM34-.LFBB6
.LM34:
	ldi r24,lo8(-122)
	out 0x20,r24
	.stabn	68,0,168,.LM35-.LFBB6
.LM35:
	sbi 0x14,0
	.stabn	68,0,169,.LM36-.LFBB6
.LM36:
	sts i,__zero_reg__
	.stabn	68,0,170,.LM37-.LFBB6
.LM37:
	ldi r18,lo8(1)
	.stabs	"/usr/avr/include/util/delay_basic.h",132,0,0,.Ltext1
.Ltext1:
	.stabn	68,0,105,.LM38-.LFBB6
.LM38:
	ldi r20,0
	ldi r21,0
.L20:
	.stabs	"ATmegaBOOT.c",132,0,0,.Ltext2
.Ltext2:
	.stabn	68,0,169,.LM39-.LFBB6
.LM39:
	lds r24,i
	cpi r24,lo8(16)
	brsh .L22
	.stabn	68,0,170,.LM40-.LFBB6
.LM40:
	in r24,0x15
	eor r24,r18
	out 0x15,r24
	.stabs	"/usr/avr/include/util/delay_basic.h",132,0,0,.Ltext3
.Ltext3:
	.stabn	68,0,105,.LM41-.LFBB6
.LM41:
	movw r24,r20
/* #APP */
 ;  105 "/usr/avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
	.stabs	"ATmegaBOOT.c",132,0,0,.Ltext4
.Ltext4:
	.stabn	68,0,169,.LM42-.LFBB6
.LM42:
/* #NOAPP */
	lds r24,i
	subi r24,lo8(-(1))
	sts i,r24
	rjmp .L20
.L23:
	.stabn	68,0,204,.LM43-.LFBB6
.LM43:
	cpi r24,lo8(49)
	breq .L68
	.stabn	68,0,219,.LM44-.LFBB6
.LM44:
	cpi r24,lo8(64)
	breq .L69
	.stabn	68,0,226,.LM45-.LFBB6
.LM45:
	cpi r24,lo8(65)
	brne .L29
	.stabn	68,0,227,.LM46-.LFBB6
.LM46:
	rcall getch
	.stabn	68,0,228,.LM47-.LFBB6
.LM47:
	cpi r24,lo8(-128)
	brne .L30
	.stabn	68,0,228,.LM48-.LFBB6
.LM48:
	ldi r24,lo8(2)
.L64:
	.stabn	68,0,229,.LM49-.LFBB6
.LM49:
	rcall byte_response
.L22:
	.stabn	68,0,192,.LM50-.LFBB6
.LM50:
	rcall getch
	.stabn	68,0,197,.LM51-.LFBB6
.LM51:
	cpi r24,lo8(48)
	brne .L23
	rjmp .L62
.L68:
	.stabn	68,0,205,.LM52-.LFBB6
.LM52:
	rcall getch
	cpi r24,lo8(32)
	brne .L22
	.stabn	68,0,206,.LM53-.LFBB6
.LM53:
	ldi r24,lo8(20)
	rcall putch
	.stabn	68,0,207,.LM54-.LFBB6
.LM54:
	ldi r24,lo8(65)
	rcall putch
	.stabn	68,0,208,.LM55-.LFBB6
.LM55:
	ldi r24,lo8(86)
	rcall putch
	.stabn	68,0,209,.LM56-.LFBB6
.LM56:
	ldi r24,lo8(82)
	rcall putch
	.stabn	68,0,210,.LM57-.LFBB6
.LM57:
	ldi r24,lo8(32)
	rcall putch
	.stabn	68,0,211,.LM58-.LFBB6
.LM58:
	ldi r24,lo8(73)
	rcall putch
	.stabn	68,0,212,.LM59-.LFBB6
.LM59:
	ldi r24,lo8(83)
	rcall putch
	.stabn	68,0,213,.LM60-.LFBB6
.LM60:
	ldi r24,lo8(80)
	rjmp .L63
.L69:
	.stabn	68,0,220,.LM61-.LFBB6
.LM61:
	rcall getch
	.stabn	68,0,221,.LM62-.LFBB6
.LM62:
	cpi r24,lo8(-122)
	brlo .L62
	.stabn	68,0,221,.LM63-.LFBB6
.LM63:
	rcall getch
	rjmp .L62
.L30:
	.stabn	68,0,229,.LM64-.LFBB6
.LM64:
	cpi r24,lo8(-127)
	brne .L31
	.stabn	68,0,229,.LM65-.LFBB6
.LM65:
	ldi r24,lo8(1)
	rjmp .L64
.L31:
	.stabn	68,0,230,.LM66-.LFBB6
.LM66:
	cpi r24,lo8(-126)
	brne .L66
	.stabn	68,0,230,.LM67-.LFBB6
.LM67:
	ldi r24,lo8(18)
	rjmp .L64
.L29:
	.stabn	68,0,236,.LM68-.LFBB6
.LM68:
	cpi r24,lo8(66)
	brne .L33
	.stabn	68,0,237,.LM69-.LFBB6
.LM69:
	ldi r24,lo8(20)
.L65:
	rcall getNch
.L62:
	.stabn	68,0,238,.LM70-.LFBB6
.LM70:
	rcall nothing_response
	rjmp .L22
.L33:
	.stabn	68,0,242,.LM71-.LFBB6
.LM71:
	cpi r24,lo8(69)
	brne .L34
	.stabn	68,0,243,.LM72-.LFBB6
.LM72:
	ldi r24,lo8(5)
	rjmp .L65
.L34:
	.stabn	68,0,248,.LM73-.LFBB6
.LM73:
	ldi r25,lo8(-80)
	add r25,r24
	cpi r25,lo8(3)
	brlo .L62
	.stabn	68,0,272,.LM74-.LFBB6
.LM74:
	cpi r24,lo8(85)
	brne .L35
	.stabn	68,0,273,.LM75-.LFBB6
.LM75:
	rcall getch
	sts address,r24
	.stabn	68,0,274,.LM76-.LFBB6
.LM76:
	rcall getch
	sts address+1,r24
	rjmp .L62
.L35:
	.stabn	68,0,279,.LM77-.LFBB6
.LM77:
	cpi r24,lo8(86)
	brne .L36
	.stabn	68,0,280,.LM78-.LFBB6
.LM78:
	ldi r24,lo8(4)
	rcall getNch
.L66:
	.stabn	68,0,281,.LM79-.LFBB6
.LM79:
	ldi r24,0
	rjmp .L64
.L36:
	.stabn	68,0,285,.LM80-.LFBB6
.LM80:
	cpi r24,lo8(100)
	breq .+2
	rjmp .L37
	.stabn	68,0,286,.LM81-.LFBB6
.LM81:
	rcall getch
	sts length+1,r24
	.stabn	68,0,287,.LM82-.LFBB6
.LM82:
	rcall getch
	sts length,r24
	.stabn	68,0,288,.LM83-.LFBB6
.LM83:
	lds r24,flags
	andi r24,lo8(~(1<<0))
	sts flags,r24
	.stabn	68,0,289,.LM84-.LFBB6
.LM84:
	rcall getch
	cpi r24,lo8(69)
	brne .L38
	.stabn	68,0,289,.LM85-.LFBB6
.LM85:
	lds r24,flags
	ori r24,lo8(1<<0)
	sts flags,r24
.L38:
	.stabn	68,0,129,.LM86-.LFBB6
.LM86:
	ldi r16,lo8(buff)
	ldi r17,hi8(buff)
.L39:
	.stabn	68,0,290,.LM87-.LFBB6
.LM87:
	lds r18,length
	lds r19,length+1
	movw r24,r16
	subi r24,lo8(buff)
	sbci r25,hi8(buff)
	cp r24,r18
	cpc r25,r19
	brsh .L70
	.stabn	68,0,291,.LM88-.LFBB6
.LM88:
	rcall getch
	movw r30,r16
	st Z+,r24
	movw r16,r30
	rjmp .L39
.L70:
	.stabn	68,0,293,.LM89-.LFBB6
.LM89:
	rcall getch
	cpi r24,lo8(32)
	breq .+2
	rjmp .L22
	.stabn	68,0,294,.LM90-.LFBB6
.LM90:
	lds r24,flags
	sbrs r24,0
	rjmp .L71
	ldi r16,lo8(buff)
	ldi r17,hi8(buff)
.L41:
	.stabn	68,0,295,.LM91-.LFBB6
.LM91:
	lds r18,length
	lds r19,length+1
	movw r24,r16
	subi r24,lo8(buff)
	sbci r25,hi8(buff)
	cp r24,r18
	cpc r25,r19
	brlo .+2
	rjmp .L44
	.stabn	68,0,296,.LM92-.LFBB6
.LM92:
	movw r30,r16
	ld r22,Z+
	movw r16,r30
	lds r24,address
	lds r25,address+1
	rcall __eewr_byte_m8
	.stabn	68,0,297,.LM93-.LFBB6
.LM93:
	lds r24,address
	lds r25,address+1
	adiw r24,1
	sts address+1,r25
	sts address,r24
	rjmp .L41
.L71:
	.stabn	68,0,305,.LM94-.LFBB6
.LM94:
/* #APP */
 ;  305 "ATmegaBOOT.c" 1
	cli
 ;  0 "" 2
/* #NOAPP */
.L45:
	.stabn	68,0,306,.LM95-.LFBB6
.LM95:
	sbic 0x1c,1
	rjmp .L45
	.stabn	68,0,307,.LM96-.LFBB6
.LM96:
/* #APP */
 ;  307 "ATmegaBOOT.c" 1
	clr	r17		
	lds	r30,address	
	lds	r31,address+1	
	lsl r30				
	rol r31				
	ldi	r28,lo8(buff)	
	ldi	r29,hi8(buff)	
	lds	r24,length	
	lds	r25,length+1	
	sbrs r24,0		
	rjmp length_loop		
	adiw r24,1		
	length_loop:		
	cpi	r17,0x00	
	brne	no_page_erase	
	rcall  wait_spm		
	ldi	r16,0x03	
	sts	87,r16		
	spm			
	rcall  wait_spm		
	ldi	r16,0x11	
	sts	87,r16		
	spm			
	no_page_erase:		
	ld	r0,Y+		
	ld	r1,Y+		
	rcall  wait_spm		
	ldi	r16,0x01	
	sts	87,r16		
	spm			
	inc	r17		
	cpi r17,32	        
	brlo	same_page	
	write_page:		
	clr	r17		
	rcall  wait_spm		
	ldi	r16,0x05	
	sts	87,r16		
	spm			
	rcall  wait_spm		
	ldi	r16,0x11	
	sts	87,r16		
	spm			
	same_page:		
	adiw	r30,2		
	sbiw	r24,2		
	breq	final_write	
	rjmp	length_loop	
	wait_spm:  
	lds	r16,87		
	andi	r16,1           
	cpi	r16,1           
	breq	wait_spm       
	ret			
	final_write:		
	cpi	r17,0		
	breq	block_done	
	adiw	r24,2		
	rjmp	write_page	
	block_done:		
	clr	__zero_reg__	
	
 ;  0 "" 2
/* #NOAPP */
.L44:
	.stabn	68,0,403,.LM97-.LFBB6
.LM97:
	ldi r24,lo8(20)
	rjmp .L63
.L37:
	.stabn	68,0,409,.LM98-.LFBB6
.LM98:
	cpi r24,lo8(116)
	breq .+2
	rjmp .L46
	.stabn	68,0,410,.LM99-.LFBB6
.LM99:
	rcall getch
	sts length+1,r24
	.stabn	68,0,411,.LM100-.LFBB6
.LM100:
	rcall getch
	sts length,r24
	.stabn	68,0,412,.LM101-.LFBB6
.LM101:
	rcall getch
	cpi r24,lo8(69)
	brne .L47
	.stabn	68,0,412,.LM102-.LFBB6
.LM102:
	lds r24,flags
	ori r24,lo8(1<<0)
	sts flags,r24
	rjmp .L48
.L47:
	.stabn	68,0,414,.LM103-.LFBB6
.LM103:
	lds r24,flags
	andi r24,lo8(~(1<<0))
	sts flags,r24
	.stabn	68,0,415,.LM104-.LFBB6
.LM104:
	lds r24,address
	lds r25,address+1
	lsl r24
	rol r25
	sts address+1,r25
	sts address,r24
.L48:
	.stabn	68,0,417,.LM105-.LFBB6
.LM105:
	rcall getch
	cpi r24,lo8(32)
	breq .+2
	rjmp .L22
	.stabn	68,0,418,.LM106-.LFBB6
.LM106:
	ldi r24,lo8(20)
	rcall putch
	.stabn	68,0,419,.LM107-.LFBB6
.LM107:
	ldi r16,0
	ldi r17,0
.L49:
	.stabn	68,0,419,.LM108-.LFBB6
.LM108:
	lds r24,length
	lds r25,length+1
	cp r16,r24
	cpc r17,r25
	brsh .L61
	.stabn	68,0,420,.LM109-.LFBB6
.LM109:
	lds r24,flags
	sbrs r24,0
	rjmp .L50
	.stabn	68,0,421,.LM110-.LFBB6
.LM110:
	lds r24,address
	lds r25,address+1
	rcall __eerd_byte_m8
	rjmp .L67
.L50:
	.stabn	68,0,424,.LM111-.LFBB6
.LM111:
	sbrc r24,1
	rjmp .L52
	.stabn	68,0,424,.LM112-.LFBB6
.LM112:
	lds r30,address
	lds r31,address+1
/* #APP */
 ;  424 "ATmegaBOOT.c" 1
	lpm r24, Z
	
 ;  0 "" 2
/* #NOAPP */
.L67:
	rcall putch
.L52:
	.stabn	68,0,425,.LM113-.LFBB6
.LM113:
	lds r24,address
	lds r25,address+1
	adiw r24,1
	sts address+1,r25
	sts address,r24
	.stabn	68,0,419,.LM114-.LFBB6
.LM114:
	subi r16,-1
	sbci r17,-1
	rjmp .L49
.L46:
	.stabn	68,0,433,.LM115-.LFBB6
.LM115:
	cpi r24,lo8(117)
	brne .L54
	.stabn	68,0,434,.LM116-.LFBB6
.LM116:
	rcall getch
	cpi r24,lo8(32)
	breq .+2
	rjmp .L22
	.stabn	68,0,435,.LM117-.LFBB6
.LM117:
	ldi r24,lo8(20)
	rcall putch
	.stabn	68,0,436,.LM118-.LFBB6
.LM118:
	ldi r24,lo8(30)
	rcall putch
	.stabn	68,0,437,.LM119-.LFBB6
.LM119:
	ldi r24,lo8(-109)
	rcall putch
	.stabn	68,0,438,.LM120-.LFBB6
.LM120:
	ldi r24,lo8(7)
.L63:
	rcall putch
.L61:
	.stabn	68,0,439,.LM121-.LFBB6
.LM121:
	ldi r24,lo8(16)
	rcall putch
	rjmp .L22
.L54:
	.stabn	68,0,444,.LM122-.LFBB6
.LM122:
	cpi r24,lo8(118)
	breq .+2
	rjmp .L22
	rjmp .L66
	.size	main, .-main
	.stabs	"ch:r(3,2)",64,0,130,24
	.stabs	"w:r(3,4)",64,0,131,16
	.stabn	192,0,0,.LFBB6-.LFBB6
	.stabn	224,0,0,.Lscope6-.LFBB6
.Lscope6:
	.stabs	"",36,0,0,.Lscope6-.LFBB6
	.stabd	78,0,0
.global	app_start
	.section .bss
	.type	app_start, @object
	.size	app_start, 2
app_start:
	.zero	2
	.comm	i,1,1
.global	pagesz
	.data
	.type	pagesz, @object
	.size	pagesz, 1
pagesz:
	.byte	-128
	.comm	buff,256,1
	.comm	flags,1,1
	.comm	length,2,1
	.comm	address,2,1
	.stabs	"address:G(0,48)",32,0,105,0
	.stabs	"length:G(0,51)",32,0,110,0
	.stabs	"flags:G(0,52)",32,0,115,0
	.stabs	"buff:G(0,53)=ar(0,50);0;255;(3,2)",32,0,117,0
	.stabs	"pagesz:G(3,2)",32,0,120,0
	.stabs	"i:G(3,2)",32,0,122,0
	.stabs	"app_start:G(0,54)=*(0,55)=f(0,47)",32,0,126,0
	.text
	.stabs	"",100,0,0,.Letext0
.Letext0:
	.ident	"GCC: (GNU) 4.9.0"
.global __do_copy_data
.global __do_clear_bss
