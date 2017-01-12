/*****************************************************************************/
/*                                 TGI Stuff                                 */
/*****************************************************************************/

#include <tgi.h>

#ifndef DYN_DRV
#  define DYN_DRV       1
#endif

#define COLOR_BACK      TGI_COLOR_BLACK
#define COLOR_BORDER    15					// Light-Grey
#define COLOR_FORE      TGI_COLOR_WHITE

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

    cprintf ("Loading %s...\n", Name);
    F = tgi_load_vectorfont (Name);
    CheckTGIError ("tgi_load_vectorfont");
    return F;
}

static void StartTGI () 
{
    //static const unsigned char Palette[2] = { COLOR_BACK, COLOR_FORE };
	
#if DYN_DRV
    /* Load the driver */
    tgi_load_driver (tgi_stddrv);
    CheckTGIError ("tgi_load_driver");
#else
    /* Install the driver */
    tgi_install (tgi_static_stddrv);
    CheckTGIError ("tgi_install");
#endif

    /* Initialize the driver */
    bordercolor (COLOR_BORDER);
    tgi_init ();
    CheckTGIError ("tgi_init");
	tgi_clear ();

    /* Get stuff from the driver */
    MaxX = tgi_getmaxx ();
    MaxY = tgi_getmaxy ();
    AspectRatio = tgi_getaspectratio ();

	/* Load the vector fonts */
    font = LoadFont ("litt.tch");
    tgi_install_vectorfont (font);
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

static void DrawButton (int x, int y, int w, int h, char *text) 
{
	// Box
	tgi_line(x, y, x+w, y);	
	tgi_line(x, y+h, x+w, y+h);	
	tgi_line(x, y, x, y+h);	
	tgi_line(x+w, y, x+w, y+h);	
	tgi_outtextxy(x+20, y+h-3, text);
}

static void DrawPanel () 
{
	// Separator
	tgi_setcolor (COLOR_FORE);
	tgi_line(220, 0, 220, 200);
	tgi_outtextxy(240, 10, "C64 Studio");	
	DrawButton(230, 20, 80, 12, "Add Box");
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
