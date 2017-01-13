/*****************************************************************************/
/*                           Integer Math Library                            */
/*****************************************************************************/

int iSqrt(int n)
{
	 int root = 0, remainder = n, place = 0x4000;
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