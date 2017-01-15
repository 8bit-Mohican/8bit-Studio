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
							 {   0,     0,  6400,   256} };						 

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

static void Rasterize(fix8 pos[3], fix8 rot[3], fix8 dim[3], int nVerts, fix8 **verts, int **pxls)
{	
	// Rasterize mesh
	int i,k;
	for (i = 0; i < nVerts; ++i) {
		for (k = 0; k < 3; ++k) {
			// Read point from buffer, and transform
			wldPt[k] = ReadFix8(verts, i*3+k); 
			wldPt[k] = (wldPt[k]*dim[k])/256;
			wldPt[k] += pos[k];
		}

		// Project point to screen
		ComputePixel(wldPt);
		(*pxls)[i*2+0] = scrPt[0]; 
		(*pxls)[i*2+1] = scrPt[1]; 
	}
}

static void RenderMesh(int nTris, int **tris, fix8 **norms, int **pxls) 
{
	fix8 normal[3];
	int vertices[3];	
	int i,x0,y0,x1,y1,x2,y2;
	bool *visible = (bool*) malloc (nTris/8+1);

	// Check normals against camera in first loop
	// (this allows sequential reading of data in REU)
	for (i = 0; i < nTris; ++i) {
		normal[0] = ReadFix8(norms, i*3+0);
		normal[1] = ReadFix8(norms, i*3+1);
		normal[2] = ReadFix8(norms, i*3+2);
		if (VectorVectorDot(normal,camVec) > 0) {
			visible[i] = true;
		} else {
			visible[i] = false;			
		}
	}
		
	// Draw visible triangles in second loop
	// (this allows sequential reading of data in REU)
	for (i = 0; i < nTris; ++i) {
		if (visible[i]) {
			vertices[0] = ReadInt(tris, i*3+0);
			vertices[1] = ReadInt(tris, i*3+1);
			vertices[2] = ReadInt(tris, i*3+2);
			
/*			// Trick for faster rendering? 
			// Only Rasterize vertices which are drawn
			for (k = 0; k < 3; ++k) {
				if ((*pxls)[vertices[k]*2+0] == -1) {
					wldPt[0] = ReadFix8(verts, vertices[k]*3+0); 
					wldPt[1] = ReadFix8(verts, vertices[k]*3+1); 
					wldPt[2] = ReadFix8(verts, vertices[k]*3+2);
					ComputePixel(wldPt);
					(*pxls)[vertices[k]*2+0] = scrPt[0]; 
					(*pxls)[vertices[k]*2+1] = scrPt[1]; 
				}
			}
*/			
			x0 = (*pxls)[vertices[0]*2+0]; 
			y0 = (*pxls)[vertices[0]*2+1];
			x1 = (*pxls)[vertices[1]*2+0]; 
			y1 = (*pxls)[vertices[1]*2+1];
			x2 = (*pxls)[vertices[2]*2+0]; 
			y2 = (*pxls)[vertices[2]*2+1];
			DrawLine(x0, y0, x1, y1);
			DrawLine(x1, y1, x2, y2);
			DrawLine(x2, y2, x0, y0);
		}
	}
	
	// Free memory
	free(visible);
}

fix8 wX[3] = {256*30, 0, 0};
fix8 wY[3] = {0, 256*30, 0};

static void RenderAxes() 
{
	int pX[2], pY[2];
	int vX[2], vY[2];
	int nX, nY;

	// Shrink render
	screenW = 30; screenH = 30;
	
	// Rasterize 
	ComputePixel(wX);
	pX[0] = scrPt[0]; pX[1] = scrPt[1];
	ComputePixel(wY);
	pY[0] = scrPt[0]; pY[1] = scrPt[1];

	// Compute Vectors
	vX[0] = pX[0]-15; vX[1] = pX[1]-15; 
	vY[0] = pY[0]-15; vY[1] = pY[1]-15;
	nX = iSqrt(vX[0]*vX[0]+vX[1]*vX[1]);
	nY = iSqrt(vY[0]*vY[0]+vY[1]*vY[1]);
	
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
	
	// Restore render
	screenW = 220; screenH = 200;
}

static void ResetCanvas()
{
	// Reset canvas
	tgi_setcolor (COLOR_BACK);
	tgi_bar(0, 0, screenW-1, screenH-1);
	tgi_setcolor (COLOR_FORE);	
}
	
