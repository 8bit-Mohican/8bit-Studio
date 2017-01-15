/*****************************************************************************/
/*                            Primitives Library                             */
/*****************************************************************************/

#include "cc65-libmemory.h"

const char* CreateBox(int *nVerts, int *nTris, fix8 **verts, fix8 **norms, int **tris, int **pxls)
{
	static char name[] = "Box";
	int v = 0;
	int n = 0;
	int t = 0;
		
	// Allocate memory for triangles/normals/vertices
	(*nVerts) = 8;
	(*nTris) = 12;
	AllocateInt(tris, (*nTris)*3);
	AllocateFix8(norms, (*nTris)*3);
	AllocateFix8(verts, (*nVerts)*3);

	// Allocate pixel data in main memory (for fast drawing)
	(*pxls) = (int*) malloc ((*nVerts)*2*sizeof(int));
	
	// Generate vertices
	WriteFix8(verts, v++, -128);
	WriteFix8(verts, v++, -128);
	WriteFix8(verts, v++, -128);

	WriteFix8(verts, v++, -128);
	WriteFix8(verts, v++,  128);
	WriteFix8(verts, v++, -128);

	WriteFix8(verts, v++,  128);
	WriteFix8(verts, v++,  128);
	WriteFix8(verts, v++, -128);

	WriteFix8(verts, v++,  128);
	WriteFix8(verts, v++, -128);
	WriteFix8(verts, v++, -128);

	WriteFix8(verts, v++, -128);
	WriteFix8(verts, v++, -128);
	WriteFix8(verts, v++,  128);

	WriteFix8(verts, v++, -128);
	WriteFix8(verts, v++,  128);
	WriteFix8(verts, v++,  128);

	WriteFix8(verts, v++,  128);
	WriteFix8(verts, v++,  128);
	WriteFix8(verts, v++,  128);

	WriteFix8(verts, v++,  128);
	WriteFix8(verts, v++, -128);
	WriteFix8(verts, v++,  128);
	
	// Generate triangles
	WriteInt(tris, t++, 0);
	WriteInt(tris, t++, 1);
	WriteInt(tris, t++, 2);
	
	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, -256);	
	
	WriteInt(tris, t++, 0);
	WriteInt(tris, t++, 2);
	WriteInt(tris, t++, 3);

	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, -256);	
	
	WriteInt(tris, t++, 0);
	WriteInt(tris, t++, 1);
	WriteInt(tris, t++, 5);

	WriteFix8(norms, n++, -256);
	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, 0);	
	
	WriteInt(tris, t++, 0);
	WriteInt(tris, t++, 5);
	WriteInt(tris, t++, 4);

	WriteFix8(norms, n++, -256);
	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, 0);	

	WriteInt(tris, t++, 4);
	WriteInt(tris, t++, 5);
	WriteInt(tris, t++, 6);

	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, 256);	
	
	WriteInt(tris, t++, 4);
	WriteInt(tris, t++, 6);
	WriteInt(tris, t++, 7);

	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, 256);	
	
	WriteInt(tris, t++, 3);
	WriteInt(tris, t++, 2);
	WriteInt(tris, t++, 6);

	WriteFix8(norms, n++, 256);
	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, 0);	
	
	WriteInt(tris, t++, 3);
	WriteInt(tris, t++, 6);
	WriteInt(tris, t++, 7);
	
	WriteFix8(norms, n++, 256);
	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, 0);	

	WriteInt(tris, t++, 3);
	WriteInt(tris, t++, 0);
	WriteInt(tris, t++, 4);

	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, -256);
	WriteFix8(norms, n++, 0);	
	
	WriteInt(tris, t++, 3);
	WriteInt(tris, t++, 4);
	WriteInt(tris, t++, 7);

	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, -256);
	WriteFix8(norms, n++, 0);	

	WriteInt(tris, t++, 1);
	WriteInt(tris, t++, 2);
	WriteInt(tris, t++, 5);

	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, 256);
	WriteFix8(norms, n++, 0);	
	
	WriteInt(tris, t++, 2);
	WriteInt(tris, t++, 5);
	WriteInt(tris, t++, 6);

	WriteFix8(norms, n++, 0);
	WriteFix8(norms, n++, 256);
	WriteFix8(norms, n++, 0);	
	
	return name;
} 
