/*****************************************************************************/
/*                            Memory Management                              */
/*****************************************************************************/

#ifndef DEFINE_MEMORY
#define DEFINE_MEMORY

#include <stdio.h>
#include <em.h>

#include "cc65-libfix8.h"

static int useREU = 0;

static int pageAlloc = 0; 
static int pageSelec = 0;
static int pageMaxim = 0;
static int* pageIntPoint;
static fix8* pageFix8Point;

static void InitMemory() 
{
	printf ("Loading REU Driver...\n");
	
	if (em_load_driver("c64-reu.emd") == EM_ERR_OK) {
		useREU = 1;
		pageMaxim = em_pagecount();
		pageIntPoint = (int*) em_use (0);
		pageFix8Point = (fix8*) em_use (0);
		printf ("RAM Extension Unit: %d * 256 bytes\n", pageMaxim);
		
	} else {
		printf ("RAM Extension Unit: Not found!\n");
	}
}

static void SwitchPage(int page)
{
	em_commit();
	pageIntPoint = (int*) em_map (page);
	pageFix8Point = (fix8*) em_map (page);		
	pageSelec = page;
}

int dump;

static void AllocateFix8(fix8 **pointer, int num)
{
	if (useREU) {
		// Allocate pages in Extended Memory
		dump = pageAlloc;
		(*pointer) = (fix8*) malloc (sizeof(fix8));	
		if (pageAlloc + (4*num/256)+1 < pageMaxim) {
			(*pointer)[0] = pageAlloc;
			pageAlloc += (4*num/256)+1;
		} else {
			printf ("No more memory available!\n");
			(*pointer)[0] = -1;
		}
		printf ("Allocated %d pages of Fix8\n", pageAlloc-dump);
	} else {
		(*pointer) = (fix8*) malloc (num*sizeof(fix8));
	}
}

static void AllocateInt(int **pointer, int num)
{
	if (useREU) {	
		// Allocate pages in Extended Memory
		dump = pageAlloc;
		(*pointer) = (int*) malloc (sizeof(int));	
		if (pageAlloc + (2*num/256)+1 < pageMaxim) {
			(*pointer)[0] = pageAlloc;
			pageAlloc += (2*num/256)+1;
		} else {
			printf ("No more memory available!\n");
			(*pointer)[0] = -1;
		}
		printf ("Allocated %d pages of Int\n", pageAlloc-dump);
	} else {
		(*pointer) = (int*) malloc (num*sizeof(int));
	}
}

static void WriteFix8(fix8 **pointer, int index, fix8 value)
{
	if (useREU) {		
		int page = (*pointer)[0] + ((index*4)/256);
		if (page != pageSelec) { SwitchPage(page); }
		pageFix8Point[((index*4)%256)/4] = value;
	} else {
		(*pointer)[index] = value;
	}
}

static void WriteInt(int **pointer, int index, int value)
{
	if (useREU) {		
		int page = (*pointer)[0] + ((index*2)/256);
		if (page != pageSelec) { SwitchPage(page); }
		pageIntPoint[((index*2)%256)/2] = value;
	} else {
		(*pointer)[index] = value;		
	}
}

static fix8 ReadFix8(fix8 **pointer, int index)
{
	if (useREU) {		
		int page = (*pointer)[0] + ((index*4)/256);
		if (page != pageSelec) { SwitchPage(page); }
		return pageFix8Point[((index*4)%256)/4];
	} else {
		return (*pointer)[index];
	}
}

static int ReadInt(int **pointer, int index)
{
	if (useREU) {		
		int page = (*pointer)[0] + ((index*2)/256);
		if (page != pageSelec) { SwitchPage(page); }
		return pageIntPoint[((index*2)%256)/2];
	} else {
		return (*pointer)[index];
	}
}

#endif