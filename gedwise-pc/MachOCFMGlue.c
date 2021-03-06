/* =============================================================================
	PROJECT:	Cubix (PB1)
	FILE:		MachOCFMGlue.c
	
    PURPOSE:	Code that does the magic needed to call MachO functions
				from CFM and CFM functions from Carbon.
    
    COPYRIGHT:	Copyright (c) 2002 by M. Uli Kusterer, all rights reserved.
				Thanks to George Warner, Chris Silverberg and Ricky Sharp for
				clues on how to do this and implementation snippets.
    
    REVISIONS:
        Fri Jul 26 2002	witness	Created.
   ========================================================================== */

/* -----------------------------------------------------------------------------
    Headers:
   -------------------------------------------------------------------------- */

#include <Carbon/Carbon.h>
#include "MachOCFMGlue.h"


/* -----------------------------------------------------------------------------
    Globals:
   -------------------------------------------------------------------------- */

UInt32 gGlueTemplate[6] = { 0x3D800000, 0x618C0000, 0x800C0000, 0x804C0004,
							0x7C0903A6, 0x4E800420};

/* -----------------------------------------------------------------------------
    CFMFunctionPtForMachOFunctionPtr:
       This function allocates a block of CFM glue code which contains the
	   instructions to call a CFM routine from a MachO application.
	   
	   Syntax analogous to CFMFunctionPointerForMachOFunctionPointer().
		
    REVISIONS:
        Fri Jul 26 2002	witness	Created.
   -------------------------------------------------------------------------- */

void*	MachOFunctionPtrForCFMFunctionPtr( void* inCfmProcPtr )
{
    UInt32	*vMachProcPtr = (UInt32*) NewPtr( sizeof(gGlueTemplate) );	// sizeof() really returns the data size here, not the size of a pointer. Trust me.
	
    vMachProcPtr[0] = gGlueTemplate[0] | ((UInt32)inCfmProcPtr >> 16);
    vMachProcPtr[1] = gGlueTemplate[1] | ((UInt32)inCfmProcPtr & 0xFFFF);
    vMachProcPtr[2] = gGlueTemplate[2];
    vMachProcPtr[3] = gGlueTemplate[3];
    vMachProcPtr[4] = gGlueTemplate[4];
    vMachProcPtr[5] = gGlueTemplate[5];
    MakeDataExecutable( vMachProcPtr, sizeof(gGlueTemplate) );
	
    return( vMachProcPtr );
}


/* -----------------------------------------------------------------------------
    DisposeMachOFunctionPtr:
        Disposes of the fake TVector created by
		CFMFunctionPointerForMachOFunctionPointer().
		
    REVISIONS:
        Fri Jul 26 2002	witness	Created.
   -------------------------------------------------------------------------- */

void DisposeMachOFunctionPtr( void *inMachProcPtr )
{
    if( inMachProcPtr )
		DisposePtr( inMachProcPtr );
}