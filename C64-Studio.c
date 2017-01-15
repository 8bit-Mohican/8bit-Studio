
#include <cc65.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>

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
int meshSel = 0;
int meshNum = 0;
int guiTrigger = 0;
char lastChar;

int main (void) 
{
	unsigned int i;
	bool doRender = true;
	clock_t time;
	
	// Mesh Data
	const char *names[MAXMESH];
	fix8 *pos[MAXMESH], *rot[MAXMESH], *dim[MAXMESH];
	int nVerts[MAXMESH], nTris[MAXMESH];
	fix8 *verts[MAXMESH], *norms[MAXMESH];
	int *tris[MAXMESH], *pxls[MAXMESH];
	
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
	}
	
	// Read STL Mesh
	pos[0][0] = Int2Fix8(0); pos[0][1] = Int2Fix8(0); pos[0][2] = Int2Fix8(0);	
	rot[0][0] = Int2Fix8(0); rot[0][1] = Int2Fix8(0); rot[0][2] = Int2Fix8(0);	
	dim[0][0] = Int2Fix8(1); dim[0][1] = Int2Fix8(1); dim[0][2] = Int2Fix8(1);
	names[0] = ReadSTL("logo.stl", &nVerts[0], &nTris[0], &verts[0], &norms[0], &tris[0], &pxls[0]);

	// Create Cylinder
	meshNum = 4;
	for (i = 1; i < meshNum; ++i) {
		rot[i][0] = Int2Fix8(0);   rot[i][1] = Int2Fix8(0);  rot[i][2] = Int2Fix8(0);	
		dim[i][0] = Int2Fix8(10);  dim[i][1] = Int2Fix8(10); dim[i][2] = Int2Fix8(10);		
	}
	pos[1][0] = Int2Fix8(-20); pos[1][1] = Int2Fix8(0);   pos[1][2] = Int2Fix8(0);	
	pos[2][0] = Int2Fix8(0);   pos[2][1] = Int2Fix8(-20); pos[2][2] = Int2Fix8(0);	
	pos[3][0] = Int2Fix8(20);  pos[3][1] = Int2Fix8(0);   pos[3][2] = Int2Fix8(0);	
	names[1] = CreateSphere(&nVerts[1], &nTris[1], &verts[1], &norms[1], &tris[1], &pxls[1]);
	names[2] = CreateCylinder(6, &nVerts[2], &nTris[2], &verts[2], &norms[2], &tris[2], &pxls[2]);
	names[3] = CreateBox(&nVerts[3], &nTris[3], &verts[3], &norms[3], &tris[3], &pxls[3]);	

	// Initialize Screen
	StartTGI();
	DrawGUI(meshSel, names, pos[meshSel], rot[meshSel], dim[meshSel]);

	// Show Mouse
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
				}
			// Located in GUI part?
			} else {
				guiTrigger = 1;
			}
		} else {
			// Reset mouse
			if (mouseX != -1) {
				mouseX = -1;
				doRender = true;
			}
			
			// Check GUI actions?
			if (guiTrigger) {
				guiTrigger = 0;

				// Change mesh selection?
				if (info.pos.x > listX && info.pos.x < listX+listW && 
				    info.pos.y > listY && info.pos.y < listY+listH ) {
					meshSel = (info.pos.y - listY) / 12;
					DrawList(listX, listY, listW, listH, meshSel, names);
					DrawProps(pos[meshSel], rot[meshSel], dim[meshSel]);
				}				
			}
		}
			
		// Redraw if necessary
		if (doRender) {
			// Reset state
			doRender = false;
			time = clock();
			UpdateCamera();
			for (i = 0; i < meshNum; ++i) {
				Rasterize(pos[i], rot[i], dim[i], nVerts[i], &verts[i], &pxls[i]);
			}

			// Render scene
			ResetCanvas();
			for (i = 0; i < meshNum; ++i) {
				RenderMesh(nTris[i], &tris[i], &norms[i], &pxls[i]);
			}
			RenderAxes();							
			time = clock() - time;			
		}
	}
	StopTGI();
	
	// Show stats
	ShowStats(time, 1);
	
    // Done
    return EXIT_SUCCESS;
}
