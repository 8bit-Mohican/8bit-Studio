/*****************************************************************************/
/*                              Rendering Library                            */
/*****************************************************************************/

#include "cc65-libmatrix.h"
#include "cc65-libtgi.h"

unsigned int screenW = 220;
unsigned int screenH = 200;

fix8 canvasW = 2*256;
fix8 canvasH = 2*256;

fix8 worldToCamera[4][4] = { { 256,     0,     0,     0},
							 {   0,   256,     0,     0},
							 {   0,     0,   256,     0},
							 {   0,     0,  6400,   256} };						 

fix8 camVec[3] = {0,0,256};

fix8 xCam = 45;
fix8 zCam = -45;
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

int scrPt[2];

static void PixelCoords(fix8 pWorld[3])
{
	fix8 pScreen[2];

	MatrixVectorMult(worldToCamera, pWorld);
    pScreen[0] = (256 * MVM[0]) / -MVM[2]; 
    pScreen[1] = (256 * MVM[1]) / -MVM[2];

    scrPt[0] = screenW * (pScreen[0] + canvasW/2) / canvasW;
    scrPt[1] = screenH - screenH * (pScreen[1] + canvasH/2) / canvasH;
}

static void Rasterize(int nVerts, fix8 *verts, int **pxls)
{
	fix8 pWorld[3];
	int i;

	/* Allocate memory */
	(*pxls) = (int*) malloc (nVerts*2*sizeof(int));
	
	/* Project points to screen */
	for (i = 0; i < nVerts; ++i) {
		pWorld[0] = verts[i*3+0]; pWorld[1] = verts[i*3+1]; pWorld[2] = verts[i*3+2];
		PixelCoords(pWorld);
		(*pxls)[i*2+0] = scrPt[0]; (*pxls)[i*2+1] = scrPt[1];
	}
}

fix8 wX1[3] = {256*-50, 0, 0};
fix8 wX2[3] = {256*50, 0, 0};
fix8 wY1[3] = {0, 256*-50, 0};
fix8 wY2[3] = {0, 256*50, 0};
//fix8 wZ1[3] = {0, 0, 256*-50};
//fix8 wZ2[3] = {0, 0, 256*50};

static void RenderAxes() 
{
	int pX1[2], pX2[2];
	int pY1[2], pY2[2]; 
	//int pZ1[2], pZ2[2]; 

	/* Shrink render */
	screenW = 33; screenH = 30;
	
	/* Rasterize */
	PixelCoords(wX1);
	pX1[0] = scrPt[0]; pX1[1] = scrPt[1];
	PixelCoords(wX2);
	pX2[0] = scrPt[0]; pX2[1] = scrPt[1];
	PixelCoords(wY1);
	pY1[0] = scrPt[0]; pY1[1] = scrPt[1];
	PixelCoords(wY2);
	pY2[0] = scrPt[0]; pY2[1] = scrPt[1];
	//PixelCoords(wZ1);
	//pZ1[0] = scrPt[0]; pZ1[1] = scrPt[1];
	//PixelCoords(wZ2);
	//pZ2[0] = scrPt[0]; pZ2[1] = scrPt[1];

	/* Blank the sector */
	tgi_setcolor (COLOR_BACK);
	tgi_bar(0, 169, screenW-1, 169+screenH-1);
	tgi_setcolor (COLOR_FORE);
	
	/* Draw axes */
	tgi_line(0, 169, 33, 169);
	tgi_line(33, 169, 33, 199);
	tgi_line(pX1[0], pX1[1]+169, pX2[0], pX2[1]+169);
	tgi_line(pY1[0], pY1[1]+169, pY2[0], pY2[1]+169);	
	//tgi_line(pZ1[0], pZ1[1], pZ2[0], pZ2[1]);	
	
	/* Restore render */
	screenW = 220; screenH = 200;
}

fix8 normal[3];

static void RenderMesh(int nTris, int *tris, fix8 *norms, int *wXls) 
{
	unsigned int i,x0,y0,x1,y1,x2,y2;				

	/* Draw all triangles */
	tgi_setcolor (COLOR_FORE);
	for (i = 0; i < nTris; ++i) {
		normal[0] = norms[i*3+0];
		normal[1] = norms[i*3+1];
		normal[2] = norms[i*3+2];
		if (VectorVectorDot(normal,camVec) > 0) {
			x0 = wXls[tris[i*3+0]*2+0]; y0 = wXls[tris[i*3+0]*2+1];
			x1 = wXls[tris[i*3+1]*2+0]; y1 = wXls[tris[i*3+1]*2+1];
			x2 = wXls[tris[i*3+2]*2+0]; y2 = wXls[tris[i*3+2]*2+1];
			tgi_line(x0, y0, x1, y1);
			tgi_line(x1, y1, x2, y2);
			tgi_line(x2, y2, x0, y0);
		}
	}
}

static void ResetCanvas()
{
	// Reset canvas
	tgi_setcolor (COLOR_BACK);
	tgi_bar(0, 0, screenW-1, screenH-1);
}
	
