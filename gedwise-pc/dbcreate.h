/////////////////////////////////////////////////////////////////////////////////////////
// Program:			GedWise 7.0
//
// File:			DBCreate.h
//
// Written by:		Daniel T. Rencricca
//
// Last Revision:	March 9, 2006
//
// Description:		Header file for DBCreate.cpp.
/////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DBCREATE_H_
#define _DBCREATE_H_

#include "gedwise.h"
#include "defines.h"

// --------------------------------------------------------------------------------------
// Defines and Enumerated Types
// --------------------------------------------------------------------------------------

#define	TEMP_FILE	"_tmpFile.dat"
#define	TEMP_FILE2	"_tmpFile2.dat"

typedef enum { // THE ORDER BELOW MUST NOT CHANGE
	GRecDBc,
	IndiDBc,
	EvenDBc,
	SCitDBc,
	RCitDBc,
	FamiDBc,
	ChilDBc,
	SourDBc,
	RepoDBc,
	NoteDBc,
	TotlDBc
	};

typedef struct {
 char	xRefKey[XREF_SZ+1];
 char	recNo[RNUM_SZ+1];
 char	aliaKey[ALIA_SZ+1];
 } IndxRecType;

typedef struct {
 char	xRefKey[XREF_SZ+1];
 UINT	chiCnt;
 UINT	recCnt;
  } FamCRecArType;

typedef struct {
 fpos_t	pos;  // position in EvenDB file (size = 8 bytes)
 } EvenRecArType;


// --------------------------------------------------------------------------------------
// Class Definitions
// --------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// DBCreateIndex Class																						//
/////////////////////////////////////////////////////////////////////////////////////////
class DBCreateIndex// : public wxApp  
{
public:
	DBCreateIndex (GedWiseFrame* pParent = NULL);
	~DBCreateIndex () {};
	bool		CreateIndex (void);

private:

	GedWiseFrame*	pGedWiseFrame;
};

/////////////////////////////////////////////////////////////////////////////////////////
// DBIndiRefSwap Class																						//
/////////////////////////////////////////////////////////////////////////////////////////
class DBIndiRefSwap// : public wxApp  
{
public:
	DBIndiRefSwap (GedWiseFrame* pParent = NULL);
	~DBIndiRefSwap ();
	bool		ProcessDB (void);

private:
	INT32		FindRecNo (char* xRefKey, char* aliaNoStr, bool retAliaStr = false) ;
	bool		AllocateArray (UINT maxRecs);
	void		DeallocateArray (void);
	bool		LoadIndexArray (void);
	bool		PrepDB (UINT dbNum);

	IndxRecType	**m_arIndxRec;
	UINT32		m_iArraySz;

	GedWiseFrame*	pGedWiseFrame;
};

/////////////////////////////////////////////////////////////////////////////////////////
// DBFamiRefSwap Class																							//
/////////////////////////////////////////////////////////////////////////////////////////
class DBFamiRefSwap// : public wxApp  
{
public:
	DBFamiRefSwap (GedWiseFrame* pParent = NULL);
	~DBFamiRefSwap ();
	bool		ProcessFiles (void);

private:
	bool		AllocateArray (void);
	void		DeallocateArray (void);
	bool		LoadFamCArray (void);
	INT32		FindRecNo (char* xRefKey);
	bool		PrepFamiDB (void);
	bool		PrepIndiDB (void);

	FamCRecArType	**m_arFamCRec;
	UINT32			m_dFamCArraySz;
	UINT				m_iFamcCnt;

	GedWiseFrame*	pGedWiseFrame;
};

/////////////////////////////////////////////////////////////////////////////////////////
// DBSubEvenNo Class																							//
/////////////////////////////////////////////////////////////////////////////////////////
class DBSubEvenNo// : public wxApp  
{
public:
	DBSubEvenNo (GedWiseFrame* pParent = NULL);
	~DBSubEvenNo ();
	bool		ProcessFiles (void);

private:
	bool		AllocateArray (void);
	void		DeallocateArray (void);
	bool		LoadEvenArray (void);
	bool		SubEvenNo (UINT dbNum);
	bool		EvenRefSwap (char* evenNoStr);
	bool		PrepDBFiles (void);
	bool		ProcessDb (void);

	#ifdef _DEBUG
	void		CheckEvenNos (void); // checks that event numbers ordered correctly
	#endif

	EvenRecArType**	m_arEvenRec;
	UINT32			m_dEvenArraySz;
	UINT			m_iEvenNoNew;
	UINT			m_iEvenCnt;
	FILE*			m_fpEvenT;
	wxString		m_sFilePathNameEvenT; // Temporary file for writing event records

	GedWiseFrame*	pGedWiseFrame;
};

/////////////////////////////////////////////////////////////////////////////////////////
// DBSubRefNo Class																							//
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
	char	xRefKey[XREF_SZ+1];
  } XRefArrayType;


class DBSubRefNo// : public wxApp  
{
public:
	DBSubRefNo (GedWiseFrame* pParent = NULL);
	~DBSubRefNo (void);
	bool		ProcessFiles (void);

protected:	
	bool		AllocateArray (UINT maxRecs);
	void		DeallocateArray (void);
	bool		LoadXRefArray (UINT dbNum);
	INT32		FindRecNo (char* xRefKey);
	bool		PrepDB (UINT dbNum);

	XRefArrayType**	m_arXRefRec;
	UINT		m_iArraySz;
	UINT		m_iRecCnt;

	GedWiseFrame*	pGedWiseFrame;
};

/////////////////////////////////////////////////////////////////////////////////////////
// DBMergeChilDb Class																						//
/////////////////////////////////////////////////////////////////////////////////////////
class DBMergeChilDb
{
public:
	DBMergeChilDb (GedWiseFrame* pParent = NULL) {pGedWiseFrame = pParent;};
	~DBMergeChilDb (){};
	bool		ProcessFiles (void);

private:
	bool		MergeFiles (void);

	GedWiseFrame*	pGedWiseFrame;

	#ifdef _DEBUG
	UINT		m_iRecCntFromFiles;
	#endif

};

/////////////////////////////////////////////////////////////////////////////////////////
// DBCombFiles Class																							//
/////////////////////////////////////////////////////////////////////////////////////////
class DBCombFiles
{
public:
	DBCombFiles (GedWiseFrame* pParent = NULL);
	~DBCombFiles (void) {};
	
	bool		CombineFiles (void);

	GedWiseFrame*	pGedWiseFrame;
};

#endif // _DBCREATE_H_
