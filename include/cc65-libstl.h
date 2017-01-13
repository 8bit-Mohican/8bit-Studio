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

static void ReadSTL(char *filename, int *nVerts, int *nTris, fix8 **verts, fix8 **norms, int **tris, int **pxls) 
{
	FILE* fp;
	int v = 0; int n = 0; int t = 0;	
	unsigned char header[80];
	unsigned char buffer[4];
	unsigned int i,j,k;
	fix8 vX, vY, vZ;
	unsigned int vN;
	
	// Try to open file...
	fp = fopen(filename, "rb");
	if (!fp) {
		gotoxy (0, 1); cprintf ("Can't open Mesh...");
		return;
	}

	// Read Header
	fread(header, sizeof(header), 1, fp);	
	fread(buffer, 4, 1, fp);
	(*nTris) = GET_WORD(buffer);
	(*nVerts) = 3*GET_WORD(buffer);
	gotoxy (0, 1); cprintf (header);	
	gotoxy (0, 2); cprintf ("Triangles: %d", GET_WORD(buffer));
	
	// Allocate memory
	(*verts) = (fix8*) malloc ((*nVerts)*3*sizeof(fix8));
	(*norms) = (fix8*) malloc ((*nTris)*3*sizeof(fix8));
	(*pxls) = (int*) malloc ((*nVerts)*2*sizeof(int));
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

		// Read 3 Vertices
		for (j = 0; j < 3; ++j) {
			// Read Buffer
			fread(buffer, 4, 1, fp);
			vX = Buf2Fix8(buffer);
			fread(buffer, 4, 1, fp);
			vY = Buf2Fix8(buffer);
			fread(buffer, 4, 1, fp);
			vZ = Buf2Fix8(buffer);
		
			// Look for pre-existing Vertex
			vN = v;
			for (k = 0; k < (v/3); ++k) {
				if ((*verts)[3*k+0] == vX && (*verts)[3*k+1] == vY && (*verts)[3*k+2] == vZ) {
					vN = 3*k;
					break;
				}
			}
			
			// Save Vertex
			if (vN < v) {
				(*verts)[vN+0] = vX;
				(*verts)[vN+1] = vY;
				(*verts)[vN+2] = vZ;
			} else {
				(*verts)[v++] = vX;
				(*verts)[v++] = vY;
				(*verts)[v++] = vZ;			
			}
			
			// Record Vertex Index
			(*tris)[t++] = vN/3;
		}
		
		// Discard Attribute
		fread(buffer, 2, 1, fp);		
	}
	
	// Shrink Vertices Memory
	(*nVerts) = v/3;
	(*verts) = (fix8*) realloc ((*verts), (*nVerts)*3*sizeof(fix8));

	// Close file
	fclose(fp);
}