/*****************************************************************************/
/*                              Rendering Library                            */
/*****************************************************************************/

#include "cc65-libdraw.h"
#include "cc65-libmath.h"
#include "cc65-libmatrix.h"

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

static void PixelCoords(fix8 wldPt[3])
{
	MatrixVectorMult(worldToCamera, wldPt);
    canPt[0] = (256 * MVM[0]) / -MVM[2]; 
    canPt[1] = (256 * MVM[1]) / -MVM[2];

    scrPt[0] = screenW * (canPt[0] + canvasW/2) / canvasW;
    scrPt[1] = screenH - screenH * (canPt[1] + canvasH/2) / canvasH;
}

fix8 wldPt[3];

static void Rasterize(int nVerts, fix8 *verts, int **pxls)
{	
	// Project points to screen
	int i;
	for (i = 0; i < nVerts; ++i) {
		wldPt[0] = verts[i*3+0]; wldPt[1] = verts[i*3+1]; wldPt[2] = verts[i*3+2];
		PixelCoords(wldPt);
		(*pxls)[i*2+0] = scrPt[0]; (*pxls)[i*2+1] = scrPt[1];
	}
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
	PixelCoords(wX);
	pX[0] = scrPt[0]; pX[1] = scrPt[1];
	PixelCoords(wY);
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
	
	// Draw axes
	DrawLine(14-(15*vX[0])/nX, 183-(15*vX[1])/nX, 14+(7*vX[0])/nX, 183+(7*vX[1])/nX);
	DrawLine(14-(15*vY[0])/nY, 183-(15*vY[1])/nY, 14+(7*vY[0])/nY, 183+(7*vY[1])/nY);

	// Draw labels
	tgi_outtextxy(14+(11*vX[0])/nX-2, 183+(11*vX[1])/nX+2, "x");
	tgi_outtextxy(14+(11*vY[0])/nY-2, 183+(11*vY[1])/nY+2, "y");
	
	// Restore render
	screenW = 220; screenH = 200;
}

static void RenderMesh(int nTris, int *tris, fix8 *norms, int *wXls) 
{
	fix8 normal[3];
	unsigned int i,x0,y0,x1,y1,x2,y2;				

	/* Draw all triangles */
	for (i = 0; i < nTris; ++i) {
		normal[0] = norms[i*3+0];
		normal[1] = norms[i*3+1];
		normal[2] = norms[i*3+2];
		if (VectorVectorDot(normal,camVec) > 0) {
			x0 = wXls[tris[i*3+0]*2+0]; y0 = wXls[tris[i*3+0]*2+1];
			x1 = wXls[tris[i*3+1]*2+0]; y1 = wXls[tris[i*3+1]*2+1];
			x2 = wXls[tris[i*3+2]*2+0]; y2 = wXls[tris[i*3+2]*2+1];
			DrawLine(x0, y0, x1, y1);
			DrawLine(x1, y1, x2, y2);
			DrawLine(x2, y2, x0, y0);
		}
	}
}

static void ResetCanvas()
{
	// Reset canvas
	tgi_setcolor (COLOR_BACK);
	tgi_bar(0, 0, screenW-1, screenH-1);
	tgi_setcolor (COLOR_FORE);	
}
	
