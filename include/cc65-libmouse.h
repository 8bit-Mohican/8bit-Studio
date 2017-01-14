/*****************************************************************************/
/*                                Mouse Library                              */
/*****************************************************************************/

#include <stdio.h>
#include <mouse.h>

struct mouse_info info;
struct mouse_box full_box, small_box;
unsigned char width, height;

int mouseX, mouseY;

#ifdef MOUSE_DRIVER
/* A statically linked driver was named on the compiler's command line.
** Make sure that it is used instead of a dynamic one.
*/
#  undef DYN_DRV
#  define DYN_DRV       0
#else
/* Use a dynamically loaded driver, by default. */
#  ifndef DYN_DRV
#    define DYN_DRV     1
#  endif
#endif

#ifdef __CBM__
/* Set dark-on-light colors. */
const unsigned char mouse_def_pointercolor = COLOR_WHITE;
#endif

static void __fastcall__ CheckError (const char* S, unsigned char Error)
{
    if (Error != MOUSE_ERR_OK) {
        cprintf ("\n%s: %s(%u)\r\n", S, mouse_geterrormsg (Error), Error);

        /* Wait for a key-press, so that some platforms can show the error
        ** message before they remove the current screen.
        */
        if (doesclrscrafterexit ()) {
            cgetc ();
        }
        exit (EXIT_FAILURE);
    }
}

static void InitMouse ()
{
	printf ("Loading Mouse Driver...\n");
#if DYN_DRV
    /* Load and install the driver. */
    CheckError ("mouse_load_driver", mouse_load_driver (&mouse_def_callbacks, mouse_stddrv));
#else
    /* Install the driver. */
    CheckError ("mouse_install", mouse_install (&mouse_def_callbacks,
#  ifdef MOUSE_DRIVER
                               MOUSE_DRIVER
#  else
                               mouse_static_stddrv
#  endif
                               ));
#endif
    /* Get the initial bounding box. */
    mouse_getbox (&full_box);
    screensize (&width, &height);
}
	

	
/*			
	//Save Screen
	mouse_info(&info);	
	mouseX = info.pos.x;
	mouseY = info.pos.y;
	mouseMask[0][0] = tgi_getpixel(mouseX,  mouseY);
	mouseMask[0][1] = tgi_getpixel(mouseX,  mouseY+1);
	mouseMask[1][0] = tgi_getpixel(mouseX+1,mouseY);
	mouseMask[1][1] = tgi_getpixel(mouseX+1,mouseY+1);	
	mouseMask[2][0] = tgi_getpixel(mouseX+2,mouseY+2);
	mouseMask[2][1] = tgi_getpixel(mouseX+3,mouseY+3);		

			//Reset screen
			tgi_setcolor(mouseMask[0][0]);
			tgi_setpixel(mouseX,  mouseY);
			tgi_setcolor(mouseMask[0][1]);
			tgi_setpixel(mouseX,  mouseY+1);
			tgi_setcolor(mouseMask[1][0]);
			tgi_setpixel(mouseX+1,mouseY);
			tgi_setcolor(mouseMask[1][1]);
			tgi_setpixel(mouseX+1,mouseY+1);
			tgi_setcolor(mouseMask[2][0]);
			tgi_setpixel(mouseX+2,mouseY+2);
			tgi_setcolor(mouseMask[2][1]);
			tgi_setpixel(mouseX+3,mouseY+3);				
			
			mouseX = info.pos.x;
			mouseY = info.pos.y;
			mouseMask[0][0] = tgi_getpixel(mouseX,  mouseY);
			mouseMask[0][1] = tgi_getpixel(mouseX,  mouseY+1);
			mouseMask[1][0] = tgi_getpixel(mouseX+1,mouseY);
			mouseMask[1][1] = tgi_getpixel(mouseX+1,mouseY+1);	
			mouseMask[2][0] = tgi_getpixel(mouseX+2,mouseY+2);
			mouseMask[2][1] = tgi_getpixel(mouseX+3,mouseY+3);	
	
			//Show mouse
			tgi_setcolor(COLOR_FORE);
			tgi_bar(info.pos.x, info.pos.y, info.pos.x+1, info.pos.y+1);		
			tgi_line(info.pos.x+2, info.pos.y+2, info.pos.x+3, info.pos.y+3);
*/
