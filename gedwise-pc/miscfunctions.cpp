/////////////////////////////////////////////////////////////////////////////////////////
// Program:		GedWise 7.0
//				Copyright © 2001 - 2005 Battery Park Software Corporation
//
// Module:		MiscFunctions.cpp
//
// Written by:	Daniel T. Rencricca
//
// Date:		March 9, 2005
//
// Description:	This module contains miscellaneous helper functions for the various
//				GedWise modules.
//
/////////////////////////////////////////////////////////////////////////////////////////
#include "stdwx.h"
#include "miscfunctions.h"
#include "defines.h"

char		cNullStr[2] = "\0";
const char	StrTAB[2] = {CHR_TAB, CHR_NUL};
const char	StrEOL[2] = {CHR_LF, CHR_NUL};

extern	FILE *LogfileP;

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	FileIOError
//
// Description:	Called if an I/O error was found.  It displays an error
//				message and writes the error message to the log file.
//
// Parameters:	-> funcName	=	the name of the function where the error occurred.
//				-> fileName =	the name of the file accessed when error occurred.
//				-> errType  =	the type of error that occurred: 
//								cWriteErr = error writing to a file.
//								cOpenErr  =	error openning a file.
//								cReadErr  =	error reading from a file.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void FileIOError (char* funcName, wxString& fileName, UINT errType)
{
	wxString errMsg;

	switch (errType)
		{
		case cWriteErr:
			errMsg.Printf (ISD_ERR_FWRITE, fileName.c_str());
			break;
		
		case cOpenErr:
			errMsg.Printf (ISD_ERR_FOPEN, fileName.c_str());
			break;
		
		case cReadErr:
			errMsg.Printf (ISD_ERR_FREAD, fileName.c_str());
			break;
		
		default:
			errMsg.Printf (ISD_ERR_FUNK, fileName.c_str());
			break;
		}
	
	AddLogEntry (errMsg);
	wxMessageBox (errMsg, funcName, wxOK | wxICON_ERROR);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	FileIOError
//
// Description:	Called if an I/O error was found.  It displays an error
//				message, writes the error message to the log file and sets
//				the BadExit variable to true and erases all files created.
//
// Parameters:	-> errMsg -	error message to display and write to log file.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void FileIOError (wxString& errMsg)
{
	AddLogEntry (errMsg);
	wxMessageBox (errMsg, IDS_TTL_ERROR, wxOK | wxICON_ERROR);
}


/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GetFileRec
//
// Description:	Gets a  record from the given file stream and loads it into the buffer
//				string. This function eliminates any end-of-line characters and returns
//				the buffer with a string terminator.
//
//				NOTE: Only call this function on files created by GedWise.  Do NOT call
//				it on the GEDCOM files, since there will be CR's in the Notes.
///
// Parameters:	-> buffer	- pointer to a buffer string to be filled from file record.
//				-> num		- number or characters to read from file.
//				-> stream	- file stream from which to read record.
//                                      
// Returns:		A pointer to the buffer, or NULL if at end of file.
/////////////////////////////////////////////////////////////////////////////////////////
char* GetFileRec (char* buffer, int num, FILE* stream) 
{
	int		bPos = 0;
	char	c = CHR_NUL;

	ASSERT (stream);

	if (!stream) return NULL;

	while (bPos < num)  {

		c = (char) fgetc (stream);

		if ((c == CHR_LF) || (c == EOF && feof (stream))) {
			break;
			}

		else {
			buffer[bPos] = c;
			bPos++;
			}
		} 

	buffer[bPos] = CHR_NUL; // add string terminator

	if (feof (stream) && bPos == 0)
		
		return NULL;  // return NULL only when at EOF and nothing in buffer
	
	else
	
		return (buffer);
 }


/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GetFileRecAny
//
// Description:	Gets a  record from the given file stream and loads it into the buffer
//				string. This function eliminates any end-of-line characters and returns
//				the buffer with a string terminator.
//
//				NOTE: Call this function only on a GEDCOM file in order to check for CR
//				characters.  These will only really be on the GEDCOMs created on a MAC.
//				Also, the Mac fget function does strip out the EOL characters.
///
// Parameters:	-> buffer	- pointer to a buffer string to be filled from file record.
//				-> num		- number or characters to read from file.
//				-> stream	- file stream from which to read record.
//                                      
// Returns:		A pointer to the buffer, or NULL if at end of file.
/////////////////////////////////////////////////////////////////////////////////////////

char* GetFileRecAny (char* buffer, int num, FILE* stream) 
{
	int		bPos = 0;
	char	c	 = CHR_NUL;

	ASSERT (stream);

	if (!stream) return NULL;

	while (bPos < num)  {

		c = (char) fgetc (stream);  // fgetc disposes of CR automatically, except on MAC

		if (c == CHR_CR) {
	

			// on Mac's, clear any CL character after the CR
			//#if defined (__WXMAC__)
			
			c = (char) fgetc (stream);
		
			if (c != CHR_LF)
				ungetc (c, stream);
			
			//#endif


			break;
			}

		else if ((c == CHR_LF) || (c == EOF && feof (stream))) {
			break;
			}

		else {
			buffer[bPos] = c;
			bPos++;
			}
		} 

	buffer[bPos] = CHR_NUL; // add string terminator

	if (feof (stream) && bPos == 0)
		
		return NULL;  // return NULL only when at EOF and nothing in buffer
	
	else
	
		return (buffer);
 }


/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ReplaceTabs
//
// Description:	Replaces the tab characters in a string with a '\0' in order
//				to terminate each string of data.  Pointer to beginning of
//				each string must be set before calling this routine.
//			
//				NOTE: 'str' must terminate with NULL.
//
// Parameters:	str  - pointer to a null terminated string,
//                                      
// Returns:		The string after replacement of characters.
/////////////////////////////////////////////////////////////////////////////////////////
long ReplaceTabs (char* str)
{
	char* p;
	p = str;

	// Scan and replace all tabs with a NULL terminator character
	while (*p) {
		if (*p == CHR_TAB)
			*p = CHR_NUL;
		++p;
		}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ReplaceReturns
//
// Description:	Replaces the CR, LF and FF character in a string with a LF character.
//				This function must be called only for a null terminated string. It is
//				only called for Note Recs to make sure there are no bad characters
//				in the note text.
//
// Parameters:	-> str  - pointer to a null terminated string,
//
// Returns:		the string after replacement of characters.
/////////////////////////////////////////////////////////////////////////////////////////
long ReplaceReturns (char *str)
{
	char *p;
	p = str;

	// Scan and replace EOL characters with a CHR_LF terminator character
	while (*p) {
		//if (*p == CHR_CR || *p == CHR_LF || *p == CHR_FF)
		if (*p == CHR_CR || *p == CHR_FF)
			*p = CHR_LF; // Palm OS wants a LF character to start new line
		++p;
		}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	FindUpToNextTab
//
// Description:	Finds the next tab or terminator character in a string and increments
//				the position of the pointer in that string. If pointer is at an end of
//				string/line or newline character, it will not increment any further.
//
//				NOTE: 'p' must not have any EOL characters and must terminate with NULL.
//
// Parameters:	<-> **p  - pointer to a character in a string.
//                                      
// Returns:		A pointer to the character after the next found tab.
/////////////////////////////////////////////////////////////////////////////////////////
char* FindUpToNextTab (char **p)
{
	char* orig = *p;

	// advance to next character until end of line/string or tab character found
	while (**p != CHR_NUL && **p != CHR_TAB) {
		(*p)++;
		}

	// skip the found tab character
	if (**p != CHR_NUL) {
		(*p)++;
		}

	return orig;
}

////////////////////////////////////////////////////////////////////////////////////
// Function: 	RefFinderStr
//
// Description:	Finds the nth (keyNumber) occurrence of the key string within
//              the field string, where the key represents the delimiter of an
//              event (E), souC (S) repC (R) or XRef (@).
//      		      
// Parameters:  -> keyNumber -	nth occurence of key within field.
//              -> key       - one character string to look in field for.
//              -> field     - string in which to look for key
//              <- xRefStr   - string containing the nth key
//
// Returns: 	If unsuccessful a false is returned.
////////////////////////////////////////////////////////////////////////////////////
bool RefFinderStr (UINT keyNumber, char key, char* field, char* xRefStr)
{
	UINT i = 0;
	UINT j = 0;
	UINT counter = 1;
	UINT fieldLength;

	wxASSERT (field);
   
	*xRefStr = CHR_NUL;

   // if no ref to search for stop return false (error).
   if (! field)
      return false;

   fieldLength = strlen (field);
   
   while (i <= fieldLength && counter <= keyNumber) {
      if (key == field[i]) {
         if (counter == keyNumber) {
            i++;
            while (field[i] != key && i < fieldLength) {
               xRefStr[j] = field[i];
               i++; j++;
               }
            xRefStr[j] = CHR_NUL;
            return true;
            }
         counter++;            
         }
      i++;
      }

   return false; 
}

////////////////////////////////////////////////////////////////////////////////////
// Function: 	RefFinderNum
//
// Description:	Finds the nth (keyNumber) occurrence of the key string within
//				the field string, where the key represents the delimiter of an
//				event (E, B or D).  The number immediately following the key is
//				returned.  The field must contain numbers separated by a
//				delimiter 'key'.
//      		      
// Parameters:  -> keyNumber -	nth occurence of key within field.
//              -> key       - one character string to look in field for.
//              -> field     - string in which to look for key
//              <- xRefStr   - string containing the nth key
//
// Returns: 	If unsuccessful a false is returned.
////////////////////////////////////////////////////////////////////////////////////
bool RefFindNum (UINT keyNumber, char key, const char* field, char* xRefStr)
{
   UINT	i = 0;
   UINT j = 0;
   UINT counter = 1;
   UINT fieldLength;

   // if no ref to search for stop return false (error).
   if (! field || ! *field)
      return false;

	*xRefStr = CHR_NUL;

   fieldLength = strlen (field);
   
   while (i <= fieldLength && counter <= keyNumber) {
      if (key == field[i]) {
         if (counter == keyNumber) {
            i++;
				while ((field[i] >= '0' && field[i] <= '9') && i <= fieldLength) {
               xRefStr[j] = field[i];
               i++; j++;
               }
            xRefStr[j] = CHR_NUL;
            return true;
            }
         counter++;            
         }
      i++;
      }

   return false; 
}

////////////////////////////////////////////////////////////////////////////////////
// Function: 	WriteRecToFile
//
// Description:	Writes the data in a record to a file.
//      		      
// Parameters:  -> fp		-	pointer to open file to write to.
//				-> aRec		-	record to write to file.
//              -> recFlds	-	Number of fields to write to file.
//
// Returns:		True if successful, else false.
////////////////////////////////////////////////////////////////////////////////////
UINT WriteRecToFile (FILE *fp, char* aRec[], UINT recFlds)
{
	UINT i;

	for (i = 0; i < recFlds; i++) {
		if (i > 0)
			if (fprintf (fp,  StrTAB) < 0) // add tab separator
				return false;
		
		if (fprintf (fp, "%s",	aRec[i]) < 0) // add record data
				return false;
		}

	if (fprintf (fp, StrEOL) < 0) // add the newline character to terminate record
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////
// Function: 	ReadRecFromBuf
//
// Description:	Reads record data from a buffer string.  This routine will make
//				sure every pointer either points to a data field in "buf" or a
//				globally defined empty string, "cNullStr".
//      		      
// Parameters:  -> buf	  -	pointer to open file to write to.
//				-> aRec	  -	record to write to file.
//              -> recFlds -	Number of fields read from file.
//
// Returns: 	True if successful, else false.
//
// Revisions:	None.
////////////////////////////////////////////////////////////////////////////////////
void ReadRecFromBuf (char* buf, char* aRec[], UINT* recFlds)
{
	char* p = buf;

	wxASSERT (buf);

	*recFlds = 0;

	// initialize pointers
	for (UINT i = 0; i < MAX_FLDS; i++)
		aRec[i] = cNullStr;

	if (! buf) return;

	while (*recFlds < MAX_FLDS) {

		aRec[*recFlds] = FindUpToNextTab (&p);
		
		ASSERT (*aRec[*recFlds] != CHR_LF);
		
		//if (*aRec[*recFlds] == CHR_EOL)
		//	break;
		
		(*recFlds)++;
		}

	ReplaceTabs (buf); // replace tabs with null terminators
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	AddLogEntry
//
// Description:	Adds a message to the log file and displays a message if there is 
//				an error writing to the file.
//
// Inputs:		-> logMessage - Message to add to the log file.
//
// Returns:		Nothing.
//
// Revisions:	None.
/////////////////////////////////////////////////////////////////////////////////////////
void AddLogEntry (wxString logMessage)
{
	// do nothing if log file is not open
	if (!LogfileP) {
		wxASSERT (false);
		return;
		}

	fputs (logMessage.c_str(), LogfileP); // changed from fprinf on 4/2/2007
	//fprintf (LogfileP, logMessage.c_str());
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	StrToUpr
//
// Description:	Converts a string to uppercase letters.
//
// Inputs:		<> src	-	string to convert.
//
// Returns:		Pointer for string.
//
// Revisions:	None.
/////////////////////////////////////////////////////////////////////////////////////////
char* StrToUpr (char *src)
{
	char *c;

	for (c = src; *c; c++) {
		*c = toupper (*c);
		}

	return src;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	wxFindAppPath
//
// Description:	Finds the absolute path where the application is located.
//
// Inputs:		-> argv0			- wxTheApp->argv[0]
//				-> cwd				- current working directory at startup
//				-> appVariableName	- name of a variable containing the directory
//
// Returns:		a string containing the absolute path where the application is located
//
// Revisions:	None.
/////////////////////////////////////////////////////////////////////////////////////////
wxString wxFindAppPath (const wxString& argv0, const wxString& cwd, const wxString& appVariableName)
{
    wxString str;

    // Try appVariableName
    if (!appVariableName.IsEmpty())
    {
        str = wxGetenv (appVariableName);
        if (!str.IsEmpty())
            return str;
    }

	#if defined (__WXMAC__) && !defined (__DARWIN__)
    // On Mac, the current directory is the relevant one when
    // the application starts.
    return cwd;
	#endif

    if (wxIsAbsolutePath(argv0))
        return wxPathOnly(argv0);
    else
    {
        // Is it a relative path?
        wxString currentDir (cwd);
        if (currentDir.Last() != wxFILE_SEP_PATH)
            currentDir += wxFILE_SEP_PATH;

        str = currentDir + argv0;
        if (wxFileExists(str))
            return wxPathOnly(str);
    }

    // OK, it's neither an absolute path nor a relative path.
    // Search PATH.

    wxPathList pathList;
    pathList.AddEnvList ("PATH");
    str = pathList.FindAbsoluteValidPath (argv0);

    if (!str.IsEmpty())
        return wxPathOnly(str);

    // Failed
    return wxEmptyString;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	StrIColl
//
// Description:	Performs a case-insensitive comparison of str1 and str2 according to
//				the code page currently in use. This functions should be used only when
//				there is a difference between the character set order and the
//				lexicographic character order in the current code page and this
//				difference is of interest for the string comparison.
//
// Inputs:		-> src1	-	first string to compare.
//				-> src2	-	second string to compare.
//
// Returns:		<0	if str1 less than str2 
//				0	str1 identical to str2 
//				>0	str1 greater than str2 
//
// Revisions:	None.
/////////////////////////////////////////////////////////////////////////////////////////
/*int StrIColl (const char *str1, const char *str2 )
{
//char str1Trx[300] = "";
//char str2Trx[300] = "";

	//using namespace std;

	// Obtain a reference to the collate facet in
	// the default locale imbued in cout.
	//const collate<char>& co = _USE(cout.getloc(),  collate<char>);

	//const collate<char>& co =
    //      use_facet(cout.getloc(),(collate<char>*)0, true);

//		const char* str1End = str1 + strlen(str1);
//		const char* str2End = str2 + strlen(str2);

		
//string	str1Trn = str1;
//string	str2Trn = str2;

//str1Trn = co.transform (str1Trn.begin(), str1Trn.end());
//str2Trn = co.transform (str2Trn.begin(), str2Trn.end());

//int z = strxfrm (str1Trx, "Test", 300);


//int x = strxfrm (str1Trx, str1, 300);
//int y = strxfrm (str2Trx, str2, 300);

//int retxfrm= strcmp(str1Trx, str2Trx); 

//	int result = co.compare (str1Trn.begin(), str1Trn.end(), str2Trn.begin(), str2Trn.end());
	//int result = co.compare (str1, str1End, str2, str2End);

//	#if defined (_DEBUG) && defined (WIN32)
//	int result1 = _stricoll (str1, str2);
//	int result2 = strcoll (str1, str2);

//	if (result1 != result2)
//		int test2 = 0xfffff;

//	ASSERT (_stricoll (str1, str2) == result);
//	#endif

	return strcoll (str1, str2);;

}

  long ReplaceTabs (char* str)
{
	char* p;
	p = str;

	// Scan and replace all tabs with a NULL terminator character
	while (*p) {
		if (*p == CHR_TAB) || *p == CHR_EOL)
			*p = CHR_NUL;
		++p;
		}

	return 0;
}
 


  char* GetFileRec (char* buffer, int num, FILE* stream) 
{
	char*	fErr = NULL;
	int		bPos = 0;


	ASSERT (stream);

	if (!stream) return NULL;


	fErr = fgets (buffer, num, stream); // read the record up to the '\n'

	// remove any line-feed and carriage-return characters
	bPos = strlen (buffer) - 1; // skip string terminator
	
	while ((buffer[bPos] == CHR_LF || buffer[bPos] == CHR_CR) && bPos >= 0) {
		buffer[bPos] = CHR_NUL;
		bPos--;
		}

	return fErr;

 }

*/



