	push	sp
	push	1
	add
	pop	sp
	jmp	L501
L500:
	push	sp
	push	0
	add
	pop	sp
	push	fp[-4]
	push	1
	compLT
	j0	L000
	push	1
	ret
L000:
	push	fp[-4]
	push	fp[-4]
	push	1
	sub
	call	L500, 1
	mul
	ret
L501:
	push	"Please enter a +ve int <= 12: "
	puts_
	geti
	pop	sb[0]
	push	sb[0]
	push	12
	compLE
	j0	L001
	push	sb[0]
	call	L500, 1
	puti
	jmp	L002
L001:
	push	sb[0]
	puti_
	push	" > 12!!"
	puts
L002:
	jmp	L999
L998:
	push	999999
	puti
L999:
