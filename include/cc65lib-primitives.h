/*****************************************************************************/
/*                            Primitives Library                             */
/*****************************************************************************/

#include "cc65lib-math.h"
#include "cc65lib-matrix.h"
#include "cc65lib-memory.h"

const char* CreateBox(int *nVerts, int *nTris, fix8 **verts, fix8 **norms, int **tris, int **pxls)
{
	static char name[] = "Box";
	static fix8 vList[8][3] = {{-128,-128,-128},{-128, 128,-128},{ 128, 128,-128},{ 128,-128,-128},
							   {-128,-128, 128},{-128, 128, 128},{ 128, 128, 128},{ 128,-128, 128}};
	static fix8 tList[12][3] = {{0,1,2},{0,2,3},{0,5,1},{0,4,5}, 
								{4,6,5},{4,7,6},{3,2,6},{3,6,7}, 
								{3,4,0},{3,7,4},{1,5,2},{2,5,6}};	
	int v = 0;
	int t = 0;
	int i;
		
	// Allocate memory for triangles/normals/vertices
	(*nTris) = 12;
	(*nVerts) = 8;
	MallocInt(tris, (*nTris)*3);
	MallocFix8(norms, (*nTris)*3);
	MallocFix8(verts, (*nVerts)*6);

	// Allocate pixel data in main memory (for fast drawing)
	(*pxls) = (int*) malloc ((*nVerts)*2*sizeof(int));

	// Write faces
	for (i = 0; i < 12; ++i) {
		WriteInt(tris, t++, tList[i][0]);
		WriteInt(tris, t++, tList[i][1]);
		WriteInt(tris, t++, tList[i][2]);
	}
	
	// Write vertices
	for (i = 0; i < 8; ++i) {
		WriteFix8(verts, v++, vList[i][0]);
		WriteFix8(verts, v++, vList[i][1]);
		WriteFix8(verts, v++, vList[i][2]);
		v += 3;
	}
	
	return name;
} 

const char* CreateCone(int faces, int *nVerts, int *nTris, fix8 **verts, fix8 **norms, int **tris, int **pxls)
{
	static char name[] = "Cone";
	int v = 0;
	int t = 0;
	int i;
	
	// Allocate memory for triangles/normals/vertices
	(*nTris) = 2*faces;
	(*nVerts) = (faces+2);
	MallocInt(tris, (*nTris)*3);
	MallocFix8(norms, (*nTris)*3);
	MallocFix8(verts, (*nVerts)*6);
	
	// Allocate pixel data in main memory (for fast drawing)
	(*pxls) = (int*) malloc ((*nVerts)*2*sizeof(int));
	
	// Generate vertices
	WriteFix8(verts, v++, 0);
	WriteFix8(verts, v++, 0);
	WriteFix8(verts, v++, -128);
	v += 3;

	WriteFix8(verts, v++, 0);
	WriteFix8(verts, v++, 0);
	WriteFix8(verts, v++, 128);	
	v += 3;
	
	for (i = 0; i < faces; ++i) {
		// Generate vertices
		WriteFix8(verts, v++, cc65_cos((360*i)/faces)/2);
		WriteFix8(verts, v++, cc65_sin((360*i)/faces)/2);
		WriteFix8(verts, v++, -128);
		v += 3;
		
		// Generate bottom triangle
		WriteInt(tris, t++, i+2);
		WriteInt(tris, t++, 0);
		if (i < (faces-1)) {
			WriteInt(tris, t++, i+3);		
		} else {
			WriteInt(tris, t++, 2);			
		}
		
		// Generate side triangle
		WriteInt(tris, t++, 1);
		WriteInt(tris, t++, i+2);
		if (i < (faces-1)) {
			WriteInt(tris, t++, i+3);		
		} else {
			WriteInt(tris, t++, 2);			
		}	
	}
	
	return name;
}

const char* CreateCylinder(int faces, int *nVerts, int *nTris, fix8 **verts, fix8 **norms, int **tris, int **pxls)
{
	static char name[] = "Cylinder";
	int v = 0;
	int t = 0;
	int i;
	
	// Allocate memory for triangles/normals/vertices
	(*nTris) = 4*faces;
	(*nVerts) = 2*(faces+1);
	MallocInt(tris, (*nTris)*3);
	MallocFix8(norms, (*nTris)*3);
	MallocFix8(verts, (*nVerts)*6);
	
	// Allocate pixel data in main memory (for fast drawing)
	(*pxls) = (int*) malloc ((*nVerts)*2*sizeof(int));
	
	// Generate vertices
	WriteFix8(verts, v++, 0);
	WriteFix8(verts, v++, 0);
	WriteFix8(verts, v++, -128);
	v += 3;

	WriteFix8(verts, v++, 0);
	WriteFix8(verts, v++, 0);
	WriteFix8(verts, v++, 128);	
	v += 3;
	
	for (i = 0; i < faces; ++i) {
		// Generate vertices
		WriteFix8(verts, v++, cc65_cos((360*i)/faces)/2);
		WriteFix8(verts, v++, cc65_sin((360*i)/faces)/2);
		WriteFix8(verts, v++, -128);
		v += 3;

		WriteFix8(verts, v++, cc65_cos((360*i)/faces)/2);
		WriteFix8(verts, v++, cc65_sin((360*i)/faces)/2);
		WriteFix8(verts, v++, 128);
		v += 3;	
		
		// Generate bottom triangle
		WriteInt(tris, t++, 2*i+2);
		WriteInt(tris, t++, 0);
		if (i < (faces-1)) {
			WriteInt(tris, t++, 2*(i+1)+2);		
		} else {
			WriteInt(tris, t++, 2);			
		}
		
		// Generate side triangles
		WriteInt(tris, t++, 2*i+3);
		WriteInt(tris, t++, 2*i+2);
		if (i < (faces-1)) {
			WriteInt(tris, t++, 2*(i+1)+2);		
		} else {
			WriteInt(tris, t++, 2);			
		}		

		WriteInt(tris, t++, 2*i+3);		
		if (i < (faces-1)) {
			WriteInt(tris, t++, 2*(i+1)+2);
			WriteInt(tris, t++, 2*(i+1)+3);
		} else {
			WriteInt(tris, t++, 2);
			WriteInt(tris, t++, 3);
		}		
		
		// Generate top triangle
		WriteInt(tris, t++, 1);
		WriteInt(tris, t++, 2*i+3);
		if (i < (faces-1)) {
			WriteInt(tris, t++, 2*(i+1)+3);		
		} else {
			WriteInt(tris, t++, 3);			
		}		
	}
	
	return name;
}

const char* CreateSphere(int faces, int *nVerts, int *nTris, fix8 **verts, fix8 **norms, int **tris, int **pxls)
{
	static char name[] = "Sphere";
	static fix8 vList[12][3] = {{-67,0,109},{ 67,0,109},{-67,0,-109},{ 67,0,-109},
								{0,109, 67},{0,109,-67},{0,-109, 67},{0,-109,-67},
								{109, 67,0},{-109,67,0},{ 109,-67,0},{-109,-67,0}};
	static fix8 tList[20][3] = {{0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
								{8,10,1},{8,3,10},{5,3,8}, {5,2,3}, {2,7,3},
								{7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
								{6,1,10},{9,0,11},{9,11,2},{9,2,5}, {7,2,11}};
	fix8 norm, vec[3];
	int v = 0;
	int t = 0;
	int i;
	
	// Allocate memory for triangles/normals/vertices
	if (faces <= 20) {
		(*nTris) = 20;
		(*nVerts) = 12;
	} else {
		(*nTris) = 80;
		(*nVerts) = 72;	
	}
	MallocInt(tris, (*nTris)*3);
	MallocFix8(norms, (*nTris)*3);
	MallocFix8(verts, (*nVerts)*6);

	// Allocate pixel data in main memory (for fast drawing)
	(*pxls) = (int*) malloc ((*nVerts)*2*sizeof(int));	
		
	// Write base vertices
	for (i = 0; i < 12; ++i) {
		WriteFix8(verts, v++, vList[i][0]);
		WriteFix8(verts, v++, vList[i][1]);
		WriteFix8(verts, v++, vList[i][2]);
		v += 3;
	}
				
	// 2 resolutions are available
	if (faces <= 20) {
		// Write low-res faces
		for (i = 0; i < 20; ++i) {
			WriteInt(tris, t++, tList[i][0]);
			WriteInt(tris, t++, tList[i][1]);
			WriteInt(tris, t++, tList[i][2]);
		}
	} else {
		// Write high-res faces and extra vertices
		for (i = 0; i < 20; ++i) {
			// Extra vertex 1
			V3plusV3(vList[tList[i][0]], vList[tList[i][1]], &vec[0]);
			norm = NORM3(vec);
			WriteFix8(verts, v++, (128*vec[0])/norm);
			WriteFix8(verts, v++, (128*vec[1])/norm);
			WriteFix8(verts, v++, (128*vec[2])/norm);
			v += 3;

			// Extra vertex 2
			V3plusV3(vList[tList[i][1]], vList[tList[i][2]], &vec[0]);
			norm = NORM3(vec);
			WriteFix8(verts, v++, (128*vec[0])/norm);
			WriteFix8(verts, v++, (128*vec[1])/norm);
			WriteFix8(verts, v++, (128*vec[2])/norm);
			v += 3;

			// Extra vertex 3
			V3plusV3(vList[tList[i][2]], vList[tList[i][0]], &vec[0]);
			norm = NORM3(vec);
			WriteFix8(verts, v++, (128*vec[0])/norm);
			WriteFix8(verts, v++, (128*vec[1])/norm);
			WriteFix8(verts, v++, (128*vec[2])/norm);
			v += 3;
			
			// Face 1
			WriteInt(tris, t++, tList[i][0]);
			WriteInt(tris, t++, 12+(i*3)+0);
			WriteInt(tris, t++, 12+(i*3)+2);
			
			// Face 2
			WriteInt(tris, t++, 12+(i*3)+0);
			WriteInt(tris, t++, tList[i][1]);
			WriteInt(tris, t++, 12+(i*3)+1);

			// Face 3
			WriteInt(tris, t++, 12+(i*3)+2);
			WriteInt(tris, t++, 12+(i*3)+1);
			WriteInt(tris, t++, tList[i][2]);
			
			// Face 4
			WriteInt(tris, t++, 12+(i*3)+0);
			WriteInt(tris, t++, 12+(i*3)+1);
			WriteInt(tris, t++, 12+(i*3)+2);
		}
	}			
	
	return name;
}
