/////////////////////////////////////////////////////////////////////////////////////////
// Program:			GedWise 6.0
//
// File:			SortRoutines.h
//
// Written by:		Daniel T. Rencricca
//
// Last Revision:	April 25, 2004
//
// Description:		Header file for SortRoutines.cpp.
/////////////////////////////////////////////////////////////////////////////////////////

#ifndef _SORT_ROUTINES_H_
#define _SORT_ROUTINES_H_

#include "gedwise.h"
#include "defines.h"

/////////////////////////////////////////////////////////////////////////////////////////
// Constant Definitions
/////////////////////////////////////////////////////////////////////////////////////////
#define SRTFILE "_sort%03d.dat"	// Temporary sort file name
#define HLDFILE "_holder.dat"	// Temporary file to hold sorted data.

const char cErrFileOpen[]	= "Error #%s opening file: %s\n";
const char cErrFileRead[]	= "Error #%s reading from file: %s\n";
const char cErrFileWrite[]	= "Error #%s writing to file: %s\n";
const char cErrFileRen[]	= "Error #%s renaming file: %s\n";
const char cErrFileClose[]	= "Error #%s closing file: %s\n";
const char cAttemptRename[]	= "Re-attempting file rename #%s\n";
const char cNoMemory[]		= "Error #%s insufficient memory for array.\n";

#define IDS_PROG_FILESORT	"Sorting file %u of %u"
#define IDS_PROG_INDXSORT	"Sorting Index"
#define IDS_PROG_CHILSORT	"Sorting Child Database"


// Note that the number of sort files makes the biggest difference in sorting time.
// Increasing the BUF_ARR_SZ seems to slow down the sort time signficantly.
#define MAX_LN_SZ		36000	// max size of line read from input file (eg NoteDB)
#define SRT_FL_ARR_SZ	24		// max number of temporary sort files created
#define BUF_ARR_SZ		24		// max number of elements for buffer array
#define	MIN_ARR_SZ		3		// minimum size of m_aSrtFlArr & m_aBufArr arrrays
#define	PROG_DIV		63		// num passes to skip bef prog bar update (must be power of 2 less 1)
#define	FNAME_SZ		16		// maximum size of a file name (eg "_sort000.dat")

/////////////////////////////////////////////////////////////////////////////////////////
//Define the maximum size of the sort key.
const int	SRT_KEY_SZ	= NAME_SZ*2 + TIT_SZ + BIRY_SZ;
const int	IDX_KEY_SZ	= XREF_SZ + RNUM_SZ + AFLG_SZ;
/////////////////////////////////////////////////////////////////////////////////////////

typedef char DataLineType[MAX_LN_SZ+1];  // a line of data read from input file.

typedef struct { // holds line of data and its sort key
	char			key[SRT_KEY_SZ+1];
	DataLineType	dataLine;
	} BufRecType;
 
typedef struct {
	FILE*		fp;				// file pointer to a temporary sort file
	char		name[FNAME_SZ];	// sort file name (eg _sort001.dat) [test w/ sz 160000000]
	BufRecType	rec;			// line records
	bool		eof;			// end of file flag
	} SrtFlRecType;


/////////////////////////////////////////////////////////////////////////////////////////
// These enumerated types represent the lexicographic sort order for the Palm OS
// Character set.  Do NOT change the order of these.
/////////////////////////////////////////////////////////////////////////////////////////
typedef enum
	{
	CHR_NIL,
	CHR_QT,		// '
	CHR_HYPH,	// -
	CHR_SPC,	//   SPACE
	CHR_EXC,	// ! EXCLAMATION MARK
	CHR_QTS,	// " DOUBLE QUOTES
	CHR_PND,	// # POUND SIGN
	CHR_DOL,	// $
	CHR_PCT,	// %
	CHR_AMP,	// &
	CHR_OPAR,	// (
	CHR_CPAR,	// )
	CHR_AST,	// *
	CHR_COMA,	// ,
	CHR_PER,	// .
	CHR_FSL,	// /
	CHR_COL,	// :
	CHR_SCOL,	// ;
	CHR_QST,	// ?
	CHR_ATS,	// @
	CHR_OBRK,	// [
	CHR_BSL,	// \ BACKSLASH
	CHR_CBRK,	// ]
	CHR_CRT,	// ^
	CHR_UNDL,	// _
	CHR_CQT,	// `
	CHR_OBRC,	// {
	CHR_VLN,	// |
	CHR_CBRC,	// }
	CHR_TILD,	// ~
	CHR_IEXC,	// INVERTED EXLCAMATION POINT
	CHR_RQT,	// RIGHT SINGLE QUOTE
	CHR_IQST,	// INVERTED QUESTION MARK
	CHR_RDQT,	// RIGHT DOUBLE QUOTE
	CHR_PLUS,	// +
	CHR_LT,		// <
	CHR_EQ,		// =
	CHR_GT,		// >
	CHR_LARWS,	// <<
	CHR_RARWS,	// >>
	CHR_POUN,	// POUND
	CHR_YEN,	// YEN
	CHR_EURO,	// EURO
	CHR_SECT,	// § SECTION SIGN
	CHR_DEGR,	// ° DEGREE SIGN
	CHR_DOT,	// · CENTER DOT
	CHR_UDEF,	// UNDEFINED CHARACTER

	CHR_0,
	CHR_1,
	CHR_2,
	CHR_3,
	CHR_4,
	CHR_5,
	CHR_6,
	CHR_7,
	CHR_8,
	CHR_9,
	CHR_A,
	CHR_AE,
	CHR_B,
	CHR_C,
	CHR_D,
	CHR_ETH,
	CHR_E,
	CHR_F,
	CHR_G,
	CHR_H,
	CHR_I,
	CHR_J,
	CHR_K,
	CHR_L,
	CHR_M,
	CHR_N,
	CHR_O,
	CHR_OE,
	CHR_P,
	CHR_Q,
	CHR_R,
	CHR_S,
	CHR_SS,
	CHR_T,
	CHR_THOR, // þ THORN
	CHR_U,
	CHR_V,
	CHR_W,
	CHR_X,
	CHR_Y,
	CHR_Z
	};

/////////////////////////////////////////////////////////////////////////////////////////
// This array defines the characters in code page ISO 8859 / Windows 1252. Only the
// printable characters that can appear in a person's surname are defined here.
/////////////////////////////////////////////////////////////////////////////////////////
const unsigned char sortOrderArr[256] = {

// 0 - 1F
CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,
CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,
CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,
CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,	CHR_NIL,

// 20 - 3F
CHR_SPC,	CHR_EXC,	CHR_QTS,	CHR_PND,	CHR_DOL,	CHR_PCT,	CHR_AMP,	CHR_QT, 
CHR_OPAR,	CHR_CPAR,	CHR_AST,	CHR_PLUS,	CHR_COMA,	CHR_HYPH,	CHR_PER,	CHR_FSL, 
CHR_0,		CHR_1,		CHR_2,		CHR_3,		CHR_4,		CHR_5,		CHR_6,		CHR_7,
CHR_8,		CHR_9,		CHR_COL,	CHR_SCOL,	CHR_LT,		CHR_EQ,		CHR_GT,		CHR_QST,

// 40 - 5F
CHR_ATS,	CHR_A,		CHR_B,		CHR_C,		CHR_D,		CHR_E,		CHR_F,		CHR_G,
CHR_H,		CHR_I,		CHR_J,		CHR_K,		CHR_L,		CHR_M,		CHR_N,		CHR_O,
CHR_P,		CHR_Q,		CHR_R,		CHR_S,		CHR_T,		CHR_U,		CHR_V,		CHR_W,
CHR_X,		CHR_Y,		CHR_Z,		CHR_OBRK,	CHR_BSL,	CHR_CBRK,	CHR_CRT,	CHR_UNDL,

// 60 - 7F
CHR_CQT,	CHR_A,		CHR_B,		CHR_C,		CHR_D,		CHR_E,		CHR_F,		CHR_G,
CHR_H,		CHR_I,		CHR_J,		CHR_K,		CHR_L,		CHR_M,		CHR_N,		CHR_O,
CHR_P,		CHR_Q,		CHR_R,		CHR_S,		CHR_T,		CHR_U,		CHR_V,		CHR_W,
CHR_X,		CHR_Y,		CHR_Z,		CHR_OBRC,	CHR_VLN,	CHR_CBRC,	CHR_TILD,	CHR_NIL,
 
// 80 - 9F
CHR_EURO,	CHR_UDEF,	CHR_UDEF,	CHR_F,		CHR_UDEF,	CHR_UDEF,	CHR_UDEF,	CHR_UDEF,
CHR_UDEF,	CHR_UDEF,	CHR_S,		CHR_UDEF,	CHR_OE,		CHR_UDEF,	CHR_UDEF,	CHR_UDEF,
CHR_UDEF,	CHR_UDEF,	CHR_RQT,	CHR_UDEF,	CHR_RDQT,	CHR_UDEF,	CHR_UDEF,	CHR_UDEF,
CHR_UDEF,	CHR_UDEF,	CHR_S,		CHR_O,		CHR_OE,		CHR_UDEF,	CHR_UDEF,	CHR_Y,

// A0 - BF
CHR_UDEF,	CHR_IEXC,	CHR_UDEF,	CHR_POUN,	CHR_UDEF,	CHR_YEN,	CHR_UDEF,	CHR_SECT, 
CHR_UDEF,	CHR_UDEF,	CHR_UDEF,	CHR_LARWS,	CHR_UDEF,	CHR_UDEF,	CHR_UDEF,	CHR_UDEF, 
CHR_DEGR,	CHR_UDEF,	CHR_UDEF,	CHR_UDEF,	CHR_UDEF,	CHR_UDEF,	CHR_UDEF,	CHR_DOT, 
CHR_UDEF,	CHR_UDEF,	CHR_UDEF,	CHR_RARWS,	CHR_UDEF,	CHR_UDEF,	CHR_UDEF,	CHR_IQST, 

// C0 - DF
CHR_A,		CHR_A,		CHR_A,		CHR_A,		CHR_A,		CHR_A,		CHR_AE,		CHR_C,
CHR_E,		CHR_E,		CHR_E,		CHR_E,		CHR_I,		CHR_I,		CHR_I,		CHR_I,
CHR_ETH,	CHR_N,		CHR_O,		CHR_O,		CHR_O,		CHR_O,		CHR_O,		CHR_UDEF,
CHR_O,		CHR_U,		CHR_U,		CHR_U,		CHR_U,		CHR_Y,		CHR_THOR,	CHR_SS,

// E0 - FF
CHR_A,		CHR_A,		CHR_A,		CHR_A,		CHR_A,		CHR_A,		CHR_AE,		CHR_C,
CHR_E,		CHR_E,		CHR_E,		CHR_E,		CHR_I,		CHR_I,		CHR_I,		CHR_I,
CHR_ETH,	CHR_N,		CHR_O,		CHR_O,		CHR_O,		CHR_O,		CHR_O,		CHR_UDEF,
CHR_O,		CHR_U,		CHR_U,		CHR_U,		CHR_U,		CHR_Y,		CHR_THOR,	CHR_Y
};

// Palm defines some characters differently from CP1252 / ISO8859-1. These include:
//	0xD5 - chrCapital_O_Tilde
//	0x83 - chrSmall_F_Hook 
//	0x8A - chrCapital_S_Caron
//	0x8C - chrCapital_OE
//	0x9A - chrSmall_S_Caron
//	0x9B - chrSmall_OE
//	0x9F - chrCapital_Y_Diaeresis


/////////////////////////////////////////////////////////////////////////////////////////
// SortRoutines Class Definition
/////////////////////////////////////////////////////////////////////////////////////////
class SortRoutines
{

public:

	SortRoutines (GedWiseFrame* pParent); // constructor
	~SortRoutines ();
	bool	SortOneFile (UINT fileNo, bool sortMultFiles);
	bool	SortMultFiles (void);

protected:

	bool	AddToBuffer (int position, int *totBufNums, const UINT fileNo);
	void	AllocateBufArr (int maxSz);
	void	AllocateSrtFlArr (int maxSz);
	void	DeallocateBufArr (UINT bufArrSz);
	void	DeallocateSrtFlArr (int srtFlArrSz);
	void	DeleteSortFiles (void);
	void	FileIOError (wxString& errMsg);
	bool	FindLowest (int *pos, char *lowStr, UINT totBufSz);
	char*	GetFirstKey (const char* dataLine);
	char*	GetIndiKey (const char* dataLine);
	char*	GetIndxKey (const char* dataLine); 
	char*	GetKey (char* dataLine, const UINT fileNo);
	bool	InitTempFiles (int startFileN);
	bool	MakeRuns (const UINT fileNo);
	bool	MergeSort (const UINT fileNo);
	bool	RewindF (const int pos, const UINT fileNo);
	bool	SortFile (UINT fileNo);
	void	SortList (int totBufSz);
	void	SortListIncr (const int totBufSz, int pos);
	int		StrCmp (const char *str1, const char *str2);
	bool	TermTmpFiles (void);

	#ifdef _DEBUG
	void	CheckSort (const UINT fileNo); // checks that files are sorted correctly
	#endif

	GedWiseFrame*	pGedWiseFrame;

	BufRecType**	m_aBufArr;			// buffer of text lines to be sorted
	SrtFlRecType**	m_aSrtFlArr;		// sort file array
	int				m_iBufArrSz;		// holds actual size of m_Buffer array
	int				m_iSrtFlArrSz;		// holds actual size of buffer array
	UINT			m_iLineTot;			// counter for total lines in infile
	USHORT			m_iTotInFiles;		// count of total sort files
	int				m_iSrtFileN;		// current sort file num beging processed
	UINT			m_iFileCnt;
	FILE*			m_fpInfile;			// the input file containing unsorted text lines
	wxString		m_sHoldFile;		// the name of the temporary Holder File
	char			m_szPadStrZ[RNUM_SZ+1];	// pads string with zeros in GetIndxKey
	char			m_szIndiSortKey[SRT_KEY_SZ+1];		// IndiDB sort key
	char			m_szIndxSortKey[IDX_KEY_SZ+1];		// IndxDB sort key
	char			m_szFirSortKey[XREF_SZ+DKEY_SZ+1];	// ChilDB sort key
	bool			m_bUseLocale;
};

#endif //SORT_ROUTINES_H_
