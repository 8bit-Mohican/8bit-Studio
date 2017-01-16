.define MAXBITSQRT16 7

.export _sqrt16,sqrt16

sqrt16:
.PROC _sqrt16
	sta arg
	stx arg+1
	
	ldy #0
	sty sqr
	sty sqr+1
	;sty sqrtemp
	;sty sqrtemp+1

	sty ab
	sty ab+1

	ldx #MAXBITSQRT16
sqrtlop: 
	lda sqr
	clc
	adc ab
	php
	clc
	adc sqrtblo,x
	tay
	lda sqr+1
	adc sqrtbhi,x
	plp
	adc ab+1
	
	lsr ab+1
	ror ab

	cmp arg+1
	bcc use
	beq @1
	bcs dontuse
@1:	cpy arg
	beq use
	bcs dontuse
use:
	sty sqr
	sta sqr+1

	jsr collectbits
dontuse: 
	dex
  bpl sqrtlop
	
	lda ab
	clc
	adc sqr
	ldx sqr+1
	bcc @2
	inx
@2: cpx arg+1
	bcc useabplus1
	beq @3
	bcs useab
@3: cmp arg
	bcs useab
useabplus1: lda ab
	adc #1
	ldx #0
	bcc @4
	inx
@4:	rts
useab:
	lda ab
	
	ldx #0
	rts

collectbits: 
	lda ab
	ora sqrtblo,x
	sta ab
	
	lda ab+1
	ora sqrtbhi,x
	sta ab+1
	rts
.ENDPROC
.BSS

ab: .RES 2
;ab2: .RES 2
sqr: .RES 2
arg: .RES 2
;sqrtemp: .RES 2
;diff: .RES 2
.DATA

sqrtblo: .BYTE 1,4,16,64,<256,<(32*32),<(64*64),<(128*128)
sqrtbhi: .BYTE 0,0,0,0,>256,>(32*32),>(64*64),>(128*128)
.CODE
