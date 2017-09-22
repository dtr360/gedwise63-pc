/////////////////////////////////////////////////////////////////////////////////////////
// Program:		GedWise
//				Copyright © 2001 - 2005 Battery Park Software Corporation
//
// Module:		DBCreate.cpp
//
// Written by:	Daniel T. Rencricca
//
// Date:		October 15, 2004
// Revised:		March 9, 2005 -	Converted to wxWidgets
//
// Description:	This module processes the individual database files created by the 
//				GedComParser class.  It then combines the files into one large file
//				containing all the parts fo the database.
//
/////////////////////////////////////////////////////////////////////////////////////////

#include "stdwx.h"
#include "dbcreate.h"
#include "parsegedcom.h"
#include "miscfunctions.h"

#ifdef _DEBUG // include after last #include statement in cpp file where 'new' is used.
#define new DEBUG_NEW
#endif 

extern DBFileRec		DBFiles[][DBCount];
extern UINT	IndiCnt;
extern UINT	EvenNoCnt;
extern UINT NameCnt;
extern UINT	FamiCnt;
extern UINT ChilCnt;
extern UINT	SourCnt;
extern UINT RepoCnt;
extern UINT RCitNoCnt;
extern UINT SCitNoCnt;
extern UINT	CheckNo;

UINT	IndxCnt;

/////////////////////////////////////////////////////////////////////////////////////////
// DBCreateIndex Class Routines																			//
//																													//
// This class creates an index, where each record consists of and IndiNo, a record		//
// number in IndiDB, and aliaKey (0 for primary person or 1 for alias name)				//
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	DBCreateIndex
//
// Description:	Constructor for DBCreateIndex.
//
// Parameters:	pParent -	parent dialog.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
DBCreateIndex::DBCreateIndex (GedWiseFrame* pParent)
{
	pGedWiseFrame = pParent;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	CreateIndex
//
// Description:	Parses the IndiDB file and extracts the fields it need to
//				create the IndxDB (index) file.
//
//				NOTE: the IndiDB must be sorted prior to calling this function.
//
// Parameters:	None.
//
// Returns:		True if successful, else false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBCreateIndex::CreateIndex (void)
{
	INT			errCode = 0;
	char		buffer[BUFFER_SZ];
	char		aliaKey[ALIA_SZ+1];
	FILE*		fpIndx = NULL;
	wxString	filePathName;
	char*		indiRec[MAX_FLDS];
	UINT		recFlds; // count of fields read from buffer

	// Open the individual database (IndiDB) file.
	wxASSERT (DBFiles[IndiDB]->fp == NULL);
	
	filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[IndiDB]->name;
	
	if ((DBFiles[IndiDB]->fp = fopen (filePathName.c_str(), "rt")) == NULL) {
		FileIOError ("DBCreateIndex::CreateIndex", filePathName, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	// Open/create the index database (IndxDB) file.
	filePathName = pGedWiseFrame->GetWorkDirectory () + INDX_DB_FILE;
	
	if ((fpIndx = fopen (filePathName.c_str(), "wt")) == NULL) {
		FileIOError ("DBCreateIndex::CreateIndex", filePathName, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	IndxCnt = 0;
	CheckNo = 0;

	// Read the individual file line by line to create index. //
	while (true) {

		if (GetFileRec (buffer, BUFFER_SZ, DBFiles[IndiDB]->fp) == NULL)
			goto CloseExit;	

		// Update progress bar and check if Canceled button hit.
		if ((CheckNo & DB_PROG_DIV) == 0) {
			pGedWiseFrame->CheckStatus ();
			if (pGedWiseFrame->Canceled ())
				goto CloseExit;
				}
		CheckNo++;

		// Write info after the sort key and the child number to file.
		ReadRecFromBuf (buffer, indiRec, &recFlds);

		// AliaKey can be NULL (primary person w/o aliases), "A" (primary person
		// w/ aliases) or 2 digit string representing the alias number (01, 02, etc.)
		if (! *indiRec[IndiAliaFld] || !strcmp (indiRec[IndiAliaFld], STR_ALI))
			strcpy (aliaKey, "0");
		else
			strcpy (aliaKey, "1");
		
		// Copy info to index file.
		if (fprintf (fpIndx, "%s\t%u\t%s\n", indiRec[IndiIndiNoFld], IndxCnt, aliaKey) < 0) {
			FileIOError ("DBCreateIndex::CreateIndex", filePathName, cWriteErr);
			errCode = -1;  // signal error
			goto CloseExit;
			}

		IndxCnt++; // increment record counter
		}

	/////////
	CloseExit:
	/////////

	// Close the files just used.
	if	(DBFiles[IndiDB]->fp) {
		fclose (DBFiles[IndiDB]->fp);
		DBFiles[IndiDB]->fp = NULL;
		}

	if (fpIndx) {
		fclose (fpIndx);
		DBFiles[IndxDB]->fp = NULL;
		}

	if (pGedWiseFrame->Canceled ())
		return false;
	
	if (errCode < 0) {
		pGedWiseFrame->SetBadExit ();
		return false;
		}

	wxASSERT (IndxCnt == NameCnt);
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
// DBIndiRefSwap Class Routines														   //
//																					   //
// This class replaces the IndiNo xrefs in the IndiDB, ChilDB, EvenDB and FamiDB with  //
// the corresponding record numbers in the IndiDB.									   //
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	DBIndiRefSwap
//
// Description:	Constructor for DBIndiRefSwap.
//
// Parameters:	pParent -	parent class.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
DBIndiRefSwap::DBIndiRefSwap (GedWiseFrame* pParent)
{
	pGedWiseFrame	= pParent;
	m_iArraySz		= 0;
	m_arIndxRec		= NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////
// ~DBIndiRefSwap
//
// Description:	Destructor for DBIndiRefSwap.
//
// Paramters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
DBIndiRefSwap::~DBIndiRefSwap (void)
{
	if (m_arIndxRec)
		DeallocateArray ();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	AllocateArray
//
// Description:	Allocates room on the heap for the Array used to look up xRefNos.  Also
//				sets the m_iArraySz variable to the total number of families in the
//				db maxRecs should be equal to m_iArraySz upon leaving this routine.
//
// Parameters:	-> maxRecs - the number of array elements to create.
//
// Returns:		True if allocated successfully.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBIndiRefSwap::AllocateArray (UINT maxRecs)
{
	try {
		m_arIndxRec = new IndxRecType*[maxRecs];
		for (m_iArraySz = 0; m_iArraySz < maxRecs; m_iArraySz++)
 			m_arIndxRec[m_iArraySz] = new IndxRecType;
		}
		
	catch (...) {
		pGedWiseFrame->SetBadExit ();
		wxString msg (_("DBIndiRefSwap::AllocateArray \n\n"
						"Insufficient memory to allocate Array.\n"));
		wxLogSysError (msg);
		AddLogEntry (msg);
		return false;
		}

	wxASSERT (m_iArraySz == IndxCnt);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	DeallocateArray
//
// Description:	Deletes the Array from the heap.
//
// Inputs:		None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void DBIndiRefSwap::DeallocateArray (void)
{
   for (UINT i = 0; i < m_iArraySz; i++)
		delete[] m_arIndxRec[i];
   
	delete[] m_arIndxRec;

	m_arIndxRec = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////
// Function:	FindRecNo
//
// Description:	Searches the IndxArray for a record where the xRefKey matches
//				the record's xRefKey.  Returns the record number in IndiDB of
//				person matching the xRefKey. Also returns a string containing the
//				record's number of aliases in IndiDB (each separated by an "A"
//				character). The XRefKey must alway be found, else there is a problem
//				with missing data in the GEDCOM file.
//				Note the following: if a person has aliases, the first record entry for
//				that person is the primary name, immediately followed by alias name 
//				records.
//
// Parameters:	-> xRefKey	 -	Individual XRef string to search for.
//				<- aliaNoStr -	string containing alias name record numbers
//				-> retAliaInf- true if returning alias record numbers
//
// Returns:		The record number in IndiDB of person matching the XRefKey.
 /////////////////////////////////////////////////////////////////////////////////////////
INT DBIndiRefSwap::FindRecNo (char* xRefKey, char* aliaNoStr, bool retAliaInfo) 
{
   UINT	numOfRecords = m_iArraySz;
   UINT  i, kmin; // all positions in the database.
   INT   result;  // result of comparing two records
   UINT	rProbe;   // mega record number

   kmin = rProbe = 0;
   
   while (numOfRecords > 0) {
      
      i = numOfRecords / 2;
      rProbe = kmin + i;

      // compare the two keys.
      result = strcmp (xRefKey, m_arIndxRec[rProbe]->xRefKey);

		if (result == 0)
			goto FoundMatch;
     
      if (result < 0)
			numOfRecords = i;
      else {
			kmin = rProbe + 1;
			numOfRecords = numOfRecords - i - 1;
			}
         
      } // while loop

	goto ErrExit;

	//////////
	FoundMatch:
	//////////

	while (rProbe > 0) {
		if (strcmp (xRefKey, m_arIndxRec[rProbe-1]->xRefKey) == 0)
			rProbe--;
		else
			break;
		}

	if (!retAliaInfo) {
		return atoi (m_arIndxRec[rProbe]->recNo);
		}
	else {
		rProbe++;
		*aliaNoStr = '\0';

		while (strcmp (xRefKey, m_arIndxRec[rProbe]->xRefKey) == 0) {
			strncat (aliaNoStr, STR_ALI, AREF_SZ - strlen (aliaNoStr));
			strncat (aliaNoStr, m_arIndxRec[rProbe]->recNo, AREF_SZ  - strlen (aliaNoStr));
			rProbe++;
			if (rProbe >= m_iArraySz) break;
			}

		return 0; // success
		}

	///////
	ErrExit:
	///////

	// We should never get here unless there is a data error in the GEDCOM file.
	// write entry to log file about missing XRefKey.
	wxString msg ("");
	msg.Printf ("Data Error: %s cross reference missing in GEDCOM file\n", xRefKey);
	AddLogEntry (msg);
	
	wxASSERT (false);

	return -1;  // error
} 

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	LoadIndexArray
//
// Description:	Loads the IndxArray with fiels from the Index File created in CreateIndex.
//				Parses the IndxDB file and extracts the fields it need to create the
//				IndexArray (index).
//
// Parameters:	None.
//
// Returns:		True if successfull or false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBIndiRefSwap::LoadIndexArray (void)
{
	UINT		recPos	= 0;
	INT			errCode	= 0;
	char		buffer[BUFFER_SZ];
	wxString	filePathName;
	char*		aRec[MAX_FLDS];
	UINT		recFlds;

	// Open/create the index database (IndxDB) file
	filePathName = pGedWiseFrame->GetWorkDirectory () + INDX_DB_FILE;
	if ((DBFiles[IndxDB]->fp = fopen (filePathName.c_str(), "rt")) == NULL) {
		FileIOError ("DBIndiRefSwap::LoadIndexArray", filePathName, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	// Read the index file line by line to create index.
	while (true) {

		if (GetFileRec (buffer, BUFFER_SZ, DBFiles[IndxDB]->fp) == NULL)
			goto CloseExit;	

		ReadRecFromBuf (buffer, aRec, &recFlds);

		strcpy (m_arIndxRec[recPos]->xRefKey, aRec[0]);
		strcpy (m_arIndxRec[recPos]->recNo, aRec[1]);
		strcpy (m_arIndxRec[recPos]->aliaKey, aRec[2]);

		recPos++;
		}

	/////////
	CloseExit:
	/////////

	wxASSERT (recPos == m_iArraySz);
	
	if (DBFiles[IndxDB]->fp) {
		fclose (DBFiles[IndxDB]->fp);
		DBFiles[IndxDB]->fp = NULL;
		}

	if (errCode < 0) {
		pGedWiseFrame->SetBadExit ();
		return false;
		}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	PrepDB
//
// Description:	Reads lines from a database file and writes each line to a temporary
//				file replacing the XRefNo's with a record number from the appropriate
//				database.  It looks up the XRefNo in an IndxArray array that is created
//				in LoadIndexArray.
//
//				NOTE: This routine strips off the sort key from the ChilDB.
//
// Parameters:	-> dbNum -  the number of the database to process (eg SCitDB, RCitDB,
//							IndiDB).
//
// Returns:		True if successful or false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBIndiRefSwap::PrepDB (UINT dbNum)  
{
	INT			errCode = 0;
	char		buffer[BUFFER_SZ];
	FILE		*fpT = NULL;
	wxString	filePathName;
	wxString	filePathNameT;
	char*		aRec[MAX_FLDS];
	INT32		aRecNo;
	char		husbXRefStr[XREF_SZ+1];
	char		wifeXRefStr[XREF_SZ+1];
	char		aRecNoStr[XREF_SZ+1];
	UINT		recFlds;
	char*		aliaNoStr = NULL;
	char*		sPos; // pointer to buffer
	
	aliaNoStr	= new char[AREF_SZ+1]; // allocate room on heap for data

	// Open the database file to process.
	filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[dbNum]->name;
	if ((DBFiles[dbNum]->fp = fopen (filePathName.c_str(), "r+t")) == NULL) {
		FileIOError ("DBIndiRefSwap::PrepDB", filePathName, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}
	
	// Open the Temporary file for writing.
	filePathNameT = pGedWiseFrame->GetWorkDirectory () + TEMP_FILE;
	if ((fpT = fopen(filePathNameT.c_str(), "wt")) == NULL) {
		FileIOError ("DBIndiRefSwap::PrepDB", filePathNameT, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	while (true) {

		if (GetFileRec (buffer, BUFFER_SZ, DBFiles[dbNum]->fp) == NULL)
			goto CloseExit;

		// Update progress bar and check if Canceled button hit.
		if ((CheckNo & DB_PROG_DIV) == 0) {
			pGedWiseFrame->CheckStatus ();
			if (pGedWiseFrame->Canceled ())
				goto CloseExit;
			}
		CheckNo++;
	
		// The following looks up XRefNo's and replace them with record
		// numbers from the appropriate database.
		switch (dbNum)
			{
			case ChilDB:
				
				// Read and write data after the sort key.
				sPos = strchr (buffer, CHR_TAB) + 1; // skip sort key and tab
				ReadRecFromBuf (sPos, aRec, &recFlds);

				aRecNo = FindRecNo (aRec[1], aliaNoStr); // aRec[1] must have data

				if (aRecNo >= 0)
					sprintf (aRecNoStr, "%u", aRecNo); // samve child number
				else // record not found
					*aRecNoStr = '\0';
				
				aRec[1] = aRecNoStr;

				break;

			case FamiDB:
				
				ReadRecFromBuf (buffer, aRec, &recFlds);
				
				*husbXRefStr = *wifeXRefStr = '\0';

				if (*aRec[1]) { // get husband's record number from Index
					aRecNo = FindRecNo (aRec[1], aliaNoStr);
					if (aRecNo >= 0)
						sprintf (husbXRefStr, "%u", aRecNo);
					}

				if (*aRec[2]) { // get wife's record number from Index
					aRecNo = FindRecNo (aRec[2], aliaNoStr);
					if (aRecNo >= 0)
						sprintf (wifeXRefStr, "%u", aRecNo);
					}

				aRec[1] = husbXRefStr;
				aRec[2] = wifeXRefStr;

				break;

			case IndiDB:

				ReadRecFromBuf (buffer, aRec, &recFlds);

				//wxASSERT (recFlds+1 == IndiTotRecFlds);
				wxASSERT (recFlds == IndiTotRecFlds);

				*aliaNoStr = '\0';

				// Get alias information for individual
				if (*aRec[IndiAliaFld]) { // then individual has alias name(s)
					
					if (!strcmp (aRec[IndiAliaFld],STR_ALI)) { // then this is primary indiv name
						// replace the "A" with the string of alias record numbers (in IndiDB)
						FindRecNo (aRec[IndiIndiNoFld], aliaNoStr, true);
						}
					else { // must be alias name record
						// for alias name record, replace XRefNo w/ record number in IndiDB.
						aRecNo = FindRecNo (aRec[IndiIndiNoFld], aliaNoStr);
						wxASSERT (aRecNo >= 0);
						strcpy (aliaNoStr, STR_ALI);

						if (aRecNo >= 0) {
							sprintf (aRecNoStr, "%u", aRecNo);
							aRec[IndiIndiNoFld] = aRecNoStr;
							}
						}

					aRec[IndiAliaFld] = aliaNoStr;
					}

				break;
				
			case EvenDB:
				
				ReadRecFromBuf (buffer, aRec, &recFlds);

				wxASSERT ((aRec[EvenOrgNoFld][0] == CHR_FAM || aRec[EvenOrgNoFld][0] == CHR_IND));

				if (aRec[EvenOrgNoFld][0] == CHR_IND) { // formatted as "I####" or "F####"
					
					aRecNo =  FindRecNo (&aRec[EvenOrgNoFld][1], aliaNoStr);
	
					if (aRecNo >= 0)
						sprintf (aRecNoStr, "%c%u", CHR_IND, aRecNo);
					else { // record not found
						wxASSERT (false);
						*aRecNoStr = '\0';
						}
					
					aRec[EvenOrgNoFld] = aRecNoStr;
					}

				break;

			default:
				wxASSERT (false);
				recFlds = 0;
				break;
			} // switch
			
		// Save record field data back to a  file
		if (!WriteRecToFile (fpT, aRec, recFlds)) {
			FileIOError ("DBIndiRefSwap::PrepDB", filePathNameT, cWriteErr);
			errCode = -1;
			goto CloseExit;
			}

		} // while loop

	/////////
	CloseExit:
	/////////

	if (DBFiles[dbNum]->fp) {
		fclose (DBFiles[dbNum]->fp);
		DBFiles[dbNum]->fp = NULL;
		}

	if (fpT) {
		fclose (fpT);
		if (errCode < 0)
			remove (filePathNameT.c_str());
		else {
			remove (filePathName.c_str());
			rename (filePathNameT.c_str(), filePathName.c_str());
			}
		}

	if (aliaNoStr) {
		delete [] aliaNoStr;
		aliaNoStr = NULL;
		}

	if (pGedWiseFrame-> Canceled ())
		return false;

	if (errCode < 0) {
		pGedWiseFrame->SetBadExit ();
		return false;
		}

	return true;
  }

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ProcessDB
//
// Description:	Calls the routines to create the IndxDB file and then sort it.
//
// Parameters:	None:
//
// Returns:		True if successful or false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBIndiRefSwap::ProcessDB (void) 
{	
	// Allocate an array in memory
	if (!AllocateArray (IndxCnt))
		return false;

	// Load the array with xrefs from IndiDB.
	if (!LoadIndexArray ())
		return false;

	// Swap ChilDB child xrefs for record number in IndiDB.
	if (!PrepDB (ChilDB))
		return false;
	
	// Swap FamiDB husband and wife xrefs for record numbers in IndiDB.
	if (!PrepDB (FamiDB))
		return false;

	// Swap EvenDB OrgNo xrefs for record numbers in IndiDB.
	if (!PrepDB (EvenDB))
		return false;

	// Swap IndiDB Alias xrefs for record numbers in IndiDB.
	if (!PrepDB (IndiDB))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// DBFamiRefSwap Class Routines														   //
//																					   //
// This class sets a flag in each IndiDB record to indicate whether the individual has //
// children. It also sets in each FamiDB record the total children in that family and  //
// the record number (in ChilDB) of the first child.								   //
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	DBFamiRefSwap
//
// Description:	Constructor for DBFamiRefSwap.
//
// Parameters:	pParent -	parent dialog.
//
// Returns:		Nothing.
////////////////////////////////////////////////////////////////////////////////////////
DBFamiRefSwap::DBFamiRefSwap (GedWiseFrame* pParent)
{
	pGedWiseFrame = pParent;
	m_iFamcCnt		= 0;
	m_arFamCRec		= NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	~DBFamiRefSwap
//
// Description:	Destructor for DBFamiRefSwap
//
// Paramters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
DBFamiRefSwap::~DBFamiRefSwap(void)
{
	if (m_arFamCRec)
		DeallocateArray ();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	AllocateArray
//
// Description:	Allocates room on the heap for the AllocateFamCArray.  Also sets the
//				m_dFamCArraySz variable to the total number of families in the db.
//				FamiCnt should be equal to m_dFamCArraySz upon leaving this routine.
//
// Parameters:	None.
//
// Returns:		True if allocated successfully.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBFamiRefSwap::AllocateArray (void)
{
	try {
		m_arFamCRec = new FamCRecArType*[FamiCnt];
		for (m_dFamCArraySz = 0; m_dFamCArraySz < FamiCnt; m_dFamCArraySz++)
 			m_arFamCRec[m_dFamCArraySz] = new FamCRecArType;
		}

	catch (...) {
		pGedWiseFrame->SetBadExit ();
		wxString msg (_("DBFamiRefSwap::AllocateArray \n\n"
						"Insufficient memory to allocate Array.\n"));
		wxLogSysError (msg);
		AddLogEntry (msg);
		return false;
		}

	wxASSERT (m_dFamCArraySz > 0);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	DeallocateArray
//
// Description:	Deletes the DeAllocateFamCArray.
//
// Inputs:		None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void DBFamiRefSwap::DeallocateArray (void)
{
   for (UINT i = 0; i < m_dFamCArraySz; i++)
		delete[] m_arFamCRec[i];
   
	delete[] m_arFamCRec;
		
	m_arFamCRec = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	LoadFamCArray
//
// Description:	Reads each line from the child database file in order to load the
//				m_arFamCRec. This array holds the family xref number, the total number
//				of children in the family, and the record number of the first child
//				of that family in the ChilDB.
//
// Parameters:	None.
//
// Returns:		True if successful or false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBFamiRefSwap::LoadFamCArray (void)  
{
	int			errCode = 0;
	char		buffer[BUFFER_SZ];
	wxString	filePathName;
	char		lastXRef[XREF_SZ+1] = "@@NOREC@@";
	char		currXRef[XREF_SZ+1];// = "\0";
	UINT		arrPos = 0;
	UINT		recNo = 0;
	char*		endPos;
	UINT		x;

	#ifdef _DEBUG
	UINT		totChiCnt = 0;
	#endif

	if (!m_dFamCArraySz)
		goto CloseExit;

	// Open the Child database file.
	filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[ChilDB]->name;
	if ((DBFiles[ChilDB]->fp = fopen(filePathName.c_str(), "rt")) == NULL) {
		FileIOError ("DBFamiRefSwap::LoadFamCArray", filePathName, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	m_arFamCRec[m_iFamcCnt]->chiCnt = 0; // init

	while (true) {
		
		if (GetFileRec (buffer, BUFFER_SZ, DBFiles[ChilDB]->fp) == NULL)
			goto CloseExit;

		// Get the xref from the buffer.
		endPos = strchr (buffer, CHR_TAB); // find first TAB
		if (endPos) {
			x = endPos - buffer;
			strncpy (currXRef, buffer, x);
			}
		else {
			x = 0;
			}

		currXRef[x] = '\0'; // add terminator

		wxASSERT (endPos);
		wxASSERT (*currXRef);
		
		if (strcmp (currXRef, lastXRef) == 0) { // never equal first time through loop
			// more than one child in family...start counting
			(m_arFamCRec[arrPos]->chiCnt)++;
			}

		else { // we have reached a new family...save data for prior family
			
			if (recNo > 0) {

				#ifdef _DEBUG
				totChiCnt += m_arFamCRec[arrPos]->chiCnt;
				#endif

				arrPos++;
				if (arrPos >= FamiCnt) {
					wxASSERT (false);
					break;
					}
				}
			
			m_iFamcCnt++;
			strcpy (lastXRef, currXRef);
			m_arFamCRec[arrPos]->recCnt = recNo;
			strcpy (m_arFamCRec[arrPos]->xRefKey, currXRef);
			m_arFamCRec[arrPos]->chiCnt = 1;
			}

		recNo++;
		}

	/////////
	CloseExit:
	/////////

	#ifdef _DEBUG
	if (strcmp (currXRef, lastXRef) == 0)
		totChiCnt += m_arFamCRec[arrPos]->chiCnt;
	wxASSERT (totChiCnt == recNo);
	#endif

	wxASSERT (m_iFamcCnt <= FamiCnt);

	if (DBFiles[ChilDB]->fp) {
		fclose (DBFiles[ChilDB]->fp);
		DBFiles[ChilDB]->fp = NULL;
		}

	if (errCode < 0) {
		pGedWiseFrame->SetBadExit ();
		return false;
		}
	
	return true;
  }

////////////////////////////////////////////////////////////////////////////////////////
// Function:	FindRecNo
//
// Description:	Searches the FamCArray for a record where the xRefKey that matches
//				the record's xRefKey.
//
// Parameters:	-> xRefKey	-	key to search for in array.
//
// Returns:		The record number of the matching record in the FamCArray.  If a match 
//				is not fount, then -1 is returned.
 /////////////////////////////////////////////////////////////////////////////////////////
INT DBFamiRefSwap::FindRecNo (char* xRefKey) 
{
   UINT    	numOfRecords = m_iFamcCnt;
   UINT   	i, kmin; // all positions in the database.
   INT     	result;  // result of comparing two records
   UINT		rProbe;  // mega record number

   kmin = rProbe = 0;
   
   while (numOfRecords > 0) {
      
      i = numOfRecords / 2;
      rProbe = kmin + i;

      // Compare the two keys.
      result = strcmp (xRefKey, m_arFamCRec[rProbe]->xRefKey);

		if (result == 0)
			return rProbe;
     
      if (result < 0)
			numOfRecords = i;
      else {
			kmin = rProbe + 1;
			numOfRecords = numOfRecords - i - 1;
			}
         
      } // while loop

	return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	PrepFamiDB
//
// Description:	Reads lines from the family database file and writes each line to
//				a new file but adds fields for the total count of the children in
//				the family and the record number of the first child in the ChilDB.
//				This function uses the FamCArray to get the record numbers and child
//				count. The FamCArray must be initialized and loaded prior to calling
//				this function.
//
// Parameters:	None.
//
// Returns:		True if successful or false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBFamiRefSwap::PrepFamiDB (void)  
{
	int			errCode = 0;
	char		buffer[BUFFER_SZ];
	FILE		*fpT = NULL;
	wxString	filePathName;
	wxString	filePathNameT;
	char*		famRec[MAX_FLDS];
	char		chiCntStr[XREF_SZ+1];
	char		recCntStr[XREF_SZ+1];
	INT32		recNum;
	UINT		recFlds;

	#ifdef _DEBUG
	UINT recCnt = 0;
	#endif
	
	// Open the Family database file.
	filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[FamiDB]->name;
	if ((DBFiles[FamiDB]->fp = fopen(filePathName.c_str(), "r+t")) == NULL) {
		FileIOError ("DBFamiRefSwap::PrepFamiDB", filePathName, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	// Open the Temporary file for writing.
	filePathNameT = pGedWiseFrame->GetWorkDirectory () + TEMP_FILE;
	if ((fpT = fopen(filePathNameT.c_str(), "wt")) == NULL) {
		FileIOError ("DBFamiRefSwap::PrepFamiDB", filePathNameT, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	while (true) {

		if (GetFileRec (buffer, BUFFER_SZ, DBFiles[FamiDB]->fp) == NULL)
			goto CloseExit;

		// Update progress bar and check if Canceled button hit.
		if ((CheckNo & DB_PROG_DIV) == 0) {
			pGedWiseFrame->CheckStatus ();
			if (pGedWiseFrame->Canceled ())
				goto CloseExit;
			}
		CheckNo++;

		ReadRecFromBuf (buffer, famRec, &recFlds);

		*chiCntStr = *recCntStr = '\0';

		recNum = FindRecNo (famRec[0]);

		if (recNum >= 0) {
			sprintf (chiCntStr, "%u", m_arFamCRec[recNum]->chiCnt);
			sprintf (recCntStr, "%u", m_arFamCRec[recNum]->recCnt);
			}

		famRec[FamiChilNo] = recCntStr;  // record number of first child in ChilDB
		famRec[FamiChilCnt] = chiCntStr; // number of children in family

		recFlds = FamiTotRecFlds; // write back all fields, even if empty

		// Save record field data back to a  file.
		if (!WriteRecToFile (fpT, famRec, recFlds)) {
			FileIOError ("DBFamiRefSwap::PrepFamiDB", filePathNameT, cWriteErr);
			errCode = -1;
			goto CloseExit;
			}
		
		#ifdef _DEBUG
		recCnt++;
		#endif
	
		}

	/////////
	CloseExit:
	/////////
	
	if (DBFiles[FamiDB]->fp) {
		fclose (DBFiles[FamiDB]->fp);
		DBFiles[FamiDB]->fp = NULL;
		}

	if (fpT) {
		fclose (fpT);
		if (errCode < 0)
			remove (filePathNameT.c_str());
		else {
			remove (filePathName.c_str());
			rename (filePathNameT.c_str(), filePathName.c_str());
			}
		}
	
	if (pGedWiseFrame-> Canceled ())
		return false;

	if (errCode < 0) {
		pGedWiseFrame->SetBadExit ();
		return false;
		}

	#ifdef _DEBUG
	wxASSERT (recCnt == FamiCnt);
	#endif

	return true;
  }

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	PrepIndiDB
//
// Description:	Reads lines from the individual database file and writes each line to
//				a new file but adds a flag field that indicates that individual has
//				children in the database.  The routine iterates through the individual's
//				families checking each one for a matching family xRef in the FamCArray.
//				If a matching record is found in the FamCArray, it means individual has
//				at least one child in the database.
//				The FamCArray must be initialized and loaded prior to calling this
//				function.
//
// Parameters:	None.
//
// Returns:		True if successful or false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBFamiRefSwap::PrepIndiDB (void)  
{
	int		errCode = 0;
	char		buffer[BUFFER_SZ];
	FILE		*fpT = NULL;
	wxString	filePathName;
	wxString	filePathNameT;
	char*		indiRec[MAX_FLDS];
	char		chilFlagStr[2];
	UINT		x;
	UINT		recFlds;
	char		chilXRef[XREF_SZ+1];

	// Open the IndiDB database file.
	filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[IndiDB]->name;
	if ((DBFiles[IndiDB]->fp = fopen (filePathName.c_str(), "r+t")) == NULL) {
		FileIOError ("DBFamiRefSwap::PrepIndiDB", filePathName, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	// Open the Temporary file for writing.
	filePathNameT = pGedWiseFrame->GetWorkDirectory () + TEMP_FILE;
	if ((fpT = fopen (filePathNameT.c_str(), "wt")) == NULL) {
		FileIOError ("DBFamiRefSwap::PrepIndiDB", filePathNameT, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	while (true) {

		if (GetFileRec (buffer, BUFFER_SZ, DBFiles[IndiDB]->fp) == NULL)
			goto CloseExit;

		// Update progress bar and check if Canceled button hit.
		if ((CheckNo & DB_PROG_DIV) == 0) {
			pGedWiseFrame->CheckStatus ();
			if (pGedWiseFrame->Canceled ())
				goto CloseExit;
			}
		CheckNo++;

		ReadRecFromBuf (buffer, indiRec, &recFlds);

		*chilFlagStr = '\0';
		
		if (indiRec[IndiFamSNoFld]) {
			
			x = 1; // init
			
			// Check for children in all families.
			while (RefFinderStr (x, '@', indiRec[IndiFamSNoFld], chilXRef)) {
				
				if (FindRecNo (chilXRef) >= 0) {
					strcpy (chilFlagStr, "1");
					break; // only need to confirm at least 1 child
					}

				x++;
				}

			indiRec[IndiChilFld] = chilFlagStr;
			}

		//wxASSERT (recFlds+1 == IndiTotRecFlds || recFlds+1 == IndiTotRecFlds-1);
		wxASSERT (recFlds == IndiTotRecFlds);
		
		recFlds = IndiTotRecFlds; // write all fields back, even if empty

		// Save record field data back to a  file.
		if (!WriteRecToFile (fpT, indiRec, recFlds)) {
			FileIOError ("DBFamiRefSwap::PrepIndiDB", filePathNameT, cWriteErr);
			errCode = -1;
			goto CloseExit;
			}

		} // while loop

	/////////
	CloseExit:
	/////////

	if (DBFiles[IndiDB]->fp) {
		fclose (DBFiles[IndiDB]->fp);
		DBFiles[IndiDB]->fp = NULL;
		}

	if (fpT) {
		fclose (fpT);
		if (errCode < 0)
			remove (filePathNameT.c_str());
		else {
			remove (filePathName.c_str());
			rename (filePathNameT.c_str(), filePathName.c_str());
			}
		}
	
	if (pGedWiseFrame-> Canceled ())
		return false;

	if (errCode < 0) {
		pGedWiseFrame->SetBadExit ();
		return false;
		}

	return true;
  }

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ProcessFiles
//
// Description:	Processes the database files files for DBFamiRefSwap.
//						
// Parameters:	None.	
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBFamiRefSwap::ProcessFiles (void)
{
	// Allocate an array in memory.
	if (!AllocateArray ())
		return false;

	// Load data into FamCArray.
	if (!LoadFamCArray ())
		return false;

	// Set flag to mark whether each Individual has children.
	if (!PrepIndiDB ())
		return false;
	
	// Add total children & first child record number (in ChilDB) to each FamiDB record.
	if (!PrepFamiDB ())
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// DBSubRefNo Class Routines														   //
//																					   //
// This class replaces the XRefNo's in the SCitDB (SourDB XRefNo's), RCitDB (RepoDB	   //
// XRefNo's), and IndiDB (FamiDB XRefNo's) with the appropriate record numbers.		   //
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	DBSubRepoNo
//
// Description:	Constructor for DBSubRepoNo. This class substitutes the XRefNo's for
//				for the actual record numbers in the SourDB, RepoDB and IndiDB.
//
// Parameters:	<- pParent - parent dialog.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
DBSubRefNo::DBSubRefNo (GedWiseFrame* pParent)
{
	pGedWiseFrame = pParent;
	m_iArraySz		= 0;
	m_arXRefRec		= NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	~DBSubRepoNo
//
// Description:	Destructor for DBSubSourNo.  Calls the DeallocateArray function to
//				delete the array created on the heap.
//
// Paramters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
DBSubRefNo::~DBSubRefNo (void)
{
	if (m_arXRefRec)
		DeallocateArray ();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	AllocateArray
//
// Description:	Allocates room on the heap for the Array used to look up xRefNos.
//
// Parameters:	-> maxRecs -	the number of array elements to create.
//
// Returns:		True if allocated successfully.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBSubRefNo::AllocateArray (UINT maxRecs)
{
	try {
		m_arXRefRec = new XRefArrayType*[maxRecs];
		for (m_iArraySz = 0; m_iArraySz < maxRecs; m_iArraySz++)
 			m_arXRefRec[m_iArraySz] = new XRefArrayType;
		}
		
	catch (...) {
		pGedWiseFrame->SetBadExit ();
		wxString msg (_("DBSubRefNo::AllocateArray \n\n"
						"Insufficient memory to allocate Array.\n"));
		wxLogSysError (msg);
		AddLogEntry (msg);
		return false;
		}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	DeallocateArray
//
// Description:	Deletes the lookup Array from the heap.
//
// Inputs:		None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void DBSubRefNo::DeallocateArray (void)
{
   for (UINT i = 0; i < m_iArraySz; i++)
		delete[] m_arXRefRec[i];
   
	delete[] m_arXRefRec;
	
	m_arXRefRec = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		LoadXRefArray
//
// Description:		Parses a given database file and extracts the xRef field it needs to
//					create a look up array.  It is assumed the XRef field is always the
//					first field in the file.
//
// Parameters:		-> dbNum -	the number of the database to process (eg SCitDB, RCitDB,
//								IndiDB).
//
// Returns:			True if successful or false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBSubRefNo::LoadXRefArray (UINT dbNum)
{
	int			errCode = 0;
	char		buffer[BUFFER_SZ];
	wxString	filePathName;
	char*		endPos;
	UINT		x;

	if (!m_iArraySz)
		goto CloseExit;

	// Open the m_arXRefRec database file.
	filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[dbNum]->name;
	if ((DBFiles[dbNum]->fp = fopen (filePathName.c_str(), "rt")) == NULL) {
		FileIOError ("DBSubRefNo::LoadXRefArray", filePathName, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	m_iRecCnt = 0;

	while (m_iRecCnt <= m_iArraySz) {
		
		if (GetFileRec (buffer, BUFFER_SZ, DBFiles[dbNum]->fp) == NULL)
			goto CloseExit;

		// Get the xref from the buffer.
		endPos = strchr (buffer, CHR_TAB);
		
		if (endPos) {
			x = endPos - buffer;
			strncpy (m_arXRefRec[m_iRecCnt]->xRefKey, buffer, x);
			}
		else {
			x = 0;
			}

		m_arXRefRec[m_iRecCnt]->xRefKey[x] = '\0'; // add string terminator

		wxASSERT (*m_arXRefRec[m_iRecCnt]->xRefKey);
		wxASSERT (endPos);
		
		m_iRecCnt++;

		wxASSERT (m_iRecCnt <= m_iArraySz);
		}

	/////////
	CloseExit:
	/////////

	if (DBFiles[dbNum]->fp) {
		fclose (DBFiles[dbNum]->fp);
		DBFiles[dbNum]->fp = NULL;
		}

	if (errCode < 0) {
		pGedWiseFrame->SetBadExit ();
		return false;
		}

	return true;
  }

////////////////////////////////////////////////////////////////////////////////////////
// Function:	FindRecNo
//
// Description:	Searches the m_arXRefRec for a record where the given key that matches
//				the record's xRefKey.
//
// Parameters:	-> xRefKey - the key to look up in the array.
//
// Returns:		The record number if it was found, else -1.
 /////////////////////////////////////////////////////////////////////////////////////////
INT32 DBSubRefNo::FindRecNo (char* xRefKey) 
{
   UINT  numOfRecords = m_iArraySz;
   UINT  i, kmin; // all positions in the database.
   INT   result;  // result of comparing two records
   UINT	rProbe;  // mega record number

   kmin = rProbe = 0;
   
	wxASSERT (xRefKey);
	if (! xRefKey)
		return -1;

   while (numOfRecords > 0) {
      
      i = numOfRecords / 2;
      rProbe = kmin + i;

      // compare the two keys.
      result = strcmp (xRefKey, m_arXRefRec[rProbe]->xRefKey);

		if (result == 0)
			return rProbe;
     
      if (result < 0)
			numOfRecords = i;
      else {
			kmin = rProbe + 1;
			numOfRecords = numOfRecords - i - 1;
			}
         
      } // while loop

	return -1; // xRefKey not found
} 

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	PrepDB
//
// Description:	Reads lines from a database file and writes each line to a temporary
//				file replacing the XRefNo's with a record number from the appropriate
//				database.  It looks up the XRefNo in an m_arXRefRec array that is
//				created in LoadXRefArray. The XRefNo MUST be the first field in the
//				given database. The databases used to create the arrays are: SourDB,
//				RepoDB and FamiDB. These arrays are used to replace the XRefNo's in
//				the SCitDB (SourDB XRefNo's), RCitDB (RepoDB XRefNo's), and IndiDB 
//				(FamiDB XRefNo's).
//
//	Parameters:	-> dbNum -	the number of the database to process (eg SCitDB, RCitDB,
//							or IndiDB).
//
//	Returns:	True if successful or false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBSubRefNo::PrepDB (UINT dbNum)  
{
	INT			errCode = 0;
	char		buffer[BUFFER_SZ];
	FILE*		fpT = NULL;
	wxString	filePathName;
	wxString	filePathNameT;
	char*		aRec[MAX_FLDS];
	INT32		aRecNo;
	char		aRecNoStr[XREF_SZ+2]; // add 1 extra for starting character ('I' or 'F')
	char		famSXRef[XREF_SZ+1];
	char		famSRecNoStr[XREF_SZ+2]; // add 1 extra for starting '@' character
	UINT		recFlds;
	UINT		x;

	#ifdef _DEBUG
	UINT		recCnt = 0;
	#endif

	char*		famSNoStr = NULL;
	famSNoStr	= new char[FREF_SZ+1];
	
	// Open the database file to process. 
	filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[dbNum]->name;
	if ((DBFiles[dbNum]->fp = fopen(filePathName.c_str(), "r+t")) == NULL) {
		FileIOError ("DBSubRefNo::PrepDB", filePathName, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	// Open the Temporary file for writing.
	filePathNameT = pGedWiseFrame->GetWorkDirectory () + TEMP_FILE;
	if ((fpT = fopen(filePathNameT.c_str(), "wt")) == NULL) {
		FileIOError ("DBSubRefNo::PrepDB", filePathNameT, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	while (true) {
		
		if (GetFileRec (buffer, BUFFER_SZ, DBFiles[dbNum]->fp) == NULL)
			goto CloseExit;

		ReadRecFromBuf (buffer, aRec, &recFlds);
		
		// Look up XRefNo's and replace them with record numbers from appropriate database.
		switch (dbNum)
			{
			case SCitDB:
			case RCitDB:
				
				aRecNo = FindRecNo (aRec[0]);
				
				if (aRecNo >= 0) {
					sprintf (aRecNoStr, "%u", (UINT32) aRecNo);
					}
				else { // sour record not found for scit record...this is fine
					*aRecNoStr = '\0';
					}

				aRec[0] = aRecNoStr;

				break;

			case IndiDB:

				wxASSERT (recFlds == IndiTotRecFlds || recFlds == IndiTotRecFlds-1);

				// First,take care of FamC XRefNo (only one per record).
				if (*aRec[IndiFamCNoFld]) {
					
					aRecNo = FindRecNo (aRec[IndiFamCNoFld]);
					
					if (aRecNo >= 0) {
						sprintf (aRecNoStr, "%u", (UINT32) aRecNo);
						}
					else {
						*aRecNoStr = '\0';
						wxASSERT_MSG (false, "GEDCOM ERROR: Person has FAMC xref but this family record is not defined in GEDCOM");
						}

					aRec[IndiFamCNoFld] = aRecNoStr;
					}

				// Next, take care of FamS XRefNo's (can be several per record).
				*famSNoStr = '\0';
				
				if (*aRec[IndiFamSNoFld]) {

					x = 1;
			
					while (RefFinderStr (x, '@', aRec[IndiFamSNoFld], famSXRef)) {
		
						aRecNo = FindRecNo (famSXRef);
						if (aRecNo >= 0) {
							sprintf (famSRecNoStr, "%c%u", CHR_DLM, (UINT32) aRecNo);
							strcat (famSNoStr, famSRecNoStr); // already size checked in parser
							}
						
						wxASSERT (*famSRecNoStr);
						x++;
						}
					
					aRec[IndiFamSNoFld] = famSNoStr;
					}

				break;

			case EvenDB:

				wxASSERT ((aRec[EvenOrgNoFld][0] == CHR_FAM || aRec[EvenOrgNoFld][0] == CHR_IND));

				if (aRec[EvenOrgNoFld][0] == CHR_FAM) { // formatted as "I####" or "F####"
					
					aRecNo = FindRecNo (&aRec[EvenOrgNoFld][1]); // xref starts aft 'F' or 'I'
			
					if (aRecNo >= 0) {
						sprintf (aRecNoStr, "%c%u", CHR_FAM, (UINT32) aRecNo);
						}
					else { // record not found
						*aRecNoStr = '\0';
						wxASSERT (false);
						}
			
					aRec[EvenOrgNoFld] = aRecNoStr;
					}

				break;			
			
			default:
				wxASSERT (false);
				break;
			} // switch
			
		// Save record field data back to a  file.
		if (!WriteRecToFile (fpT, aRec, recFlds)) {
			FileIOError ("DBSubRefNo::PrepDB", filePathNameT, cWriteErr);
			errCode = -1;
			goto CloseExit;
			}

		#ifdef _DEBUG
		recCnt++;
		#endif

		} // while loop

	/////////
	CloseExit:
	/////////

	#ifdef _DEBUG
	switch (dbNum)
		{
		case SCitDB:
			wxASSERT (recCnt == SCitNoCnt);
			break;
		case RCitDB:
			wxASSERT (recCnt == RCitNoCnt);
			break;
		case IndiDB:
			wxASSERT (recCnt == NameCnt);
			break;
		case EvenDB:
			wxASSERT (recCnt == EvenNoCnt);
			break;
		}
	#endif


	if (DBFiles[dbNum]->fp) {
		fclose (DBFiles[dbNum]->fp);
		DBFiles[dbNum]->fp = NULL;
		}

	if (fpT) {
		fclose (fpT);
		if (errCode < 0)
			remove (filePathNameT.c_str());
		else {
			remove (filePathName.c_str());
			rename (filePathNameT.c_str(), filePathName.c_str());
			}
		}
	
	if (famSNoStr) {
		delete [] famSNoStr;
		famSNoStr = NULL;
		}

	if (errCode < 0) {
		pGedWiseFrame->SetBadExit ();
		return false;
		}

	return true;
  }

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ProcessFiles
//
// Description:	Processes the database files files for DBSubRefNo.
//
// Parameters:	None.
//
// Returns:		true if processed successfully.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBSubRefNo::ProcessFiles (void)
{
	if (!AllocateArray (FamiCnt))
		return false;
	
	if (!LoadXRefArray (FamiDB))
		return false;
	
	if (!PrepDB (IndiDB))
		return false;
	
	if (!PrepDB (EvenDB))
		return false;

	DeallocateArray (); // deallocate FamiDB index
	
	if (!AllocateArray (SourCnt))
		return false;
	
	if (!LoadXRefArray (SourDB))
		return false;
	
	if (!PrepDB (SCitDB))
		return false;

	DeallocateArray (); // deallocate SourDB index

	if (!AllocateArray (RepoCnt))
		return false;
	
	if (!LoadXRefArray (RepoDB))
		return false;
	
	if (!PrepDB (RCitDB))
		return false;

	DeallocateArray (); // deallocate RepoDB index

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
// DBSubEvenNo Class Routines														   //
//																					   //
// This class re-orders the records in the EvenDB to the order found in the IndiDB	   //
// and FamiDB files. It also swaps the record number in IndiDB and EvenDB for new	   //
// record numbers in the re-ordered EvenDB file.									   //
// This reshuffling is done so that the search function in the handheld viewer can	   //
// list found records in order of the individua in the IndiDB.						   //
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		DBSubEvenNo
//
//	Description:	Constructor for DBEvenNoSwap.
//
// Parameters:		pParent -	parent dialog.
//
// Returns:			Nothing.
////////////////////////////////////////////////////////////////////////////////////////
DBSubEvenNo::DBSubEvenNo (GedWiseFrame* pParent)
{
	pGedWiseFrame = pParent;
	m_iEvenNoNew	= 0;
	m_fpEvenT		= NULL;
	m_arEvenRec		= NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	~DBSubEvenNo
//
// Description:	Destructor for DBEvenNoSwap
//
// Paramters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
DBSubEvenNo::~DBSubEvenNo (void)
{
	if (m_arEvenRec)
		DeallocateArray ();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	AllocateArray
//
// Description:	Allocates room on the heap for the m_arEvenRec.  Also sets the
//				m_dEvenArraySz variable to the total number of events in the db.
//				EvenNoCnt should be equal to m_dEvenArraySz upon leaving this routine.
//
// Parameters:	None.
//
// Returns:		True if allocated successfully.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBSubEvenNo::AllocateArray (void)
{
	try {
		m_arEvenRec = new EvenRecArType*[EvenNoCnt];
		for (m_dEvenArraySz = 0; m_dEvenArraySz < EvenNoCnt; m_dEvenArraySz++)
 			m_arEvenRec[m_dEvenArraySz] = new EvenRecArType;
		}

	catch (...) {
		pGedWiseFrame->SetBadExit ();
		wxString msg (_("DBSubEvenNo::AllocateArray \n\n"
						"Insufficient memory to allocate Array.\n"));
		wxLogSysError (msg);
		AddLogEntry (msg);
		return false;
		}

	wxASSERT (m_dEvenArraySz == EvenNoCnt && m_dEvenArraySz > 0);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	DeallocateArray
//
// Description:	Deletes the DeAllocateFamCArray.
//
// Inputs:		None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void DBSubEvenNo::DeallocateArray (void)
{
   for (UINT i = 0; i < m_dEvenArraySz; i++)
		delete[] m_arEvenRec[i];
   
	delete[] m_arEvenRec;

	m_arEvenRec = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	LoadEvenArray
//
// Description:	Reads each line from the EvenDB file and saves the starting position
//				of each line in the m_arEvenRec.
//
//				The m_arEvenRec allows a much faster lookup of lines in the EvenDB file.
//
//				NOTE: The m_arEvenRec must be allocated before calling this function.
//
// Parameters:	None.
//
// Returns:		True if successful or false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBSubEvenNo::LoadEvenArray (void)
{
	int			errCode = 0;
	char		buffer[BUFFER_SZ];
	wxString	filePathName;
	
	// Open the Event database file.
	filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[EvenDB]->name;
	if ((DBFiles[EvenDB]->fp = fopen (filePathName.c_str(), "r+t")) == NULL) {
		FileIOError ("DBSubEvenNo::LoadEvenArray", filePathName, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}
	
	// Read each line from the EvenDB file.
	for (m_iEvenCnt = 0; m_iEvenCnt < EvenNoCnt; m_iEvenCnt++) {
		
		// get the file position (the start of the next record)
		if (fgetpos (DBFiles[EvenDB]->fp, &m_arEvenRec[m_iEvenCnt]->pos) != 0) {
			AddLogEntry ("\nError #001 calling fgetpos: DBSubEvenNo::LoadEvenArray. \n");
			errCode = -1; // the only errors fgetpos can return are fatal ones.
			goto CloseExit;
			}

		if (GetFileRec (buffer, BUFFER_SZ, DBFiles[EvenDB]->fp) == NULL) {
			wxASSERT (false);
			break; // reached end of file...this should never happen
			}
		} // for loop

	/////////
	CloseExit:
	/////////

	if (m_iEvenCnt != EvenNoCnt) {
		AddLogEntry ("\nError #002 too few records read: DBSubEvenNo::LoadEvenArray. \n");
		wxASSERT (false);
		}

	if (DBFiles[EvenDB]->fp) {
		fclose (DBFiles[EvenDB]->fp);
		DBFiles[EvenDB]->fp = NULL;
		}		

	if (errCode < 0) {
		pGedWiseFrame->SetBadExit ();
		return false;
		}

	return true;
  }

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	EvenRefSwap
//
// Description:	The routine looks up the event number (evenNoStr) in the m_arEvenRec
//				to find the position in the EvenDB of the evenNoStr record.  It reads
//				the record from EvenDB and writes to a temporary file that will become
//				the new EvenDB database.
//
//				The m_iEvenNoNew is increased by 1 if the lookup was successful. It 
//				should allway be successful or else there is a fatal problem.
//
//				The m_arEvenRec must be initialized and loaded prior to calling this
//				function.
//
//	Parameters:	-> evenNoStr	-	the event number to look up in EvenDB.
//
//	Returns:	True if successful or false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBSubEvenNo::EvenRefSwap (char* evenNoStr)
{
	int		errCode = 0;
	char	buffer[BUFFER_SZ];
	UINT	evenNo;

	wxASSERT (evenNoStr && *evenNoStr != '\0');

	evenNo = (UINT) atoi (evenNoStr);

	// Check for out of bounds error.
	if (evenNo >= EvenNoCnt) {
		AddLogEntry ("\nError #003 evenNoStr too high: DBSubEvenNo::EvenRefSwap. \n");
		wxASSERT (false);
		return false; // should never happen, but not fatal error
		}

	// Save position in EvenDB file to m_arEvenRec[evenNo]->pos
	if (fsetpos (DBFiles[EvenDB]->fp, &m_arEvenRec[evenNo]->pos) != 0) {
		AddLogEntry ("\nError #004 calling fsetpos: DBSubEvenNo::EvenRefSwap. \n");
		wxASSERT (false);
		return false;
		}

	// Read record from EvenDB file
	if (fgets (buffer, BUFFER_SZ, DBFiles[EvenDB]->fp) == NULL) {  // do not call GetFileRec here
		wxString hldStr = DBFiles[EvenDB]->name;
		FileIOError ("DBSubEvenNo::EvenRefSwap", hldStr, cReadErr); // CHECK THIS
		errCode = -1;
		goto CloseExit;
		}

	// Write record data back to Temporary file
	if (fwrite (buffer, sizeof (char), strlen (buffer), m_fpEvenT) == 0) {
		FileIOError ("DBSubEvenNo::EvenRefSwap", m_sFilePathNameEvenT, cWriteErr);
		errCode = -1;
		goto CloseExit;
		}

	m_iEvenNoNew++; // update only if no errors

	/////////
	CloseExit:
	/////////
	
	if (errCode < 0) {
		pGedWiseFrame->SetBadExit ();
		wxASSERT (false);
		return false;
		}

	return true;
  }

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	SubEvenNo
//
// Description:	Reads each line from the IndiDB and FamiDB and swaps every event number
//				for an event number in the new EvenDB being created.  It calls EvenRefSwap
//				to do the event number swapping and write each event to a temporary file
//				that will become the new EvenDB.  
//				The purpose of this function is to make sure the event numbers are in
//				sequential order in the IndiDB and FamiDB. So when a search is done on
//				a users handheld, the events will be listed for each person where each
//				person is in alphabetical order.
//
//				If EvenRefSwap fails, then the event record number is left blank so that
//				it does not cause a problem in the GedWise viewer
//
//				The m_arEvenRec must be initialized and loaded prior to calling this
//				function.
//
// Parameters:	None.
//
// Returns:		True if successful or false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBSubEvenNo::SubEvenNo (UINT dbNum)  
{
	INT			errCode = 0;
	char		buffer[BUFFER_SZ];
	FILE		*fpT = NULL;
	wxString	filePathName;
	wxString	filePathNameT;
	char*		dbRec[MAX_FLDS];
	UINT		recFlds;
	char		evenXRef[XREF_SZ+1];
	UINT		j;
	UINT		evenCnt;
	char		evenListStr[NREF_SZ*2 + 1 + 1]; // add 1 for ' ' and 1 for terminator (eg "1234 5")
	char		evenBirNoStr[NREF_SZ+1];
	char		evenDthNoStr[NREF_SZ+1];
	char		evenMarNoStr[NREF_SZ+1];
	UINT		evenNoHold = 0;

	wxASSERT (dbNum == IndiDB || dbNum == FamiDB);
	
	// Open the dbNum database file for reading (IndiDB or FamiDB).
	filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[dbNum]->name;
	if ((DBFiles[dbNum]->fp = fopen (filePathName.c_str(), "r+t")) == NULL) {
		FileIOError ("DBSubEvenNo::SubEvenNo", filePathName, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	// Open a Temporary file for writing new IndiDB / FamiDB.
	filePathNameT = pGedWiseFrame->GetWorkDirectory () + TEMP_FILE;
	if ((fpT = fopen (filePathNameT.c_str(), "wt")) == NULL) {
		FileIOError ("DBSubEvenNo::SubEvenNo", filePathNameT, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	while (true) {
	
		if (GetFileRec (buffer, BUFFER_SZ, DBFiles[dbNum]->fp) == NULL)
			break; // then at end of file

		// Update progress bar and check if Canceled button hit.
		if ((CheckNo & DB_PROG_DIV) == 0) {
			pGedWiseFrame->CheckStatus ();
			if (pGedWiseFrame->Canceled ())
				goto CloseExit;
			}
		CheckNo++;

		ReadRecFromBuf (buffer, dbRec, &recFlds);

		*evenBirNoStr = *evenDthNoStr = *evenMarNoStr = *evenListStr = '\0';

		switch (dbNum)
			{
			case IndiDB:
				
				// Replace primary birth event number.
				if (dbRec[IndiEvenBNoFld] && *dbRec[IndiEvenBNoFld]) {
			
					if (EvenRefSwap (dbRec[IndiEvenBNoFld]) )
						sprintf (evenBirNoStr, "%u", m_iEvenNoNew - 1);
						
					dbRec[IndiEvenBNoFld] = evenBirNoStr;
					}
		
				// Replace general individual event numbers.
				if (dbRec[IndiEvenNoFld] && *dbRec[IndiEvenNoFld]) {
					j			= 1;
					evenCnt	= 0;

					while (RefFindNum (j, CHR_EVN, dbRec[IndiEvenNoFld], evenXRef)) {
						if (EvenRefSwap (evenXRef)) {
							if (evenCnt == 0) {
								evenNoHold = m_iEvenNoNew - 1; // save first event rec number
								}
							evenCnt++;
							}
						j++;
						} // while

					if (evenCnt > 0) // if at least one event found in field
						sprintf (evenListStr, "%u %u", evenNoHold, evenCnt);

					dbRec[IndiEvenNoFld] = evenListStr;
					} 

				// Replace primary death event number.
				if (dbRec[IndiEvenDNoFld] && *dbRec[IndiEvenDNoFld]) {
					
					if (EvenRefSwap (dbRec[IndiEvenDNoFld]))
						sprintf (evenDthNoStr, "%u", m_iEvenNoNew-1);
						
					dbRec[IndiEvenDNoFld] = evenDthNoStr;
					}

				recFlds = IndiTotRecFlds; // write all fields back, even if empty

				break;

		case FamiDB:
			
			// Replace the primary marriage record and all family events
			if (dbRec[FamiEvenMNo] && *dbRec[FamiEvenMNo]) {
				
				if (EvenRefSwap (dbRec[FamiEvenMNo]))
					sprintf (evenMarNoStr, "%u", m_iEvenNoNew - 1);
				
				dbRec[FamiEvenMNo] = evenMarNoStr;
				}
			
			if (dbRec[FamiEvenNo] && *dbRec[FamiEvenNo]) {
				j			= 1;
				evenCnt	= 0;

				while (RefFindNum (j, CHR_EVN, dbRec[FamiEvenNo], evenXRef)) {
					if (EvenRefSwap (evenXRef)) {
						if (evenCnt == 0) {
							evenNoHold = m_iEvenNoNew - 1; // save first event rec number
							}
						evenCnt++;
						}
					j++;
					} // while

				if (evenCnt > 0) // if at least one event found in field
					sprintf (evenListStr, "%u %u", evenNoHold, evenCnt);

				dbRec[FamiEvenNo] = evenListStr;
				}

			recFlds = FamiTotRecFlds; // write all fields back, even if empty
			break;
	
		default:
			wxASSERT (false);
			break;
		} // switch

		// Save record field data back to a  file.
		if (!WriteRecToFile (fpT, dbRec, recFlds)) {
			FileIOError ("DBSubEvenNo::SubEvenNo", filePathNameT, cWriteErr);
			errCode = -1;
			goto CloseExit;
			}

		} // while loop

	/////////
	CloseExit:
	/////////

	// Close the IndiDB / FamiDB file.
	if (DBFiles[dbNum]->fp) {
		fclose (DBFiles[dbNum]->fp);
		DBFiles[dbNum]->fp = NULL;
		}

	// Rename Temporary file to the IndiDB / FamiDB file.
	if (fpT) {
		fclose (fpT);
		if (errCode < 0)
			remove (filePathNameT.c_str());
		else {
			remove (filePathName.c_str());
			rename (filePathNameT.c_str(), filePathName.c_str());
			}
		}
	
	if (pGedWiseFrame->Canceled ())
		return false;

	if (errCode < 0) {
		pGedWiseFrame->SetBadExit ();
		return false;
		}

	return true;
  }

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	SwapEvenNos
//
// Description:	This routine calls the functions to swap the event numbers in the
//				IndiDB and EvenDB.
//
// Parameters:	None.
//
// Returns:		True if successful or false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBSubEvenNo::ProcessDb (void)  
{
	INT			errCode = 0;
	wxString	filePathName;
	
	// Open the EvenDB database file for reading.
	filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[EvenDB]->name;
	if ((DBFiles[EvenDB]->fp = fopen (filePathName.c_str(), "r+t")) == NULL) {
		FileIOError ("DBSubEvenNo::ProcessDb", filePathName, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}
	
	// Open the Temporary2 file for writing new EvenDB.
	m_sFilePathNameEvenT = pGedWiseFrame->GetWorkDirectory () + TEMP_FILE2;
	if ((m_fpEvenT = fopen (m_sFilePathNameEvenT.c_str(), "wt")) == NULL) {
		FileIOError ("DBSubEvenNo::ProcessDb", m_sFilePathNameEvenT, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	// Swap Event Numbers in IndiDB.
	if (!SubEvenNo (IndiDB))
		goto CloseExit;

	// Swap Event Numbers in FamiDB.
	if (!SubEvenNo (FamiDB))
		goto CloseExit;

	/////////
	CloseExit:
	/////////

	// Close the EvenDB file.
	if (DBFiles[EvenDB]->fp) {
		fclose (DBFiles[EvenDB]->fp);
		DBFiles[EvenDB]->fp = NULL;
		}

	// Rename Temporary2 file to EvenDB file.
	if (m_fpEvenT) {
		fclose (m_fpEvenT);
		if (errCode < 0)
			remove (m_sFilePathNameEvenT.c_str());
		else {
			remove (filePathName.c_str());
			rename (m_sFilePathNameEvenT.c_str(), filePathName.c_str());
			}
		}
	
	if (pGedWiseFrame->Canceled ())
		return false;

	if (errCode < 0) {
		pGedWiseFrame->SetBadExit ();
		return false;
		}

	return true;
  }

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ProcessFiles
//
// Description:	Processes the IndiDB and FamiDB files for DBSubEvenNo.
//						
// Parameters:	None.	
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBSubEvenNo::ProcessFiles (void)
{
	// Create space for m_arEvenRec in memory.
	if (!AllocateArray ())
		return false;

	// Load data into m_arEvenRec.
	if (!LoadEvenArray ())
		return false;

	// Process the IndiDB and FamiDB databases.
	if (!ProcessDb ())
		return false;

	wxASSERT (m_iEvenNoNew == EvenNoCnt);

	#ifdef _DEBUG
	CheckEvenNos ();
	#endif

	return true;
}

#ifdef _DEBUG
/////////////////////////////////////////////////////////////////////////////////////////
// Function:	CheckEvenNos - FOR DEBUGING PURPOSES ONLY
//
// Description:	Checks all the event record numbers in the IndiDB and FamiDb to make
//				sure they are in sequential order.
//				FOR DEBUGING PURPOSES ONLY.  Do no compile this in final code.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void DBSubEvenNo::CheckEvenNos (void)
{
	wxString	filePathName;
	UINT		recFlds;
	char*		dbRec[MAX_FLDS];
	char		buffer[BUFFER_SZ];
	UINT		chkTotStart = 0;
	UINT		chkTotEnd = 0;

	filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[IndiDB]->name;
	DBFiles[IndiDB]->fp = fopen (filePathName.c_str(), "r+t");

	while (GetFileRec (buffer, BUFFER_SZ, DBFiles[IndiDB]->fp) != NULL) {

		ReadRecFromBuf (buffer, dbRec, &recFlds);
	
		if (dbRec[IndiEvenBNoFld] && *dbRec[IndiEvenBNoFld]) {
					
				chkTotEnd = atoi (dbRec[IndiEvenBNoFld]);
				wxASSERT (chkTotEnd == chkTotStart);
				chkTotStart += 1;
				}

		if (dbRec[IndiEvenNoFld] && *dbRec[IndiEvenNoFld]) {
	
			chkTotEnd = atoi (dbRec[IndiEvenNoFld]);

			wxASSERT (chkTotEnd == chkTotStart);

			char* cPos = strchr (dbRec[IndiEvenNoFld], ' ');
			wxASSERT (cPos != NULL);
	
			cPos ++;
			chkTotStart += atoi (cPos);
			}

		if (dbRec[IndiEvenDNoFld] && *dbRec[IndiEvenDNoFld]) {
					
			chkTotEnd = atoi (dbRec[IndiEvenDNoFld]);
			wxASSERT (chkTotEnd == chkTotStart);
			chkTotStart += 1;
			}

		}	

	fclose (DBFiles[IndiDB]->fp);
	DBFiles[IndiDB]->fp = NULL;

	filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[FamiDB]->name;
	DBFiles[FamiDB]->fp = fopen (filePathName.c_str(), "r+t");
		
		while (GetFileRec (buffer, BUFFER_SZ, DBFiles[FamiDB]->fp) != NULL) {

			ReadRecFromBuf (buffer, dbRec, &recFlds);
	
			if (dbRec[FamiEvenMNo] && *dbRec[FamiEvenMNo]) {
				chkTotEnd = atoi (dbRec[FamiEvenMNo]);
				wxASSERT (chkTotEnd == chkTotStart);
				chkTotStart += 1;
				}
			
			if (dbRec[FamiEvenNo] && *dbRec[FamiEvenNo]) {
				
				chkTotEnd = atoi (dbRec[FamiEvenNo]);

				wxASSERT (chkTotEnd == chkTotStart);

				char* cPos = strchr (dbRec[FamiEvenNo], ' ');
				wxASSERT (cPos != NULL);
				cPos ++;

				chkTotStart += atoi (cPos);

				}
	}

	fclose (DBFiles[FamiDB]->fp);
	DBFiles[FamiDB]->fp = NULL;
}
#endif


/////////////////////////////////////////////////////////////////////////////////////////
// DBMergeChilDb Class Routines
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	MergeFiles
//
// Description:	This routine merges the ChilDB and ChSoDB files.  The ChSoDB file is
//				only created if the user chooses to sort the children as per the order
//				in the GEDCOM file.
//
//				NOTE: the first sort key is removed from the ChilDB file, as it is no
//				longer needed.
//
// Parameters:	None.
//
// Returns:		True if successful or false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBMergeChilDb::MergeFiles (void)  
{
	INT			errCode = 0;
	char		buffer2[BUFFER_SZ];
	char		buffer1[BUFFER_SZ];
	FILE		*fpT = NULL;
	wxString	filePathName1;
	wxString	filePathName2;
	wxString	filePathNameT;
	char*		dbRec1[MAX_FLDS];
	char*		dbRec2[MAX_FLDS];
	UINT		recFlds;
	INT			result;
	char*		sPos1;
	bool		eof1 = false;
	bool		eof2 = false;
	bool		handled1 = true;
	bool		handled2 = true;

 	// Open the ChSoDB database file for reading.
	filePathName1 = pGedWiseFrame->GetWorkDirectory () + DBFiles[ChSoDB]->name;
	if ((DBFiles[ChSoDB]->fp = fopen (filePathName1.c_str(), "r+t")) == NULL) {
		FileIOError ("DBMergeChilDb::MergeFiles", filePathName1, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}
	
	// Open the ChilDB database file for reading.
	filePathName2 = pGedWiseFrame->GetWorkDirectory () + DBFiles[ChilDB]->name;
	if ((DBFiles[ChilDB]->fp = fopen (filePathName2.c_str(), "r+t")) == NULL) {
		FileIOError ("DBMergeChilDb::MergeFiles", filePathName2, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}
 
	// Open the Temporary file for writing new ChilDB.
	filePathNameT = pGedWiseFrame->GetWorkDirectory () + TEMP_FILE;
	if ((fpT = fopen (filePathNameT.c_str(), "wt")) == NULL) {
		FileIOError ("DBMergeChilDb::MergeFiles", filePathNameT, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	while (true) {

		if (!eof1 && handled1) {

			eof1 = (GetFileRec (buffer1, BUFFER_SZ, DBFiles[ChSoDB]->fp) == NULL);

			if (!eof1) {
				handled1 = false;
				sPos1 = strchr (buffer1, CHR_TAB) + 1; // skip first sort key
				ReadRecFromBuf (sPos1, dbRec1, &recFlds);
				}
			}

		if (!eof2 && handled2) {
			
			eof2 = (GetFileRec (buffer2, BUFFER_SZ, DBFiles[ChilDB]->fp) == NULL);
			
			if (!eof2) {
				handled2 = false;
				ReadRecFromBuf (buffer2, dbRec2, &recFlds);
				}
			}

		if (eof1 && eof2)
			break;

		if (eof1) {
	
			result = 1;
	
		} else if (eof2) {
		
			result = -1;
		
		} else { // (!eof1 && !eof2)
		
			// Compare the F####'s
			result = strcmp (dbRec1[0+1], dbRec2[0+1]); // add 1 to skip 1st sort field

			// if F####'s are same then compare I####'s
			if (result == 0)
				result = strcmp (dbRec1[1+1], dbRec2[1+1]); // add 1 to skip 2nd sort field
			}
		
			#ifdef _DEBUG
			m_iRecCntFromFiles++;
			#endif

			if (result == 0) {
				
				dbRec1[ChilPedi+1]   = dbRec2[ChilPedi+1];   // add 1 to skip sort key
				dbRec1[ChilNoteNo+1] = dbRec2[ChilNoteNo+1]; // add 1 to skip sort key
				dbRec1[ChilCiteNo+1] = dbRec2[ChilCiteNo+1];  // add 1 to skip sort key
				
				handled1 = true;
				handled2 = true;
				result	= -1; // set as -1 to take record from dbRec1 below
				}

			if (result < 0) {

				// Save ChSoDB record field data back to a  file.
				if (!WriteRecToFile (fpT, dbRec1, ChilTotRecFlds+1)) { // add 1 for sort fld
					FileIOError ("DBMergeChilDb::MergeFiles", filePathNameT, cWriteErr);
					errCode = -1;
					goto CloseExit;
					}
				handled1 = true;

			} else { // (result > 0)
					// This would rarely happen, and only likely for adopted child.
					
					// Replace the sort key to change the I### to a child sort number. Use the
					// Fami xref plus the number 999 to make record sort to bottom of family.
					char	sortKey[XREF_SZ+DKEY_SZ+1];
					char	fmtStr[15];

					sprintf (fmtStr, "%%-%us%%u", XREF_SZ);
					sprintf (sortKey, fmtStr, dbRec2[0+1], 999); // get F#### and 999
					dbRec2[0] = sortKey;

				// Save ChilDB record field data back to a  file.
				if (!WriteRecToFile (fpT, dbRec2, 5+1)) {
					FileIOError ("DBMergeChilDb::MergeFiles", filePathNameT, cWriteErr);
					errCode = -1;
					goto CloseExit;
					}
				handled2 = true;
				}

		} // while (true)

	/////////
	CloseExit:
	/////////

	// Close the ChilDB file
	if (DBFiles[ChilDB]->fp) {
		fclose (DBFiles[ChilDB]->fp);
		DBFiles[ChilDB]->fp = NULL;
		}

	// Close the ChSoDB file
	if (DBFiles[ChSoDB]->fp) {
		fclose (DBFiles[ChSoDB]->fp);
		DBFiles[ChSoDB]->fp = NULL;
		}

	// Rename Temporary file to ChilDB file.
	if (fpT) {
		fclose (fpT);
		if (errCode < 0)
			remove (filePathNameT.c_str());
		else {
			remove (filePathName1.c_str());
			remove (filePathName2.c_str());
			rename (filePathNameT.c_str(), filePathName2.c_str());
			}
		}

	if (pGedWiseFrame->Canceled ())
		return false;

	if (errCode < 0) {
		pGedWiseFrame->SetBadExit ();
		return false;
		}

	return true;
  }

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ProcessFiles
//
// Description:	Processes the IndiDB and FamiDB files for DBSubEvenNo.
//						
// Parameters:	None.	
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBMergeChilDb::ProcessFiles (void)
{

	#ifdef _DEBUG
	m_iRecCntFromFiles = 0;
	#endif

  // Merge the ChilDB and ChSoDB databases.
	if (!MergeFiles ())
		return false;

	wxASSERT (m_iRecCntFromFiles == ChilCnt);

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
// DBCombFiles Class Routines
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	DBCombFiles
//
// Description:	Constructor for DBCombFiles.
//
// Parameters:	pParent -	parent dialog.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
DBCombFiles::DBCombFiles (GedWiseFrame* pParent)
{
	pGedWiseFrame = pParent;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	CombineFiles
//
// Description:	Combines individual database files into one large file.
//						
// Parameters:	None.
//
// Returns:		True if successfull or false if error.
/////////////////////////////////////////////////////////////////////////////////////////
bool DBCombFiles::CombineFiles (void)
{
	char		buffer[MAX_FLINE_SZ]; // max possible size of line is note record
	INT			errCode = 0;
	wxString	filePathName;
	wxString	filePathNameOrg;
	UINT		dbRef = 0;
	UINT		i;
	char*		startBuf;

	// Open-create the Comb database file
	filePathNameOrg = pGedWiseFrame->GetWorkDirectory () + DBFiles[PdbDB]->name;
	if ((DBFiles[PdbDB]->fp = fopen (filePathNameOrg.c_str(), "wt")) == NULL) {
		FileIOError ("DBCombFiles::CombineFiles", filePathNameOrg, cOpenErr);
		errCode = -1;
		goto CloseExit;
		}

	for (i = GRecDBc; i < TotlDBc; i++) {
		// Open each file to be combined into the Comb file
		switch (i)
			{
			case GRecDBc:
				dbRef = GRecDB;
				break;
			case IndiDBc:
				dbRef = IndiDB;
				break;
			case EvenDBc:
				dbRef = EvenDB;
				break;
			case SCitDBc:
				dbRef = SCitDB;
				break;
			case RCitDBc:
				dbRef = RCitDB;
				break;
			case FamiDBc:
				dbRef = FamiDB;
				break;
			case ChilDBc:
				dbRef = ChilDB;
				break;
			case SourDBc:
				dbRef = SourDB;
				break;
			case RepoDBc:
				dbRef = RepoDB;
				break;
			case NoteDBc:
				dbRef = NoteDB;
				break;
			default:
				wxASSERT (false);
				break;
				}
		
		DBFiles[dbRef]->fp = NULL;
		filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[dbRef]->name;
		if ((DBFiles[dbRef]->fp = fopen (filePathName.c_str(), "rt")) == NULL) {
			FileIOError ("DBCombFiles::CombineFiles", filePathName, cOpenErr);
			errCode = -1;
			goto CloseExit;
			}

		// Copy the database line by line.
		while (true) {

			if (fgets (buffer, MAX_FLINE_SZ, DBFiles[dbRef]->fp) == NULL) // do not call GetFileRec
				
				if (feof (DBFiles[dbRef]->fp)) { // check if we at end of GEDCOM file
					break;
					}
				else // must be a read error
					goto CloseExit;
	
				startBuf = buffer;

				// for the following db files, eliminate the xRefNo before writing to file
				if (i == FamiDBc || i == ChilDBc || i == SourDBc || i == RepoDBc)
					FindUpToNextTab (&startBuf);

			// copy info to combined data file
			if (fprintf (DBFiles[PdbDB]->fp, "%s", startBuf) < 0) {
				FileIOError ("DBCombFiles::CombineFiles", filePathName, cWriteErr);
				errCode = -1;  // signal error
				goto CloseExit;
				}
			}
		 
		fprintf (DBFiles[PdbDB]->fp, "%s\n", EOD_MARKER); // mark end of database
		fclose (DBFiles[dbRef]->fp);
		DBFiles[dbRef]->fp = NULL;
		remove (filePathName.c_str());
		}

	/////////
	CloseExit:
	/////////

	// close the files just used.
	if	(DBFiles[dbRef]->fp) {
		fclose (DBFiles[dbRef]->fp);
		DBFiles[dbRef]->fp = NULL;  // this must be done
		}
	
	if (DBFiles[PdbDB]->fp) {
		fclose (DBFiles[PdbDB]->fp);
		DBFiles[PdbDB]->fp = NULL;  // this must be done
		}

	// remove the IndxDB file
	filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[IndxDB]->name;
	remove (filePathName.c_str());

	if (errCode < 0)
		return false;

	return true;
}
