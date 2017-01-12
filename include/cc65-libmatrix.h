/*****************************************************************************/
/*                               Matrix Library                              */
/*****************************************************************************/

#ifndef DEFINE_MATRIX
#define DEFINE_MATRIX

#include "cc65-libfix8.h"

fix8 MVM[3];

static void MatrixVectorMult(fix8 mat[4][4], fix8 src[3])
{
	fix8 a, b, c, w;
	
	a = (src[0] * mat[0][0] + src[1] * mat[1][0] + src[2] * mat[2][0])/256 + mat[3][0];
	b = (src[0] * mat[0][1] + src[1] * mat[1][1] + src[2] * mat[2][1])/256 + mat[3][1];
	c = (src[0] * mat[0][2] + src[1] * mat[1][2] + src[2] * mat[2][2])/256 + mat[3][2];
	w = (src[0] * mat[0][3] + src[1] * mat[1][3] + src[2] * mat[2][3])/256 + mat[3][3];
	
	MVM[0] = 256 * a / w;
	MVM[1] = 256 * b / w;
	MVM[2] = 256 * c / w;
}

static fix8 VectorVectorDot(fix8 v1[3], fix8 v2[3])
{
	return (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2])/256;
}

#endif
