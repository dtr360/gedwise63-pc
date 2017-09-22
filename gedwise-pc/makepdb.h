/////////////////////////////////////////////////////////////////////////////////////////
// Program:			GedWise 7.0
//
// File:			MakePDB.h
//
// Written by:		Daniel T. Rencricca
//
// Last Revision:	April 16, 2004
//
// Description:		Header file for MakePDB.h.
/////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MAKEPDB_H_
#define _MAKEPDB_H_

#include "gedwise.h"
#include "defines.h"

/////////////////////////////////////////////////////////////////////////////////////////
// Defines and Typedefs																						//
/////////////////////////////////////////////////////////////////////////////////////////
#define PACK_REC_MAX		40   // Mini-records in each mega-record
#define MAX_PDB_NAME_SZ		32   // max size of database name and path
#define NOTETEXT_FLD		1	 // field in Note Record that contains the note text
#define PAD_CHR				0x55 // pad char to make mini record even sized (WORD aligned)

#define PALM_REC_SZ_MAX		65000	// Max size of Palm Record
#define PBD_REC_CNT_POS		76		// location of record count in db Header

#define TIME_OFFSET			2082844886LU	// time offset from Windows values
#define PDB_HEADER_SZ		78
#define PDB_REC_ENTRY_SZ	8
#define BUF_BEFORE_RECS		2
#define RECID_MAX			0xFFFFFF		// max size for Palm record id number

#define TEMP_PDB_FILE		"_tmpPdb.dat"

#define BitAtPosition(pos)	((WORD) 1 << (pos))
#define SetBitMacro(bitfield, index) ((bitfield) |= BitAtPosition(index))

typedef enum { // THIS ORDER MUST MATCH THAT IN GedWisePC APP.
	IndiDBm,
	EvenDBm,
	SCitDBm,
	RCitDBm,
	FamiDBm,
	ChilDBm,
	SourDBm,
	RepoDBm,
	NoteDBm,
	CountDBm
	};

/////////////////////////////////////////////////////////////////////////////////////////
// Application Info Block Record																			//
/////////////////////////////////////////////////////////////////////////////////////////
struct AppInfoRecord
{
	WORD	dbVers;
	char	gedDate[15];
	char	gedFile[26];
	char	loadDate[15];
	WORD	peopleCnt;
	WORD	startDB[CountDBm];
};

/////////////////////////////////////////////////////////////////////////////////////////
// MakePDB Class Definition																				//					//
/////////////////////////////////////////////////////////////////////////////////////////
class MakePDB// : public wxApp
{
// Construction
public:
	MakePDB (GedWiseFrame* pParent = NULL);
	~MakePDB (void);
	bool		CreatePDB (void);

protected:
	bool		InitFiles (void);
	void		OutByte (int b);
	void		OutShort (WORD s);
	void		OutLong (DWORD l);
	void		OutBytes (char *b, UINT nByte);
	void		OutText (char *b, UINT len);
	
	int			ReadDBRecord (void* buf, WORD *recSize);
	bool		CopyDBRec (void);
	void		GetGedcomInfo (void* buf);

	FILE*		fo;  // file being written to
	FILE*		fi;  // file being read from
	FILE*		m_pTmpFile;
	wxString	m_sInFilePathName;
	wxString	m_sOutFileName;
	wxString	m_sTmpFileName;
	WORD		m_iRecCnt;
	WORD		m_iDbNum;
	bool		m_bDataInBuffer;
	char*		BigBuffer;
	char*		TmpBuffer;

	GedWiseFrame*	pGedWiseFrame;
	AppInfoRecord	AppInfoRec;
};

/////////////////////////////////////////////////////////////////////////////////////////
// Generic GedWise Database Record																		//
/////////////////////////////////////////////////////////////////////////////////////////
struct DbRecord
{
	DbRecord () {for (int i = 0; i<MAX_FLDS;i++) fields[i] = 0; m_iFldCount = 0;}
	~DbRecord () {for (int i = 0; i < m_iFldCount; i++) delete [] fields[i];}

	#pragma pack(2)
	char* fields[MAX_FLDS];
	int	m_iFldCount;
	#pragma pack()
};

/////////////////////////////////////////////////////////////////////////////////////////
// PDB Database Format Information																		//
/////////////////////////////////////////////////////////////////////////////////////////
/*
-- PDB Format

PBDHeader
PDBRecordEntry x numRecords
2 bytes 
AppInfo (if applicable)
SortInfo (if applicable)
Records

-- Palm Database Header - for information purposes [78 Bytes]
typedef struct PDBHeader
{
	char    name[MAX_PDB_NAME_SZ];
	WORD	attribute flags;
	WORD	version;
	DWORD	creationTime;
	DWORD	modificationTime;
	DWORD	backupTime;
	DWORD	modificationNumber;
	DWORD	appInfoOffset;
	DWORD	sortInfoOffset;
	DWORD	type;
	DWORD	creator;
	DWORD	uniqueID;
	DWORD	nextRecordID;
	WORD	numRecords;
} PDBHeader;

-- Palm Record Entry Data - for information purposes [8 Bytes]
typedef struct PDBRecordEntry
{
	DWORD	offset;
	BYTE	attr;
	BYTE	uniqueID[3];
} PDBRecordEntry;

enum PDBFlags {
	pdbResourceFlag		= 0x0001,   // Is this a resource file ?
   pdbReadOnlyFlag		= 0x0002,   // Is database read only ?
   pdbAppInfoDirtyFlag	= 0x0004,   // Is application info block dirty ?
   pdbBackupFlag		= 0x0008,   // Back up to PC if no conduit defined
   pdbOKToInstallNewer	= 0x0010,   // OK to install a newer version if current database open
   pdbResetAfterInstall	= 0x0020,   // Must reset machine after installation
   pdbStream			= 0x0080,   // Used for file streaming
   pdbOpenFlag			= 0x8000		// Not closed properly
};

enum PDBVersion {
	pdbVerReadOnly		= 0x0001,
	pdbVerWrite			= 0x0002,
	pdbVerReadWrite		= 0x0003,
	pdbVerLeaveOpen		= 0x0004,
	pdbVerExclusive		= 0x0008,
	pdbVerShowSecret	= 0x0010
};
*/

#endif // _MAKEPDB_H_
