/*****************************************************************************/
/*                                CSG Library                                */
/*****************************************************************************/

#include "cc65-libmemory.h"

static void Transform(fix8 pos[3], fix8 rot[3], fix8 dim[3], int nTris, int nVerts, int **tris, fix8 **norms, fix8 **verts)
{
	int vertices[3];	
	fix8 cosA,sinA,cosB,sinB,cosC,sinC;
	fix8 x,y,z,uX,uY,uZ,vX,vY,vZ,norm;
	int i;
	
	// Compute cos/sin
	cosA = cc65_cos(rot[0]/256); sinA = cc65_sin(rot[0]/256);
	cosB = cc65_cos(rot[1]/256); sinB = cc65_sin(rot[1]/256);
	cosC = cc65_cos(rot[2]/256); sinC = cc65_sin(rot[2]/256);

	// Transform all vertices		
	for (i = 0; i < nVerts; ++i) {
		// Read point from buffer and rescale
		x = (ReadFix8(verts,i*6+0) * dim[0])/256; 
		y = (ReadFix8(verts,i*6+1) * dim[1])/256; 
		z = (ReadFix8(verts,i*6+2) * dim[2])/256; 

		// Write the rotated/translated point
		WriteFix8(verts,i*6+3, (x*cosC*cosB - y*sinC*cosA + y*(cosC*sinB*sinA/256) + z*sinC*sinA + z*(cosC*sinB*cosA/256)) / 65536 + pos[0]);
		WriteFix8(verts,i*6+4, (x*sinC*cosB + y*cosC*cosA + y*(sinC*sinB*sinA/256) - z*cosC*sinA + z*(sinC*sinB*cosA/256)) / 65536 + pos[1]);
		WriteFix8(verts,i*6+5, (-x*sinB*256 + y*cosB*sinA + z*cosB*cosA) / 65536 + pos[2]);
	}
	
	// Recompute normals
	for (i = 0; i < nTris; ++i) {
		// Get vertices indices
		vertices[0] = ReadInt(tris,i*3+0);
		vertices[1] = ReadInt(tris,i*3+1);
		vertices[2] = ReadInt(tris,i*3+2);
		
		// Compute face vectors 
		uX = ReadFix8(verts,vertices[1]*6+3) - ReadFix8(verts,vertices[0]*6+3);
		uY = ReadFix8(verts,vertices[1]*6+4) - ReadFix8(verts,vertices[0]*6+4);
		uZ = ReadFix8(verts,vertices[1]*6+5) - ReadFix8(verts,vertices[0]*6+5);
		vX = ReadFix8(verts,vertices[2]*6+3) - ReadFix8(verts,vertices[0]*6+3);
		vY = ReadFix8(verts,vertices[2]*6+4) - ReadFix8(verts,vertices[0]*6+4);
		vZ = ReadFix8(verts,vertices[2]*6+5) - ReadFix8(verts,vertices[0]*6+5);
		
		// Compute normal from cross product
		x = (uY*vZ - uZ*vY);
		y = (uZ*vX - uX*vZ);
		z = (uX*vY - uY*vX);
		WriteFix8(norms,i*3+0, x/256);
		WriteFix8(norms,i*3+1, y/256);
		WriteFix8(norms,i*3+2, z/256);
		//norm = sqrt(x*x + y*y + z*z);
		//WriteFix8(norms,i*3+0, (x*256)/norm);
		//WriteFix8(norms,i*3+1, (y*256)/norm);
		//WriteFix8(norms,i*3+2, (z*256)/norm);
	}
}
