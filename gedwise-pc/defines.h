/////////////////////////////////////////////////////////////////////////////////////////
// Program:			GedWise 6.3
//
// File:			Defines.h
//
// Written by:		Daniel T. Rencricca
//
// Last Revision:	March 9, 2005
//
// Description:		Header file containing defines used by various modules.



//					10/19/2005 Allowed RESI tag in a Family Event Record
/////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DEFINES_H_
#define _DEFINES_H_


#if defined (__WXDEBUG__) && !defined(__WXMSW__)
#define _DEBUG
#endif

#if defined (__WXMAC__)
#define NO_REV_BYTES
#endif



#define DB_VERSION		 0x06	// MUST MATCH GEDWISE VIEWER APP
#define	CREATOR_ID		'Rend'	// MUST MATCH GEDWISE VIEWER APP
#define	CREATOR_ID_STR	"Rend"	// MUST MATCH GEDWISE VIEWER APP
#define	DB_TYPE			"gedw"	// MUST MATCH GEDWISE VIEWER APP
#define	MIN_GED_VERS	 5.5
#define	LOG_START_STR	"**** GedWise 6.3"
#define WEBSITE_URL		"http://www.batteryparksoftware.com"
#define PURCHASE_URL	"http://www.batteryparksoftware.com/purchase.htm"
//#define  PURCHASE_URL	"http://www.palmgear.com"

#define PREFFILE		"prefs.dat"	  // user preference file
#define LOGFILE			"logfile.txt" // log file
#define	ERR_LOG_FILE	"errlog.txt"  // error log file
#define	MANUAL_FILE_MSW	"gedwise.chm" // user manual
#define	MANUAL_FILE_UNI	"gedwise.htb" // user manual

#define	INSTALL_DIR		"gedwise"	  // directory where GedWise is installed
#define LNX_USER_DIR	"usr"		  // Linux user directory
#define LNX_SHARE_DIR	"share"		  // Linux share directory

#define	INDI_DB_FILE	"_tmp-In.dat"
#define INDX_DB_FILE	"_tmp-Ix.dat"
#define	EVEN_DB_FILE	"_tmp-Ev.dat"
#define	CHIL_DB_FILE	"_tmp-Ch.dat"
#define	CHSO_DB_FILE	"_tmp-Cs.dat"
#define	FAMI_DB_FILE	"_tmp-Fa.dat"
#define	SCIT_DB_FILE	"_tmp-SC.dat"
#define	SOUR_DB_FILE	"_tmp-So.dat"
#define	RCIT_DB_FILE	"_tmp-RC.dat"
#define	REPO_DB_FILE	"_tmp-Re.dat"
#define	NOTE_DB_FILE	"_tmp-No.dat"
#define	COMB_DB_FILE	"_tmp-Cb.dat"
#define	GREC_DB_FILE	"_tmp-Ge.dat"
#define PDB_DB_FILE		"-Rend.pdb"
#define	SRCH_DB_FILE	"*-Rend.pdb"

#define	MAX_LOGFILE_SZ	131072
#define	TOT_PREFS		14		// number of preferences to save/retrieve
#define DB_NAME_DFLT	"GenDB"	// default name for database
#define UD_TAG_CHAR		'_'		// character that starts user defined tags
#define	NOTE_SZ			35000	// max size of note as number
#define NOTE_SZ_STR		"35000"	// max size of note as text


// Windows uses a pair of CR and LF characters to terminate lines. UNIX (Including
// Linux and FreeBSD) uses an LF character only. The Macintosh, finally, uses a CR character only.

#define	CHR_TAB			'\t'	// tab character
#define	CHR_LF			'\n'	// newline character
#define	CHR_FF			'\f'	// formfeed character
#define	CHR_CR			'\r'	// carriage return character
#define	CHR_NUL			'\0'	// end of line character

#if defined (__WXMSW__)

	#define CHR_EOL			CHR_LF

#elif defined (__WXGTK__) || defined (__WXX11__) || defined (__WXMOTIF__)

	#define CHR_EOL			CHR_LF

#elif defined (__WXMAC__)

	#define CHR_EOL			CHR_CR

#endif


#define	CHR_DLM			'@'		// field delimiter
#define	CHR_EVN			'E'		// character to separate event numbers
#define	STR_ALI			"A"		// character for start of alia record number
#define CHR_FAM			'F'		// character to indicate xref is FamiDB record
#define CHR_IND			'I'		// character to indicate xref is IndiDB record
#define EOD_MARKER		"<<ENDOFDATABASE>>"

#define	BUFFER_SZ		4096	// should be plenty big enough for one line of GEDCOM file
#define	MAX_FLINE_SZ	36000	// max line size is Note (extra room for flags, note num, and size)
#define	DB_PROG_DIV		127		// passes to skip bef progress bar update (must be power of 2 less 1)

#define	NAME_SZ			90		// size of name field (excl. term.)
#define TIT_SZ			40		// size of title
#define	LSPN_SZ			9		// size of lifespan field (excl. term.)
#define	BIRY_SZ			4		// size of birth year (eg 1965)
#define	XREF_SZ			21		// size of cross reference identifier field (excl. term.)
#define	ALIA_SZ			2		// size of alias field (excl. term.)
#define AFLG_SZ			1		// size of alias flag field in IndxDB file
#define	MAX_FLDS		16		// maximum fields per record.
#define	ALIA_TAB		4		// tab number in IndidB where Alias field is located
#define	DKEY_SZ			8		// date key size for ChildDB record
#define RNUM_SZ			7		// size for largest possible record number (e.g 999999)
#define	DB_NAME_SZ		6		// max size of database name
#define DB_FNAME_SZ		13
#define	PC_FNAME_SZ		DB_NAME_SZ + 9 

// Colors for dialog boxes
#define	DLG_BACK_COLOR	224, 223, 227
#define	DIALOGTEXT		RGB (0,0,0)

// Macro Definitions
#define SwapShort(UnsShort) ((UnsShort)>>8)|((UnsShort)<<8)

// File Open and Write Errors
#define	cWriteErr							0x01
#define	cOpenErr							0x02
#define	cUnkErr								0x03
#define	cReadErr							0x04

#define ASSERT assert


#ifndef WIN32

#ifndef TRUE
	TRUE	1
#endif

#ifndef	FALSE
	FALSE 0
#endif

typedef unsigned int	ULONG32;
typedef int				LONG32;

typedef unsigned int	UINT32;
typedef int				INT32;

typedef unsigned int	UINT;
typedef int				INT;

typedef unsigned long	DWORD;
typedef unsigned short	WORD;

typedef unsigned long	ULONG;
typedef long			LONG;

typedef unsigned short	USHORT;
typedef short			SHORT;

typedef unsigned char	UCHAR;

#define MAX_PATH		260

typedef int				BOOL;

#endif


#ifdef WIN32

#pragma warning(disable: 4800)
#pragma warning(disable: 4310)
#pragma warning(disable: 4100)

#endif


#endif // _DEFINES_H_
