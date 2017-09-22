/////////////////////////////////////////////////////////////////////////////////////////
// Program:		GedWise 7.0
//				Copyright © 2001 - 2005 Battery Park Software Corporation
//
// Module:		MakePDB.cpp
//
// Written by:	Daniel T. Rencricca
//
// Date:		July 15, 2002
// Revised		March 9, 2005
//
// Description:	This module creates the Palm database "pdb" file from the raw data
//				file created in the DBCreate module.  In creating the database this 
//				module packs up to PACK_REC_MAX records into each Palm record.
/////////////////////////////////////////////////////////////////////////////////////////

#include "stdwx.h"
#include "makepdb.h"
#include "parsegedcom.h"
#include "miscfunctions.h"

#ifdef _DEBUG // include after last #include statement in cpp file where 'new' is used.
#define new DEBUG_NEW
#endif 

extern DBFileRec	DBFiles[][DBCount];
extern UINT			CheckNo;

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	MakePDB
//
// Description:	Constructor for MadPDB. Initializes variables.
//
// Parameters:	None.
//                                      
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
MakePDB::MakePDB (GedWiseFrame* pParent)
{
	pGedWiseFrame = pParent;

	// initialize variables
	fo				= NULL;
	fi				= NULL;
	BigBuffer		= NULL;
	TmpBuffer		= NULL;
	m_pTmpFile		= NULL;
	m_iRecCnt		= 0;
	m_bDataInBuffer	= false;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	~MakePDB
//
// Description:	Destructor for MakePDB. Closes all files and erases temporary files.
//
// Parameters:	None.
//                                      
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
MakePDB::~MakePDB (void)
{
	if (fo)
		fclose (fo);

	if (fi)
		fclose (fi);

	if (m_pTmpFile)
		fclose (m_pTmpFile);

	remove (m_sInFilePathName.c_str());
	remove (m_sTmpFileName.c_str());

	if (pGedWiseFrame->GetBadExit ())
		remove (m_sOutFileName.c_str());

	if (BigBuffer) {
		delete BigBuffer;
		BigBuffer = NULL;
		}

	if (TmpBuffer) {
		delete TmpBuffer;
		TmpBuffer = NULL;
		}
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	InitFiles
//
// Description:	Opens the files used by the MakePDB class.
//
// Parameters:	None.
//                                      
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
bool MakePDB::InitFiles (void)
{
   m_sInFilePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[PdbDB]->name;
	if ((fi = fopen (m_sInFilePathName.c_str(), "rt")) == NULL) {
		FileIOError (" MakePDB::InitFiles", m_sInFilePathName, cOpenErr);
		return false;
		}

	m_sOutFileName = pGedWiseFrame->GetWorkDirectory () + pGedWiseFrame->GetDbName () +
		PDB_DB_FILE;
	if ((fo = fopen (m_sOutFileName.c_str(), "wb")) == NULL) {
		FileIOError (" MakePDB::InitFiles", m_sOutFileName, cOpenErr);
		return false;
		}

	m_sTmpFileName = pGedWiseFrame->GetWorkDirectory () + TEMP_PDB_FILE;
	if ((m_pTmpFile = fopen (m_sTmpFileName.c_str(), "w+b")) == NULL) {
		FileIOError (" MakePDB::InitFiles", m_sTmpFileName, cOpenErr);
		return false;
		}

	try {
		BigBuffer = new char[PALM_REC_SZ_MAX+2];
		TmpBuffer = new char[MAX_FLINE_SZ+1];
		}
	catch (...) {
		return false;
		}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OutByte
//
// Description:	Writes a byte to the open file stream. The file	must be open in
//				the "byte" mode prior to callng this function.
//
// Parameters:	b	-	the byte to write.
//                                      
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void MakePDB::OutByte (int b)
{
	putc (b, fo);  // write only lower 8 bits of int
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OutShort
//
// Description:	Writes a WORD to the open file stream reversing the bytes. The file
//				must be open in the "byte" mode prior to calling this function.
//
// Parameters:	s	-	the WORD (2 bytes) to write.
//                                      
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void MakePDB::OutShort (WORD s)
{
//	#if defined (NO_REV_BYTES)
//		putc (s, fo);
//		putc (s >> 8, fo);
//	#else
		putc (s >> 8, fo);
		putc (s, fo);
//	#endif
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OutLong
//
// Description:	Writes a DWORD to the open file stream reversing the bytes. The file
//				must be open in the "byte" mode prior to calling this function.
//
// Parameters:	l	-	the DWORD (4 bytes) to write.
//                                      
// Returns:		Nothing.
//
/////////////////////////////////////////////////////////////////////////////////////////
void MakePDB::OutLong (DWORD l)
{
//	#if defined (NO_REV_BYTES)
//		putc (l, fo);
//		putc (l >> 8, fo);
//		putc (l >> 16, fo);
//		putc (l >> 24, fo);
//	#else
		putc (l >> 24, fo);
		putc (l >> 16, fo);
		putc (l >> 8, fo);
		putc (l, fo);
//	#endif
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OutBytes
//
// Description:	Writes a sequence of "nByte" bytes (of type CHAR) to the open file
//				stream. The file must be open in the "byte" mode prior to calling
//				this function.
//
// Parameters:	b		-	pointer to byte(s) to write.
//				nBytes	-	number of bytes to write.
//                                      
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void MakePDB::OutBytes (char *b, UINT nByte)
{
	fwrite (b, nByte, 1, fo);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OutText
//
// Description:	Writes a text string of "len" length to the open file. The string is 
//				padded with 0's if the string length is less than "len". The file must
//				be open in the "byte" mode prior to calling this function.
//
// Parameters:	b	-	pointer to string to write.
//				len	-	length of string to write.
//                                      
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void MakePDB::OutText (char *b, UINT len)
{
	UINT sl = strlen (b);

	wxASSERT ((sl + 1) <= len);

	OutBytes (b, sl + 1);
	len -= sl + 1;

	while (len-- > 0) {
		OutByte (0);
		}
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GetGedcomInfo
//
// Description:	Reads an the Gedcom Information file and packes a record for the
//				Application	Info Block of the IndiDB database.
//
// Parameters:	buf	- pointer to a string containing the GEDCOM information.
//				rec	- record that contains GEDCOM information.
//                                      
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void MakePDB::GetGedcomInfo (void* buf)
{
	char*	p = (char *) buf;

	char*	date		= FindUpToNextTab (&p);
	char*	version		= FindUpToNextTab (&p);
	char*	fileName	= FindUpToNextTab (&p);
	char*	loadDate	= FindUpToNextTab (&p);
	char*	peopleCnt	= FindUpToNextTab (&p);

	ReplaceTabs ((char *) buf);  // replace tabs with null terminators

	strncat (AppInfoRec.gedDate, date, sizeof (AppInfoRec.gedDate)-1);
	AppInfoRec.dbVers	= atoi (version);
	strncat (AppInfoRec.gedFile, fileName, sizeof (AppInfoRec.gedFile)-1);
	strncat (AppInfoRec.loadDate, loadDate, sizeof (AppInfoRec.loadDate)-1);
	AppInfoRec.peopleCnt = atoi (peopleCnt);

	return;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ReadDBRecord
//
// Description:	Reads an record from a text file and formats the record for a Palm OS 
//				database.  It creates a "flags" field for each record to track the
//				fields that have data.  It also packs small records into a mega record
//				for all databases.  Each record is packed up to PACK_REC_MAX mini-
//				records, except for the NoteDB which is packed until the record is
//				filled up to the MAX_PALM_REC_SZ.
//
// Parameters:	buf		-	pointer to the buffer that will contain finalized record.
//				recSize -	size of record we are writing to HH.
//                                      
// Returns:		 1	= good record, so save it to HH
//				 0	= end of file
//				-1	= error or user pressed cancel
/////////////////////////////////////////////////////////////////////////////////////////
int MakePDB::ReadDBRecord (void* buf, WORD *recSize)
{
	WORD	recCnt = 0;
	WORD	flags;
	char*	s = (char*) buf;
	char*	pFlags;
	char*	recSzP;
	WORD	recSz;
	WORD	startDBPos;
	
	*recSize = 0; // initialize	

	while (true) {

		// Read next record from file.  If the last record read was from
		// the NOTE database and that record was too large to add
		// to the record being created, then that NOTE would be still be
		// in the TmpBuffer and dataInBuffer == true;
		if (!m_bDataInBuffer) {

			if (GetFileRec (TmpBuffer, MAX_FLINE_SZ, fi) == NULL) {
				if (feof (fi)) {
					break;
					}
				else
					return -1; // signal error
				}

			if ((CheckNo & DB_PROG_DIV) == 0) {
			 	pGedWiseFrame->CheckStatus ();
				if (pGedWiseFrame->Canceled ())
				return -1;
				}
			CheckNo++;
			}

		if (m_iDbNum == NoteDBm) {
			int len = strlen (TmpBuffer);
			if (len > PALM_REC_SZ_MAX - (s - (char *) buf) - 10) { // subtract another 10 just in case
				m_bDataInBuffer = true;
				break;
				}
			else
				m_bDataInBuffer = false;
			}

		// Store start positions of each mini-database in a combined database
		if (!strncmp (TmpBuffer, EOD_MARKER, sizeof (EOD_MARKER)-1)) {
			m_iDbNum++;
			if (m_iDbNum >= CountDBm)
				break;

			// The next line is very important.  We want to set the startDBPos to
			// the following line number in the file only if we have added at
			// least one mini-record in the mega-record currently being processed.
			startDBPos = m_iRecCnt + ((recCnt > 0) ? 1 : 0);
			AppInfoRec.startDB[m_iDbNum] = startDBPos;
			if (recCnt == 0)
				continue;
			else
				break;
			}
		
			char	 *p = TmpBuffer;
			char	 *fields[MAX_FLDS];
			DbRecord *rec = new DbRecord;
	
			// Load up the rec record
			while (rec->m_iFldCount < MAX_FLDS) {
				fields[rec->m_iFldCount] = FindUpToNextTab(&p);
				if (*fields[rec->m_iFldCount] == '\n')
					break;
				rec->m_iFldCount++;
				}
			ReplaceTabs (TmpBuffer); // replace tabs with null terminators

			if (m_iDbNum == NoteDBm)
				ReplaceReturns(fields[NOTETEXT_FLD]); // replace Returns with Null terminators

			// Copy data into record structure.
			for (int x=0; x < rec->m_iFldCount; x++) {
				rec->fields[x] = new char[strlen(fields[x])+1];
				strcpy(rec->fields[x], fields[x]);
				}

			// Add 2-bytes for flags and 2 bytes for sort number before each mini-record.
			recSzP = s; 
			pFlags = s + sizeof (WORD);
			s+= sizeof (WORD) + sizeof (WORD); // add space for flags and record size
		
			*(WORD *) &flags = 0;  // initialize flag bits to zeros.

			// set each flag bit for the data in the record
			for (int i=0; i < rec->m_iFldCount; i++) {
				if (*rec->fields[i]) {
					SetBitMacro (flags, i);
					strcpy (s, rec->fields[i]);
					s+= strlen(s) + 1;
					}
				}						  

			// must reverse bytes for HH
			#if defined (NO_REV_BYTES)
				*((WORD *)pFlags) = *(WORD *)&flags;
			#else
				*((WORD *)pFlags) = SwapShort (*(WORD *)&flags);
			#endif
				
			delete rec;

			recCnt++;
			
			// We have to align each starting record on the WORD for the HH
			if ((s - (char *) buf) % 2 != 0) {
				*s= PAD_CHR;
				s++;
				}

			// set record size
			recSz = s - recSzP;

			#if defined (NO_REV_BYTES)
				*((WORD *) recSzP) = recSz;
			#else
				*((WORD *) recSzP) = SwapShort (recSz);
			#endif			

			if (m_iDbNum != NoteDBm && recCnt >= PACK_REC_MAX)
				break;

		}  // main while loop

	if (recCnt == 0)  // then we are at end of file
		return 0;

	*recSize = s - (char *) buf;

	return 1; // sucess
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	CopyDBRec
//
// Description:	Copies records to a temporary file, packing each record.
//				Each record is immediately preceded by the length of that record.
//
// Parameters:	None.
//                                      
// Returns:		0 if successful or the error number if error occurred.			
/////////////////////////////////////////////////////////////////////////////////////////
bool MakePDB::CopyDBRec (void)
{
	WORD	recSize;
	int		contRead;
	
	m_iDbNum = IndiDBm;  // initialize

	// initialize AppInfoRec. The first element is always 0
	memset (&AppInfoRec, 0, sizeof(AppInfoRec));

	if (GetFileRec (TmpBuffer, MAX_FLINE_SZ, fi) == NULL)
		goto ErrExit;
	
	GetGedcomInfo (TmpBuffer);

	// skip next line of file. It will have database separator.
	if (GetFileRec (TmpBuffer, MAX_FLINE_SZ, fi) == NULL)
		goto ErrExit;

	// read record from PC file and convert record to HH format
	while (true) {

		contRead = ReadDBRecord (BigBuffer, &recSize);

		if (contRead < 0)  // error occurred
			goto ErrExit;

		if (contRead == 0) // at end of file
			break;

		m_iRecCnt++;

		// write size of record first
		if (fwrite ((char*) &recSize, sizeof (WORD), 1, m_pTmpFile) < 1)
			goto ErrExit;

		// now write the record
		if (fwrite (BigBuffer, sizeof (char), recSize, m_pTmpFile) < recSize)
			goto ErrExit;

		} // while loop

	return true;

	///////
	ErrExit:
	///////

	if (pGedWiseFrame-> Canceled ())
		return false;

	pGedWiseFrame->SetBadExit ();
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	CreatePDB
//
// Description:	Creates the PDB database.  The header contains 78 bytes, including the
//				name of the database and other information
//
// Parameters:	None.
//                                      
// Returns:		True if successful else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool MakePDB::CreatePDB (void)
{
	DWORD		recOffset;
	DWORD		appInfoOffset;
	time_t		today;
	//long		today; // 4/7/2006 DTR
	WORD		recSize;
	WORD		recPos;
	long		recID;
	int			x, i;
	char		dbName[MAX_PDB_NAME_SZ];
	wxString 	hldString;

	// Open files we need to use.
	if (!InitFiles ())
		goto ErrExit;

	// Copy records to a file, each record preceeded by its length.
	if (!CopyDBRec ())
		goto ErrExit;

	rewind (m_pTmpFile);

   // -- Create PDB Header -- //
	time (&today);

	sprintf (dbName, "%s-Rend", pGedWiseFrame->GetDbName ().c_str());

	appInfoOffset	= PDB_HEADER_SZ + (m_iRecCnt * PDB_REC_ENTRY_SZ)  + BUF_BEFORE_RECS;
	recOffset		= appInfoOffset + sizeof (AppInfoRec);

   // Header Fields
	OutText	(dbName, MAX_PDB_NAME_SZ);	// name
	OutShort (0);			 			// attribute flags
	OutShort (0);						// version
	OutLong	(today + TIME_OFFSET);		// creationTime
	OutLong	(today + TIME_OFFSET);		// modificationTime
	OutLong	(0);						// backupTime
	OutLong	(0); 						// modificationNumber
	OutLong	(appInfoOffset);			// appInfoOffset
	OutLong	(0); 						// sortInfoOffset
	OutBytes (DB_TYPE, 4); 				// type
	OutBytes (CREATOR_ID_STR, 4);		// Creator
	OutLong	(0); 						// uniqueID - Palm default is 0
	OutLong	(0); 						// nextRecordID - Must be 0
	OutShort (m_iRecCnt);				// numRecords

	wxASSERT (ftell (fo) == PDB_HEADER_SZ);

	if (ferror (fo)) // check for file write error
		goto ErrExit;

	// -- Create PDB Record List -- //

	// generate random number for record's unique ID
	srand ( (unsigned) time (NULL) );
	
	#ifdef __WXMSW__
	
	recID = rand() * (rand () % 100);
	
	#else
	
	recID = rand();
	
	#endif
	
	if (recID < 0) // make sure number is positive
		recID = recID * -1;

	// make sure max number is 0xFFFFFF
	while (recID > RECID_MAX) // need to check on Linux builds b/c RAND_MAX is higher
		recID = recID % RECID_MAX;
	
	wxASSERT (recID <= RECID_MAX);
	wxASSERT (recID >=0);

	i = 0;

	while (true) {

		OutLong	(recOffset);		// offset
		OutByte	(0);				// attributes for record
		OutByte	(recID >> 16);		// unique ID for record (byte 1)
		OutShort (recID % 65536);	// unique ID for record (bytes 2 and 3)

		i++; recID++;
		if (i == m_iRecCnt)
			break;

		if (fread ((WORD*) &recSize, sizeof (WORD), 1, m_pTmpFile) < 1)
			goto ErrExit;
		recOffset+= recSize;
		recPos = fseek (m_pTmpFile, (LONG) recSize, SEEK_CUR);
		
		} // while loop

	wxASSERT (i == m_iRecCnt);

   // write 2 bytes for padding before Application Info Block Record
	OutByte (0);
	OutByte (0);

	// -- Write Application Info Block Record --
	OutShort (AppInfoRec.dbVers);
	OutText	 (AppInfoRec.gedDate, sizeof (AppInfoRec.gedDate));
	OutText	 (AppInfoRec.gedFile, sizeof (AppInfoRec.gedFile));
	OutText	 (AppInfoRec.loadDate, sizeof (AppInfoRec.loadDate));
	OutShort (AppInfoRec.peopleCnt);

	for (x=0; x < CountDBm; x++)
		OutShort (AppInfoRec.startDB[x]);

	if (ferror (fo)) // check for file write error
		goto ErrExit;

	// -- Write the actual records in the database. --
	i = 0;
	rewind (m_pTmpFile);

	while (i < m_iRecCnt) {
		fread ((WORD *)&recSize, sizeof (WORD), 1, m_pTmpFile);	// read size of next record
		fread (BigBuffer, sizeof (char), recSize, m_pTmpFile);	// read the record up to recSize bytes
		OutBytes (BigBuffer, recSize);							// write the record to the pdb

		if (ferror (fo) || ferror (m_pTmpFile)) // check for file write error
			goto ErrExit;

		i++;
		}

	return true;

	///////
	ErrExit:
	///////

	if (pGedWiseFrame-> Canceled ())
		return false;

	pGedWiseFrame->SetBadExit ();

	return false;
}
