.define MAXBITSQRT8 3

.export _sqrt8,sqrt8

sqrt8:
.PROC _sqrt8
	ldy #0
	sta arg
	sty ab

	ldx #MAXBITSQRT8
sqrtlop: tya
	clc
	adc ab
	adc sqrtblo,x
	lsr ab
	cmp arg
	beq @0;dontjump out directly, because ab must be further 
				;lsr'red. 
	bcs dontuse
@0:
	tay
	jsr collectbits
dontuse: 
	dex
	bpl sqrtlop
	
	tya
	clc
	adc ab;this is the mid between a^2 and (a+1)^2
	cmp arg;is arg < middle, then use ab, else ab+1
					;this means, the result is rounded.
	lda ab
	bcs useab
useabplus1:
	adc #1
	ldx #0
	rts
useab:ldx #0
	rts

collectbits:
	lda ab
	ora sqrtblo,x
	sta ab
	rts
.ENDPROC

.BSS
ab: .RES 1
arg: .RES 1

.DATA
sqrtblo: .BYTE 1,4,16,64

