/*****************************************************************************/
/*                           Integer Math Library                            */
/*****************************************************************************/

#define SIGN(A) (A > 0 ? 1 : (A < 0 ? -1 : 0))

unsigned char __fastcall__ sqrt8(unsigned char arg);
unsigned char __fastcall__ sqrt16(unsigned int arg);

int sqrt32(long arg)
{
	 long root = 0;
	 long remainder = arg;
	 long place = 0x40000000;
	 while (place > remainder) {
		place = place >> 2; 
	 }
	 while (place) {
		 if (remainder >= root + place) {
			 remainder = remainder - root - place;
			 root = root + (place << 1);
		 }
		 root = root >> 1;
		 place = place >> 2;
	 }
	 return root;
}

int sqrt(long arg)
{
	if (arg < 0x100) {
		return sqrt8(arg);
	} else if (arg < 0x10000) {
		return sqrt16(arg);
	} else {
		return sqrt32(arg);		
	}
}

