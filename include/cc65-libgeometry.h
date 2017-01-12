/*****************************************************************************/
/*                             Geometry Library                              */
/*****************************************************************************/

#include "cc65-libmatrix.h"

static void CreateBox(fix8 dimensions[3], fix8 position[3], int *nVerts, int *nTris, fix8 **verts, fix8 **norms, int **tris)
{
	int v = 0;
	int n = 0;
	int t = 0;
	
	(*nVerts) = 8;
	(*nTris) = 12;
	
	/* Allocate memory */
	(*verts) = (fix8*) malloc ((*nVerts)*3*sizeof(fix8));
	(*norms) = (fix8*) malloc ((*nTris)*3*sizeof(fix8));
	(*tris) = (int*) malloc ((*nTris)*3*sizeof(int));
	
	/* Generate vertices */	
	(*verts)[v++] = -dimensions[0]/2;
	(*verts)[v++] = -dimensions[1]/2;
	(*verts)[v++] = -dimensions[2]/2;
	
	(*verts)[v++] = -dimensions[0]/2;
	(*verts)[v++] =  dimensions[1]/2;
	(*verts)[v++] = -dimensions[2]/2;	

	(*verts)[v++] =  dimensions[0]/2;
	(*verts)[v++] =  dimensions[1]/2;
	(*verts)[v++] = -dimensions[2]/2;	
	
	(*verts)[v++] =  dimensions[0]/2;
	(*verts)[v++] = -dimensions[1]/2;
	(*verts)[v++] = -dimensions[2]/2;
	
	(*verts)[v++] = -dimensions[0]/2;
	(*verts)[v++] = -dimensions[1]/2;
	(*verts)[v++] =  dimensions[2]/2;
	
	(*verts)[v++] = -dimensions[0]/2;
	(*verts)[v++] =  dimensions[1]/2;
	(*verts)[v++] =  dimensions[2]/2;	

	(*verts)[v++] =  dimensions[0]/2;
	(*verts)[v++] =  dimensions[1]/2;
	(*verts)[v++] =  dimensions[2]/2;	
	
	(*verts)[v++] =  dimensions[0]/2;
	(*verts)[v++] = -dimensions[1]/2;
	(*verts)[v++] =  dimensions[2]/2;	
	
	/* Generate triangles */	
	(*tris)[t++] = 0;
	(*tris)[t++] = 1;
	(*tris)[t++] = 2;
	
	(*norms)[n++] = 0;
	(*norms)[n++] = 0;
	(*norms)[n++] = Int2Fix8(-1);	
	
	(*tris)[t++] = 0;
	(*tris)[t++] = 2;
	(*tris)[t++] = 3;		

	(*norms)[n++] = 0;
	(*norms)[n++] = 0;
	(*norms)[n++] = Int2Fix8(-1);	
	
	(*tris)[t++] = 0;
	(*tris)[t++] = 1;
	(*tris)[t++] = 5;	

	(*norms)[n++] = Int2Fix8(-1);
	(*norms)[n++] = 0;
	(*norms)[n++] = 0;
	
	(*tris)[t++] = 0;
	(*tris)[t++] = 5;
	(*tris)[t++] = 4;

	(*norms)[n++] = Int2Fix8(-1);
	(*norms)[n++] = 0;
	(*norms)[n++] = 0;

	(*tris)[t++] = 4;
	(*tris)[t++] = 5;
	(*tris)[t++] = 6;

	(*norms)[n++] = 0;
	(*norms)[n++] = 0;
	(*norms)[n++] = Int2Fix8(1);
	
	(*tris)[t++] = 4;
	(*tris)[t++] = 6;
	(*tris)[t++] = 7;

	(*norms)[n++] = 0;
	(*norms)[n++] = 0;
	(*norms)[n++] = Int2Fix8(1);
	
	(*tris)[t++] = 3;
	(*tris)[t++] = 2;
	(*tris)[t++] = 6;

	(*norms)[n++] = Int2Fix8(1);
	(*norms)[n++] = 0;
	(*norms)[n++] = 0;
	
	(*tris)[t++] = 3;
	(*tris)[t++] = 6;
	(*tris)[t++] = 7;
	
	(*norms)[n++] = Int2Fix8(1);
	(*norms)[n++] = 0;
	(*norms)[n++] = 0;

	(*tris)[t++] = 3;
	(*tris)[t++] = 0;
	(*tris)[t++] = 4;	

	(*norms)[n++] = 0;
	(*norms)[n++] = Int2Fix8(-1);
	(*norms)[n++] = 0;
	
	(*tris)[t++] = 3;
	(*tris)[t++] = 4;
	(*tris)[t++] = 7;

	(*norms)[n++] = 0;
	(*norms)[n++] = Int2Fix8(-1);
	(*norms)[n++] = 0;

	(*tris)[t++] = 1;
	(*tris)[t++] = 2;
	(*tris)[t++] = 5;

	(*norms)[n++] = 0;
	(*norms)[n++] = Int2Fix8(1);
	(*norms)[n++] = 0;
	
	(*tris)[t++] = 2;
	(*tris)[t++] = 5;
	(*tris)[t++] = 6;	

	(*norms)[n++] = 0;
	(*norms)[n++] = Int2Fix8(1);
	(*norms)[n++] = 0;
	
	/* Adjust position */
	for (v = 0; v < (*nVerts); ++v) {
	    (*verts)[v*3+0] += position[0];
	    (*verts)[v*3+1] += position[1];
	    (*verts)[v*3+2] += position[2];
	}
} 
