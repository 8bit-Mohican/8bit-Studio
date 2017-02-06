/*****************************************************************************/
/*                               Drawing Stuff                               */
/*****************************************************************************/

#include <stdbool.h>
#include <tgi.h>

#ifndef DYN_DRV
#  define DYN_DRV       1
#endif

#define COLOR_BACK      TGI_COLOR_BLACK
#define COLOR_BORDER    TGI_COLOR_GRAY2
#define COLOR_FORE      TGI_COLOR_WHITE

static unsigned MaxX;
static unsigned MaxY;
static unsigned AspectRatio;

static const unsigned char Palette[2] = { COLOR_FORE, COLOR_BACK };
const tgi_vectorfont *font = 0;

typedef struct {
	int x[2];
	int y[2];
	void (*f)(void);
} Trigger;


static void CheckTGIError (const char *S) 
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

static const tgi_vectorfont* LoadFont (const char *Name)
/* Load a font, do error checking */
{   
    const tgi_vectorfont* F;

    F = tgi_load_vectorfont (Name);
    CheckTGIError ("tgi_load_vectorfont");
    return F;
}

static void InitTGI () 
{	
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
    tgi_init ();
    CheckTGIError ("tgi_init");

	// Set colors
    bordercolor (COLOR_BORDER);
    tgi_setpalette (Palette);	
	tgi_setcolor (COLOR_BACK);
	tgi_clear ();	

    // Get stuff from the driver
    MaxX = tgi_getmaxx ();
    MaxY = tgi_getmaxy ();
    AspectRatio = tgi_getaspectratio ();
	
	// Set Text Style
	tgi_settextstyle (0x100, 0x100, TGI_TEXT_HORIZONTAL, TGI_FONT_VECTOR);
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

static void DrawButton (int x, int y, int w, int h, char *text, bool highlight) 
{
	tgi_setcolor (COLOR_FORE);
	if (highlight) {
		tgi_bar(x+1, y+1, x+w-1, y+h-1);
		tgi_setcolor (COLOR_BACK);		
	}
	tgi_outtextxy(x+5, y+h-3, text);
	tgi_setcolor (COLOR_FORE);
	tgi_line(x, y, x+w, y);	
	tgi_line(x, y+h, x+w, y+h);	
	tgi_line(x, y, x, y+h);	
	tgi_line(x+w, y, x+w, y+h);
}

static void DrawList (int x, int y, int w, int h, const char *names[], int sel) 
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

