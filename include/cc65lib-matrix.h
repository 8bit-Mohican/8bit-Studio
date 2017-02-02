/*****************************************************************************/
/*                               Matrix Library                              */
/*****************************************************************************/

#ifndef DEFINE_MATRIX
#define DEFINE_MATRIX

#include "cc65lib-fix8.h"

static void M43multV3(fix8 *mat, fix8 *vec, fix8 *out)
{
	// TODO: convert to ASM for faster computation, as this is frequently called
	out[0] = (vec[0]*mat[0+0] + vec[1]*mat[3+0] + vec[2]*mat[6+0] + 256*mat[9+0]) / 256;
	out[1] = (vec[0]*mat[0+1] + vec[1]*mat[3+1] + vec[2]*mat[6+1] + 256*mat[9+1]) / 256;
	out[2] = (vec[0]*mat[0+2] + vec[1]*mat[3+2] + vec[2]*mat[6+2] + 256*mat[9+2]) / 256;
}

/*
static void M44multV3(fix8 *mat, fix8 *vec, fix8 *out)
{
	// TODO: convert to ASM for faster computation, as this is frequently called
	fix8 w = (vec[0]*mat[0+3] + vec[1]*mat[4+3] + vec[2]*mat[8+3])/256 + mat[12+3];	
	out[0] = (vec[0]*mat[0+0] + vec[1]*mat[4+0] + vec[2]*mat[8+0] + 256*mat[12+0]) / w;
	out[1] = (vec[0]*mat[0+1] + vec[1]*mat[4+1] + vec[2]*mat[8+1] + 256*mat[12+1]) / w;
	out[2] = (vec[0]*mat[0+2] + vec[1]*mat[4+2] + vec[2]*mat[8+2] + 256*mat[12+2]) / w;
}

static void M33multV3(fix8 *mat, fix8 *vec, fix8 *out)
{
	// TODO: convert to ASM for faster computation, as this is frequently called
	out[0] = (vec[0]*mat[0+0] + vec[1]*mat[3+0] + vec[2]*mat[6+0])/256;
	out[1] = (vec[0]*mat[0+1] + vec[1]*mat[3+1] + vec[2]*mat[6+1])/256;
	out[2] = (vec[0]*mat[0+2] + vec[1]*mat[3+2] + vec[2]*mat[6+2])/256;
}
*/

static void V3plusV3(fix8 *v1, fix8 *v2, fix8 *out)
{
	out[0] = v1[0]+v2[0];
	out[1] = v1[1]+v2[1];
	out[2] = v1[2]+v2[2];
}

static fix8 V3dotV3(fix8 *v1, fix8 *v2)
{
	return (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2])/256;
}

#endif
