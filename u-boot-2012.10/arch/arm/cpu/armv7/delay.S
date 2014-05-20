/* param:
 *	none
 * return:
 * 	none
 */
.global delay

delay:
	// TODO r0, lr 压栈
	push	{r0, lr}
	mov	r0, #0x1000000
delay_loop:
	cmp	r0, #0
	sub	r0, r0, #1
	bne	delay_loop
	
	// TODO lr, r0 出栈
	pop	{r0, lr}
	mov	pc, lr
