/*****************************************************************************/
/*                            Fixed Point Library                            */
/*****************************************************************************/

#ifndef DEFINE_FIX8
#define DEFINE_FIX8

#define Int2Fix8(val) (val*256)
#define Bit8(a,b,c,d,e,f,g,h) (a<<7 | b<<6 | c<<5 | d<<4 | e<<3 | f<<2 | g<<1 | h<<0)
#define GetBit(buffer, index) ((buffer[index/8] >> (7 - index % 8)) & 1)

typedef long fix8;

static unsigned long Pow2(int exp)
{
	unsigned char i;
	unsigned long pow2 = 1;
	for (i = 0; i < exp; ++i) {
		pow2 *=2;
	}
	return pow2;
}

static fix8 Float(int sgn, int exp, unsigned int man)
{
	fix8 val = 256 + man;
	if (exp >= 0) {
		val *= Pow2(abs(exp));
	} else {
		val /= Pow2(abs(exp));
	}
	val *= sgn;
	return val;
}

static fix8 Buf2Fix8(unsigned char* buffer)
{
	static int sgn, exp;
	static unsigned int man;
	sgn = 1-2*(GetBit(buffer,24));
	exp = Bit8(GetBit(buffer,25),GetBit(buffer,26),GetBit(buffer,27),GetBit(buffer,28),
			   GetBit(buffer,29),GetBit(buffer,30),GetBit(buffer,31),GetBit(buffer,16))-127;
	man = Bit8(GetBit(buffer,17),GetBit(buffer,18),GetBit(buffer,19),GetBit(buffer,20),
			   GetBit(buffer,21),GetBit(buffer,22),GetBit(buffer,23),GetBit(buffer,14));
	return Float(sgn,exp,man);
}

#endif
