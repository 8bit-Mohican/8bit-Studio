
#include <cc65.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>

#include "include/cc65-libcsg.h"
#include "include/cc65-libmemory.h"
#include "include/cc65-libmouse.h"
#include "include/cc65-libprimitives.h"
#include "include/cc65-librender.h"
#include "include/cc65-libstl.h"

#define MAXMESH 8


/*****************************************************************************/
/*                                 Main Code                                 */
/*****************************************************************************/

// Mesh Counters
int meshNum = 0;
int guiTrigger = 0;
char lastChar;

// Mesh Data
const char *names[MAXMESH];
static char renderMask[MAXMESH];
static fix8 *pos[MAXMESH], *rot[MAXMESH], *dim[MAXMESH];
static int nVerts[MAXMESH], nTris[MAXMESH];
static fix8 *verts[MAXMESH], *norms[MAXMESH];
static int *tris[MAXMESH], *pxls[MAXMESH];

int main (void) 
{
	unsigned int i;
	bool doRender = true;
	clock_t time;
	
	// Reset Screen
	clrscr (); gotoxy (0, 0);
	printf ("C64 Studio is Initializing...\n");
	
	// Init Memory
	InitMemory();
	InitMouse();
	InitTGI();
	
	// Initialize position/dimension
	for (i = 0; i < MAXMESH; ++i) {
		names[i] = NULL;
		pos[i] = (fix8*) malloc (3*sizeof(fix8));
		rot[i] = (fix8*) malloc (3*sizeof(fix8));
		dim[i] = (fix8*) malloc (3*sizeof(fix8));
		renderMask[i] = MASK_RASTERIZE | MASK_DRAW;
	}

	// Create primitives
	meshNum = 4;
	for (i = 0; i < meshNum; ++i) {
		rot[i][0] = Int2Fix8(0);   rot[i][1] = Int2Fix8(0);  rot[i][2] = Int2Fix8(0);	
		dim[i][0] = Int2Fix8(10);  dim[i][1] = Int2Fix8(10); dim[i][2] = Int2Fix8(10);		
	}
	pos[0][0] = Int2Fix8(-20); pos[0][1] = Int2Fix8(0);   pos[0][2] = Int2Fix8(0);	
	pos[1][0] = Int2Fix8(0);   pos[1][1] = Int2Fix8(-20); pos[1][2] = Int2Fix8(0);	
	pos[2][0] = Int2Fix8(20);  pos[2][1] = Int2Fix8(0);   pos[2][2] = Int2Fix8(0);	
	names[0] = CreateSphere(20, &nVerts[0], &nTris[0], &verts[0], &norms[0], &tris[0], &pxls[0]);
	names[1] = CreateCylinder(8, &nVerts[1], &nTris[1], &verts[1], &norms[1], &tris[1], &pxls[1]);
	names[2] = CreateBox(&nVerts[2], &nTris[2], &verts[2], &norms[2], &tris[2], &pxls[2]);

	// Read STL Mesh
	pos[3][0] = Int2Fix8(0); pos[3][1] = Int2Fix8(0); pos[3][2] = Int2Fix8(0);	
	rot[3][0] = Int2Fix8(0); rot[3][1] = Int2Fix8(0); rot[3][2] = Int2Fix8(0);	
	dim[3][0] = Int2Fix8(1); dim[3][1] = Int2Fix8(1); dim[3][2] = Int2Fix8(1);
	names[3] = ReadSTL("logo.stl", &nVerts[3], &nTris[3], &verts[3], &norms[3], &tris[3], &pxls[3]);

	// Transform Meshes
	for (i = 0; i < meshNum; ++i) {
		Transform(pos[i], rot[i], dim[i], nTris[i], nVerts[i], &tris[i], &norms[i], &verts[i]);
	}
	
	// Initialize Screen
	StartTGI();
	DrawGUI(names, pos[selMesh], rot[selMesh], dim[selMesh]);

	// Show Mouse
	UpdateCamera();
	mouse_show ();
	mouseX = -1;
	
    // Main loop of Application
	while (lastChar != 'q') {
		// Process keyboard info
		if (kbhit()) {
			lastChar = cgetc();
		}
	
		// Process mouse info
        mouse_info (&info);	
		if (info.buttons & MOUSE_BTN_LEFT) {
			// Located in viewport part?
			if (info.pos.x < 220) {
				// Rotate the axes
				if (mouseX == -1) {
					mouseX = info.pos.x;
					mouseY = info.pos.y;				
				} else if (abs(mouseX - info.pos.x) > 0 || abs(mouseY - info.pos.y) > 0) {
					// Compute delta
					xCam = (xCam + (mouseY - info.pos.y))%360;
					zCam = (zCam + (mouseX - info.pos.x))%360;
					mouse_move(mouseX, mouseY);

					// Refresh axes
					UpdateCamera();
					RenderAxes();
					
					// Set render flags
					doRender = true;
					for (i = 0; i < MAXMESH; ++i) {
						renderMask[i] = MASK_RASTERIZE | MASK_DRAW;
					}
				}
			// Located in GUI part?
			} else {
				guiTrigger = 1;
			}
		} else if (info.buttons & MOUSE_BTN_RIGHT) {
			// Located in viewport part?
			if (info.pos.x < 220) {
				// Reset axes
				xCam = 30; zCam = 330;				
				UpdateCamera();
				RenderAxes();

				// Set render flags
				doRender = true;
				for (i = 0; i < MAXMESH; ++i) {
					renderMask[i] = MASK_RASTERIZE | MASK_DRAW;
				}				
			}
		} else {
			// Reset mouse
			mouseX = -1;
			
			// Check GUI actions?
			if (guiTrigger) {
				guiTrigger = 0;

				// Change mesh selection?
				if (info.pos.x > listX && info.pos.x < listX+listW && 
				    info.pos.y > listY && info.pos.y < listY+listH ) {
					selMesh = (info.pos.y - listY) / 12;
					DrawList(listX, listY, listW, listH, names, selMesh);
					DrawProps(pos[selMesh], rot[selMesh], dim[selMesh]);
				}

				// Edit mesh properties?
				if (info.pos.x > propsX && info.pos.x < propsX+propsW && 
				    info.pos.y > propsY && info.pos.y < propsY+propsH ) {
					rot[selMesh][0] = Int2Fix8(45);
					rot[selMesh][2] = Int2Fix8(45);
					Transform(pos[selMesh], rot[selMesh], dim[selMesh], nTris[selMesh], nVerts[selMesh], &tris[selMesh], &norms[selMesh], &verts[selMesh]);
					DrawProps(pos[selMesh], rot[selMesh], dim[selMesh]);

					// Set render flags
					doRender = true;
					for (i = 0; i < MAXMESH; ++i) {
						renderMask[i] = MASK_DRAW;
					}
					renderMask[selMesh] |= MASK_RASTERIZE; 					
				}				
				
				// Change tab selection?
				if (info.pos.x > tabsX && info.pos.x < tabsX+tabsW && 
				    info.pos.y > tabsY && info.pos.y < tabsY+tabsH ) {
					selTab = (info.pos.x - tabsX) / 33;
					DrawControls();
				}
			}
			
			// Redraw Scene?
			if (doRender) {
				// Reset state
				doRender = false;
				
				// Render scene
				time = clock();
				ResetCanvas();
				for (i = 0; i < meshNum; ++i) {
					RenderMesh(nTris[i], nVerts[i], &tris[i], &norms[i], &verts[i], &pxls[i], &renderMask[i]);
				}
				RenderAxes();							
				time = clock() - time;
				DrawPerf(time);
			}
		}
	}
	StopTGI();
	
    // Done
    return EXIT_SUCCESS;
}
