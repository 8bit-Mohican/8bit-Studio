/*****************************************************************************/
/*                              Rendering Library                            */
/*****************************************************************************/

#include <stdbool.h>

#include "cc65-libdraw.h"
#include "cc65-libmath.h"
#include "cc65-libmatrix.h"
#include "cc65-libmemory.h"

fix8 canvasW = 2*256;
fix8 canvasH = 2*256;

fix8 worldToCamera[4][4] = { { 256,     0,     0,     0},
							 {   0,   256,     0,     0},
							 {   0,     0,   256,     0},
							 {   0,     0, 10240,   256} };						 

fix8 camVec[3] = {0,0,256};

fix8 xCam = 30;
fix8 zCam = 330;
fix8 xCos, zCos;
fix8 xSin, zSin;

static void UpdateCamera() 
{
	xCos = cc65_cos(xCam); xSin = cc65_sin(xCam);
	zCos = cc65_cos(zCam); zSin = cc65_sin(zCam);
	worldToCamera[0][0] = ( zCos        );
	worldToCamera[0][1] = (-zSin *  xCos) / 256;
	worldToCamera[0][2] = (-zSin * -xSin) / 256;
	worldToCamera[1][0] = ( zSin	    );
	worldToCamera[1][1] = ( zCos *  xCos) / 256;
	worldToCamera[1][2] = ( zCos * -xSin) / 256;
	worldToCamera[2][0] = (      0	    );
	worldToCamera[2][1] = ( 	    xSin);
	worldToCamera[2][2] = ( 	    xCos);	
	camVec[0] =  zSin * -xSin / 256;
	camVec[1] = -zCos * -xSin / 256;
	camVec[2] = -xCos;	
}

fix8 canPt[2];
int scrPt[2];

static void ComputePixel(fix8 wldPt[3])
{
	MatrixVectorMult(worldToCamera, wldPt);
    canPt[0] = (256 * MVM[0]) / -MVM[2]; 
    canPt[1] = (256 * MVM[1]) / -MVM[2];

    scrPt[0] = screenW * (canPt[0] + canvasW/2) / canvasW;
    scrPt[1] = screenH - screenH * (canPt[1] + canvasH/2) / canvasH;
}

fix8 wldPt[3];

static void Rasterize(int nVerts, fix8 **verts, int **pxls)
{	
	// Rasterize mesh
	int i,k;
	for (i = 0; i < nVerts; ++i) {
		// Read transformed point
		for (k = 0; k < 3; ++k) {
			wldPt[k] = ReadFix8(verts,i*6+k+3); 
		}

		// Project point to screen
		ComputePixel(wldPt);
		(*pxls)[i*2+0] = scrPt[0]; 
		(*pxls)[i*2+1] = scrPt[1]; 
	}
}

static void Transform(fix8 pos[3], fix8 rot[3], fix8 dim[3], int nVerts, fix8 **verts)
{
	fix8 cosA,sinA,cosB,sinB,cosC,sinC;
	fix8 x,y,z;
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
}

static void RenderMesh(fix8 pos[3], fix8 rot[3], fix8 dim[3], int nTris, int nVerts, int **tris, fix8 **norms, fix8 **verts, int **pxls, char *renderMask) 
{
	fix8 normal[3];
	int vertices[3];	
	int i,x0,y0,x1,y1,x2,y2;
	bool *visible;
	
	// Do Transform?
	if ((*renderMask) & MASK_TRANSFORM) {
		Transform(pos, rot, dim, nVerts, verts);
		(*renderMask) &= ~MASK_TRANSFORM;
	}	

	// Do Rasterize?
	if ((*renderMask) & MASK_RASTERIZE) {
		Rasterize(nVerts, verts, pxls);
		(*renderMask) &= ~MASK_RASTERIZE;
	}

	// Do Draw?
	if ((*renderMask) & MASK_DRAW) {
		// Check normals against camera in first loop
		// (this allows sequential reading of data in REU)
		visible = (bool*) malloc (nTris/8+1);
		for (i = 0; i < nTris; ++i) {
			normal[0] = ReadFix8(norms, i*3+0);
			normal[1] = ReadFix8(norms, i*3+1);
			normal[2] = ReadFix8(norms, i*3+2);
			if (VectorVectorDot(normal,camVec) >= 0) {
				visible[i] = true;
			} else {
				visible[i] = false;			
			}
		}
			
		// Draw visible triangles in second loop
		// (this allows sequential reading of data in REU)
		for (i = 0; i < nTris; ++i) {
			if (visible[i]) {
				// Get vertices indices
				vertices[0] = ReadInt(tris, i*3+0);
				vertices[1] = ReadInt(tris, i*3+1);
				vertices[2] = ReadInt(tris, i*3+2);		
				
				// Get pixel coordinates
				x0 = (*pxls)[vertices[0]*2+0]; 
				y0 = (*pxls)[vertices[0]*2+1];
				x1 = (*pxls)[vertices[1]*2+0]; 
				y1 = (*pxls)[vertices[1]*2+1];
				x2 = (*pxls)[vertices[2]*2+0]; 
				y2 = (*pxls)[vertices[2]*2+1];
				
				// Draw the triangle sides
				DrawLine(x0, y0, x1, y1);
				DrawLine(x1, y1, x2, y2);
				DrawLine(x2, y2, x0, y0);
			}
		}
		
		// Free memory
		(*renderMask) &= ~MASK_DRAW;
		free(visible);
	}
}

fix8 wX[3] = {256*30, 0, 0};
fix8 wY[3] = {0, 256*30, 0};

static void RenderAxes() 
{
	int vX[2], vY[2];
	int nX, nY;

	// Shrink render size
	screenW = 30; screenH = 30;
	
	// Compute vectors
	ComputePixel(wX);
	vX[0] = scrPt[0]-15; vX[1] = scrPt[1]-15;
	ComputePixel(wY);
	vY[0] = scrPt[0]-15; vY[1] = scrPt[1]-15;

	// Compute norm
	nX = sqrt(vX[0]*vX[0]+vX[1]*vX[1]);
	nY = sqrt(vY[0]*vY[0]+vY[1]*vY[1]);
	
	// Reset block
	tgi_setcolor (COLOR_BACK);
	tgi_bar(0, 169, screenW, 169+screenH);
	tgi_setcolor (COLOR_FORE);
	tgi_line(0, 169, 30, 169);
	tgi_line(30, 169, 30, 199);
	
	// Draw axes and labels
	if (nX > 0) {
		DrawLine(14-(15*vX[0])/nX, 183-(15*vX[1])/nX, 14+(7*vX[0])/nX, 183+(7*vX[1])/nX);
		tgi_outtextxy(14+(11*vX[0])/nX-2, 183+(11*vX[1])/nX+2, "x");
	} else { tgi_outtextxy(14, 183, "x"); }
	if (nY > 0) {
		DrawLine(14-(15*vY[0])/nY, 183-(15*vY[1])/nY, 14+(7*vY[0])/nY, 183+(7*vY[1])/nY);
		tgi_outtextxy(14+(11*vY[0])/nY-2, 183+(11*vY[1])/nY+2, "y");
	} else { tgi_outtextxy(14, 183, "y"); }
	
	// Restore render size
	screenW = 220; screenH = 200;
}

static void ResetCanvas()
{
	// Reset canvas
	tgi_setcolor (COLOR_BACK);
	tgi_bar(0, 0, screenW-1, screenH-1);
	tgi_setcolor (COLOR_FORE);	
}
	
