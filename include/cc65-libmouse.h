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
const unsigned char mouse_def_pointercolor = COLOR_BLACK;
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
	
static void StopMouse ()
{
#if DYN_DRV
    /* Uninstall and unload the driver. */
    CheckError ("mouse_unload", mouse_unload ());
#else
    /* Uninstall the static driver. */
    CheckError ("mouse_uninstall", mouse_uninstall ());
#endif
}
