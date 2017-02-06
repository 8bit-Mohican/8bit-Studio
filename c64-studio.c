
#include <cc65.h>
#include <conio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "include/cc65lib-memory.h"
#include "include/cc65lib-mouse.h"
#include "include/cc65lib-primitives.h"
#include "include/cc65lib-csg.h"
#include "include/cc65lib-stl.h"
#include "include/cc65lib-gui.h"
#include "include/cc65lib-render.h"

#define MAXMESH 16


/*****************************************************************************/
/*                                 GUI Code                                 */
/*****************************************************************************/

static int selMesh = 0;
const int listX = 230;
const int listY = 18;
const int listW = 80;
const int listH = 48;

const int propsX = 250;
const int propsY = 68;
const int propsW = 70;
const int propsH = 36;

static int selTab = 0;
const int tabsX = 220;
const int tabsY = 115;
const int tabsW = 100;
const int tabsH = 12;

static void DrawProps (fix8 pos[3], fix8 rot[3], fix8 dim[3])
{
	char dump[8];
		
	// Reset background
	tgi_setcolor (COLOR_BACK);
	tgi_bar(225, 70, 319, 110);
	tgi_setcolor (COLOR_FORE);

	// Draw props
	tgi_outtextxy(225, 80, "Pos:");
	snprintf(dump, 8, "%ld", pos[0]/256);
	tgi_outtextxy(250, 80, dump);
	snprintf(dump, 8, "%ld", pos[1]/256);
	tgi_outtextxy(275, 80, dump);
	snprintf(dump, 8, "%ld", pos[2]/256);
	tgi_outtextxy(300, 80, dump);

	tgi_outtextxy(225, 92, "Rot:");
	snprintf(dump, 8, "%ld", rot[0]/256);
	tgi_outtextxy(250, 92, dump);
	snprintf(dump, 8, "%ld", rot[1]/256);
	tgi_outtextxy(275, 92, dump);
	snprintf(dump, 8, "%ld", rot[2]/256);
	tgi_outtextxy(300, 92, dump);
	
	tgi_outtextxy(225, 104, "Dim:");
	snprintf(dump, 8, "%ld", dim[0]/256);
	tgi_outtextxy(250, 104, dump);
	snprintf(dump, 8, "%ld", dim[1]/256);
	tgi_outtextxy(275, 104, dump);
	snprintf(dump, 8, "%ld", dim[2]/256);
	tgi_outtextxy(300, 104, dump);
}

static void DrawControls()
{
	// Reset background
	tgi_setcolor (COLOR_BACK);
	tgi_bar(221, 116, 319, 189);
	tgi_setcolor (COLOR_FORE);
	
	// Draw tabs
	DrawButton(220, 115, 33, 12, "Prim", selTab == 0);
	DrawButton(253, 115, 33, 12, "Bool", selTab == 1);
	DrawButton(286, 115, 33, 12, "Scene", selTab == 2);
		
	// Draw buttons
	if (selTab == 0) {
		DrawButton(230, 130, 80, 12, "Add Box", false);
		DrawButton(230, 145, 80, 12, "Add Cone", false);
		DrawButton(230, 160, 80, 12, "Add Cylinder", false);
		DrawButton(230, 175, 80, 12, "Add Sphere", false);
	}
	
	if (selTab == 1) {
		DrawButton(230, 130, 80, 12, "Difference", false);
		DrawButton(230, 145, 80, 12, "Intersect", false);
		DrawButton(230, 160, 80, 12, "Union", false);
	}	
	
	if (selTab == 2) {
		DrawButton(230, 130, 80, 12, "Load Scene", false);
		DrawButton(230, 145, 80, 12, "Save Scene", false);
		DrawButton(230, 160, 80, 12, "Web Browser", false);
	}		
}

static void DrawStatus (const char *message)
{
	// Reset background
	tgi_setcolor (COLOR_BACK);
	tgi_bar(0, 191, 319, 199);
	
	// Output message
	tgi_setcolor (COLOR_FORE);
	tgi_outtextxy(10, 198, message);
}

static void DrawPerf (int num, clock_t time)
{
	char dump[30];
	unsigned long sec;
    unsigned sec10;

    // Calculate stats
    sec = (time * 10) / CLK_TCK;
    sec10 = sec % 10;
    sec /= 10;
	
    // Output stats
	snprintf(dump, 30, "Rendered %d meshes in %lu.%us", num, sec, sec10);
	DrawStatus(&dump[0]);
}

static void DrawGUI (const char *names[], fix8 pos[3], fix8 rot[3], fix8 dim[3]) 
{	
	// Separators
	tgi_setcolor (COLOR_FORE);
	tgi_line(220, 0, 220, 190);
	tgi_line(220, 115, 319, 115);
	tgi_line(0, 190, 319, 190);
	
	// Top Section
	tgi_outtextxy(240, 10, "C64 Studio");	
	DrawList(listX, listY, listW, listH, names, selMesh);
	DrawProps(pos, rot, dim);
	
	// Bottom Section
	DrawControls();
}

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
		renderMask[i] = MASK_RASTER | MASK_DRAW;
	}

	// Create primitives
	meshNum = 4;
	pos[0][0] = Int2Fix8(-10); pos[0][1] = Int2Fix8(-10); pos[0][2] = Int2Fix8(0);	
	pos[1][0] = Int2Fix8(-10); pos[1][1] = Int2Fix8( 10); pos[1][2] = Int2Fix8(0);	
	pos[2][0] = Int2Fix8( 10); pos[2][1] = Int2Fix8(-10); pos[2][2] = Int2Fix8(0);	
	pos[3][0] = Int2Fix8( 10); pos[3][1] = Int2Fix8( 10); pos[3][2] = Int2Fix8(0);		
	for (i = 0; i < meshNum; ++i) {
		rot[i][0] = Int2Fix8(0);   rot[i][1] = Int2Fix8(0);  rot[i][2] = Int2Fix8(0);	
		dim[i][0] = Int2Fix8(15);  dim[i][1] = Int2Fix8(15); dim[i][2] = Int2Fix8(15);		
	}
	names[0] = CreateBox(&nVerts[0], &nTris[0], &verts[0], &norms[0], &tris[0], &pxls[0]);
	names[1] = CreateCone(8, &nVerts[1], &nTris[1], &verts[1], &norms[1], &tris[1], &pxls[1]);
	names[2] = CreateCylinder(8, &nVerts[2], &nTris[2], &verts[2], &norms[2], &tris[2], &pxls[2]);
	names[3] = CreateSphere(60, &nVerts[3], &nTris[3], &verts[3], &norms[3], &tris[3], &pxls[3]);

	// Read STL Mesh
	//pos[3][0] = Int2Fix8(10); pos[3][1] = Int2Fix8(0); pos[3][2] = Int2Fix8(0);	
	//rot[3][0] = Int2Fix8(0); rot[3][1] = Int2Fix8(0); rot[3][2] = Int2Fix8(0);	
	//dim[3][0] = Int2Fix8(1); dim[3][1] = Int2Fix8(1); dim[3][2] = Int2Fix8(1);
	//names[3] = ReadSTL("logo.stl", &nVerts[3], &nTris[3], &verts[3], &norms[3], &tris[3], &pxls[3]);

	// Transform Meshes
	for (i = 0; i < meshNum; ++i) {
		Transform(pos[i], rot[i], dim[i], nTris[i], nVerts[i], &tris[i], &norms[i], &verts[i]);
	}
	
	// CSG Test: Split Faces
	//SplitFaces(nTris[0], nVerts[0], &tris[0], &norms[0], &verts[0], nTris[1], nVerts[1], &tris[1], &norms[1], &verts[1]);
	//while (!kbhit()) { }
	//cgetc();
		
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
						renderMask[i] = MASK_RASTER | MASK_DRAW;
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
				xCam = 40; zCam = 150;				
				UpdateCamera();
				RenderAxes();

				// Set render flags
				doRender = true;
				for (i = 0; i < MAXMESH; ++i) {
					renderMask[i] = MASK_RASTER | MASK_DRAW;
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
					renderMask[selMesh] |= MASK_RASTER; 					
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
				DrawStatus("Rendering, please wait...");
				
				// Render scene
				time = clock();
				ResetCanvas();
				for (i = 0; i < meshNum; ++i) {
					RenderMesh(nTris[i], nVerts[i], &tris[i], &norms[i], &verts[i], &pxls[i], &renderMask[i]);
				}
				RenderAxes();							
				time = clock() - time;
				DrawPerf(meshNum, time);
			}
		}
	}
	
	// Turn off everything
	StopMouse();
	StopTGI();
	
    // Done
    return EXIT_SUCCESS;
}
