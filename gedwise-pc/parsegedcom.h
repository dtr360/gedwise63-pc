/////////////////////////////////////////////////////////////////////////////////////////
// Program:			GedWise 7.0
//
// File:			GedcomParser.h
//
// Written by:		Daniel T. Rencricca
//
// Last Revision:	February 25, 2005
//
// Description:		Header file for GedcomParser.cpp.
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef _PARSE_GEDCOM_H_
#define _PARSE_GEDCOM_H_

#include "defines.h"

#define ALIA_MAX	20   // maximum alias names per individual
#define DATE_SZ		35   // maximun size for date string
#define PLAC_SZ		120  // maximum size for place
#define ADDR_SZ		350  // maximum size for address
#define DESC_SZ		180  // maximum size for description
#define EVEN_MAX	200  // maximum number of events for event sort array
#define NREF_SZ		8    // room for 7 characters + seperator (@9999999) or 8 digit number
#define EREF_SZ		1600 // room for min EVEN_MAX Numeric Event XRefs at NREF_SZ
#define SREF_SZ		270  // room for min 38 Numeric Sour Cite XRefs at NREF_SZ
#define FREF_SZ		460  // room for min 20 FAMS XRefs at XREF_SZ characters + seperator
#define AREF_SZ		ALIA_MAX*NREF_SZ // room for 20 alias record numbers at 8 characters each

#define LEVELNO_SZ	4	  // (excl. terminator)
#define TAGTYPE_SZ	XREF_SZ + 2 //there may be XRef in this field (so add room for @'s)
#define TAGDESC_SZ	BUFFER_SZ - TAGTYPE_SZ - LEVELNO_SZ // (excl. terminator)

#define PAD_STR "                    " // 20 spaces used to pad child sort key

#define IDS_ERR_FWRITE	"\nFatal Error %s writing to file: %s    \nGEDCOM Processing Terminated.\n"
#define IDS_WRN_BADREC	"%-14s Record on line %6d ignored: %s%c"
//#define IDS_ERR_FOPEN	"Problem opening %s.    \n"

/////////////////////////////////////////////////////////////////////////////////////////
// Database File Related																					//
/////////////////////////////////////////////////////////////////////////////////////////
typedef enum {
	IndiDB,
	EvenDB,
	ChilDB,
	ChSoDB,
	FamiDB,
	SCitDB,
	SourDB,
	RCitDB,
	RepoDB,
	NoteDB,
	GRecDB,
	IndxDB,
	PdbDB,
	ErrLog,
	DBCount
	};

struct DBFileRec {
	FILE	*fp;			// file pointer to a pc database file
	char	name[DB_FNAME_SZ+1];	// filename of a pc database file
	};

/////////////////////////////////////////////////////////////////////////////////////////
// Primary Data Structures for Records																	//
/////////////////////////////////////////////////////////////////////////////////////////
// Note: Each packed mini-record also needs 2 bytes for the size + 2 bytes for the flags
//			+ 1 byte for the word alignment (if needed).
//			Each mega record will need 2 bytes for the sort number
//			The total for these = (40 x 5) + 2 = 202 bytes per mega record

struct IndiRecType {			// size: 2833 bytes, but 980 after xrefs substituted
	char	surNam[NAME_SZ+1];	// Surname of the individual [91]
	char	givNam[NAME_SZ+1];	// Given name of the individual [91]
	char	life[LSPN_SZ+1];	// Years of birth and death (####/####) [10]
	char	sex[2];				// Sex of individual (M or F). [2]
	char	alias[AREF_SZ+1];	// Alas flag (A=alias name) or alias rec list [161]
	char	indiNo[XREF_SZ+1];	// Reference identifier for individual [21]
	char	title[TIT_SZ+1];	// Title of the individual [41]	
	char	famSNo[FREF_SZ+1];	// Record numbers for families in FamiRec [461 / 181]
	char	famCNo[XREF_SZ+1];	// Record number for birth family in ChilRec [21 / 9]
	char	evenBNo[NREF_SZ+1];	// Record number for primary birth in EvenRec [9]
	char	evenNo[EREF_SZ+1];	// Record number for events in EvenRec [1601 / 20]
	char	evenDNo[NREF_SZ+1];	// Record number for primary death in EvenRec [9]
	char	noteNo[XREF_SZ+1];	// Cross reference identifier for a NoteRec [21]
	char	citeNo[SREF_SZ+1];	// Record numbers for citations in SCitRec [271]
	char  refn[21];				// User Reference Number. [21]
	char	chilFlg[2];			// 1 = individual has children in ChilRec database [2]
	};	
	// NOTE: evenNo will initially hold a string of event number seperated by "E"'s. It
	// will change in DBCreate to hold the first event number followed by as space 
	// followed by the total events for that person (e.g. "712 5" means the person
	// has 5 events starting at record number 712 and going through 716.)

#define	IndiAliaFld			4
#define	IndiIndiNoFld		5
#define	IndiFamSNoFld		7
#define	IndiFamCNoFld		8
#define	IndiEvenBNoFld		9
#define	IndiEvenNoFld		10
#define	IndiEvenDNoFld		11
#define	IndiChilFld			15
#define	IndiTotRecFlds		16

struct EvenRecType {			// size: 1511 bytes
	char	type[5];			// Holds the event type tag- must be a 4 character identifier.
	char	desc[DESC_SZ+1];	// Event/Attribute descriptor (extra room in case TYPE tag used for IDNO or other Attribute).
	char	date[DATE_SZ+1];	// Date or date range for event.
	char	plac[PLAC_SZ+1];	// Place of event.
	char	addr[ADDR_SZ+1];	// Address of event.
	char	age[13];			// Age at event.
	char	agnc[121];			// Agency responsible for event.
	char	caus[91];			// Cause of event.
	char	hAge[13];			// Age of wife at event; For Family Events
	char	wAge[13];			// Age of husband at event; For Family Events
	char	reli[25];			// Religion of event.
	char	statL[11];			// LDS baptism status
	char	tempL[5];			// LDS temple code
	char	noteNo[XREF_SZ+1];	// Cross reference identifier for a NoteRec
	char	citeNo[SREF_SZ+1];	// Cross reference identifiers for a SCitRec.
	char	OrgNo[XREF_SZ+2];	// Cross reference to record number in IndiRec or FamiRec for event. Add 1 for initial "I" or "F"
	};	

#define	EvenOrgNoFld	15	// zero-based field number
#define	EvenTotRecFlds	16

struct FamiRecType {			// size: 1998 / 392 bytes
	char	famiNo[XREF_SZ+1];	// Reference identifier for a FamiRec [21 / 0]
	char	husbNo[XREF_SZ+1];	// Cross reference identifier for a husband (indiNo) [21 / 21]
	char	wifeNo[XREF_SZ+1];	// Cross reference identifier for a wife (indiNo) [21 / 21]
	char	nchi[4];			// Number of children [4 / 4]
	char	evenMNo[XREF_SZ+1];	// Cross reference id for Primary Marriage Event_Rec [21 / 21]
	char	evenNo[EREF_SZ+1];	// Cross reference identifiers for a EvenRec [701 / 16]
	char	noteNo[XREF_SZ+1];	// Cross reference identifier for a NoteRec [21 / 21]
	char	citeNo[SREF_SZ+1];	// Cross reference identifiers for a SCitRec [270 / 270]
	char	chilNo[NREF_SZ+1];	// record number of first child in FamiCDB [9 / 9]
	char	chiCnt[NREF_SZ+1];	// Count of actual number of children in db [9 / 9]
	};
	// NOTE: evenNo will initially hold a string of event number seperated by "E"'s. It
	// will change in DBCreate to hold the first event number followed by a space 
	// followed by the total events for that person (e.g. "712 5" means the person
	// has 5 events starting at at record number 712 and going through 716.)

#define	FamiEvenMNo			4
#define	FamiEvenNo			5
#define	FamiChilNo			8
#define	FamiChilCnt			9
#define	FamiTotRecFlds		10

struct ChilRecType {			// size: 323 bytes
	char	famiNo[XREF_SZ+1];	// Cross Reference identifier for a FamiRec.
	char	indiNo[XREF_SZ+1];	// Cross Reference identifier for an IndiRec.
	char	pedi[8];			// Pedigree: {B}irth, {A}dopted, {F}oster, {S}ealing.
	char	noteNo[XREF_SZ+1];	// Cross reference identifier for a NoteRec.
	char	citeNo[249];		// Cross reference identifiers for a SCitRec.
	};	

#define	ChilPedi			2
#define	ChilNoteNo			3
#define	ChilCiteNo			4
#define	ChilTotRecFlds		5

struct CiteRecType {			// size: 1638 bytes, 1624 after xref substituted
	char	sourNo[XREF_SZ+1];	// Cross reference identifier for a SourRec [21 / 8]
	char	noteNo[XREF_SZ+1];	// Cross reference identifier for a NoteRec. [22]
	char	page[269];			// Where within source.
	char	even[16];			// Event type cited from: an event attribute type.
	char	role[16];			// Roll in event: {CHIL|HUSB|WIFE|MOTH|FATH|SPOU|<role descriptor>}
	char	quay[2];			// Certainty assessment of this source {0|1|2|3}
	char	date[91];			// Entry recording date.
	char	text[1200];			// Text from source. size was 249
	};							
											
struct SourRecType {			// size: [1661 / 1638]
	char	sourNo[XREF_SZ+1];	// Reference identifier for a SourRec [22/ 0]
	char	rcitNo[NREF_SZ+1];	// Cross reference identifier for a RCitRec. [9]
	char	noteNo[XREF_SZ+1];	// Cross reference identifier for a NoteRec. [22/22]
	char	titl[249];			// Source Title.
	char	even[91];			// Events Recorded.
	char	date[DATE_SZ+1];	// Date Period of Events Recorded. [36]
	char	plac[PLAC_SZ+1];	// Source Jurisdiction Place. [121]
	char	agnc[121];			// Responsible Agency.
	char	auth[249];			// Source Originator (author).
	char	text[470];			// Text from Source.
	char	publ[249];			// Source Publication Facts.
	char  numb[21];				// Microfilm roll number.
	};
								
struct RCitRecType {			// size: 216 bytes
	char	repoNo[XREF_SZ+1];	// Cross reference identifier for a RepoRec [21 / 8]
	char	noteNo[XREF_SZ+1];	// Cross reference identifier for a NoteRec.
	char	caln[121];			// Call Number.
	char	medi[51];			// Source Media Type {audio|book|card|electronic|fische|film|magazine} (16 is standard)
	};	
							
struct RepoRecType {			// size: 486 bytes
	char	repoNo[XREF_SZ+1];	// Reference identifier for a RepoRec. [21 / 0]
	char	noteNo[XREF_SZ+1];	// Cross reference identifier for a NoteRec.
	char	name[91];			// Name of repository.
	char	addr[ADDR_SZ+1];	// Address of repository.
	};	

struct NoteRecType {			// size: 35022 bytes
	char	noteNo[XREF_SZ+1];	// Reference identifier for a Note_Rec.
	char	note[NOTE_SZ+1];	// Note text.
	};

struct GedcRecType {
	char	date[DATE_SZ+1];	// Date the GedCom file was created.
	char	ver[10];			// Holds the GedCom version number.
	char	fileName[MAX_PATH];	// Date or date range for event.
	char	dateLoaded[DATE_SZ+1]; // Date GedCom was loaded into GedWise
	char	peopleCnt[7];
	};	

struct EvenSrtRec {
	UINT	evenNo;				// Reference id for each Event_Rec (9999999 + seperator).
	long	dateVal;			// Holds the numerical value of the date. 
	};

/////////////////////////////////////////////////////////////////////////////////////////
// GEDREC CLASS DEFINITION																					//
/////////////////////////////////////////////////////////////////////////////////////////
class	ParseGedcom //: public wxApp
{
public:
	// Construction
	ParseGedcom (GedWiseFrame* pParent = NULL);	// standard constructor
	~ParseGedcom ();  // destructor

	GedcRecType	GedFileRec;
	char			*buffer;  // holds one line of data read from GEDCOM file

	bool	GetGedComData (GedcRecType *gedRec, bool writeData);
	bool	StartParsing (void);

protected:
	void	AddSpaceAfterComma (char *str_holder, const int maxSize);
	void	BadDataErrorHandler (char *typeStr);
	void	CleanExit (void);
	char*	CleanStr (char *aString);
	char*	CleanDateStr (char *aString);
	void	ConsAddrField (char *str_holder, const int maxSize);
	void	ConsPlacField (char *str_holder, const int maxSize);
	void	ConvAnselToCP1252 (char* line);
	long	ConvertDateToNumber (char *date, char *year);
	int		ConvEvenTag (const char* evenTag);
	bool	ConvertMonthToNumber (char *monthStr, int pos, char* monthVal);
	void	FileErrorHandler(const short whichFile);
	void	GetChilRec (const UINT sortNo, const char* famiNo);
	void	GetChilFamRec (const char *indiNo, const long ChilBDate, bool* areBirthParents);
	void	GetCiteRec (char *scit_ref);
	void	GetEventRec (const bool indAttr, EvenSrtRec *evenSortRec, const char *recNo, const bool primRec = false);
	void	GetFamiRec (void);
	void	GetFieldDesc (char *field, int maxSize, bool clrFld=false);
	
	void	GetIndiRec (void);
	bool	GetNoteRec (char *noteNo, char *noteText, bool finalize);
	bool	GetNoteRecX (void);
	void	GetRCitRec (char *rcitNo);
	void	GetRepoRec (void);
	void	GetSourRec (void);
	bool	IsXRef (const char *noteXRef);
	void	ParseName (IndiRecType &ind_rec);
	void	RecLoop (void);
	bool	ReadNextRec (void);
	void	ResetGedFile (void);
	void	SkipToStartLevel (bool addToLog);
	void	SortEventArray (EvenSrtRec eventSort[], char *evenNo);
	void	StripTabs (char* line);

	char	m_szLevelNo[LEVELNO_SZ+1];
	char	m_szTagType[TAGTYPE_SZ+1];
	char	m_szTagDesc[TAGDESC_SZ+1];
	bool	m_bHandled;		// set to false if have not handled current LineRec.
	UINT	m_iERecCnt;		// counter for number of events for one individual.
	bool	m_bFileEnd;		// set to true when end of GEDCOM file is reached.
	bool	m_bStartUpdateStats;  // signals program to update status messages/progress bar
	char	m_szAdoptiveFamiNo[XREF_SZ+1]; // holds individuals adoptive family number.
	bool	m_bCharAnsel;			// set to true if ANSEL GEDCOM

	// The following are used in event record and individual record subroutines.
	char	m_szDthYrHld[5]; // holds the year of death.
	char	m_szBirYrHld[5]; // holds the year of birth.

	GedWiseFrame*	pGedWiseFrame;

friend class CGedWisePCDlg;
};

#endif // _PARSE_GEDCOM_H_

/*
NA#define	chrReverseSolidus						0x005C
NA#define chrEuroSign								0x0080
??#define	chrSingleLow9QuotationMark				0x0082
NA#define	chrSmall_F_Hook							0x0083
??#define	chrDoubleLow9QuotationMark				0x0084
NA#define	chrHorizontalEllipsis					0x0085
NA#define	chrDagger								0x0086
NA#define	chrDoubleDagger							0x0087
??#define	chrModifierCircumflexAccent				0x0088
NA#define	chrPerMilleSign							0x0089
==#define	chrCapital_S_Caron						0x008A
NA#define	chrSingleLeftPointingAngleQuotationMark	0x008B
==#define	chrCapital_OE							0x008C
==#define	chrCapital_Z_Caron						0x008E
??#define	chrLeftSingleQuotationMark				0x0091
==#define	chrRightSingleQuotationMark				0x0092
??#define	chrLeftDoubleQuotationMark				0x0093
==#define	chrRightDoubleQuotationMark				0x0094
NA#define	chrBullet								0x0095
NA#define	chrEnDash								0x0096
NA#define	chrEmDash								0x0097
??#define	chrSmallTilde							0x0098
NA#define	chrTradeMarkSign						0x0099
==#define	chrSmall_S_Caron						0x009A
NA#define	chrSingleRightPointingAngleQuotationMark 0x009B
==#define	chrSmall_OE								0x009C
==#define	chrSmall_Z_Caron						0x009E
==#define	chrCapital_Y_Diaeresis					0x009F
??#define	chrNoBreakSpace							0x00A0
==#define	chrInvertedExclamationMark				0x00A1
NA#define	chrCentSign								0x00A2
==#define	chrPoundSign							0x00A3
NA#define	chrCurrencySign							0x00A4
NA#define	chrYenSign								0x00A5
NA#define	chrBrokenBar							0x00A6
NA#define	chrSectionSign							0x00A7
??#define	chrDiaeresis							0x00A8
==#define	chrCopyrightSign						0x00A9
NA#define	chrFeminineOrdinalIndicator				0x00AA
NA#define	chrLeftPointingDoubleAngleQuotationMark	0x00AB
NA#define	chrNotSign								0x00AC
??#define	chrSoftHyphen							0x00AD
==#define	chrRegisteredSign						0x00AE
??#define	chrMacron								0x00AF
==#define	chrDegreeSign							0x00B0
==#define	chrPlusMinusSign						0x00B1
NA#define	chrSuperscriptTwo						0x00B2
NA#define	chrSuperscriptThree						0x00B3
??#define	chrAcuteAccent							0x00B4
NA#define	chrMicroSign							0x00B5
NA#define	chrPilcrowSign							0x00B6
==#define	chrMiddleDot							0x00B7
??#define	chrCedilla								0x00B8
NA#define	chrSuperscriptOne						0x00B9
NA#define	chrMasculineOrdinalIndicator			0x00BA
NA#define	chrRightPointingDoubleAngleQuotationMark 0x00BB
NA#define	chrVulgarFractionOneQuarter				0x00BC
NA#define	chrVulgarFractionOneHalf				0x00BD
NA#define	chrVulgarFractionThreeQuarters			0x00BE
==#define	chrInvertedQuestionMark					0x00BF
==#define	chrCapital_A_Grave						0x00C0
==#define	chrCapital_A_Acute						0x00C1
==#define	chrCapital_A_Circumflex					0x00C2
==#define	chrCapital_A_Tilde						0x00C3
==#define	chrCapital_A_Diaeresis					0x00C4
==#define	chrCapital_A_RingAbove					0x00C5
==#define	chrCapital_AE							0x00C6
==#define	chrCapital_C_Cedilla					0x00C7
==#define	chrCapital_E_Grave						0x00C8
==#define	chrCapital_E_Acute						0x00C9
==#define	chrCapital_E_Circumflex					0x00CA
==#define	chrCapital_E_Diaeresis					0x00CB
==#define	chrCapital_I_Grave						0x00CC
==#define	chrCapital_I_Acute						0x00CD
==#define	chrCapital_I_Circumflex					0x00CE
==#define	chrCapital_I_Diaeresis					0x00CF
==#define	chrCapital_Eth							0x00D0
==#define	chrCapital_N_Tilde						0x00D1
==#define	chrCapital_O_Grave						0x00D2
==#define	chrCapital_O_Acute						0x00D3
==#define	chrCapital_O_Circumflex					0x00D4
==#define	chrCapital_O_Tilde						0x00D5
==#define	chrCapital_O_Diaeresis					0x00D6
NA#define	chrMultiplicationSign					0x00D7
==#define	chrCapital_O_Stroke						0x00D8
==#define	chrCapital_U_Grave						0x00D9
==#define	chrCapital_U_Acute						0x00DA
==#define	chrCapital_U_Circumflex					0x00DB
==#define	chrCapital_U_Diaeresis					0x00DC
==#define	chrCapital_Y_Acute						0x00DD
==#define	chrCapital_Thorn						0x00DE
NA#define	chrSmall_SharpS							0x00DF
==#define	chrSmall_A_Grave						0x00E0
==#define	chrSmall_A_Acute						0x00E1
==#define	chrSmall_A_Circumflex					0x00E2
==#define	chrSmall_A_Tilde						0x00E3
==#define	chrSmall_A_Diaeresis					0x00E4
==#define	chrSmall_A_RingAbove					0x00E5
==#define	chrSmall_AE								0x00E6
==#define	chrSmall_C_Cedilla						0x00E7
==#define	chrSmall_E_Grave						0x00E8
==#define	chrSmall_E_Acute						0x00E9
==#define	chrSmall_E_Circumflex					0x00EA
==#define	chrSmall_E_Diaeresis					0x00EB
==#define	chrSmall_I_Grave						0x00EC
==#define	chrSmall_I_Acute						0x00ED
==#define	chrSmall_I_Circumflex					0x00EE
==#define	chrSmall_I_Diaeresis					0x00EF
==#define	chrSmall_Eth							0x00F0
==#define	chrSmall_N_Tilde						0x00F1
==#define	chrSmall_O_Grave						0x00F2
==#define	chrSmall_O_Acute						0x00F3
==#define	chrSmall_O_Circumflex					0x00F4
==#define	chrSmall_O_Tilde						0x00F5
==#define	chrSmall_O_Diaeresis					0x00F6
NA#define	chrDivisionSign							0x00F7
==#define	chrSmall_O_Stroke						0x00F8
==#define	chrSmall_U_Grave						0x00F9
==#define	chrSmall_U_Acute						0x00FA
==#define	chrSmall_U_Circumflex					0x00FB
==#define	chrSmall_U_Diaeresis					0x00FC
==#define	chrSmall_Y_Acute						0x00FD
==#define	chrSmall_Thorn							0x00FE
==#define	chrSmall_Y_Diaeresis					0x00FF
*/

