/////////////////////////////////////////////////////////////////////////////////////////
// Program:			GedWise 6.0
//
// Module:			SortRoutines.cpp
//
// Written by:		Daniel T. Rencricca
//
// Last Revision:	April 26, 2004
//
// Description:		This program uses a polyphase mergesort to sort a text file. Each
//					line of text in the file is assumed to be a seperate record and must
//					end with	an endline ('\n') character. Additionally, it assumes that
//					there is a string of numbers at a specific position within each line
//					of text that is used as the sort key for that line of text. It does
//					the sort by performing the following steps: (1)create a total of
//					MAX_SRT_FILES sort files to be used to temporarily hold text data;
//					(2) read up to MAX_ARR_SZ lines of a text file into a buffer array,
//					where each element of the array consists of the text string and an
//					integer key that is copied from the text string; (3) sort the array
//					by the key; (4) find the lowest key in the buffer that is greater
//					than the last key read, and copy the text string associated with that
//					key from the buffer to a temporary sort file; (5) read in one new
//					line of text into the buffer and resort the buffer; (7) repeat
//					from step 3 until there are no more keys left that are greater than
//					the last key read; (8) repeat from step 2 until the last sort file
//					has been filled; (9) read the first text string from each of
//					the sort files into a tempfile array and get the integer key from
//					each line of text; (10) Find the lowest key in the tempfile array
//					and write the associated text string into a temporary holder file;
//					(11) read a new text string from the sort file which previously had
//					the lowest key and get the key from that string; (12) repeat from
//					step 10 until all sort files have been fully read; (13) erase
//					the sort files and repeat from step 1 until the input file has been
//					fully read.
/////////////////////////////////////////////////////////////////////////////////////////
#include "stdwx.h"
#include "sortroutines.h"
#include "miscfunctions.h"
#include "parsegedcom.h"
//#include <locale.h>

#ifdef _DEBUG // include after last #include statement in cpp file where 'new' is used.
#define new DEBUG_NEW
#endif 

#define _THIS_FILE	"SortRoutines"

#ifdef _DEBUG
UINT OrgLineCnt;
UINT UpdateCnt;
#endif

extern DBFileRec	DBFiles[][DBCount];


/////////////////////////////////////////////////////////////////////////////////////////
// Function:		SortRoutines
//
//	Description:	Constructor for SortRoutines.
//
// Parameters:		pParent - parent.
//
// Returns:			Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
SortRoutines::SortRoutines (GedWiseFrame* pParent)
{
	pGedWiseFrame = pParent;
	
	#ifdef _DEBUG
	OrgLineCnt	= 0;
	UpdateCnt	= 0;
	#endif

	// initialize variables
	m_iBufArrSz		= 0;
	m_iSrtFlArrSz	= 0;
	m_bUseLocale	= false;
	m_aBufArr		= NULL;
	m_aSrtFlArr		= NULL;
	m_fpInfile		= NULL;
	m_sHoldFile		= pGedWiseFrame->GetWorkDirectory () + HLDFILE;

	// Initialize for GetIndxKey function
	memset (m_szPadStrZ, '0', RNUM_SZ);	// initialize with zero character
	m_szPadStrZ[RNUM_SZ] = CHR_NUL;		// terminate with NULL
	
	// make space on heap for m_aBufArr and m_aSrtFlArr arrays
	AllocateBufArr (BUF_ARR_SZ);
	AllocateSrtFlArr (SRT_FL_ARR_SZ);

	// initialize m_aSrtFlArr array file pointers
	for (int x = 0; x < m_iSrtFlArrSz; x++)
		m_aSrtFlArr[x]->fp = NULL; 
	
	// remove in case prior failed processing
	remove (m_sHoldFile.c_str());

	//#if defined (__WXMSW__)
	//setlocale (LC_ALL, "English_United States.1252"); // windows
	//#else
	//cLoc = setlocale (LC_ALL, "en_US.iso8859-1"); // linux
	//#endif
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	~SortRoutines
//
// Description:	Destructor for SortRoutines. Close all sort files and deallocate
//				temporary arrays.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
SortRoutines::~SortRoutines ()
{
	DeleteSortFiles ();
	
	if (m_fpInfile)
		fclose (m_fpInfile);
	
	remove (m_sHoldFile.c_str());
	
	DeallocateBufArr (m_iBufArrSz);

	DeallocateSrtFlArr (m_iSrtFlArrSz);
}

/////////////////////////////////////////////////////////////////////////////////////////
// MEMORY ALLOCATION SUBROUTINES																			//
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	AllocateBufArr
//
// Description:	Allocates room on the heap for the m_aBufArr array. If there is 
//				insufficient memory, it attempts to obtain 80% of amount 
//				reached on first attempt.
//
// Parameters:	-> maxSz - size of array we want to allocate.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void SortRoutines::AllocateBufArr (int maxSz)
{
	try {
		m_aBufArr = new BufRecType*[maxSz];
		for (m_iBufArrSz = 0; m_iBufArrSz < maxSz; m_iBufArrSz++)
 			m_aBufArr[m_iBufArrSz] = new BufRecType;
		}
	
	catch (...) {
		DeallocateBufArr (m_iBufArrSz);
		maxSz = (int) (m_iBufArrSz * .80); // try smaller size
		m_aBufArr = new BufRecType*[maxSz];
		for (m_iBufArrSz = 0; m_iBufArrSz < maxSz; m_iBufArrSz++)
 			m_aBufArr[m_iBufArrSz] = new BufRecType;
		}

	wxASSERT (maxSz == m_iBufArrSz);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	AllocateSrtFlArr
//
// Description:	Allocates room on the heap for the m_aSrtFlArr array. If there is 
//				insufficient memory, it attempts to obtain 80% of amount 
//				reached on first attempt.
//
// Parameters:	-> maxSz - size of array we want to allocate.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void SortRoutines::AllocateSrtFlArr (int maxSz)
{
	try {
		m_aSrtFlArr = new SrtFlRecType*[maxSz];
		for (m_iSrtFlArrSz = 0; m_iSrtFlArrSz < maxSz; m_iSrtFlArrSz++)
 			m_aSrtFlArr[m_iSrtFlArrSz] = new SrtFlRecType;

		wxASSERT (maxSz == m_iSrtFlArrSz);
		}
	
	catch (...) {
		DeallocateSrtFlArr (m_iSrtFlArrSz);
		maxSz = (int) (m_iSrtFlArrSz * .80); // try smaller array
		m_aSrtFlArr = new SrtFlRecType*[maxSz];
		for (m_iSrtFlArrSz = 0; m_iSrtFlArrSz < maxSz; m_iSrtFlArrSz++)
 			m_aSrtFlArr[m_iSrtFlArrSz] = new SrtFlRecType;
		}

}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	DeallocateBufArr
//
// Description:	Deletes the m_aBufArr array.
//
// Parameters:	-> bufArrSz	- number of elements in Buffer array.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void SortRoutines::DeallocateBufArr (UINT bufArrSz)
{
   wxASSERT (m_aBufArr != NULL); // should always exist

	if (m_aBufArr) {
	
		for (UINT i = 0; i < bufArrSz; i++)
			delete[] m_aBufArr[i];
   
		delete[] m_aBufArr;

		m_aBufArr = NULL;
		}
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	DeallocateSrtFlArr
//
// Description:	Deletes the m_aSrtFlArr array.
//
// Parameters:	-> srtFlArrSz	-	number of items in m_aSrtFlArr array.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void SortRoutines::DeallocateSrtFlArr (int srtFlArrSz)
{
   wxASSERT (m_aSrtFlArr != NULL); // should always exist

	if (m_aSrtFlArr) {
	
		for (int i = 0; i < srtFlArrSz; i++)
			delete[] m_aSrtFlArr[i];
   
		delete[] m_aSrtFlArr;

		m_aSrtFlArr = NULL;
		}
}

/////////////////////////////////////////////////////////////////////////////////////////
// FILE MANAGEMENT SUBROUTINES																			//
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	FileIOError
//
// Description:	Called if an I/O error was found.  It displays an error
//				message, writes the error message to the log file, sets
//				the BadExit variable to true and erases all files created.
//
// Parameters:	msg	- error message to display and write to log file.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void SortRoutines::FileIOError (wxString& msg)
{
	wxMessageBox (msg, _THIS_FILE, wxOK | wxICON_ERROR);
	AddLogEntry (msg);
 	pGedWiseFrame->SetBadExit ();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	InitTempFiles
//
// Description:	Initialize merge files by creating up to m_iSrtFlArrSz temporary sort
//				files to be used to hold the runs of data read from the input file.
//
// Parameters:	-> startFileN - starting sort file number to create.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
bool SortRoutines::InitTempFiles (int startFileN)
{
	wxString filePathName;

  	// create names for temporary merge sort files.
	for (int x = startFileN; x < m_iSrtFlArrSz; x++) {
		
		sprintf (m_aSrtFlArr[x]->name, SRTFILE, x); // create names for temporary files.
		
		filePathName = pGedWiseFrame->GetWorkDirectory () + m_aSrtFlArr[x]->name;

		if ((m_aSrtFlArr[x]->fp = fopen (filePathName.c_str(), "w+t")) == NULL) {
			wxString msg;
			msg.Printf (cErrFileOpen, "SR01", filePathName.c_str());
			FileIOError (msg);
			return false;
       	}
   	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	DeleteSortFiles
//
// Description:	Delete temporary merge files that were created.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void SortRoutines::DeleteSortFiles (void) 
{
	int			x;
	wxString	filePathName;

  	for (x = 0; x < m_iSrtFlArrSz; x++) {
     	
		if (m_aSrtFlArr[x]->fp) {
        	
			fclose (m_aSrtFlArr[x]->fp); // close stream
			m_aSrtFlArr[x]->fp = NULL;

			filePathName = pGedWiseFrame->GetWorkDirectory () + m_aSrtFlArr[x]->name;
			remove (filePathName.c_str()); // erase file
			}
		}

	return;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	TermTmpFiles
//
// Description:	Clean up temporary merge sort files. Create a temporary Holder file 
//				to hold sorted data.
//
//	Parameters:	None.
//
//	Returns:	Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
bool SortRoutines::TermTmpFiles (void) 
{
	wxString	filePathName;
	wxString	msg;
	int			lastPos = m_iSrtFlArrSz - 1; // do not erase last array item
	UINT		i = 0;

	AddLogEntry ("S-"); // track number of times sort files created

	// Close the last sort file (as it will be renamed)
	if (m_aSrtFlArr[lastPos]->fp) {
	
		if (fclose (m_aSrtFlArr[lastPos]->fp)) {
			msg.Printf (cErrFileClose, "SR02a", m_aSrtFlArr[lastPos]->name);
			AddLogEntry (msg);
			wxASSERT (false);
			// this should never happen, but don't make it fatal error for now
			}
			
		m_aSrtFlArr[lastPos]->fp = NULL;
		}

	// Rename the m_aSrtFlArr[lastPos] file to _holder.dat file.
	// Allow 6 attempts to rename file, as some RAID systems cannot keep up with file I/O.
	filePathName = pGedWiseFrame->GetWorkDirectory () + m_aSrtFlArr[lastPos]->name;
	
	while (rename (filePathName.c_str(), m_sHoldFile.c_str())) {
		
		msg.Printf (cAttemptRename, "SR02b");
		AddLogEntry (msg);
		
		if (i > 5) {
			msg.Printf (cErrFileRen, "SR02c", filePathName.c_str());
			FileIOError (msg);
			return false;
			}
		i++;
		}

   // Delete all merge sort files.
	DeleteSortFiles ();

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	RewindF
//
// Description:	Set pointer for m_aSrtFlArr[pos] to first record and read first
//				record key into m_aSrtFlArr[pos]. Also initializes the eof field
//				for m_aSrtFlArr[pos].
//
// Paramters:	-> pos		-	position within m_aSrtFlArr to read.
//				-> fileNo	-	determines which key to used, depending on the 
//								file being sorted.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
bool SortRoutines::RewindF (const int pos, const UINT fileNo)
{
	m_aSrtFlArr[pos]->eof = false;
	
	rewind (m_aSrtFlArr[pos]->fp);

	// Read next data element from the merge file m_aSrtFlArr[x].rec.dataLine
	if (fgets (m_aSrtFlArr[pos]->rec.dataLine, sizeof (DataLineType),
		m_aSrtFlArr[pos]->fp) == NULL) {
		
		if (feof (m_aSrtFlArr[pos]->fp)) // if we are at end of this m_aSrtFlArr
     		m_aSrtFlArr[pos]->eof = true; // if yes, then mark file as done.
		else {
			wxString msg;
			msg.Printf (cErrFileRead, "SR05a", m_aSrtFlArr[pos]->name);
			FileIOError (msg);
			return false;
      	}
		}
	else {
		strcpy (m_aSrtFlArr[pos]->rec.key, GetKey (m_aSrtFlArr[pos]->rec.dataLine,
			fileNo));
		}

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
// FILE SORTING SUBROUTINES  													 							//
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GetIndiKey
//
// Description:	Parses a line of text to retreive the sort key for that line.  The
//				sort key is assumed to be located at position 0 in each string of
//				text.  The key is made up of an individual's surname 	+ first name
//				+ title + birth year.  
//
// Parameters:	-> dataLine	 -	the line of text to be parsed.
//
// Returns:		An string of text representing the sort key.	
/////////////////////////////////////////////////////////////////////////////////////////
char* SortRoutines::GetIndiKey (const char *dataLine) 
{
	UINT	x=0;			// position in dataLine
	UINT	y;				// position in sort key
	UINT	dataLineLen;	// length of line read from file
	UINT	cnt = 0;		// counter
	
	#define	SUR_NAME_ST	0						// start position of given name
	#define	GIV_NAME_ST	NAME_SZ					// start position of surname
	#define	TITLE_ST	GIV_NAME_ST + NAME_SZ	// start position of title
	#define	BIR_YR_ST	TITLE_ST + TIT_SZ		// start position of birth year

	// init m_szIndiSortKey: we must use 0' chars b/c strcoll ignores ' ' chars
	memset (m_szIndiSortKey, '0', sizeof (m_szIndiSortKey));

	dataLineLen = strlen (dataLine) - 1;
	
	y = SUR_NAME_ST;

	// Read surname until we reach the tab.
	while (dataLine[x] != CHR_TAB && x < dataLineLen) {
		
		wxASSERT (dataLine[x] != CHR_LF);

		if (y < SUR_NAME_ST + NAME_SZ) {

			m_szIndiSortKey[y] = dataLine[x];

			y++;
			}

		x++;
      }
	
	x++; // skip past tab
	y = GIV_NAME_ST;

	// Read first name until we reach the tab.
	while (dataLine[x] != CHR_TAB && x < dataLineLen) {
		
		wxASSERT(dataLine[x] != CHR_EOL);

		if (y < GIV_NAME_ST + NAME_SZ) {
			m_szIndiSortKey[y] = dataLine[x];
			y++;
			}

		x++; 
      }

	x++; // skip past tab
	y = BIR_YR_ST;

	// Read lifespan until tab. Once we have read 4 characters, ignore the rest
	// of the field. The lifespan field's birth year will either be 4 digits
	//	(followed by a "/") or 0 digits (and begin with a "/"). 
	// Any birth years less than 1000 will have already been padded with 0's in
	// the GedWise Parser routines.
	while (dataLine[x] != CHR_TAB && x < dataLineLen) {
		wxASSERT(dataLine[x] != CHR_EOL);
		if (y < BIR_YR_ST + BIRY_SZ) {
			m_szIndiSortKey[y] = dataLine[x];
			y++;
			}
      x++;
      }
	
	x++; // skip past tab

	// Skip the sex, alias and xref fields by counting 3 CHR_TABs.
	while (cnt < 3 && x < dataLineLen) {
		wxASSERT(dataLine[x] != CHR_EOL);
		if (dataLine[x] == CHR_TAB) {
			cnt++;
			}
		x++;
      }

	y = TITLE_ST;

	// Read title until we reach the tab.
	while (dataLine[x] != CHR_TAB && x < dataLineLen) {
		wxASSERT(dataLine[x] != CHR_EOL);
		if (y < TITLE_ST + TIT_SZ) {
			m_szIndiSortKey[y] = dataLine[x];
			y++;
			}
		x++; 
      }

	m_szIndiSortKey[BIR_YR_ST + BIRY_SZ] = CHR_NUL;	// terminate with NULL

	wxASSERT (dataLine[dataLineLen] == CHR_EOL);
	wxASSERT (m_szIndiSortKey[strlen (m_szIndiSortKey)] == CHR_NUL);
	
	return (char*) m_szIndiSortKey;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GetFirstKey
//
// Description:	Parses a line of text to retreive the sort key for that line. The sort
//				key is assumed to be located at position 0 in each dataLine. The key
//				ends at the first tab encountered in the dataLine.
//
// Parameters:	-> dataLine	-	the line of text to be parsed.
//
// Returns:		A pointer to a string containing the sort key.
/////////////////////////////////////////////////////////////////////////////////////////
char* SortRoutines::GetFirstKey (const char *dataLine) 
{
   UINT	x = 0;

   // read until tab character found
   while ((dataLine[x] != CHR_TAB) && x < strlen (dataLine)) {
     
		m_szFirSortKey[x] = dataLine[x];
		x++;
		if (x == sizeof (m_szFirSortKey) - 1)
		break;
      }

	m_szFirSortKey[x] = CHR_NUL; // terminate

	return (char*) m_szFirSortKey;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GetIndxKey
//
// Description:	Parses a line of text from IndexDB file to retreive the sort
//				key for that line.  The key is made up of the XRef identifier
//				+ alias flag + record position number.  The alias flag is 
//				either a 0 (for primary name) or 1 for alias name.  The 
//				record position number (rPosKey) is the cross reference to 
//				the record number in the IndiDB file for the given XRef.
//
// Inputs:		-> dataLine	 -	the line of text to be parsed.
//
// Returns:		A pointer to a string containing the sort key.
/////////////////////////////////////////////////////////////////////////////////////////
char* SortRoutines::GetIndxKey (const char* dataLine) 
{
	UINT	y = 0;
	UINT	x = 0;
	char	rPosKeyT[RNUM_SZ+1];	// record number in IndiDB
	UINT	dataLineLen;	// length of line read from file

	#define	AFLG_ST	XREF_SZ	
	#define	RNUM_ST	XREF_SZ + AFLG_SZ

	memset (m_szIndxSortKey, ' ', RNUM_ST);
	dataLineLen = strlen (dataLine) - 1;
	
	// Read individual reference number until we reach the tab.
	while (dataLine[x] != CHR_TAB && x < dataLineLen) {
		wxASSERT (dataLine[x] != CHR_EOL);
      if (y < XREF_SZ) {
			m_szIndxSortKey[y] = dataLine[x];
			y++;
			}
		x++;
      }
	
	x++; // skip past tab
	y = 0;
	
	// Read record number until we reach the tab.
	while (dataLine[x] != CHR_TAB && x < dataLineLen) {
		wxASSERT(dataLine[x] != CHR_EOL);
		if (y < RNUM_SZ) {
			rPosKeyT[y] = dataLine[x];
			y++;
			}
		x++; 
      }

	rPosKeyT[y] = '\0';

	strcpy (&m_szIndxSortKey[RNUM_ST], m_szPadStrZ);
	strcpy (&m_szIndxSortKey[RNUM_ST + RNUM_SZ - strlen (rPosKeyT)], rPosKeyT);

	x++; // skip past tab

	// Read 1-character alias flag
	m_szIndxSortKey[AFLG_ST] = dataLine[x];
	
	m_szIndxSortKey[RNUM_ST + RNUM_SZ] = CHR_NUL; // terminate with NULL

	return (char*) m_szIndxSortKey;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GetKey
//
// Description:	Parses a line of text to retreive the sort key for that line.
//				The sort key is assumed to be located at position 0 in each
//				string of text.
//
// Parameters:	-> dataLine	 -	the line of text to be parsed.
//				-> fileNo	 -	specifies which key to return, depending on which file
//								is being sorted.
//
// Returns:		A pointer to a string containing the sort key.
/////////////////////////////////////////////////////////////////////////////////////////
char* SortRoutines::GetKey (char *dataLine, const UINT fileNo) 
{
	char	*key = NULL;

	wxASSERT (*dataLine);

	switch (fileNo)
		{
		case IndiDB:
			key = GetIndiKey (dataLine);
			break;

		case IndxDB:
			key = GetIndxKey (dataLine);
			break;

		default:
			key = GetFirstKey (dataLine);
			break;
		}

	return key;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	SortList
//
// Description:	Sorts the buffer array in descending order (e.g. ZXYWVU...). It bases
//				the sort on the "key" variable within each array element.
// 
// Parameters: 	-> totBufSz -	holds a count of the number of items in the
//         						buffer array so that we don't needlessly sort the
//								entire buffer if it is not full.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void SortRoutines::SortList (int totBufItems) 
{
	int				x, y;
	BufRecType*		holder;
	
	for (x = 0; x < totBufItems-1; x++) {
		wxASSERT (totBufItems > 0);
		for (y = x+1; y < totBufItems; y++) {
			if (StrCmp (m_aBufArr[y]->key, m_aBufArr[x]->key) > 0) {
			 	holder = m_aBufArr[x];
				m_aBufArr[x] = m_aBufArr[y];
				m_aBufArr[y] = holder;
      			}
			}
		}

	return;
} 

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	SortListIncr
//
// Description:	Sorts the buffer array in descending order (e.g. ZXYWVU...). It
//				assumes that only one array element (at position 'pos') is out of order.
// 
// Parameters: 	-> totBufSz	-	holds the count of the number of items in the
//         						m_aBufArr array so that we don't needlessly sort the
//								entire m_aBufArr array if it is not full.
//				-> pos		-	position of new data element added to the m_aBufArr
//								array.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void SortRoutines::SortListIncr (const int totBufSz, int pos) 
{
   BufRecType*		holder;

	if (totBufSz <= 0)
		return;

	// check if new array item needs to move up in the array.
	while (pos > 0 && StrCmp (m_aBufArr[pos]->key, m_aBufArr[pos-1]->key) > 0) {
   	holder = m_aBufArr[pos];
      m_aBufArr[pos] = m_aBufArr[pos-1];
      m_aBufArr[pos-1] = holder;
		pos--;
		}

	// check if new array item needs to move up in the array.
	while (pos < totBufSz-1 && StrCmp (m_aBufArr[pos]->key, m_aBufArr[pos+1]->key) < 0) {
   	holder = m_aBufArr[pos];
      m_aBufArr[pos] = m_aBufArr[pos+1];
      m_aBufArr[pos+1] = holder;
		pos++;
		}

	return;
} 

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	AddToBuffer
//
// Description:	Add a line(s) of text to buffer array as well as the corresponding
//				integer key for each line.  It also sorts the buffer and keeps track
//				of the number of elements in the buffer.
//
// Parameters:	-> pos		-	position in buffer array to add a new number.  If
//					 			position = -1 then start new array.
//				<> totBufSz	- 	keeps a count of the number of items in the
//               				buffer array so that we don't needlessly sort the
//								entire buffer if not full.
//				-> fileNo	-	specifies which key to return, depending on which file
//								is being sorted.
//
// Returns:		True if successful, else false only if error occurs.
/////////////////////////////////////////////////////////////////////////////////////////
bool SortRoutines::AddToBuffer (int pos, int *totBufSz, const UINT fileNo) 
{
   int	x = 0;

	if (pos == -1) { // start fresh by filling the entire buffer array.

		while (x < m_iBufArrSz) {
			
			// read next line of data
			if (fgets (m_aBufArr[x]->dataLine, MAX_LN_SZ, m_fpInfile) == NULL) {
     			if (feof (m_fpInfile)) {
					SortList (*totBufSz);
         		return true;
					}
				else {
					wxString msg;
					msg.Printf (cErrFileRead, "SR03a", "Input");
					FileIOError (msg);
					return false;
      			}
				}

			#ifdef _DEBUG
			if ((m_iLineTot & PROG_DIV) == 0)
				UpdateCnt++;
			#endif

			if ((m_iLineTot & PROG_DIV) == 0) {
				pGedWiseFrame->CheckStatus ();
				if (pGedWiseFrame->Canceled ()) {
					return false;
					}
				}

			// Get the key for current record.
			strcpy (m_aBufArr[x]->key, GetKey (m_aBufArr[x]->dataLine, fileNo));
			wxASSERT (m_aBufArr[x]->key);

    		m_iLineTot++;  // update line counter for log entry.
			(*totBufSz)++; // keep count of buffer elements
			x++;
			} // while (x < m_iBufArrSz)

		SortList (*totBufSz);
		} // (pos == -1)

	else { // just replace one element in array.
   
		wxASSERT (*totBufSz > 0);

		if (fgets (m_aBufArr[pos]->dataLine, MAX_LN_SZ, m_fpInfile) == NULL) {
     		if (feof (m_fpInfile)) {
     			*m_aBufArr[pos]->key = '\0'; // empty key so it sorts to bottom
				SortListIncr (*totBufSz, pos);
   			(*totBufSz)--; // reduce count of buffer elements
     			return true;
     			}
			else {
				wxString msg;
				msg.Printf (cErrFileRead, "SR03b", "Input");
				FileIOError (msg);
				return false;
     			}
			}
		
		#ifdef _DEBUG
		if ((m_iLineTot & PROG_DIV) == 0)
			UpdateCnt++;
		#endif
		
		if ((m_iLineTot & PROG_DIV) == 0) {
			pGedWiseFrame->CheckStatus ();
			if (pGedWiseFrame->Canceled ()) {
				return false;
				}
			}

		// Get the key for current record.
		strcpy (m_aBufArr[pos]->key, GetKey (m_aBufArr[pos]->dataLine, fileNo));
		wxASSERT (m_aBufArr[pos]->key);

		m_iLineTot++; // update line counter.

		SortListIncr (*totBufSz, pos);
		} // else
 	
	return true; // if we reached here then we are not at end of input file.
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	FindLowest
// 
// Description:	Find the lowest key in the BufferStr that is greater than lowStr.
//				It uses the stricmp function to properly handle strings with
//				international characters.
//
// Parameters:	<> pos			-	the position in the m_aBufArr array where lowStr
//									was found.
//   			<> lowStr		-	the lowest number in m_aBufArr that is >= the
//      							highest number in the current run.
//				-> totBufSz		-	total current items in m_aBufArr.
//
// Returns:		True if found, else false if not found.
/////////////////////////////////////////////////////////////////////////////////////////
bool SortRoutines::FindLowest (int* pos, char* lowStr, UINT totBufSz)
{
	int	x = totBufSz-1; // adj. for first array item at position 0

	wxASSERT (totBufSz > 0);

	while (x >= 0) {
		if (StrCmp (m_aBufArr[x]->key, lowStr) >= 0)	{
      		strcpy (lowStr, m_aBufArr[x]->key);
      		*pos = x;
			return true;
      		}
   		x--;
		}  // while (x >= 0)

  	// Lowest was not found.
	*lowStr = '\0'; // reset the low number for the next sort file
  	return false;   // we have to start filling a new sort file
} 

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	MergeSort
//
// Description:	MergeSort performs the following tasks:
//				(1) read the first text string from each of the sort files
//				into a m_aSrtFlArr array and get the integer key from each
//				line of text; (2) Find the lowest key in the m_aSrtFlArr array
//				and write the associated text string into a temporary holder
//				file; (3) read a new text string from the sort file which
//				previously had the lowest key and get the key from that
//				string; (4) repeat from step 1 until all sort files have
//				been fully read.
//
//	Parameters:	-> fileNo -	determines which key to used, depending on the 
//							file being sorted.
//
//	Returns:	True if successful, else false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool SortRoutines::MergeSort (const UINT fileNo)
{
	int			k;
	int			x;
	wxString	msg;

   // Prime the files and get first data line & key into m_aSrtFlArr array.
	for (x = 0; x < m_iSrtFileN; x++)
		RewindF (x, fileNo);

	while (true) {
    	
		// First, find the smallest key.
		k = -1;
		for (x = 0; x < m_iSrtFileN; x++) {
			if (m_aSrtFlArr[x]->eof) continue;
			if (k < 0 || k != x && (StrCmp (m_aSrtFlArr[k]->rec.key,
				m_aSrtFlArr[x]->rec.key) > 0))
         	k = x;
      	}

      if (k < 0) break; // break while loop if finished with all m_aSrtFlArr

		// Write m_aSrtFlArr[k].rec.dataLine to m_aSrtFlArr[m_iSrtFlArrSz-1].
		if (fwrite (m_aSrtFlArr[k]->rec.dataLine, strlen (m_aSrtFlArr[k]->rec.dataLine),
			1,	m_aSrtFlArr[m_iSrtFlArrSz-1]->fp) != 1) {
			
			msg.Printf (cErrFileWrite, "SR06a",	m_aSrtFlArr[m_iSrtFlArrSz-1]->name);
			FileIOError (msg);
			return false;
			}

		// Replace m_aSrtFlArr[k].rec->key with next item from sort file.
		if (fgets (m_aSrtFlArr[k]->rec.dataLine, sizeof (DataLineType),
			m_aSrtFlArr[k]->fp) == NULL) {
			
      	if (feof (m_aSrtFlArr[k]->fp)) // test for end of file(s)
				m_aSrtFlArr[k]->eof = true;
			else {
				msg.Printf (cErrFileRead, "#SR06b", m_aSrtFlArr[k]->name);
				FileIOError (msg);
				return false;
				}
			}

		else {
			strcpy (m_aSrtFlArr[k]->rec.key, GetKey (m_aSrtFlArr[k]->rec.dataLine, 
				fileNo));
			}
		
		} // while (true)

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	MakeRuns
//
// Description:	Make runs using replacement selection:
//				Read up to MAX_ARR_SZ lines of a text file into a buffer array, where
//				each element of the array consists of the text string and an integer
//				key that is copied from the text string; (3) sort the array by the key;
//				(4) find the lowest key in the buffer that is
//				greater than the last key read, and copy the text string associated 
//				with that key from the buffer to a temporary sort file; (5) read in
//				one new line of text into the buffer and resort the buffer; (7) repeat
//				from step 3 until	there are no more keys left that are greater than
//				the last key read; (8) repeat from step 2 until the last sort file
//				has been filled.
//
// Parameters:	-> fileNo -	determines which key to used, depending on the file
//							being sorted.
//
// Returns:		True if successful, else false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool SortRoutines::MakeRuns (const UINT fileNo)
{
	int			totBufItems = 0; 	 // number of elements in the buffer array
	int			pos			= 0;		 // position of lowest item in buffer array
	UINT		i;
	char		lowStr[SRT_KEY_SZ+1]; // lowest item that is >= highest item in current run
	bool		notEndRun;			 // signals the end of a run
	wxString	msg;				 // error message
	wxString	filePathName;		 // file path and name

	if (!InitTempFiles (0)) // inital all sort files
		return false; // error occurred
	
   if (!AddToBuffer (-1, &totBufItems, fileNo)) // fill entire buffer
		return false; // error occurred

	m_iSrtFileN = 0; // init

 	while (totBufItems > 0) { // get data from unsorted input file
		
		*lowStr	 = '\0';  // init
		notEndRun = true;  // init

		while (m_iSrtFileN < m_iSrtFlArrSz-1) { // add run of items to m_aSrtFlArr array

   		// Add run of records from m_aBufArr to m_aSrtFlArr[m_iSrtFileN] file
			while (notEndRun) {
	   		
				notEndRun = FindLowest (&pos, lowStr, totBufItems);
            
				if (notEndRun) {
	      	
					if (fwrite (m_aBufArr[pos]->dataLine, strlen (m_aBufArr[pos]->dataLine),
						1, m_aSrtFlArr[m_iSrtFileN]->fp) != 1) {
						msg.Printf (cErrFileWrite, "SR07a",	m_aSrtFlArr[m_iSrtFileN]->name);
						FileIOError (msg);
						return false;
         			}

              	wxASSERT (pos != -1);

					if (!AddToBuffer (pos, &totBufItems, fileNo))
						return false; // error occurred

					} // if (notEndRun)
				
				if (totBufItems <= 0) break; // there is no more data to sort
      		
				} // while (notEndRun)
				
			if (pGedWiseFrame->Canceled ()) // check if cancel button hit
				return false;
       	
			notEndRun = true;
         m_iSrtFileN++; // use next m_aSrtFlArr[srtFileN].fp file
   
			if (totBufItems <= 0) break; // there is no more data to sort

			} // while (m_iSrtFileN < m_aSrtFlArr-1)

		// Merge the sort files into one file.
		if (!MergeSort (fileNo))
			return false;

		// Remove temporary sort files & rename last sort file to Holder file.
		if (!TermTmpFiles ())
			return false;

		// Check if there is still data in the buffer.
		if (totBufItems > 0) {

			// Create the temporary sort files (except for _sort000.dat).
			if (!InitTempFiles (1))
				return false;

			// Rename _holder.dat file to _sort000.dat
			// Allow 6 trys to rename file: some RAID systems cannot keep up with file I/O
			i = 0;
			filePathName = pGedWiseFrame->GetWorkDirectory () + m_aSrtFlArr[0]->name;
			while (rename (m_sHoldFile.c_str(), filePathName.c_str())) {

				msg.Printf (cAttemptRename, "SR07b");
				AddLogEntry (msg);
				
				if (i > 5) { 
					msg.Printf (cErrFileRen, "SR07c", m_sHoldFile.c_str());
					FileIOError (msg);
					return false;
					}
				i++;
				}

			// Open _sort000.dat file.
			if ((m_aSrtFlArr[0]->fp = fopen (filePathName.c_str(), "r+t")) == NULL) {
				msg.Printf (cErrFileOpen, "SR07d", filePathName.c_str());
				FileIOError (msg);
				return false;
				}
		
			} // if (totBufNums > 0)

		m_iSrtFileN = 1; // skip m_aSrtFlArr[0] as that contains the data from first pass

		}  // while (totBufNums > 0)

	if (pGedWiseFrame->Canceled ()) // check if cancel button was hit
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	SortFile
//
// Description:	Opens the files to be sorted and calls the merge sort subroutines.
//
// Parameters:	-> fileNo	-	file to sort.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
bool SortRoutines::SortFile (UINT fileNo)
{
	DataLineType	cntRecLine;
	UINT			lineCnt; // count lines to process in current file
	wxString		msg;
	wxString		filePathName;

	// Make sure there was room to allocate the arrays we require.
	if (m_iBufArrSz < MIN_ARR_SZ || m_iSrtFlArrSz < MIN_ARR_SZ) {
		msg.Printf (cNoMemory, "SR08a");
		FileIOError (msg);
		return false;
		}
	
	m_iLineTot	= 0; // start line counter at zero.
	lineCnt		= 0; // count total lines in m_fpInfile

   // Open file we wish to sort.
	filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[fileNo]->name;
	if ((m_fpInfile = fopen (filePathName.c_str(), "r+t")) == NULL) {
		msg.Printf (cErrFileOpen, "SR08b", DBFiles[fileNo]->name);
		FileIOError (msg);
		return false;
   	}
      
	// Count the total lines in the file to sort.
	while (fgets (cntRecLine, MAX_LN_SZ, m_fpInfile))
   	lineCnt++;

	// Reset the progress bar
	UINT totUpdates = lineCnt == 0 ? 0 : ((lineCnt-1)/(PROG_DIV+1)) + 1; 
	pGedWiseFrame->ProgressBarReset (totUpdates);
	
	// If nothing to sort in infile then stop.
	if (lineCnt == 0) {
     	fclose (m_fpInfile);
		m_fpInfile = NULL;
		return true;
      }

	// Add log entry
	msg.Printf ("\nSorting file: %s\n", filePathName.c_str());
	AddLogEntry (msg);
		
	// Go back to first line of m_fpInfile
	rewind (m_fpInfile);
		
	// Sorting the file.
	if (!MakeRuns (fileNo))
		return false; // error occurred

	// Update the window progress bar.
	pGedWiseFrame->ProgressUpdateData ();
	wxGetApp().Yield(true);

	#ifdef _DEBUG
	//UINT totUpdates = ((lineCnt-1)/(PROG_DIV+1)) + 1;
	wxASSERT (totUpdates == UpdateCnt);
	UpdateCnt = 0;
	OrgLineCnt = lineCnt;
	CheckSort (fileNo);
	#endif

  	// Add log entries.
	msg.Printf ("\nTotal Lines Read: = %d\n", m_iLineTot);
	AddLogEntry (msg);
	
	// Close m_fpInfile file.
	fclose (m_fpInfile);
	m_fpInfile = NULL;
	
	// Remove m_fpInfile file.
	remove (filePathName.c_str());
		
	// Rename _holder.dat file to m_fpInfile's file.
	if (rename (m_sHoldFile.c_str(), filePathName.c_str())) {
		msg.Printf (cErrFileRen, "SR08c", m_sHoldFile.c_str());
		FileIOError (msg);
		return false;
		}
		
   return true;
} 

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	SortFileNo
//
// Description:	Sorts a database file.
//
// Parameters:	-> fileNo		 -	file to sort.
//				-> sortMultFiles -  true if sorting multiple files.
//
// Returns:		True if successful, else false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool SortRoutines::SortOneFile (UINT fileNo, bool sortMultFiles)
{
	wxString msg;

	// Display sorting message.
	if (sortMultFiles) {
		msg.Printf (IDS_PROG_FILESORT, m_iFileCnt, m_iTotInFiles);
		m_iFileCnt++;
	
	} else if (fileNo == IndxDB) {
		msg = IDS_PROG_INDXSORT;
	
	} else if (fileNo == ChilDB) {
		msg = IDS_PROG_CHILSORT;
		}
	
	pGedWiseFrame->UpdateProgressMessage (msg.c_str());
	
	// Sort the file.
	if (! SortFile (fileNo))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ProcessFiles
//
// Description:	Sorts each file.
//
// Parameters:	None.
//
// Returns:		True if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool SortRoutines::SortMultFiles (void)
{
	// Initialize variables.
	m_iFileCnt		= 1;
	
	// Count of total files to sort (from below);
	m_iTotInFiles	= 6 + (pGedWiseFrame->IsSortChilPerGed () ? 1 : 0);

	// Sort the files.
	m_bUseLocale = true;
	if (! SortOneFile (IndiDB, true)) // #1
		return false;

	m_bUseLocale = false;

	if (! SortOneFile (ChilDB, true)) // #2
		return false;

	if (! SortOneFile (FamiDB, true)) // #3
		return false;
	
	if (! SortOneFile (SourDB, true)) // #4
		return false;
	
	if (! SortOneFile (RepoDB, true)) // #5
		return false;
	
	if (! SortOneFile (NoteDB, true)) // #6
		return false;

	if (pGedWiseFrame->IsSortChilPerGed ()) {
		if (! SortOneFile (ChSoDB, true))  // #7
			return false;
		}

   return true;
} 

#ifdef _DEBUG
/////////////////////////////////////////////////////////////////////////////////////////
// Function:	CheckSort - FOR DEBUGING PURPOSES ONLY
//
// Description:	Checks the last sorted file to make sure it was sorted correctly.
//				Makes sure total lines in final file is same as in original file.
//				FOR DEBUGING PURPOSES ONLY.  Do no compile this in final code.
//
// Parameters:	-> fileNo - file number just sorted.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void SortRoutines::CheckSort (const UINT fileNo)
{
	DataLineType	dataLine;
	char			key1[SRT_KEY_SZ+1] = "";
	char			key2[SRT_KEY_SZ+1] = "";
	FILE*			fP;
	UINT			chkLineCnt = 0;

	fP = fopen (m_sHoldFile.c_str(), "r+t");

	while (fgets (dataLine, MAX_LN_SZ, fP) != NULL) {
		
		chkLineCnt++;
		
		strcpy (key2, GetKey (dataLine, fileNo));
		
		if (StrCmp (key2, key1) < 0) {
			wxString msg = "CheckSort Sorting Error";
			FileIOError (msg);
			}
		
		strcpy (key1, key2);
		}

	wxASSERT (chkLineCnt == OrgLineCnt);

	fclose (fP);
}
#endif


/////////////////////////////////////////////////////////////////////////////////////////
// Function:	StrCmp
//
// Description:	Performs a case-insensitive comparison of str1 and str2 according to
//				the Palm OS code page. This functions can be used only when
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
int SortRoutines::StrCmp (const char *str1, const char *str2)
{
	int	result = 0;

	if (m_bUseLocale) { // then we are sorting names
		
		int	i = 0;
	
		while (result == 0 && str1[i] && str2[i]) {
			
			result = (int) (sortOrderArr[(unsigned char) str1[i]] 
				- sortOrderArr[(unsigned char) str2[i]]);

			wxASSERT (result < 0xff && result > -0xff);

			i++;
			}

		// check if str1 is longer or shorter than str2
		if (result == 0)
			{
			if (!str1[i] && str2[i])
				result = -1;
			else if (str1[i] && !str2[i])
				result = 1;
			}

		// handle empty strings
		if (i == 0)	{ // then str1 and/or str2 is empty

			if (!*str1 && *str2)
				result = -1;
			else if (*str1)
				result = 1;
			}
		} // if (m_bUseLocale)

	else {

		result = strcmp (str1, str2);

		}

	return result;
}


/*
int SortRoutines::StrColl2(const char *str1, const char *str2)
{
	int retVal = 0;
	int retVal2;
	char c1[2] = "";
	char c2[2] = "";
	
	//char* c1;
	//char* c2;
	int pos = 0;
	int maxPos;
   
//	using namespace std;

	int len1 = strlen (str1);
	int len2 = strlen (str2);

	maxPos = len1 > len2 ? len2 : len1;

//    locale loc ( "English_United States.1252" );
//	const collate<char>& co = use_facet(loc, (collate<char>*)0, false);

	while (retVal == 0) {
		*c1 = toupper (str1[pos]);
		*c2 = toupper (str2[pos]);
		//   *c1 = str1[pos];
		//   *c2 = str2[pos];

		//strxfrm(ts1, c1, 1+ strxfrm (NULL, c1, 0));
		//strxfrm(ts2, c2, 1+ strxfrm (NULL, c2, 0));

//		retVal = co.compare(c1, c1+1, c2, c2+1);
		retVal = strcoll (c1, c2);

		pos++;
		if (pos >= maxPos)
			break;
		}
	retVal2 =  strcoll (str1, str2);
*/



