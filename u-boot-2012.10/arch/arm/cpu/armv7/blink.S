/* param:
 * 	r0: blink stimes
 * return:
 * 	none
 */
.global	blink

blink:
	// TODO r1, r2 压栈
	push	{r1, r2, lr}

blink_loop:
	sub	r0, r0, #1	

	ldr	r1, =0xE0200284

	mov	r2, #0
	str	r2, [r1]

	bl	delay

	mov	r2, #0xf
	str	r2, [r1]

	bl	delay

	cmp	r0, #0
	bne	blink_loop

	// TODO r1, r2 出栈
	ldmfd	sp!, {r1, r2, lr}
	mov	pc, lr

