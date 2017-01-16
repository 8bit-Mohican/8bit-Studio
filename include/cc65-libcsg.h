/*****************************************************************************/
/*                                CSG Library                                */
/*****************************************************************************/

// Credits: Adaptation from Net3DBool C# Library
// 			Tolerancing removed, as we are using fixed point...

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
		x = (uY*vZ - uZ*vY)/256;
		y = (uZ*vX - uX*vZ)/256;
		z = (uX*vY - uY*vX)/256;
		WriteFix8(norms,i*3+0, x);
		WriteFix8(norms,i*3+1, y);
		WriteFix8(norms,i*3+2, z);
		
		// Normalize (TODO: Buggy)
		//norm = sqrt(x*x + y*y + z*z);
		//WriteFix8(norms,i*3+0, (x*256)/norm);
		//WriteFix8(norms,i*3+1, (y*256)/norm);
		//WriteFix8(norms,i*3+2, (z*256)/norm);
	}
}

static void GetMeshBounds(int nVerts, fix8 **verts, fix8 *bounds)
{
	int i;
	
	// Initialize Bounds
	bounds[0] = ReadFix8(verts,i*6+3);
	bounds[1] = ReadFix8(verts,i*6+3);
	bounds[2] = ReadFix8(verts,i*6+4);
	bounds[3] = ReadFix8(verts,i*6+4);
	bounds[4] = ReadFix8(verts,i*6+5);
	bounds[5] = ReadFix8(verts,i*6+5);
	
	// Go through all Vertices
	for (i = 0; i < nVerts; ++i) {
		if (ReadFix8(verts,i*6+3) < bounds[0]) { bounds[0] = ReadFix8(verts,i*6+3); }
		if (ReadFix8(verts,i*6+3) > bounds[1]) { bounds[1] = ReadFix8(verts,i*6+3); }
		if (ReadFix8(verts,i*6+4) < bounds[2]) { bounds[2] = ReadFix8(verts,i*6+4); }
		if (ReadFix8(verts,i*6+4) > bounds[3]) { bounds[3] = ReadFix8(verts,i*6+4); }
		if (ReadFix8(verts,i*6+5) < bounds[4]) { bounds[4] = ReadFix8(verts,i*6+5); }
		if (ReadFix8(verts,i*6+5) > bounds[5]) { bounds[5] = ReadFix8(verts,i*6+5); }
	}
}

static void GetFaceBounds(int *indxs, fix8 **verts, fix8 *bounds)
{
	int i;
	
	// Initialize Bounds
	bounds[0] = ReadFix8(verts,indxs[0]*6+3);
	bounds[1] = ReadFix8(verts,indxs[0]*6+3);
	bounds[2] = ReadFix8(verts,indxs[0]*6+4);
	bounds[3] = ReadFix8(verts,indxs[0]*6+4);
	bounds[4] = ReadFix8(verts,indxs[0]*6+5);
	bounds[5] = ReadFix8(verts,indxs[0]*6+5);
	
	// Go through other Vertices
	for (i = 1; i < 3; ++i) {
		if (ReadFix8(verts,indxs[i]*6+3) < bounds[0]) { bounds[0] = ReadFix8(verts,indxs[i]*6+3); }
		if (ReadFix8(verts,indxs[i]*6+3) > bounds[1]) { bounds[1] = ReadFix8(verts,indxs[i]*6+3); }
		if (ReadFix8(verts,indxs[i]*6+4) < bounds[2]) { bounds[2] = ReadFix8(verts,indxs[i]*6+4); }
		if (ReadFix8(verts,indxs[i]*6+4) > bounds[3]) { bounds[3] = ReadFix8(verts,indxs[i]*6+4); }
		if (ReadFix8(verts,indxs[i]*6+5) < bounds[4]) { bounds[4] = ReadFix8(verts,indxs[i]*6+5); }
		if (ReadFix8(verts,indxs[i]*6+5) > bounds[5]) { bounds[5] = ReadFix8(verts,indxs[i]*6+5); }
	}	
}

static bool CheckBoundsOverlap(fix8 *bounds1, fix8 *bounds2)
{
	if ((bounds1[0] > bounds2[1]) || (bounds1[1] < bounds2[0]) || 
	    (bounds1[2] > bounds2[3]) || (bounds1[3] < bounds2[2]) || 
		(bounds1[4] > bounds2[5]) || (bounds1[5] < bounds2[4])) {
		return false;
	} else {
		return true;
	}
}

static fix8 DistanceToPlane(int indx1, fix8 **verts1, int indx2, fix8 **verts2, int tri2, fix8 **norms2)
{
	fix8 x1,y1,z1,x2,y2,z2;
	fix8 a,b,c,d;
	x1 = ReadFix8(verts1,indx1*6+3);
	y1 = ReadFix8(verts1,indx1*6+4);
	z1 = ReadFix8(verts1,indx1*6+5);
	x2 = ReadFix8(verts2,indx2*6+3);
	y2 = ReadFix8(verts2,indx2*6+4);
	z2 = ReadFix8(verts2,indx2*6+5);
	a = ReadFix8(norms2,tri2*3+0);
	b = ReadFix8(norms2,tri2*3+1);
	c = ReadFix8(norms2,tri2*3+2);
	d = -(a*x2 + b*y2 + c*z2);
	return (a*x1 + b*y1 + c*z1 + d)/256;
}

static void IntersectFaces(int indx1, fix8 **verts1, int tri1, fix8 **norms1, int indx2, fix8 **verts2, int tri2, fix8 **norms2, fix8 *pt, fix8 *dir) 
{
	fix8 x1,y1,z1,x2,y2,z2,d1,d2;
	fix8 uX,uY,uZ,vX,vY,vZ,norm;
	
	// Get normals
	uX = ReadFix8(norms1,tri1*3+0);
	uY = ReadFix8(norms1,tri1*3+1);
	uZ = ReadFix8(norms1,tri1*3+2);
	vX = ReadFix8(norms2,tri2*3+0);
	vY = ReadFix8(norms2,tri2*3+1);
	vZ = ReadFix8(norms2,tri2*3+2);
	
	// Compute direction from cross product
	dir[0] = (uY*vZ - uZ*vY)/256;
	dir[1] = (uZ*vX - uX*vZ)/256;
	dir[2] = (uX*vY - uY*vX)/256;
	norm = sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]);

	//if direction length is not zero (the planes aren't parallel )...
	if (norm > 0) {
		// Get vertices
		x1 = ReadFix8(verts1,indx1*6+3);
		y1 = ReadFix8(verts1,indx1*6+4);
		z1 = ReadFix8(verts1,indx1*6+5);
		x2 = ReadFix8(verts2,indx2*6+3);
		y2 = ReadFix8(verts2,indx2*6+4);
		z2 = ReadFix8(verts2,indx2*6+5);
	
		// Getting a line point, zero is set to a coordinate whose direction component isn't zero (line intersecting its origin plan)
		d1 = -(uX * x1 + uY * y1 + uZ * z1);
		d2 = -(vX * x2 + vY * y2 + vZ * z2);
		if (dir[0] != 0) {
			pt[0] = 0;
			pt[1] = (d2 * uZ - d1 * vZ) / dir[0];
			pt[2] = (d1 * vY - d2 * uY) / dir[0];
		} else if (dir[1] != 0) {
			pt[0] = (d1 * vZ - d2 * uZ) / dir[1];
			pt[1] = 0;
			pt[2] = (d2 * uX - d1 * vX) / dir[1];
		} else {
			pt[0] = (d2 * uY - d1 * vY) / dir[2];
			pt[1] = (d1 * vX - d2 * uX) / dir[2];
			pt[2] = 0;
		}
	}
	
	// Normalize
	dir[0] = (dir[0]*256/norm);
	dir[1] = (dir[1]*256/norm);
	dir[2] = (dir[2]*256/norm);
}

static void SplitFaces(int nTris1, int nVerts1, int **tris1, fix8 **norms1, fix8 **verts1, int nTris2, int nVerts2, int **tris2, fix8 **norms2, fix8 **verts2)
{
	fix8 mBounds1[6], mBounds2[6];
	fix8 fBounds1[6], fBounds2[6];
	fix8 distF1V1toF2, distF1V2toF2, distF1V3toF2;
	fix8 distF2V1toF1, distF2V2toF1, distF2V3toF1;
	int indxs1[3], indxs2[3];
	fix8 lPt[3], lDir[3];
	unsigned long sec;
    unsigned sec10;
	clock_t time;	
	int i,j;
	
	// Check is bounds overlap
	time = clock();
	GetMeshBounds(nVerts1, verts1, &mBounds1[0]);
	GetMeshBounds(nVerts2, verts2, &mBounds2[0]);
	//printf ("Mesh Bounds 1: %d,%d,%d,%d,%d,%d\n", mBounds1[0]/256, mBounds1[1]/256, mBounds1[2]/256, mBounds1[3]/256, mBounds1[4]/256, mBounds1[5]/256);
	//printf ("Mesh Bounds 2: %d,%d,%d,%d,%d,%d\n", mBounds2[0]/256, mBounds2[1]/256, mBounds2[2]/256, mBounds2[3]/256, mBounds2[4]/256, mBounds2[5]/256);
	if (!CheckBoundsOverlap(&mBounds1[0], &mBounds2[0])) { 
		printf ("Meshes don't overlap!\n");
		return; 
	}
	
	// Process each face of Mesh 1
	for (i = 0; i < nTris1; ++i) {
		
		// Get face indices
		indxs1[0] = ReadInt(tris1,i*3+0);
		indxs1[1] = ReadInt(tris1,i*3+1);
		indxs1[2] = ReadInt(tris1,i*3+2);
		
		// Get face bounds
		GetFaceBounds(&indxs1[0], verts1, &fBounds1[0]);
		//printf ("Face Bounds 1: %d,%d,%d,%d,%d,%d\n", fBounds1[0]/256, fBounds1[1]/256, fBounds1[2]/256, fBounds1[3]/256, fBounds1[4]/256, fBounds1[5]/256);
		
		// Check if this face overlap Mesh 2 bounds?
		if (CheckBoundsOverlap(&fBounds1[0], &mBounds2[0])) {
			
			// Process each face of Mesh 2
			for (j = 0; j < nTris2; ++j) {
				// Get vertices indices
				indxs2[0] = ReadInt(tris2,j*3+0);
				indxs2[1] = ReadInt(tris2,j*3+1);
				indxs2[2] = ReadInt(tris2,j*3+2);
				
				// Get face bounds
				GetFaceBounds(&indxs2[0], verts2, &fBounds2[0]);	
				//printf ("Face Bounds 2: %d,%d,%d,%d,%d,%d\n", fBounds2[0]/256, fBounds2[1]/256, fBounds2[2]/256, fBounds2[3]/256, fBounds2[4]/256, fBounds2[5]/256);
				
				// Check if this face overlap Mesh 2 bounds?
				if (CheckBoundsOverlap(&fBounds1[0], &fBounds2[0])) {
					// PART I - DO THE FACES INTERSECT?
					// POSSIBLE RESULTS: INTERSECT, NOT_INTERSECT, COPLANAR					
					
					// Compute distance from face1 vertices to face2 plane
					distF1V1toF2 = DistanceToPlane(indxs1[0], verts1, indxs2[0], verts2, j, norms2);
					distF1V2toF2 = DistanceToPlane(indxs1[1], verts1, indxs2[0], verts2, j, norms2);
					distF1V3toF2 = DistanceToPlane(indxs1[2], verts1, indxs2[0], verts2, j, norms2);		
				
					// If all the signs are zero, the planes are coplanar
					// If all the signs are positive or negative, the planes do not intersect
					// If the signs are not equal...
					if (!(SIGN(distF1V1toF2) == SIGN(distF1V2toF2) && SIGN(distF1V2toF2) == SIGN(distF1V3toF2))) {
						
						// Compute distance from face2 vertices to face1 plane
						distF2V1toF1 = DistanceToPlane(indxs2[0], verts2, indxs1[0], verts1, i, norms1);
						distF2V2toF1 = DistanceToPlane(indxs2[1], verts2, indxs1[0], verts1, i, norms1);
						distF2V3toF1 = DistanceToPlane(indxs2[2], verts2, indxs1[0], verts1, i, norms1);		

						//if the signs are not equal...
						if (!(SIGN(distF2V1toF1) == SIGN(distF2V2toF1) && SIGN(distF2V2toF1) == SIGN(distF2V3toF1))) {

							// Compute intersect line of faces
							IntersectFaces(indxs1[0], verts1, i, norms1, indxs2[1], verts2, j, norms2, &lPt[0], &lDir[0]);
							
							printf ("Intersecting Faces!\n");	
						}
					}
				}
			}			
		}
	}
	time = clock() - time;
	
	// Display processing time
    sec = (time * 10) / CLK_TCK;
    sec10 = sec % 10;
    sec /= 10;	
	printf ("Splitting faces in %lu.%us\n", sec, sec10);
}