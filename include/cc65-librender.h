/*****************************************************************************/
/*                              Rendering Library                            */
/*****************************************************************************/

#include <stdbool.h>

#include "cc65-libmath.h"
#include "cc65-libmatrix.h"
#include "cc65-libmemory.h"

static int screenW = 220;
static int screenH = 190;

fix8 canvasW = Int2Fix8(2);
fix8 canvasH = Int2Fix8(2);

fix8 worldToCamera[16] = { 256,     0,     0,
							 0,   256,     0,
							 0,     0,   256,
							 0,     0, 10240, };

fix8 camVec[3] = {0,0,256};

fix8 xCam = 40;
fix8 zCam = 150;

static void UpdateCamera() 
{
	fix8 xCos, zCos;
	fix8 xSin, zSin;
	
	// Compute cos and sin of angles
	xCos = cc65_cos(xCam); xSin = cc65_sin(xCam);
	zCos = cc65_cos(zCam); zSin = cc65_sin(zCam);
	
	// Update camera matrix
	worldToCamera[3*0+0] = ( zCos        );
	worldToCamera[3*0+1] = (-zSin * xCos) / 256;
	worldToCamera[3*0+2] = ( zSin * xSin) / 256;
	worldToCamera[3*1+0] = ( zSin	    );
	worldToCamera[3*1+1] = ( zCos * xCos) / 256;
	worldToCamera[3*1+2] = (-zCos * xSin) / 256;
	worldToCamera[3*2+0] = (      0	    );
	worldToCamera[3*2+1] = ( 	    xSin);
	worldToCamera[3*2+2] = ( 	    xCos);	
	
	// Update camera vector
	camVec[0] =  zSin * -xSin / 256;
	camVec[1] = -zCos * -xSin / 256;
	camVec[2] = -xCos;	
}

fix8 camPt[3];
int scrPt[2];

static void ComputePixel(fix8 *wldPt)
{
	M43multV3(&worldToCamera[0], wldPt, &camPt[0]);
    scrPt[0] = screenW * (canvasW/2 - (256*camPt[0])/camPt[2]) / canvasW;
    scrPt[1] = screenH - screenH * (canvasH/2 - (256*camPt[1])/camPt[2]) / canvasH;
}

static void DrawLine(int x0, int y0, int x1, int y1)
{
	// Line Drawing
	if (x0 <= screenW && x1 <= screenW) {
		tgi_line(x0, y0, x1, y1);
	} else if (x0 <= screenW && x1 > screenW) {
		tgi_line(x0, y0, screenW, y0+((y1-y0)*(screenW-x0))/(x1-x0));
	} else if (x0 > screenW && x1 <= screenW) {
		tgi_line(screenW, y1+((y0-y1)*(screenW-x1))/(x0-x1), x1, y1);
	}	
}

static void RenderMesh(int nTris, int nVerts, int **tris, fix8 **norms, fix8 **verts, int **pxls, char *renderMask) 
{
	fix8 vertice[3];
	fix8 normal[3];
	int vertices[3];	
	int i,j,x0,y0,x1,y1,x2,y2;
	bool *visible;
	
	// Do Rasterize?
	if ((*renderMask) & MASK_RASTER) {
		// Go through all vertices, and compute pixels
		for (i = 0; i < nVerts; ++i) {
			// Read transformed point
			for (j = 0; j < 3; ++j) {
				vertice[j] = ReadFix8(verts,i*6+j+3); 
			}

			// Project point to screen
			ComputePixel(&vertice[0]);
			(*pxls)[i*2+0] = scrPt[0]; 
			(*pxls)[i*2+1] = scrPt[1]; 
		}
		(*renderMask) &= ~MASK_RASTER;
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
			if (V3dotV3(&normal[0],&camVec[0]) >= 0) {
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
fix8 wZ[3] = {0, 0, 256*30};

static void RenderAxes() 
{
	int vX[2], vY[2], vZ[2];
	int nX, nY, nZ;

	// Shrink render size
	screenW = 30; screenH = 30;
	
	// Compute X/Y axis vectors
	ComputePixel(&wX[0]);
	vX[0] = scrPt[0]-15; 
	vX[1] = scrPt[1]-15;
	ComputePixel(&wY[0]);
	vY[0] = scrPt[0]-15; 
	vY[1] = scrPt[1]-15;
	ComputePixel(&wZ[0]);
	vZ[0] = scrPt[0]-15; 
	vZ[1] = scrPt[1]-15;

	// Compute norm
	nX = NORM2(vX);
	nY = NORM2(vY);
	nZ = NORM2(vZ);
	
	// Reset block
	tgi_setcolor (COLOR_BACK);
	tgi_bar(0, 159, screenW, 159+screenH);
	tgi_setcolor (COLOR_FORE);
	tgi_line(0, 159, 30, 159);
	tgi_line(30, 159, 30, 189);
	
	// Draw axes and labels
	if (nX > 0) {
		DrawLine(14-(15*vX[0])/nX, 173-(15*vX[1])/nX, 14+(7*vX[0])/nX, 173+(7*vX[1])/nX);
		tgi_outtextxy(14+(11*vX[0])/nX-2, 173+(11*vX[1])/nX+2, "x");
	} else { tgi_outtextxy(14, 173, "x"); }
	if (nY > 0) {
		DrawLine(14-(15*vY[0])/nY, 173-(15*vY[1])/nY, 14+(7*vY[0])/nY, 173+(7*vY[1])/nY);
		tgi_outtextxy(14+(11*vY[0])/nY-2, 173+(11*vY[1])/nY+2, "y");
	} else { tgi_outtextxy(14, 173, "y"); }
	if (nZ > 0) {
		DrawLine(14-(15*vZ[0])/nZ, 173-(15*vZ[1])/nZ, 14+(7*vZ[0])/nZ, 173+(7*vZ[1])/nZ);
		tgi_outtextxy(14+(11*vZ[0])/nZ-2, 173+(11*vZ[1])/nZ+2, "z");
	} else { tgi_outtextxy(14, 173, "z"); }
	
	// Restore render size
	screenW = 220; screenH = 190;
}

static void ResetCanvas()
{
	// Reset canvas
	tgi_setcolor (COLOR_BACK);
	tgi_bar(0, 0, screenW-1, screenH-1);
	tgi_setcolor (COLOR_FORE);	
}
	
