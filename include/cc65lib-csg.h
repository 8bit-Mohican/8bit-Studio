/*****************************************************************************/
/*                                CSG Library                                */
/*****************************************************************************/

// Credits: Adaptation from the Net3DBool C# Library
// 			Tolerancing has been removed, since we are using fixed point...

#include "cc65-libmath.h"

#define BOUNDARY 0
#define EDGE     1
#define VERTEX   2

typedef struct {
	fix8 direction[3];
	fix8 point[3];
} Line;

typedef struct {
	fix8 startPoint[3];
	fix8 endPoint[3];
	fix8 startDist;
	fix8 endDist;
	char type;
	int index;
} Segment;

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

static void IntersectFacesToLine(int indx1, fix8 **verts1, int tri1, fix8 **norms1, int indx2, fix8 **verts2, int tri2, fix8 **norms2, Line *line) 
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
	line->direction[0] = (uY*vZ - uZ*vY)/256;
	line->direction[1] = (uZ*vX - uX*vZ)/256;
	line->direction[2] = (uX*vY - uY*vX)/256;
	norm = NORM3(line->direction);

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
		if (line->direction[0] != 0) {
			line->point[0] = 0;
			line->point[1] = (d2 * uZ - d1 * vZ) / line->direction[0];
			line->point[2] = (d1 * vY - d2 * uY) / line->direction[0];
		} else if (line->direction[1] != 0) {
			line->point[0] = (d1 * vZ - d2 * uZ) / line->direction[1];
			line->point[1] = 0;
			line->point[2] = (d2 * uX - d1 * vX) / line->direction[1];
		} else {
			line->point[0] = (d2 * uY - d1 * vY) / line->direction[2];
			line->point[1] = (d1 * vX - d2 * uX) / line->direction[2];
			line->point[2] = 0;
		}
	}
	
	// Normalize
	line->direction[0] = (line->direction[0]*256/norm);
	line->direction[1] = (line->direction[1]*256/norm);
	line->direction[2] = (line->direction[2]*256/norm);
}

static void DefineLine(int indx1, int indx2, fix8 **verts, Line *line)
{
	// Define new line based on two points
	line->point[0] = ReadFix8(verts,indx1*6+3);
	line->point[1] = ReadFix8(verts,indx1*6+4);
	line->point[2] = ReadFix8(verts,indx1*6+5);
	line->direction[0] = ReadFix8(verts,indx2*6+3) - line->point[0];
	line->direction[1] = ReadFix8(verts,indx2*6+4) - line->point[1];
	line->direction[2] = ReadFix8(verts,indx2*6+5) - line->point[2];
}

static void IntersectLinesToPoint(Line *line1, Line *line2, fix8 *point)
{
	fix8 frac;
	
	// Find the correct case, and compute fraction
	if (abs(line1->direction[1] * line2->direction[0] - line1->direction[0] * line2->direction[1]) > 0) 
	{
		frac = ( -line1->point[1] * line2->direction[0] + line2->point[1] * line2->direction[0] + 
		          line1->point[0] * line2->direction[1] - line2->point[0] * line2->direction[1] ) * 256 / 
					( line1->direction[1] * line2->direction[0] - line1->direction[0] * line2->direction[1] );
	}
	else if (abs(line1->direction[0] * line2->direction[2] - line1->direction[2] * line2->direction[0]) > 0) 
	{
		frac = ( -line1->point[0] * line2->direction[2] + line2->point[0] * line2->direction[2] + 
			      line1->point[2] * line2->direction[0] - line2->point[2] * line2->direction[0] ) * 256 / 
					( line1->direction[0] * line2->direction[2] - line1->direction[2] * line2->direction[0]);
	}
	else if (abs(line1->direction[2] * line2->direction[1] - line1->direction[1] * line2->direction[2]) > 0) 
	{
		frac = ( -line1->point[2] * line2->direction[1] + line2->point[2] * line2->direction[1] + 
			      line1->point[1] * line2->direction[2] - line2->point[1] * line2->direction[2]) * 256 / 
					( line1->direction[2] * line2->direction[1] - line1->direction[1] * line2->direction[2]);
	}
	else {
		return;
	}

	// Return the point on the line
	point[0] = line1->point[0] + line1->direction[0] * frac / 256;
	point[1] = line1->point[1] + line1->direction[1] * frac / 256;
	point[2] = line1->point[2] + line1->direction[2] * frac / 256;
}

static fix8 LineToPointDistance(Line *line, fix8 *point)
{
	fix8 vect[3];
	vect[0] = point[0] - line->point[0];
	vect[1] = point[1] - line->point[1];
	vect[2] = point[2] - line->point[2];
	if (V3dotV3(vect, line->direction) < 0) {
		return -NORM3(vect);
	} else {
		return NORM3(vect);		
	}
}

static fix8 PointToPointDistance(fix8 *p1, fix8 *p2)
{
	fix8 vect[3];
	vect[0] = p2[0] - p1[0];
	vect[1] = p2[1] - p1[1];
	vect[2] = p2[2] - p1[2];
	return NORM3(vect);
}

static void SwapSegmentEnds(Segment *seg)
{
	fix8 tempPoint[3];
	fix8 tempDist;
	
	// Backup start point
	tempPoint[0] = seg->startPoint[0];
	tempPoint[1] = seg->startPoint[1];
	tempPoint[2] = seg->startPoint[2];
	tempDist = seg->startDist;
	
	// Swap start/end points
	seg->startPoint[0] = seg->endPoint[0];
	seg->startPoint[1] = seg->endPoint[1];
	seg->startPoint[2] = seg->endPoint[2];
	seg->startDist = seg->endDist;
	seg->endPoint[0] = tempPoint[0];
	seg->endPoint[1] = tempPoint[1];
	seg->endPoint[2] = tempPoint[2];
	seg->endDist = tempDist;
}

static void SetSegmentVertex(Line *line, int indx, fix8 **verts, Segment *seg)
{
	// First point?
	if (seg->index == 0) {
		// Set as start point		
		seg->startPoint[0] = ReadFix8(verts,indx*6+3);
		seg->startPoint[1] = ReadFix8(verts,indx*6+4);
		seg->startPoint[2] = ReadFix8(verts,indx*6+5);
		seg->startDist = LineToPointDistance(line, seg->startPoint);
		seg->index++;
		
		// Set the segment type
		seg->type = VERTEX;
		return;
	}
	
	// Second point?
	if (seg->index == 1) {
		// Set as end point		
		seg->endPoint[0] = ReadFix8(verts,indx*6+3);
		seg->endPoint[1] = ReadFix8(verts,indx*6+4);
		seg->endPoint[2] = ReadFix8(verts,indx*6+5);
		seg->endDist = LineToPointDistance(line, seg->endPoint);
		seg->index++;
		
		// Set the segment type
		if (PointToPointDistance(&(seg->startPoint[0]), &(seg->endPoint[1])) > 0) {
			seg->type = EDGE;
		}
	}	
	
	// Check distances
	if (seg->startDist > seg->endDist) {
		SwapSegmentEnds(seg);
	}	
}

static void SetSegmentEdge(Line *line, int indx1, int indx2, fix8 **verts, Segment *seg)
{
	Line edgeLine;
	
	// Intersect 2 lines to find point
	DefineLine(indx1, indx2, verts, &edgeLine);
	if (seg->index == 0) {
		// Save as start point
		IntersectLinesToPoint(line, &edgeLine, &(seg->startPoint[0]));
		seg->startDist = LineToPointDistance(line, seg->startPoint);		
		seg->index++;	

		// Set the segment type
		seg->type = VERTEX;
		
	} else if (seg->index == 1) {
		// Set as end point				
		IntersectLinesToPoint(line, &edgeLine, &(seg->endPoint[0]));
		seg->endDist = LineToPointDistance(line, seg->endPoint);
		seg->index++;

		// Set the segment type
		if (PointToPointDistance(&(seg->startPoint[0]), &(seg->endPoint[1])) > 0) {
			seg->type = EDGE;
		}		
	}
	
	// Check distances
	if (seg->startDist > seg->endDist) {
		SwapSegmentEnds(seg);
	}	
}

void IntersectLineAndFaceToSegment(Line *line, int indx[3], fix8 **verts, int sign0, int sign1, int sign2, Segment *seg)
{
	seg->index = 0;
	
	// Vertex 0 is an end?
	if (sign0 == 0) {
		SetSegmentVertex(line, indx[0], verts, seg);
		if (sign1 == sign2) {
			SetSegmentVertex(line, indx[0], verts, seg);
		}
	}
	
	// Vertex 1 is an end?
	if (sign1 == 0) {
		SetSegmentVertex(line, indx[1], verts, seg);
		if (sign2 == sign0) {
			SetSegmentVertex(line, indx[1], verts, seg);
		}
	}

	// Vertex 2 is an end?
	if (sign2 == 0) {
		SetSegmentVertex(line, indx[2], verts, seg);
		if (sign0 == sign1) {
			SetSegmentVertex(line, indx[2], verts, seg);
		}
	}
	
	// Are both ends defined?
	if (seg->index < 2) {
		// Use V0-V1?
		if ((sign0 == 1 && sign1 == -1) || (sign0 == -1 && sign1 == 1)) {
			SetSegmentEdge(line, indx[0], indx[1], verts, seg);			
		}
		// Use V1-V2?
		if ((sign1 == 1 && sign2 == -1) || (sign1 == -1 && sign2 == 1)) {
			SetSegmentEdge(line, indx[1], indx[2], verts, seg);						
		}
		// Use V2-V0?
		if ((sign2 == 1 && sign0 == -1) || (sign2 == -1 && sign0 == 1)) {
			SetSegmentEdge(line, indx[2], indx[0], verts, seg);			
		}		
	}	
}

static void SplitFace(int indx, Segment *seg1, Segment *seg2)
{
	fix8 startPoint[3],endPoint[3];
	char startType,middleType,endType;
	fix8 startDist,endDist;
	
	printf ("Splitting Face!\n");	

//	Vertex startPosVertex, endPosVertex;
//	Point3d startPos, endPos;
//	int startType, endType, middleType;
//	double startDist, endDist;

//	Face face = getFace(facePos);
//	Vertex startVertex = segment1.getStartVertex();
//	Vertex endVertex = segment1.getEndVertex();

	// Deepest start point
	if (seg2->startDist > seg1->startDist) {
		startPoint[0] = seg2->startPoint[0];
		startPoint[1] = seg2->startPoint[1];
		startPoint[2] = seg2->startPoint[2];
		startDist = seg2->startDist;
		startType = seg1->type;
	} else {
		startPoint[0] = seg1->startPoint[0];
		startPoint[1] = seg1->startPoint[1];
		startPoint[2] = seg1->startPoint[2];
		startDist = seg1->startDist;
		startType = VERTEX;
	}

	// Deepest end point
	if (seg2->endDist < seg1->endDist) {
		endPoint[0] = seg2->endPoint[0];
		endPoint[1] = seg2->endPoint[1];
		endPoint[2] = seg2->endPoint[2];
		endDist = seg2->endDist;
		endType = seg1->type;
	} else {
		endPoint[0] = seg1->endPoint[0];
		endPoint[1] = seg1->endPoint[1];
		endPoint[2] = seg1->endPoint[2];
		endDist = seg1->endDist;
		endType = VERTEX;		
	}       
	middleType = seg1->type;

/*
	//set vertex to BOUNDARY if it is start type        
	if (startType == VERTEX) {
		startVertex.setStatus(Vertex.BOUNDARY);
	}

	//set vertex to BOUNDARY if it is end type
	if (endType == Segment.VERTEX)
	{
		endVertex.setStatus(Vertex.BOUNDARY);
	}

	//VERTEX-_______-VERTEX 
	if (startType == Segment.VERTEX && endType == Segment.VERTEX)
	{
		return;
	}

	//______-EDGE-______
	else if (middleType == Segment.EDGE)
	{
		//gets the edge 
		int splitEdge;
		if ((startVertex == face.v1 && endVertex == face.v2) || (startVertex == face.v2 && endVertex == face.v1))
		{
			splitEdge = 1;
		}
		else if ((startVertex == face.v2 && endVertex == face.v3) || (startVertex == face.v3 && endVertex == face.v2))
		{     
			splitEdge = 2; 
		}
		else
		{
			splitEdge = 3;
		} 

		//VERTEX-EDGE-EDGE
		if (startType == Segment.VERTEX)
		{
			breakFaceInTwo(facePos, endPos, splitEdge);
			return;
		}

			//EDGE-EDGE-VERTEX
			else if (endType == Segment.VERTEX)
		{
			breakFaceInTwo(facePos, startPos, splitEdge);
			return;
		}

			// EDGE-EDGE-EDGE
			else if (startDist == endDist)
		{
			breakFaceInTwo(facePos, endPos, splitEdge);
		}
		else
		{
			if ((startVertex == face.v1 && endVertex == face.v2) || (startVertex == face.v2 && endVertex == face.v3) || (startVertex == face.v3 && endVertex == face.v1))
			{
				breakFaceInThree(facePos, startPos, endPos, splitEdge);
			}
			else
			{
				breakFaceInThree(facePos, endPos, startPos, splitEdge);
			}
		}
		return;
	}

	//______-FACE-______

	//VERTEX-FACE-EDGE
	else if (startType == Segment.VERTEX && endType == Segment.EDGE)
	{
		breakFaceInTwo(facePos, endPos, endVertex);
	}
	//EDGE-FACE-VERTEX
	else if (startType == Segment.EDGE && endType == Segment.VERTEX)
	{
		breakFaceInTwo(facePos, startPos, startVertex);
	}
	//VERTEX-FACE-FACE
	else if (startType == Segment.VERTEX && endType == Segment.FACE)
	{
		breakFaceInThree(facePos, endPos, startVertex);
	}
	//FACE-FACE-VERTEX
	else if (startType == Segment.FACE && endType == Segment.VERTEX)
	{
		breakFaceInThree(facePos, startPos, endVertex);
	}
	//EDGE-FACE-EDGE
	else if (startType == Segment.EDGE && endType == Segment.EDGE)
	{
		breakFaceInThree(facePos, startPos, endPos, startVertex, endVertex);
	}
	//EDGE-FACE-FACE
	else if (startType == Segment.EDGE && endType == Segment.FACE)
	{
		breakFaceInFour(facePos, startPos, endPos, startVertex);
	}
	//FACE-FACE-EDGE
	else if (startType == Segment.FACE && endType == Segment.EDGE)
	{
		breakFaceInFour(facePos, endPos, startPos, endVertex);
	}
	//FACE-FACE-FACE
	else if (startType == Segment.FACE && endType == Segment.FACE)
	{
		Vector3d segmentVector = new Vector3d(startPos.x - endPos.x, startPos.y - endPos.y, startPos.z - endPos.z);

		//if the intersection segment is a point only...
		if (Math.Abs(segmentVector.x) < TOL && Math.Abs(segmentVector.y) < TOL && Math.Abs(segmentVector.z) < TOL)
		{
			breakFaceInThree(facePos, startPos);
			return;
		}

		//gets the vertex more lined with the intersection segment
		int linedVertex;
		Point3d linedVertexPos;
		Vector3d vertexVector = new Vector3d(endPos.x - face.v1.x, endPos.y - face.v1.y, endPos.z - face.v1.z);
		vertexVector.normalize();
		double dot1 = Math.Abs(segmentVector.dot(vertexVector));
		vertexVector = new Vector3d(endPos.x - face.v2.x, endPos.y - face.v2.y, endPos.z - face.v2.z);
		vertexVector.normalize();
		double dot2 = Math.Abs(segmentVector.dot(vertexVector));
		vertexVector = new Vector3d(endPos.x - face.v3.x, endPos.y - face.v3.y, endPos.z - face.v3.z);
		vertexVector.normalize();
		double dot3 = Math.Abs(segmentVector.dot(vertexVector));
		if (dot1 > dot2 && dot1 > dot3)
		{
			linedVertex = 1;
			linedVertexPos = face.v1.getPosition();
		}
		else if (dot2 > dot3 && dot2 > dot1)
		{
			linedVertex = 2;
			linedVertexPos = face.v2.getPosition();
		}
		else
		{
			linedVertex = 3;
			linedVertexPos = face.v3.getPosition();
		}

		// Now find which of the intersection endpoints is nearest to that vertex.
		if (linedVertexPos.distance(startPos) > linedVertexPos.distance(endPos))
		{
			breakFaceInFive(facePos, startPos, endPos, linedVertex);
		}
		else
		{
			breakFaceInFive(facePos, endPos, startPos, linedVertex);
		}
	}
*/
}

static void SplitFaces(int nTris1, int nVerts1, int **tris1, fix8 **norms1, fix8 **verts1, int nTris2, int nVerts2, int **tris2, fix8 **norms2, fix8 **verts2)
{
	// Variables used in computation
	static fix8 mBounds1[6], mBounds2[6];
	static fix8 fBounds1[6], fBounds2[6];
	static fix8 distF1V1toF2, distF1V2toF2, distF1V3toF2;
	static fix8 distF2V1toF1, distF2V2toF1, distF2V3toF1;
	static int indxs1[3], indxs2[3];
	Segment seg1, seg2;
	Line line;
	
	// Variables for benchmarking
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

						// If the signs are not equal...
						if (!(SIGN(distF2V1toF1) == SIGN(distF2V2toF1) && SIGN(distF2V2toF1) == SIGN(distF2V3toF1))) {

							// Compute intersect line of faces
							IntersectFacesToLine(indxs1[0], verts1, i, norms1, indxs2[1], verts2, j, norms2, &line);
							
							// Compute intersect segments
							IntersectLineAndFaceToSegment(&line, indxs1, verts1, SIGN(distF1V1toF2), SIGN(distF1V2toF2), SIGN(distF1V3toF2), &seg1);
							IntersectLineAndFaceToSegment(&line, indxs2, verts2, SIGN(distF2V1toF1), SIGN(distF2V2toF1), SIGN(distF2V3toF1), &seg2);
							
							// Do the two segments overlap?
							if (seg1.endDist < seg2.startDist || seg2.endDist < seg1.startDist) {
								continue;
							}
							
							// Subdivide non-planar polygons
							SplitFace(i, &seg1, &seg2);
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
