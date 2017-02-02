
#include <cc65.h>
#include <conio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "include/cc65-libfix8.h"
#include "include/cc65-libmouse.h"
#include "include/cc65-libgui.h"

/*****************************************************************************/
/*                                 Main Code                                 */
/*****************************************************************************/

char lastChar;

void geturl();

int main (void) 
{
	// Reset Screen
	clrscr (); gotoxy (0, 0);
	printf ("C64 Browser is Initializing...\n");
	
	// Init Memory
	InitMouse();
	InitTGI();
	
	// Test url stuff
	geturl();
	while (!kbhit()) { }
	cgetc();
			
	// Initialize Screen
	StartTGI();

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
			// Located in GUI part?
			} else {
			}
		} else if (info.buttons & MOUSE_BTN_RIGHT) {
			// Located in viewport part?
			if (info.pos.x < 220) {
			}
		} else {
			// Reset mouse
			mouseX = -1;			
		}
	}
	
	// Turn off everything
	StopMouse();
	StopTGI();
	
    // Done
    return EXIT_SUCCESS;
}
