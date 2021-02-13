__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__tmp_reg__ = 0
__zero_reg__ = 1

	.text
.global scheduler_frame
	.type scheduler_frame, @function
scheduler_frame:
	;already return address here
	push r1
	push r0
	in r0,__SREG__
	push r0
	clr __zero_reg__
	push r2
	push r3
	push r4
	push r5
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r18
	push r19
	push r20
	push r21
	push r22
	push r23
	push r24
	push r25
	push r26
	push r27
	push r28
	push r29
	push r30
	push r31
	; return address space
	push r30
	push r31
	; saved registers space
	push r28
	push r29
	push r30
	push r31
	; get return address from above
	in r30, __SP_L__
	in r31, __SP_H__
	adiw r30, 33+6+1
	; sbiw r30, 0x08
	ld r28, Z+
	ld r29, Z+

	sbiw r30, 33+2+2
	st Z+, r28
	st Z+, r29
	pop r31
	pop r30
	pop r29
	pop r28

	ret
; LL1:
; 	lds r0, 0xc0
; 	sbrs r0, 6
; 	jmp LL1
; 	sts 0xc6, r29
; 	ld r29, Z+
; 	jmp LL1

.global useless
	.type useless, @function
useless:
	ret
