/*****************************************************************************/
/*                               Drawing Stuff                               */
/*****************************************************************************/

#include <tgi.h>

#ifndef DYN_DRV
#  define DYN_DRV       1
#endif

#define COLOR_BACK      TGI_COLOR_BLACK
#define COLOR_BORDER    15					// Light-Grey
#define COLOR_FORE      TGI_COLOR_WHITE

int screenW = 220;
int screenH = 200;

static unsigned MaxX;
static unsigned MaxY;
static unsigned AspectRatio;

static const tgi_vectorfont* font = 0;


static void CheckTGIError (const char* S) 
{
    unsigned char Error = tgi_geterror ();
    if (Error != TGI_ERR_OK) {
        cprintf ("%s: %d\n", S, Error);
        if (doesclrscrafterexit ()) {
            cgetc ();
        }
        exit (EXIT_FAILURE);
    }
}

static const tgi_vectorfont* LoadFont (const char* Name)
/* Load a font, do error checking */
{   
    const tgi_vectorfont* F;

    F = tgi_load_vectorfont (Name);
    CheckTGIError ("tgi_load_vectorfont");
    return F;
}

static void InitTGI () 
{
    //static const unsigned char Palette[2] = { COLOR_BACK, COLOR_FORE };
	
	printf ("Loading Video Driver...\n");	
#if DYN_DRV
    /* Load the driver */
    tgi_load_driver (tgi_stddrv);
    CheckTGIError ("tgi_load_driver");
#else
    /* Install the driver */
    tgi_install (tgi_static_stddrv);
    CheckTGIError ("tgi_install");
#endif

	/* Load the vector fonts */
	printf ("Loading Font (litt.tch)...\n");
    font = LoadFont ("litt.tch");
    tgi_install_vectorfont (font);
}

static void StartTGI ()
{
    // Initialize Video Mode
    bordercolor (COLOR_BORDER);
    tgi_init ();
    CheckTGIError ("tgi_init");
	tgi_clear ();

    // Get stuff from the driver
    MaxX = tgi_getmaxx ();
    MaxY = tgi_getmaxy ();
    AspectRatio = tgi_getaspectratio ();
	
	// Set Text Style
	tgi_settextstyle (0x100, 0x100, TGI_TEXT_HORIZONTAL, TGI_FONT_VECTOR);
		
	/* Set the palette, set the border color */
    //tgi_setpalette (Palette);	
}

static void StopTGI () 
{
#if DYN_DRV
    /* Unload the driver */
    tgi_unload ();
#else
    /* Uninstall the driver */
    tgi_uninstall ();
#endif
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

static void DrawButton (int x, int y, int w, int h, char *text) 
{
	tgi_line(x, y, x+w, y);	
	tgi_line(x, y+h, x+w, y+h);	
	tgi_line(x, y, x, y+h);	
	tgi_line(x+w, y, x+w, y+h);	
	tgi_outtextxy(x+5, y+h-3, text);
}

static void DrawList (int x, int y, int w, int h,  int sel, const char *names[]) 
{
	int i = 0;
	
	// Reset background
	tgi_setcolor (COLOR_BACK);
	tgi_bar(x, y, x+w, y+h);
	tgi_setcolor (COLOR_FORE);
	
	// Draw list
	tgi_line(x, y, x+w, y);	
	tgi_line(x, y+h, x+w, y+h);	
	tgi_line(x, y, x, y+h);	
	tgi_line(x+w, y, x+w, y+h);	
	tgi_line(x+w-10, y, x+w-10, y+h);	
	while (names[i] != NULL && i<9) {
		if (i==sel) {
			tgi_setcolor (COLOR_FORE);
			tgi_bar(x+8, y+(i+1)*12-10, x+w-18, y+(i+1)*12-2);
			tgi_setcolor (COLOR_BACK);
			tgi_outtextxy(x+10, y+(i+1)*12-3, names[i]);
			tgi_setcolor (COLOR_FORE);
		} else {
			tgi_outtextxy(x+10, y+(i+1)*12-3, names[i]);
		}
		i++;
	}
}

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

	tgi_outtextxy(225, 95, "Rot:");
	snprintf(dump, 8, "%ld", rot[0]/256);
	tgi_outtextxy(250, 95, dump);
	snprintf(dump, 8, "%ld", rot[1]/256);
	tgi_outtextxy(275, 95, dump);
	snprintf(dump, 8, "%ld", rot[2]/256);
	tgi_outtextxy(300, 95, dump);
	
	tgi_outtextxy(225, 110, "Dim:");
	snprintf(dump, 8, "%ld", dim[0]/256);
	tgi_outtextxy(250, 110, dump);
	snprintf(dump, 8, "%ld", dim[1]/256);
	tgi_outtextxy(275, 110, dump);
	snprintf(dump, 8, "%ld", dim[2]/256);
	tgi_outtextxy(300, 110, dump);
}

int listX = 230;
int listY = 18;
int listW = 80;
int listH = 48;

static void DrawGUI (int sel, const char *names[], fix8 pos[3], fix8 rot[3], fix8 dim[3]) 
{	
	// Separators
	tgi_setcolor (COLOR_FORE);
	tgi_line(220, 0, 220, 200);
	tgi_line(220, 120, 319, 120);
	
	// Top Section
	tgi_outtextxy(240, 10, "C64 Studio");	
	DrawList(listX, listY, listW, listH, sel, names);
	DrawProps(pos, rot, dim);
	
	// Bottom Section
	DrawButton(230, 125, 80, 12, "Add Primitive");
	DrawButton(230, 140, 80, 12, "Perform Bool");
	DrawButton(230, 160, 80, 12, "Load Scene");
	DrawButton(230, 175, 80, 12, "Save Scene");
}

static void ShowStats (clock_t t, unsigned long f) 
{
	unsigned long sec;
    unsigned      sec10;
    unsigned long fps;
    unsigned      fps10;
	
    /* Set the border and background colors */
    bordercolor (COLOR_BLUE);
    bgcolor (COLOR_BLUE);
    textcolor (COLOR_BLACK);
    clrscr ();	

    /* Calculate stats */
    sec   = (t * 10) / CLK_TCK;
    sec10 = sec % 10;
    sec  /= 10;
    fps   = (f * (CLK_TCK * 10)) / t;
    fps10 = fps % 10;
    fps  /= 10;

    /* Output stats */
    gotoxy (0, 0); cprintf ("time  : %lu.%us", sec, sec10);
    gotoxy (0, 1); cprintf ("frames: %lu", f);
    gotoxy (0, 2); cprintf ("fps   : %lu.%u", fps, fps10);

	/* Wait for input */
    if (doesclrscrafterexit ()) {
        cputsxy (0, 4, "Press any key when done...");
        (void) cgetc ();
    }
}
