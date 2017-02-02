/*****************************************************************************/
/*                                STL Library                                */
/*****************************************************************************/

/* Get little-endian unsigned numbers, using unsigned char* p.
** On many machines this could be (*(unsigned short*) p),
** but I really like portability. :-)  */
#define GET_WORD(p) (*(p) + ((unsigned) (p)[1] << 8))
#define GET_LONG(p) (GET_WORD(p) + ((unsigned long) GET_WORD(p + 2) << 16))

const char* ReadSTL(char *filename, int *nVerts, int *nTris, fix8 **verts, fix8 **norms, int **tris, int **pxls) 
{
	static char name[] = "Logo";
	int v = 0; int n = 0; int t = 0;	
	unsigned char header[80];
	unsigned char buffer[4];
	unsigned int i,j,k;
	fix8 vX, vY, vZ;
	unsigned int vN;
	unsigned long sec;
    unsigned sec10;
	clock_t time;
	FILE* fp;
	
	// Try to open file...
	fp = fopen(filename, "rb");
	if (!fp) {
		printf ("Can't open Mesh...\n");
		return NULL;
	}

	// Read Header
	printf ("\nLoading %s...\n", filename);	
	fread(header, sizeof(header), 1, fp);	
	fread(buffer, 4, 1, fp);
	(*nTris) = GET_WORD(buffer);
	(*nVerts) = 3*GET_WORD(buffer);
	printf ("Header: %s\n", header);	
	
	// Allocate memory for triangles/normals/vertices
	MallocInt(tris, (*nTris)*3);
	MallocFix8(norms, (*nTris)*3);
	MallocFix8(verts, (*nVerts)*6);
	
	// Read data from file
	time = clock();
	for (i = 0; i < (*nTris); ++i) {
		// Normals	
		fread(buffer, 4, 1, fp);
		WriteFix8(norms, n++, Buf2Fix8(buffer));
		fread(buffer, 4, 1, fp);
		WriteFix8(norms, n++, Buf2Fix8(buffer));
		fread(buffer, 4, 1, fp);
		WriteFix8(norms, n++, Buf2Fix8(buffer));

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
			for (k = 0; k < (v/6); ++k) {
				if (ReadFix8(verts, 6*k+0) == vX && 
				    ReadFix8(verts, 6*k+1) == vY && 
					ReadFix8(verts, 6*k+2) == vZ) {
					vN = 6*k;
					break;
				}
			}
			
			// Save new Vertex?
			if (vN == v) {
				WriteFix8(verts, v++, vX);
				WriteFix8(verts, v++, vY);
				WriteFix8(verts, v++, vZ);
				v += 3;
			}
			
			// Record Vertex Index
			WriteInt(tris, t++, vN/6);
		}
		
		// Discard Attribute
		fread(buffer, 2, 1, fp);	
	}
	time = clock() - time;
	
	// Shrink vertices memory
	(*nVerts) = v/6;
	ReallocFix8(verts, (*nVerts)*6);
	
	// Display processing time
    sec = (time * 10) / CLK_TCK;
    sec10 = sec % 10;
    sec /= 10;	
	printf ("%d triangles / %d vertices in %lu.%us\n", (*nTris), (*nVerts), sec, sec10);	

	// Allocate pixel data in main memory (for fast drawing)
	(*pxls) = (int*) malloc ((*nVerts)*2*sizeof(int));
	
	// Close file
	fclose(fp);
	return name;
}