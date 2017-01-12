/*****************************************************************************/
/*                                STL Library                                */
/*****************************************************************************/

#include <stdio.h>
#include "cc65-libfix8.h"

/* Get little-endian unsigned numbers, using unsigned char* p.
** On many machines this could be (*(unsigned short*) p),
** but I really like portability. :-)  */
#define GET_WORD(p) (*(p) + ((unsigned) (p)[1] << 8))
#define GET_LONG(p) (GET_WORD(p) + ((unsigned long) GET_WORD(p + 2) << 16))

static void ReadSTL(char *filename, int *nVerts, int *nTris, fix8 **verts, fix8 **norms, int **tris) 
{
	int v = 0;
	int n = 0;
	int t = 0;	
	FILE* fp;
	unsigned int i;
	unsigned char header[80];
	unsigned char buffer[4];	
	
	// Try to open file...
	fp = fopen(filename, "rb");
	if (!fp) {
		gotoxy (0, 1); cprintf ("Can't open Mesh...");
		return;
	}

	// Read Header
	fread(header, sizeof(header), 1, fp);
	gotoxy (0, 1); cprintf (header);
	
	fread(buffer, 4, 1, fp);
	(*nTris) = GET_WORD(buffer);
	(*nVerts) = 3*GET_WORD(buffer);
	gotoxy (0, 2); cprintf ("Triangles: %d", GET_WORD(buffer));
	
	// Allocate memory //
	(*verts) = (fix8*) malloc ((*nVerts)*3*sizeof(fix8));
	(*norms) = (fix8*) malloc ((*nTris)*3*sizeof(fix8));
	(*tris) = (int*) malloc ((*nTris)*3*sizeof(int));
	
	// Read data from file
	for (i = 0; i < (*nTris); ++i) {
		// Normals	
		fread(buffer, 4, 1, fp);
		(*norms)[n++] = Buf2Fix8(buffer);
		fread(buffer, 4, 1, fp);
		(*norms)[n++] = Buf2Fix8(buffer);
		fread(buffer, 4, 1, fp);
		(*norms)[n++] = Buf2Fix8(buffer);

		// Vertex 1
		fread(buffer, 4, 1, fp);
		(*verts)[v++] = Buf2Fix8(buffer);
		fread(buffer, 4, 1, fp);
		(*verts)[v++] = Buf2Fix8(buffer);
		fread(buffer, 4, 1, fp);
		(*verts)[v++] = Buf2Fix8(buffer);		

		// Vertex 2
		fread(buffer, 4, 1, fp);
		(*verts)[v++] = Buf2Fix8(buffer);
		fread(buffer, 4, 1, fp);
		(*verts)[v++] = Buf2Fix8(buffer);
		fread(buffer, 4, 1, fp);
		(*verts)[v++] = Buf2Fix8(buffer);		

		// Vertex 3
		fread(buffer, 4, 1, fp);
		(*verts)[v++] = Buf2Fix8(buffer);
		fread(buffer, 4, 1, fp);
		(*verts)[v++] = Buf2Fix8(buffer);
		fread(buffer, 4, 1, fp);
		(*verts)[v++] = Buf2Fix8(buffer);		
		
		// Triangles
		(*tris)[t++] = t;
		(*tris)[t++] = t;
		(*tris)[t++] = t;
		
		// Attribute
		fread(buffer, 2, 1, fp);		
	}

	// Close file
	fclose(fp);
}