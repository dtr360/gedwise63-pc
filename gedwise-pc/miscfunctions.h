/////////////////////////////////////////////////////////////////////////////////////////
// Program:			GedWise 7.0
//
// File:			MiscFunctions.h
//
// Written by:		Daniel T. Rencricca
//
// Last Revision:	March 9, 2006
//
// Description:		Header file for MiscFunctions.cpp.
/////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MISC_FUNCTIONS_H_
#define _MISC_FUNCTIONS_H_

#include "defines.h"

#define ISD_ERR_FWRITE	"Fatal Error writing to file: %s\n"
#define ISD_ERR_FOPEN	"Fatal Error opening file: %s\n"
#define ISD_ERR_FREAD	"Fatal Error reading from file: %s\n"
#define ISD_ERR_FUNK	"Unknown Fatal Error accessing file: %s\n"
#define	IDS_TTL_ERROR	"Fatal Error"


wxString	wxFindAppPath (const wxString& argv0, const wxString& cwd, const wxString& appVariableName);
void		FileIOError (char* funcName, wxString& fileName, UINT errType);
void		FileIOError (wxString& errMsg);
char*		GetFileRec (char* buffer, int num, FILE* stream);
char*		GetFileRecAny (char* buffer, int num, FILE* stream);
long		ReplaceTabs (char* str);
long		ReplaceReturns(char* str);
char*		FindUpToNextTab (char** p);
bool		RefFinderStr (UINT keyNumber, char key, char *field, char *xRefStr);
bool		RefFindNum (UINT keyNumber, char key, const char* field, char* xRefStr);
void		ReadRecFromBuf (char* buf, char* aRec[], UINT* recFlds);
UINT		WriteRecToFile (FILE *fp, char* aRec[], UINT recFlds);
void		AddLogEntry (wxString logMessage);
char*		StrToUpr (char *_src);
int			StrIColl (const char *str1, const char *str2 );

#endif // _MISC_FUNCTIONS_H_
