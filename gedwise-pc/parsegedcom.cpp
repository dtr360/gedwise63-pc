/////////////////////////////////////////////////////////////////////////////////////////
// Program:			GedWise
//					Copyright © 2001 - 2005 Battery Park Software Corporation
//
// Module:			GedcomParser.cpp
//
// Written by:		Daniel T. Rencricca
//
// Date:			February 25, 2004
//
// Description:		This module contains the main GEDCOM file processing routines.
//					These routines parse the GEDCOM data file selected by the user
//					and create the GedWise database files during this parsing process.
//					The module also contains functions to validate the	GEDCOM file
//					selected.
/////////////////////////////////////////////////////////////////////////////////////////
#include "stdwx.h"
#include "gedwise.h"
#include "miscfunctions.h"
#include "parsegedcom.h"
#include "charlatin.h"

#ifdef _DEBUG // must include immediately after last #include statement in every cpp file
#define new DEBUG_NEW
#endif 

DBFileRec	DBFiles[][DBCount] = {  // these must be in order of enum's
				{NULL, INDI_DB_FILE},
				{NULL, EVEN_DB_FILE},
				{NULL, CHIL_DB_FILE},
				{NULL, CHSO_DB_FILE},
				{NULL, FAMI_DB_FILE},
				{NULL, SCIT_DB_FILE},
				{NULL, SOUR_DB_FILE},
				{NULL, RCIT_DB_FILE},
				{NULL, REPO_DB_FILE},
				{NULL, NOTE_DB_FILE},
				{NULL, GREC_DB_FILE},
				{NULL, INDX_DB_FILE},
				{NULL, COMB_DB_FILE},
				{NULL, ERR_LOG_FILE}
				};

FILE	*GedFileP  = NULL;

UINT	TOTLINES;  // Global variable containing total lines in GEDCOM file.
UINT	IndiCnt;   // Global counter for number of individuals in gedcom file.
UINT	NameCnt;   // Global counter for number of names.
UINT	FamiCnt;   // Global counter for number of families in gedcom file.
UINT	ChilCnt;   // Global counter for number of children in ChilDB.
UINT	SourCnt;   // Global counter for number of sources in gedcom file.
UINT	RepoCnt;   // Global counter for number of repositories in gedcom file.
UINT	LineCnt;   // Global counter for number of lines read in gedcom file.
UINT	ErrCnt;    // Global counter for number of errors.
UINT	EvenNoCnt; // Global counter for the evenNo field in an EvenRec.
UINT	RCitNoCnt; // Global counter for number of sour-repo citations in gedcom file.
UINT	SCitNoCnt; // Global counter for number of citations in gedcom file.
UINT	NoteNoCnt; // Global counter for number of notes in gedcom file.
UINT	NoteCnt;   // Global counter for number of notes in gedcom file.
UINT	FamiNoCnt; // Global counter for the famiNo field in an FamiRec.

#define	_THIS_FILE	"ParseGedcom"

// THE FOLLOWING EVENTS ARRAY MUST MATCH ARRAY IN GEDWISE VIEWER APP
#define EVEN_ARRAY_SZ	59
#define EVEN_BIR_TYP	1	// must match position of BIRT in Events Array
#define EVEN_DTH_TYP	4	// must match position of DEAT in Events Array
#define EVEN_PRO_TYP	7	// must match position of PROB in Events Array
#define EVEN_ADO_TYP	31 // must match position of ADOP in Events Array
#define EVEN_BIR_STR	"1,2,3"	  // must match positions in Events Array
#define EVEN_DTH_STR	"4,5,6,7"  // must match positions in Events Array

char*	Events[EVEN_ARRAY_SZ] =  
	{	"UNK",  "BIRT", "BAPM", "CHR",  "DEAT", "BURI", "CREM", "PROB", "MARR", "MARB",
  		"MARC", "MARL", "MARS", "ANUL", "CENS", "DIV",  "DIVF", "ENGA", "CAST", "DSCR",
  		"EDUC", "IDNO", "NATI", "NCHI", "NMR",  "OCCU", "PROP", "RELI", "RESI", "SSN",
  		"TITL", "ADOP", "BARM", "BASM", "BLES", "CHRA", "CONF", "FCOM", "ORDN", "NATU",
  		"EMIG", "IMMI", "WILL", "GRAD", "RETI", "EVEN", "NOTE", "BAPL", "CONL", "ENDL",
  		"SLGC", "SLGS", "ASSO",	"HIST", "MILI", "MDCL", "ELEC", "DETS", "DEG"};

const char cUnknownStr[] = "Unknown";

/////////////////////////////////////////////////////////////////////////////////////////
// GedRec Function Definitions																			//
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ParseGedcom
//
// Description:	ParseGedcom constructor
//						
// Parameters:	pParent -	pointer to the parent dialog
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
ParseGedcom::ParseGedcom (GedWiseFrame* pParent)
{
	// initialize variables
	IndiCnt = FamiCnt = ChilCnt = SourCnt = RepoCnt = LineCnt = NameCnt = 0;
	ErrCnt  = EvenNoCnt = FamiNoCnt = NoteNoCnt = NoteCnt = SCitNoCnt = RCitNoCnt = 0;

	m_bFileEnd			= false;
	m_bStartUpdateStats	= false;
	m_bCharAnsel		= false;

	pGedWiseFrame = pParent;

	buffer = new char[BUFFER_SZ];
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ParseGedcom
//
// Description:	Destructor for ParseGedcom
//						
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
ParseGedcom::~ParseGedcom()
{
	if (buffer) {
		delete buffer;
		buffer = NULL;
		}
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	CleanExit
//
// Description:	Called if problem writing to file.  It closes all open files and erases
//						all files created by this class.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::CleanExit (void)
{
	wxString	filePathName;

	for (int i = IndiDB; i <= GRecDB; i++) {  
		if (DBFiles[i]->fp) {
			filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[i]->name;
			fclose (DBFiles[i]->fp);
			DBFiles[i]->fp = NULL;
			remove (filePathName.c_str());
			}
		}

	if (GedFileP) {
		fclose (GedFileP);
		GedFileP = NULL;
		}
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	FileErrorHandler
//
// Description:	Displays on screen an error in writing to a file.						
//
// Parameters:	errorType =	number identifying type of error.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::FileErrorHandler (const short whichFile)
{
	wxString	msg;

	pGedWiseFrame->SetBadExit ();
	
	msg.Printf (_(IDS_ERR_FWRITE),	DBFiles[whichFile]->name, _("PG01"));
	wxLogSysError (msg);
	AddLogEntry (msg);
	CleanExit ();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	BadDataErrorHandler
//
// Description:	Saves GEDCOM file data errors to a file.
//
// Parameters:	-> typeStr  -	string holding descriptor of type of tag in which reading
//								error occurred, eg Event, Source, Family...
//
// Returns:		nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::BadDataErrorHandler (char *typeStr)
{
	if (fprintf (DBFiles[ErrLog]->fp, IDS_WRN_BADREC, typeStr, LineCnt, buffer, CHR_EOL) < 0)
		FileErrorHandler (ErrLog);
	ErrCnt++;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ConvEvenTag
//
// Description:	Look up an event type tag (e.g. BIRT, DEAT, MARR, etc.) in the Events
//				array and return the array position of where event tag was found. This
//				same number will be used in GedWise Viewer application to display a 
//				description of the event.
//
// Parameters:	-> evenTag -	The event tag to look up.
//
// Returns:		The event number if successfull or else 0 if event not found.
/////////////////////////////////////////////////////////////////////////////////////////
int ParseGedcom::ConvEvenTag (const char* evenTag)
{
	int	index = 0;

	for (index = 0; index < EVEN_ARRAY_SZ; index++) {
		if (!strcmp (Events[index], evenTag))
			return index;
		}

	wxASSERT (false);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		StripTabs
//
//	Description:	Relaces TAB characters with spaces in the given line.
//
// Parameters:		<> line	-	line to search.
//
// Returns:			Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::StripTabs (char* line)
{
	UINT	i = 0;

	while (i < strlen (line)) {

		if (line[i] == (char) 0x0009) {
			line[i] = ' ';
			}
		i++;
		} // while loop
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ConvAnselToCP152
//
// Description:	Converts all ANSEL characters in a string to Code Page 1252
//				characters (for Windows) or the ISO 8859-1 character set (for Linux).
//				NOTE that ISO 8859-1 does not define characters 127 through 159, so
//				any characters in that range must be converted to ASCII for Linux
//				systems.
//				
//
// Parameters:	<> line	-	line to search.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::ConvAnselToCP1252 (char* line)
{
	//#define	chrCapital_Z_Caron	0x008E  versions after 4.0
	//#define	chrSmall_Z_Caron		0x009E  versions after 4.0
	
	char		calcChr;
	UINT		i = 0;
	UINT		x = 0;

	while (i < strlen (line)) {

		switch (line[i])
			{
			case (char) 0x00A1: // SLASH CAPITAL L
				calcChr = (char) 'L'; // best we can do
				break;

			case (char) 0x00A2: // CAPITAL O STROKE
				calcChr = (char) chrCapital_O_Stroke;
				break;
		
			case (char) 0x00A3: // CAPITAL ETH
				calcChr = (char) chrCapital_Eth;
				break;

			case (char) 0x00A4: // CAPITAL THORN
				calcChr = (char) chrCapital_Thorn;
				break;

			case (char) 0x00A5: // CAPITAL AE
				calcChr = (char) chrCapital_AE;
				break;

			case (char) 0x00A6: // CAPITAL OE
				calcChr = (char) chrCapital_OE;
				break;

			case (char) 0x00A7: // RIGHT SINGLEQUOTE MARK
				calcChr = (char) chrRightSingleQuotationMark; // best we can do
				break;

			case (char) 0x00A8: // MIDDLE DOT
				calcChr = (char) chrMiddleDot;
				break;

			case (char) 0x00A9: // MUSIC SHARP SIGN
				calcChr = (char) 'b';  // best we can do
				break;

			case (char) 0x00AA: // REGISTERED SIGN
				calcChr = (char) chrRegisteredSign;
				break;

			case (char) 0x00AB: // PLUS OR MINUS SIGN
				calcChr = (char) chrPlusMinusSign;
				break;

			case (char) 0x00AC: // HOOK CAPITAL O
				calcChr = (char) 'O';
				break;

			case (char) 0x00AD: // HOOK CAPITAL U
				calcChr = (char) 'U';
				break;

			case (char) 0x00AE: // ALIF
				calcChr = (char) 0x0027; // quote is best we can do
				break;

			case (char) 0x00B0: // AYN
				calcChr = (char) 0x0027; // quote is best we can do
				break;

			case (char) 0x00B1: // SLASH SMALL L
				calcChr = (char) 'l'; // best we can do
				break;

			case (char) 0x00B2: // SMALL O STROKE
				calcChr = (char) chrSmall_O_Stroke;
				break;
		
			case (char) 0x00B3: // SMALL D STROKE
				calcChr = (char) 'd'; // best we can do
				break;

			case (char) 0x00B4: // SMALL THORN
				calcChr = (char) chrSmall_Thorn;
				break;
			
			case (char) 0x00B5: // SMALL AE
				calcChr = (char) chrSmall_AE;
				break;

			case (char) 0x00B6: // SMALL OE
				calcChr = (char) chrSmall_OE;
				break;

			case (char) 0x00B7: // TVERDYI ZNAK (hard sign)
				calcChr = (char) chrRightDoubleQuotationMark; // best we can do
				break;

			case (char) 0x00B8: // DOTLESS SMALL I
				calcChr = (char) 'l'; // best we can do
				break;

			case (char) 0x00B9: // BRIT POUND SIGN
				calcChr = (char) chrPoundSign;
				break;

			case (char) 0x00BA: // SMALL ETH
				calcChr = (char) chrSmall_Eth;
				break;

			case (char) 0x00BC: // HOOK SMALL O
				calcChr = (char) 'o'; // best we can do
				break;

			case (char) 0x00BD: // HOOK SMALL U
				calcChr = (char) 'u'; // best we can do
				break;

			case (char) 0x00C0: // DEGREE SIGN
				calcChr = (char) chrDegreeSign;
				break;

			case (char) 0x00C1: // SCRIPT SMALL L
				calcChr = (char) 'l'; // best we can do
				break;

			case (char) 0x00C2: // 
				calcChr = (char) 'p'; // best we can do
				break;

			case (char) 0x00C3: // COPYRIGHT SIGN
				calcChr = (char) chrCopyrightSign;
				break;

			case (char) 0x00C4: // POUND SIGN
				calcChr = (char) 0x0023; //# is best we can do
				break;

			case (char) 0x00C5: // INVERTED QUEST MARK
				calcChr = (char) chrInvertedQuestionMark;
				break;

			case (char) 0x00C6: // INVERTED EXCL MARK
				calcChr = (char) chrInvertedExclamationMark;
				break;

			case (char) 0x00CF: // ES ZET  - can't find this in ANSEL chart
				calcChr = (char) chrSmall_SharpS;
				break;

			case (char) 0x00E1: // GRAVE : 10
				i++;
				switch (line[i])
				{
				case 'A':
					calcChr = (char) chrCapital_A_Grave;
					break;
				case 'E':
					calcChr = (char) chrCapital_E_Grave;
					break;
				case 'I':
					calcChr = (char) chrCapital_I_Grave;
					break;
				case 'O':
					calcChr = (char) chrCapital_O_Grave;
					break;
				case 'U':
					calcChr = (char) chrCapital_U_Grave;
					break;
				case 'a':
					calcChr = (char) chrSmall_A_Grave;
					break;
				case 'e':
					calcChr = (char) chrSmall_E_Grave;
					break;
				case 'i':
					calcChr = (char) chrSmall_I_Grave;
					break;
				case 'o':
					calcChr = (char) chrSmall_O_Grave;
					break;
				case 'u':
					calcChr = (char) chrSmall_U_Grave;
					break;
				default:
					calcChr = line[i];	
					break;
				} 
				break; 

			case (char) 0x00E2: // ACCUTE : 12
				i++;
				switch (line[i])
				{
				case 'A':
					calcChr = (char) chrCapital_A_Acute;
					break;
				case 'E':
					calcChr = (char) chrCapital_E_Acute;
					break;
				case 'I':
					calcChr = (char) chrCapital_I_Acute;
					break;
				case 'O':
					calcChr = (char) chrCapital_O_Acute;
					break;
				case 'U':
					calcChr = (char) chrCapital_U_Acute;
					break;
				case 'Y':
					calcChr = (char) chrCapital_Y_Acute;
					break;
				case 'a':
					calcChr = (char) chrSmall_A_Acute;
					break;
				case 'e':
					calcChr = (char) chrSmall_E_Acute;
					break;
				case 'i':
					calcChr = (char) chrSmall_I_Acute;
					break;
				case 'o':
					calcChr = (char) chrSmall_O_Acute;
					break;
				case 'u':
					calcChr = (char) chrSmall_U_Acute;
					break;
				case 'y':
					calcChr = (char) chrSmall_Y_Acute;
					break;
				default:
					calcChr = line[i];	
					break;
				}
				break;

			case (char) 0x00E3: // CIRCUMFLEX : 10
				i++;
				switch (line[i])
				{
				case 'A':
					calcChr = (char) chrCapital_A_Circumflex;
					break;
				case 'E':
					calcChr = (char) chrCapital_E_Circumflex;
					break;
				case 'I':
					calcChr = (char) chrCapital_I_Circumflex;
					break;
				case 'O':
					calcChr = (char) chrCapital_O_Circumflex;
					break;
				case 'U':
					calcChr = (char) chrCapital_U_Circumflex;
					break;
				case 'a':
					calcChr = (char) chrSmall_A_Circumflex;
					break;
				case 'e':
					calcChr = (char) chrSmall_E_Circumflex;
					break;
				case 'i':
					calcChr = (char) chrSmall_I_Circumflex;
					break;
				case 'o':
					calcChr = (char) chrSmall_O_Circumflex;
					break;
				case 'u':
					calcChr = (char) chrSmall_U_Circumflex;
					break;
				default:
					calcChr = line[i];
					break;
				}
				break;
			
			case (char) 0x00E4: // TILDA : 6
				i++;
				switch (line[i])
				{
				case 'A':
					calcChr = (char) chrCapital_A_Tilde;
					break;
				case 'N':
					calcChr = (char) chrCapital_N_Tilde;
					break;
				case 'O':
					calcChr = (char) chrCapital_O_Tilde;
					break;
				case 'a':
					calcChr = (char) chrSmall_A_Tilde;
					break;
				case 'n':
					calcChr = (char) chrSmall_N_Tilde;
					break;
				case 'o':
					calcChr = (char) chrSmall_O_Tilde;
					break;
				default:
					calcChr = line[i];	
					break;
				}
				break;

			case (char) 0x00E8: // DIAERESIS : 12
				i++;
				switch (line[i])
				{
				case 'A':
					calcChr = (char) chrCapital_A_Diaeresis;
					break;
				case 'E':
					calcChr = (char) chrCapital_E_Diaeresis;
					break;
				case 'I':
					calcChr = (char) chrCapital_I_Diaeresis;
					break;
				case 'O':
					calcChr = (char) chrCapital_O_Diaeresis;
					break;
				case 'U':
					calcChr = (char) chrCapital_U_Diaeresis;
					break;
				case 'Y':
					calcChr = (char) chrCapital_Y_Diaeresis;
					break;
				case 'a':
					calcChr = (char) chrSmall_A_Diaeresis;
					break;
				case 'e':
					calcChr = (char) chrSmall_E_Diaeresis;
					break;
				case 'i':
					calcChr = (char) chrSmall_I_Diaeresis;
					break;
				case 'o':
					calcChr = (char) chrSmall_O_Diaeresis;
					break;
				case 'u':
					calcChr = (char) chrSmall_U_Diaeresis;
					break;
				case 'y':
					calcChr = (char) chrSmall_Y_Diaeresis;
					break;
				default:
					calcChr = line[i];	
					break;
				}
				break;

			case (char) 0x00E9: // CARON : 4
				i++;
				
				//#ifdef __WXMSW__  ??? WHY WAS THIS HERE
				switch (line[i])
				{
				

				case 'S':
					calcChr = (char) chrCapital_S_Caron;
					break;
				case 's':
					calcChr = (char) chrSmall_S_Caron;
					break;
				
				default:
					calcChr = line[i];
					break;
				}
				
				//#else

				//calcChr = line[i];
				
				//#endif // __WXMSW__

				break;

			case (char) 0x00EA: // RING : 2
				i++;
				switch (line[i])
				{
				case 'A':
					calcChr = (char) chrCapital_A_RingAbove;
					break;
				case 'a':
					calcChr = (char) chrSmall_A_RingAbove;
					break;
				default:
					calcChr = line[i];
					break;
				}
				break;

			case (char) 0x00E0:
			case (char) 0x00E5:
			case (char) 0x00E6:
			case (char) 0x00E7:
			case (char) 0x00EB:
			case (char) 0x00EC:
			case (char) 0x00ED:
			case (char) 0x00EE:
			case (char) 0x00EF:
				i++;
				calcChr = line[i];
				break;

			case (char) 0x00F0: // CEDILLA : 2
				i++;
				switch (line[i])
				{
				case 'C':
					calcChr = (char) chrCapital_C_Cedilla;
					break;
				case 'c':
					calcChr = (char) chrSmall_C_Cedilla;
					break;
				default:
					calcChr = line[i];
					break;
				}
				break;

			case (char) 0x00F1:
			case (char) 0x00F2:
			case (char) 0x00F3:
			case (char) 0x00F4:
			case (char) 0x00F5:
			case (char) 0x00F6:
			case (char) 0x00F7:
			case (char) 0x00F8:
			case (char) 0x00F9:
			case (char) 0x00FA:
			case (char) 0x00FB:
			case (char) 0x00FC:
			case (char) 0x00FD:
			case (char) 0x00FE:
			case (char) 0x00FF:
				i++;
				calcChr = line[i];
				break;

			default:
				calcChr = line[i];
				break;
			}
			
		line[x] = calcChr;
		x++; i++;
		} // while loop

	line[x] = '\0';
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ConvertMonthToNumber
//
// Description:	Parses a month string and converts the month into a number for sorting
//				purposes.
//
// Parameters:	-> monthStr -	string containing the month.
//				-> pos		-	0 for English
//								1 for French
//								2 for Hebrew
//				-> monthVal -	string containing the month number
//
// Returns:		True if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool ParseGedcom::ConvertMonthToNumber (char* monthStr, int pos, char* monthVal)
{
	int			i;
	const int	mnCnt = 13;
	char		*month[mnCnt] [3] = {
								{"JAN", "VEND", "TSH"},
								{"FEB", "BRUM", "CSH"},
								{"MAR", "FRIM", "KSL"},
								{"APR", "NIVO", "TVT"},
								{"MAY", "PLUV", "SHV"},
								{"JUN", "VENT", "ADR"},
								{"JUL", "GERM", "ADS"},
								{"AUG", "FLOR", "NSN"},
								{"SEP", "PRAI", "IYR"},
								{"OCT", "MESS", "SVN"},
								{"NOV", "THER", "TMZ"},
								{"DEC", "FRUC", "AAV"},
								{"",    "COMP", "ELL"}
								};

	strcpy (monthVal, "00");
	
	// search array for monthStr
	for (i = 0; i < mnCnt; i++) {
		if (!strcmp (month[i][pos], monthStr)) {
			sprintf (monthVal, "%02u", i+1);
			return true;
			}
		}

	return false; // month not found
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ConvertDateToNumber
//
// Description:	Parses a date string and converts the date to a number for sorting
//				purposes.
//
// Returns:		Number representing the value of the date in the form of: YYYYMMDD
//						
// Tested Formats:	3 JUN 1780 03 Jun 1780 03 JUN 1780/81 3 JUN 1780 - 1781
//					3 JUN 1780-3 JUN 1781 3 Jun, 1780/81 between 1780 - 1781 between
//					3 Jun 1780-4 Jul 1781 from 3 JUN 1780 to 1781 (TO 2 JULy 1865)
/////////////////////////////////////////////////////////////////////////////////////////
long ParseGedcom::ConvertDateToNumber (char *date, char *year)
{
	char*	token;
	char	dateStr[DATE_SZ+1]	= "\0";
	char	delim[]				= " ,-./"; // delimiters for date string.
	char	dayVal[]			= "00";
	char	monthVal[]			= "00";
	char	yearVal[5];// e.g. 1965 + terminator			= "0000";	
	char	dateVal[9];// e.g. 19650712 + terminator			= "00000000";
	int		holdInt;
	bool	gotYearVal			= false;
	char*	sPos;
	
	*year = '\0'; // initialize

	if (!date || ! *date)
		return 0; // empty string

	strncat (dateStr, StrToUpr (date), DATE_SZ);  // make sure string is upper case.

	if (dateStr[0] == '(')
		return 0; // date phrases start with "(" are not parsable.

	// Make sure date is not split (eg 1 JAN 1990-6 JUL 1991)
	sPos = strchr (dateStr, '-');
	if (!sPos)
		sPos = strchr (dateStr, '/');
	if (sPos)
		*sPos = '\0'; // terminate at '-' or '/' character

	token = strtok (dateStr, delim);

	while (token != NULL) {
	
		holdInt = atoi (token); // first set value of token to holdInt
		
		if (strstr ("ABT,ABOUT,CALCULATED,ESTIMATED,BEFORE,AFTER,BTW,BETWEEN,FROM,INT", token)) {
			// do nothing;

		} else if (strstr ("AND,/-", token)) { // if = one of these then ignore rest of string.
				break; 
		
		} else if (strstr ("TO",token)) { // if token = TO then ignore rest of the string...
			if (atoi (yearVal) != 0)		  // ..but only if we already have a year value.
				break;

		} else if (strstr ("JAN,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC", token)) {
			// Get English Month //
			ConvertMonthToNumber (token, 0, monthVal);

		} else if (strstr ("VEND,BRUM,FRIM,NIVO,PLUV,VENT,GERM,FLOR,PRAI,MESS,THER,FRUC,COMP", token)) {
			// Get French Month //
			ConvertMonthToNumber (token, 1, monthVal);

		} else if (strstr ("TSH,CSH,KSL,TVT,SHV,ADR,ADS,NSN,IYR,SVN,TMZ,AAV,ELL", token)) {
			// Get Hebrew Month //
			ConvertMonthToNumber (token, 2, monthVal);

		} else if (holdInt > 0 && holdInt <= 31) {  // get the day of month
			sprintf (dayVal, "%02u", holdInt);
			wxASSERT (strlen (dayVal) == 2);

		} else if (holdInt > 31 && holdInt < 9999) {  // get the year
			sprintf (yearVal, "%04u", holdInt); 
			gotYearVal = true;
			wxASSERT (strlen (yearVal) == 4);
		}

		token = strtok (NULL, delim);
		
	}  // main while loop.
	
	//if (atoi (yearVal) == 0) {
	if (! gotYearVal) {
		return 0; // not a valid date
		}

	strcpy (year, yearVal);
	strcpy (dateVal, yearVal);

	if (*monthVal) {
		strncat (dateVal, monthVal, 2);
		strncat (dateVal, dayVal, 2);
		}

	return atoi (dateVal);
} 

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ResetGedFile
//
// Description:	Rewinds the Gedcom file and resets LineCnt and m_bFileEnd.
//
// Parameters:	None.
//
// Returns:		Nothing
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::ResetGedFile (void)
{
	rewind (GedFileP);  // start at first line in file.
	LineCnt = 0;
	m_bFileEnd = false;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		SortEventArray
// 
// Description:	Sorts the array of Events by the datVal field.  It then writes the
//				arrays EvenNo strings to one long combined string (evenNo) to be
//				returned as a parameter.  The primary name, and this is also the last
//				of the alternate names written for.
//
// Parameters:	<> evenSort	= the array to be sorted.
//				<>	evenNo	= a pointer to a string comprised of a compilation of all the
//							  evenSort array's evenNo xrefs seperated by 'E's				
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::SortEventArray (EvenSrtRec evenSort[], char *evenNo)
{
	UINT		x, y;
	EvenSrtRec	holder;
	char		hldStr[NREF_SZ+1];

	if (m_iERecCnt > 1) {

		for (x = 0; x < (m_iERecCnt-1); x++) {
   			for ( y=x+1; y < m_iERecCnt; y++)
   				if (evenSort[y].dateVal < evenSort[x].dateVal) {  // compare 2 array items.
					holder = evenSort[x];
					evenSort[x] = evenSort[y];
					evenSort[y] = holder;
      				}
				}
		}

	for (y = 0; y < m_iERecCnt; y++) {
		sprintf (hldStr, "%c%u", CHR_EVN, evenSort[y].evenNo); // add 'E' seperator
		strcat (evenNo, hldStr); // already size checked above via EVEN_MAX
		}

	wxASSERT (strlen (evenNo) <= EREF_SZ);
} 


/////////////////////////////////////////////////////////////////////////////////////////
// Function:		ReadNextRec
//
//	Description:	Reads lines from the GEDCOM file.  If a good record is read, true is
//						returned.
//
//	Parameters:		None.
//
// Returns:			True if record read successfully.  False if bad end of file.
/////////////////////////////////////////////////////////////////////////////////////////
bool ParseGedcom::ReadNextRec (void)
{
	int	x_pos;
	int	l_pos;

	while (!m_bFileEnd) {

		if (GetFileRecAny (buffer, BUFFER_SZ, GedFileP) == NULL) {

			if (feof (GedFileP)) { // check if we at end of GEDCOM file
				m_bFileEnd = true;
				return false;
				}
			else { // must be another error
				wxString msg;
				pGedWiseFrame->SetBadExit ();
				msg.Printf ("ParseGedcom::ReadNextRec \n\n" IDS_ERR_FWRITE, "PG02", "");
				wxLogSysError (msg);
				AddLogEntry (msg);
				CleanExit ();
				return false;
				}
			}
		else {

			 // update progress bar
			if (m_bStartUpdateStats && ((LineCnt & DB_PROG_DIV) == 0))
				pGedWiseFrame->CheckStatus (false);
		
			//  -- Parse the line of data --
			LineCnt++;  // update the global line counter.
			m_bHandled = false;
			x_pos = 0;
			l_pos = 0;

			*m_szLevelNo = '\0';
			*m_szTagType = '\0';
			*m_szTagDesc = '\0';

			while (buffer[l_pos] == ' ')  // skip any preceding spaces.
				l_pos++;

			// Get first of three line items = LEVEL NUMBER //
			while ((buffer[l_pos] != ' ') && (buffer[l_pos] != CHR_NUL)) {
			//while ((buffer[l_pos] != ' ') && (buffer[l_pos] != '\0') && (buffer[l_pos] != '\n')) {
				m_szLevelNo[x_pos] = buffer[l_pos]; 
				x_pos++;
				l_pos++;
				if (x_pos > LEVELNO_SZ)  // do some error checking
					goto BadData;
				}

			m_szLevelNo[x_pos] = '\0';  // don't forget string terminator.

			if (! *m_szLevelNo)  // skip blank lines
				continue;

			while(buffer[l_pos] == ' ')  // Skip any spaces.
				l_pos++;

			x_pos = 0;

			// Get second of three line items = TYPE/X-REFERENCE NUMBER //
			//while ((buffer[l_pos] != ' ') && (buffer[l_pos] != '\0') &&	(buffer[l_pos] != '\n')) {
			while ((buffer[l_pos] != ' ') && (buffer[l_pos] != CHR_NUL)) {	
				
				m_szTagType[x_pos] = buffer[l_pos]; 
				x_pos++;
				if (x_pos > TAGTYPE_SZ)  // do error checking
					goto BadData;
				l_pos++;
				}
			m_szTagType[x_pos] = '\0';  // don't forget string terminator.

			while(buffer[l_pos] == ' ')  // skip any spaces.
				l_pos++;

			x_pos = 0;

			// Get third of three line items = DESCRIPTION. //
			//while ((buffer[l_pos] != '\0') && (buffer[l_pos] != '\n')) {
			while ((buffer[l_pos] != CHR_NUL)) {
				m_szTagDesc[x_pos] = buffer[l_pos]; 
				x_pos++;
				l_pos++;
				if (x_pos > TAGDESC_SZ) // do error checking
					goto BadData;
				}
			
			m_szTagDesc[x_pos] = '\0';	// don't forget string terminator.
			
			if (*m_szTagDesc)
				StripTabs (m_szTagDesc);
			
			if (m_bCharAnsel && *m_szTagDesc)
				ConvAnselToCP1252 (m_szTagDesc);

			if (*m_szTagType)  // must always be event type field
				return true;
			else
				goto BadData;

			break;  // line successfully read so break loop
			
			///////		
			BadData:
			///////

			// write bad line to Error Log
			BadDataErrorHandler ("GedCom");

			} // else

	}  // while (!m_bFileEnd)

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ParseName
//
// Description:	Parses the name tag to seperate the given name and surname.  Also
//				adds title after surname.
//				A name must be in the standard GEDCOM 5.5 format:
//				Firstname/Lastname/Title
//
// Parameters:	-> indiRec	-	individual record to store name info in.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::ParseName (IndiRecType &indRec)
{
	char *name = m_szTagDesc;
	char *origName = name;
	char *firstName = NULL;
	char *lastName  = NULL;
	char *titleName = NULL;
	int  x;

	*indRec.givNam = '\0'; //  intialize
	*indRec.surNam = '\0'; //  initialize
	*indRec.title  = '\0'; //  initialize

	if (name && *name) {

		firstName = name; // firstName == start of name string

		while (*name != '\0' && *name != '/') // scan to find '/' character
			(name)++;
	
		if (*name != '\0') // skip the first found '/'
			(name)++;  
	
		lastName = name;  // lastName == new position in name string

		while (*name != '\0' && *name != '/') // scan to find second '/' character
			(name)++;

		while (*name != '\0' && (*name == '/' || *name == ' '))
			(name)++;

		titleName = name;

		while (*origName != '\0') {  // replace '/' characters with '\0'
			if (*origName == '/')	  // in order to terminate strings
				*origName = '\0';
			(origName)++;
			}
		}

	if (firstName && *firstName) {
		strncat (indRec.givNam, firstName , NAME_SZ);
		// erase any trailing spaces on first name
		x = strlen (indRec.givNam)-1;
		while (indRec.givNam[x] == ' ' && x > 0) {
			indRec.givNam[x] = '\0';
			x--;
			}
		}

	if (lastName && *lastName) {
		strncat (indRec.surNam, lastName , NAME_SZ);
		// erase any trailing spaces on first name
		x = strlen (indRec.surNam) - 1;
		while (indRec.surNam[x] == ' ' && x > 0) {
			indRec.surNam[x] = '\0';
			x--;
			}
		}

	if (titleName && *titleName) {
		// erase any trailing spaces on title name
		wxASSERT (strlen (titleName) <= TIT_SZ);
		strncat (indRec.title, titleName , TIT_SZ);
		x = strlen (indRec.title) - 1;
		while (indRec.title[x] == ' ' && x > 0) {
			indRec.title[x] = '\0';
			x--;
			}
		}

	if (!*indRec.surNam) {
		if (*indRec.title) { // if no last name then use title
			strcpy (indRec.surNam, indRec.title);
			*indRec.title = '\0';
			}
		else 
			strcpy (indRec.surNam, cUnknownStr);
		}

	return;
} 

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ConsAddrField
//
// Description:	Consolidates an Address into one field.
//
// Parameters:	<> strHolder	-	string to return the consolidated place data.
//				-> maxSize		-	the maximum size of the strHolder excluding the null
//												terminator(for size checking).
//											
// Returns:		The consolidated place string returned in strHolder.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::ConsAddrField (char *strHolder, const int maxSize)
{
	INT	startLevel = atoi (m_szLevelNo); // retain original level number
	bool	firstPass  = true;

	while (!m_bFileEnd) {

		// Get the next record from the file. //
		if (m_bHandled)
			if (!ReadNextRec()) return; // we are at end of file

		// if back at level 1 but it is not a PHON, EMAIL, WWW or FAX tag then return
		if (atoi (m_szLevelNo) <= startLevel && !strstr ("PHON_EMAIL_FAX_WWW", m_szTagType)
			&& !firstPass) 
			return; // then we are at next record.

		if (strstr ("CONT,CONC", m_szTagType)) {
			if (*strHolder) {
				if (!strcmp (m_szTagType, "CONT") || pGedWiseFrame->IsInslSpac ())
					strncat (strHolder, " ", maxSize - strlen (strHolder)); // add a space to seperate lines
				strncat (strHolder, m_szTagDesc, maxSize - strlen (strHolder));
				}
			m_bHandled = true;

		} else if (strstr ("ADDR,ADR1,ADR2,CITY,STAE,POST,CTRY_NAME", m_szTagType)) {
			// _NAME used in Legacy and others
			if (*strHolder)  // add comma & space to seperate data
				strncat (strHolder, ", ", maxSize - strlen (strHolder));
			strncat (strHolder, m_szTagDesc, maxSize - strlen (strHolder));
			m_bHandled = true;

		} else if (strstr ("PHON", m_szTagType)) {
			if (*strHolder)  // add comma & space to seperate data
				strncat (strHolder, ", ", maxSize - strlen (strHolder));
			strncat (strHolder, "Ph: ", maxSize - strlen (strHolder));
			strncat (strHolder, m_szTagDesc, maxSize - strlen (strHolder));
			m_bHandled = true;

		} else if (strstr ("_FAX", m_szTagType)) {
			// _FAX is nonstandard tag used in Legacy, DoroTree and draft GEDCOM 5.5.1
			if (*strHolder)  // add comma & space to seperate data
				strncat (strHolder, ", ", maxSize - strlen (strHolder));
			strncat (strHolder, "Fax: ", maxSize - strlen (strHolder));
			strncat (strHolder, m_szTagDesc, maxSize - strlen (strHolder));
			m_bHandled = true;

		} else if (strstr ("_EMAIL", m_szTagType)) {
			// _EMAIL is nonstandard tag used in Legacy, DoroTree and draft GEDCOM 5.5.1
			if (*strHolder)  // add comma & space to seperate data
				strncat (strHolder, ", ", maxSize - strlen (strHolder));
			strncat (strHolder, "Email: ", maxSize - strlen (strHolder));
			strncat (strHolder, m_szTagDesc, maxSize - strlen (strHolder));
			m_bHandled = true;

		} else if (strstr ("_WWW", m_szTagType)) {
			// Web address is nonstandard used in draft GEDCOM 5.5.1
			if (*strHolder)  // add comma & space to seperate data
				strncat (strHolder, ", ", maxSize - strlen (strHolder));
			strncat (strHolder, "Web: ", maxSize - strlen (strHolder));
			strncat (strHolder, m_szTagDesc, maxSize - strlen (strHolder));
			m_bHandled = true;

		} else if (strstr ("CHAN", m_szTagType)) {
			// Ignore this tag
			SkipToStartLevel (false);
			// do not set m_bHandled = true;

		} else if (atoi (m_szLevelNo) > startLevel) { // catch all that doesn't fit above.
			BadDataErrorHandler ("Address");
			SkipToStartLevel (true);
			// do not set m_bHandled = true;
			}

		firstPass = false;
		} // end while loop

	return;
} 

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	AddSpaceAfterComma
//
// Description:	Scans a field and adds a space after a comma if there is no space.
//
// Parameters:	<> strHolder	-	string to return the place data.
//				-> maxSize		-	the maximum size of the strHolder (for size checking) 
//
// Returns:		the consolidated place string returned in strHolder.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::AddSpaceAfterComma (char *strHolder, const int maxSize)
{
	wxString	newStr;
	int			pos = 0;
	int			length;
	char		aChar;

	if (!strHolder)
		return;

	newStr = strHolder;
	
	length = newStr.Length ();
	newStr.Trim (true);
	newStr.Trim (false);

	if (newStr.IsEmpty())
		return;
	
	aChar = newStr.Last ();
	if (aChar == ',' || aChar == '-') {
		newStr.RemoveLast ();
		}

	while (pos < length) {
		pos = newStr.find (',', pos);
		if (pos < 0) // no more commas in string
			break;
		pos++;
		// insert a space after comma
		if (newStr[pos] != ' ' && newStr[pos] != ',')
			newStr.insert (pos, " ");
		}

	*strHolder = '\0';
	strncat (strHolder, newStr.c_str(), maxSize);
} 


/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ConsPlacField
//
// Description:	Consolidates an Place into one field.
//
// Parameters:	<> strHolder	-	string to return the consolidated place data.
//				-> maxSize		-	the maximum size of the strHolder excluding the null
//									terminator (for size checking).
//
// Returns:		the consolidated place string returned in strHolder.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::ConsPlacField (char *strHolder, const int maxSize)
{
	INT	startLevel = atoi (m_szLevelNo); // retain original level number
 
	// Copy first line of address into strHolder.
	strncat (strHolder, m_szTagDesc, maxSize);
	m_bHandled = true;

	while (!m_bFileEnd) {

		if (m_bHandled)
			if (!ReadNextRec()) break;

		if (atoi (m_szLevelNo) <= startLevel) // then we are at next record.
			break;

		if (strstr("CONT,CONC",m_szTagType)) {
			if (!strcmp (m_szTagType, "CONT") || pGedWiseFrame->IsInslSpac ())
				strncat (strHolder, " ", maxSize - strlen (strHolder)); // add a space char
			strncat (strHolder, m_szTagDesc, maxSize - strlen (strHolder));
			m_bHandled = true;

		}	else if (strstr ("FORM,SOUR,NOTE", m_szTagType)) {
			// Ignore FORM, SOUR and NOTE tags for the Place //
			SkipToStartLevel (false);
			// do not set m_bHandled = true

		} else if (atoi (m_szLevelNo) > startLevel) { // catch all that doesn't fit above.
			BadDataErrorHandler ("Place");
			SkipToStartLevel (true);
			// do not set m_bHandled = true
			}

		} // end while loop

	AddSpaceAfterComma (strHolder, maxSize);

	return;
} 

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		CleanStr
//
// Description:	Returns an XREF string without '@' characters.  For instance, an XREF of
//						"@FAMILY1@" would be returned as "FAMILY1".
//	
//	Parameters:		<> aString = the date string to be cleaned
//
//	Returns:			the cleaned string in aString.
/////////////////////////////////////////////////////////////////////////////////////////
char *ParseGedcom::CleanStr (char *aString)
{
	wxASSERT (aString && *aString);

	char	strHld[XREF_SZ + 1];
	UINT	sPos	 = 0;
	char*	subStr = aString;
		
	// Trim starting '@' characters.
	while (subStr[sPos] == '@') // no need to check for string length here
		sPos++;
	
	subStr+= sPos;
 
	sPos = strlen (subStr); // start on string terminator

	// Trim ending '@' and ' ' characters.
	while ((sPos > 0) && (subStr[sPos-1] == '@' || subStr[sPos-1] == ' '))
		sPos--;

	subStr[sPos] = '\0';

	strcpy (strHld,  subStr);
	strcpy (aString, strHld);

	return aString;

}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		CleanDateStr
//
// Description:	Returns an date without any Date Escape reference which is the text
//						between the "@" characters prior to the actual date text.  Primarly 
//						useful for dates presented as "@#DGREGORIAN@ 31 DEC 1997".  The
//						returned string would be "31 DEC 1997"
//
// Parameters:		<> aString - the date string to clean.
//
// Returns:			the cleaned string in aString.
/////////////////////////////////////////////////////////////////////////////////////////
char* ParseGedcom::CleanDateStr (char *aString)
{	
	char*	sPos;
 
	// Reverse find an '@' character.
	sPos = strrchr (aString, '@');
	
	if (sPos) {
		
		sPos+= 1; // move pointer to next character

		// Trim starting ' ' character.
		while (*sPos == ' ') // no need to check for string length here
			sPos++;

		return sPos;
		}

	return aString;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	SkipToStartLevel
//
// Description:	Skips GedCom lines until we reach a line with the same level number as
//				the start level. Used for skipping lines that we want to ignore. 
//				Assumes the current line is to be skipped.
//				m_bHandled is always false when this function returns.
//			
// Parameters:	-> addToLog - TRUE if we want to add skipped line to log file
//	
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::SkipToStartLevel (bool addToLog)
{
	WORD	s_level	= atoi (m_szLevelNo); // retain original level number

	while (!m_bFileEnd) { // read next another until we get back to s_level.
		
		// add log entries for user defined types
		if (addToLog) {
			wxString msg;
			msg.Printf ("Tag Ignored on Line %6d: %s\n", LineCnt, buffer);
			AddLogEntry (msg);
			}
   
		if (!ReadNextRec ())
			return;

		if ( atoi (m_szLevelNo) <= s_level)
			return;

		}  // while (!m_bFileEnd)
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		GetFieldDesc
// 
//	Description:	Copies information from m_szTagDesc into a record field while
//						m_szTagType begins with a CONT or CONC.
//						Note: Don't set m_bHandled to true after calling this function.
//
//	Parameters:		field		=	the record field to which we are adding text.
//						maxSize	=	the size of the field including the null terminator. 
//										(used for size checking).
//						clrFld	=	true means we DO NOT want to add data to "field" if it is
//									   not empty.
//
// Returns:			Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::GetFieldDesc (char *field, int maxSize, bool clrFld)
{
	// check if we already have data in field
	if (*field && clrFld) {
		SkipToStartLevel (true);
		return;
		}

	// If already data in field, add ", "
	if (*field && *m_szTagDesc)
		strncat (field, ", ", maxSize - strlen (field) - 1);

	while (!m_bFileEnd) {
		
		// Add to field, but make sure we don't add more than max size of field (sizeof) 
		// less what we have already written to the field (strlen).
		if (*m_szTagDesc && *m_szTagDesc != '\r')
			strncat (field, m_szTagDesc, maxSize - strlen (field) - 1);

		wxASSERT (*m_szTagDesc != '\r');

		// Get the next record from the file. //
		if (!ReadNextRec ()) break;  // we are at end of file

		if (strstr ("CONT,CONC",m_szTagType) == NULL)
			break;

		if (*m_szTagDesc && (!strcmp (m_szTagType,"CONT") || pGedWiseFrame->IsInslSpac ()))
			strncat (field, " ", maxSize - strlen (field) - 1); // add space bef. line of text

		}  // while loop
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		IsNoteXRef
//
//	Description:	Checks a string to see if it is a GEDCOM XReference tag.
//	
// Parameters:		possibleXRef - pointer to string that could be XRef.
//
// Returns:			false if possibleXRef is not an XRef tag.
//						true if possibleXRef is and XRef tag.
/////////////////////////////////////////////////////////////////////////////////////////
bool ParseGedcom::IsXRef (const char *possibleXRef)
{
	if (possibleXRef[0] == CHR_DLM) {
		int  lastChar = strlen (possibleXRef)-1;
		if ((possibleXRef[lastChar] == CHR_DLM) && (lastChar < XREF_SZ))
			return true;
		}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
//	Function:		GetNoteRec
//
// Description:	Collects note data from the GEDCOM and writes it to a Note record
//				in the database.
//				The function handles non-level "0" notes.
//
//	Parameters:	<- noteNo	-	returns the note xref assigned by this function.
//				<- noteText	-	holds the note text until ready to be written.
//				-> finalize	-	true if writing final note to database record.
//	
// Returns:		false if attempting to add empty note except if finalize is true.
//				true if successfully added a note.
/////////////////////////////////////////////////////////////////////////////////////////
bool ParseGedcom::GetNoteRec (char *noteNo, char *noteText, bool finalize)
{
	NoteRecType	noteRec;
	UINT		noteSize = pGedWiseFrame->GetUserNoteSize ();

	if (pGedWiseFrame->IsExclNote () && !finalize) { // check if wants to copy notes.
		SkipToStartLevel (false);							 // finalize == true when copying ind
		return false;											 //  attribute that must be put into note.
		}

	if (finalize) {  // are we ready to write the note to file.
		
		*noteRec.noteNo = '\0'; // initialize

		sprintf (noteRec.noteNo, "%u", NoteNoCnt);// convert noteNo_cnt to a string.

		NoteNoCnt++; // update Note Reference Counter.
		NoteCnt++;
	
		strncat (noteNo, noteRec.noteNo, XREF_SZ); 

		strcpy (noteRec.note, noteText);  // size checking already done.

		if (fprintf (DBFiles[NoteDB]->fp, "%s\t%s\n", noteRec.noteNo, noteRec.note) < 0) {
			FileErrorHandler (NoteDB);
			return false;
			}
		}

	else {  // read note data

		// if m_szTagDesc is an XRef, just read the XRef and return
		if (IsXRef (m_szTagDesc)) {

			if (*noteText) { // check if already have note. if so we can't add note with xref
				SkipToStartLevel (true);
				return false;
				}

			strncpy (noteNo, CleanStr(m_szTagDesc), XREF_SZ);
			
			// get the next record from the file b/c we need to return with unhandled record
			if (!ReadNextRec ()) return false; // we are at end of file
			return false;
			}

		if (*noteText) // if already have a note, then add a return character for new paragraph.
			strncat (noteText, "\r", noteSize - strlen (noteText));

		while (!m_bFileEnd) {
		
			// Add to note field, but make sure we don't go over max size of note field less
			// what we have already written to the note.
			if (*m_szTagDesc)
				strncat (noteText, m_szTagDesc, noteSize - strlen (noteText));

			// Get the next record from the file.
			if (!ReadNextRec ()) break; // we are at end of file

			if (strstr ("CONT,CONC",m_szTagType) == NULL)
				break;
		
			if (!*m_szTagDesc) { // if line read is blank then add a line-feed character.
				strncat (noteText, "\r", noteSize - strlen (noteText));
				}	
			else if (noteText[strlen (noteText) - 1] != '\r' &&
				(!strcmp (m_szTagType,"CONT") || pGedWiseFrame->IsInslSpac ())) {
				strncat (noteText, " ", noteSize - strlen (noteText)); // add space character
				}

			}  // end while loop

		} // else

	if (!*noteText)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		GetNoteRecX
//
// Description:		Collects note data from the GEDCOM and writes it to a Note record
//					in the database.
//					This function handles level "0" notes that are referenced by a xref
//					provided by the GEDCOM file.
//
// Parameters:		None.	
//
// Returns:			false if attempting to add empty note record or true if successfully
//					added a note record.
/////////////////////////////////////////////////////////////////////////////////////////
bool ParseGedcom::GetNoteRecX (void)
{
	INT			startLevel = atoi (m_szLevelNo); // retain original level number
	NoteRecType	noteRec;
	UINT			noteSize = pGedWiseFrame->GetUserNoteSize ();

	if (pGedWiseFrame->IsExclNote ()) {  // has user decided to copy notes.
		SkipToStartLevel (false);
		return false;
		}

	*noteRec.noteNo =	*noteRec.note = '\0';

	// copy note cross reference
	strncat (noteRec.noteNo, CleanStr (m_szTagType), XREF_SZ); 

	// PAF 3.0 (and possibly others) puts note text immediately after word NOTE	//
	// (eg "0 @T1270@ NOTE Text...").  We have to adjust for this by removing		//
	// the word NOTE from the note text.														//
	if (strncmp ("NOTE", m_szTagDesc, 4) == 0 && strlen (m_szTagDesc) > 4) {
		char	strHld[TAGDESC_SZ + 1];
		strcpy (strHld, &m_szTagDesc[5]); // skip the "NOTE " characters
		strcpy (m_szTagDesc, strHld);
		}
	else
		// Get the next record from the file.
		if (!ReadNextRec()) return false; // we are at end of file

	// Add to note field, but make sure we don't go over max size of note field less what
	// we have already written to the note.
	if (*m_szTagDesc)
		strncat (noteRec.note, m_szTagDesc, noteSize - strlen (noteRec.note));

	m_bHandled = true;

	while (!m_bFileEnd) {

		// Get the next record from the file.
		if (m_bHandled)
			if (!ReadNextRec ()) break; // we are at end of file

		if (atoi (m_szLevelNo) <= startLevel) // then we are at next record.
			break;

		if (strstr ("CONT,CONC",m_szTagType)) {
			
			// Add next line of NOTE text //
			if (!*m_szTagDesc) { // check if line read is blank; if so add line-feed char
				strncat (noteRec.note,"\r", noteSize - strlen (noteRec.note));
				}
			else { // if last char was not line-feed
				
				if (noteRec.note[strlen (noteRec.note) - 1] != '\r' &&
					(!strcmp(m_szTagType,"CONT") || pGedWiseFrame->IsInslSpac ()))
				
					// add space before line of text.
					strncat (noteRec.note, " ", noteSize - strlen (noteRec.note));
					
					// add the line of note text.
					strncat (noteRec.note, m_szTagDesc, noteSize - strlen (noteRec.note));
				}

			m_bHandled = true;

		} else if (!strcmp (m_szTagType, "SOUR")) {
			SkipToStartLevel (true);
			// do not set m_bHandled = true

		} else if (strstr ("REFN,RIN,CHAN",m_szTagType)) {
			// Ignore these tags //

			SkipToStartLevel (false);
			// do not set m_bHandled = true;

		} else {
			SkipToStartLevel (true);  // really bad data
			// do not set m_bHandled = true
			}

		}  // end while loop

	NoteCnt++;

	if (fprintf (DBFiles[NoteDB]->fp, "%s\t%s\n", noteRec.noteNo, noteRec.note) < 0) {
		FileErrorHandler (NoteDB);
		return false;
		}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		GetChilFamRec
//
// Description:	Reads the current child to family data from the GEDCOM file. Writes 
//						data to the database to creates a child to family record.
//
// Parameters:		-> indiNo			 -	individual no. of person for whom creating record
//						-> chilBDate		 -	the individual's birth date.
//						<> areBirthParents -	true if individual's pedigree is birth relationship
//													else relationship is adopted.
// Returns:			Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::GetChilFamRec (const char* indiNo, const long birDate, bool* areBirthParents)
{
	INT		startLevel = atoi (m_szLevelNo); // retain original level number
	char	strHld[XREF_SZ+1] = "\0";
	char	sortKey[XREF_SZ*2+1];		// room for XREF_SZ plus size of date key / XREF_SZ
	
	// Allocate these variables on the heap to avoid stack overflow.
	ChilRecType*	cfamRec;
	cfamRec	=		new ChilRecType;
	char*			bldNote = NULL; // holds NOTE text until writen to Note file
	
	// Initialize variables that may not get used
	*cfamRec->famiNo = *cfamRec->pedi = *cfamRec->noteNo = *cfamRec->citeNo = '\0';
	
	// Copy the Family Reference Number: already set in gedcom file.
	strncat (cfamRec->famiNo, m_szTagDesc, XREF_SZ);
	
	// Create a temporary sort key for each record. DTR 5-6-2004
	strcpy (sortKey, cfamRec->famiNo);
	strncat (sortKey, PAD_STR, XREF_SZ - strlen (sortKey));
	
	if (pGedWiseFrame->IsSortChilPerGed ()) {
		strncat (sortKey, indiNo, (sizeof (sortKey) - strlen (sortKey)) - 1);
		}
	else {
		sprintf (strHld, "%lu", birDate);	
		strcat (sortKey, strHld);
		}

	m_bHandled = true;

	while (!m_bFileEnd) {

		// Get the next record from the file.
		if (m_bHandled)
			if (!ReadNextRec()) break; // we are at end of file

		if (atoi (m_szLevelNo) <= startLevel) // then we are at next record.
			break;

		if (!strcmp (m_szTagType, "PEDI")) {
			// Record Pedigree of Child's Relationship //
			
			GetFieldDesc (cfamRec->pedi, sizeof (cfamRec->pedi), true); // only take first PEDI
			// do not set m_bHandled = true
		
		} else if (!strcmp (m_szTagType, "NOTE")) {
			// Record an Note for the Family Event //
			
			if (!bldNote) {
				bldNote = new char[NOTE_SZ+1];
				*bldNote = '\0';
				}
			GetNoteRec (cfamRec->noteNo, bldNote, false);
			// do not set m_bHandled = true
		
		} else if (!strcmp (m_szTagType, "SOUR")) {
			// Record a Source Citation for record //
			
			GetCiteRec (cfamRec->citeNo);
			// do not set m_bHandled = true

		} else if (m_szTagType[0] == UD_TAG_CHAR) {
			// Genealogy Program Specific Erroneous Tags //
			// User Defined Tags starting with "_" = Family Tree Maker and others
			
			SkipToStartLevel (true);
			// do not set m_bHandled = true

		} else if (atoi (m_szLevelNo) > startLevel) {// catch all that does not fit above
			BadDataErrorHandler ("Child");
			SkipToStartLevel (true);
			// do not set m_bHandled = true
			}

		 // Check if error occurred or cancel button hit.
		if (pGedWiseFrame->GetBadExit ())
			goto DoDeletes;  
		
		}  // end of main while loop.

	if (bldNote && *bldNote)
		GetNoteRec (cfamRec->noteNo, bldNote, true); // now write the note to note file

	if (!*areBirthParents)
		strcpy (cfamRec->pedi, "adopted");
	else
		if (*cfamRec->pedi && strstr ("birthBIRTH", cfamRec->pedi) == NULL)
			*areBirthParents = false;

	// Check if user opts to sort children as per GEDCOM.
	if (pGedWiseFrame->IsSortChilPerGed () &&
		! (*cfamRec->pedi || *cfamRec->noteNo || *cfamRec->citeNo)) {
		// if not other data, then don't write record, as it will be written in GedFamiRecord
		goto DoDeletes; // do not write record here
		}

	if (fprintf (DBFiles[ChilDB]->fp, "%s\t%s\t%s\t%s\t%s\t%s\n", sortKey,
		cfamRec->famiNo, indiNo, cfamRec->pedi, cfamRec->noteNo, cfamRec->citeNo) < 0)
		FileErrorHandler (ChilDB);

	/////////
	DoDeletes:
	/////////
	
	delete [] cfamRec;
	cfamRec = NULL;

	if (bldNote) {
		delete [] bldNote;
		bldNote = NULL;
		}

	return;
} 

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		GetChilRec
//
// Description:	Writes one child to family record to the ChSoDB database. This routine
//						is only used if the user has selected the option to sort children as
//						listed in the GEDCOM file.
//
// Parameters:		-> sortNo	 -	individual number of person for whom creating record
//						-> famiNo	 -	family XRef for family of individual.
//											
// Returns:			Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::GetChilRec (const UINT sortNo, const char* famiNo)
{
	char	sortKey2[XREF_SZ+DKEY_SZ+1];	// room for XREF_SZ plus size of date
	char	sortKey1[XREF_SZ*2+1];			// room for 2 XREF_SZ
	char	indiNo[XREF_SZ+1] = "\0";
	char	fmtStr[15];							// format specifier for sort keys

	// Copy the Individual Reference Number (already set in gedcom file).
	strncat (indiNo, CleanStr (m_szTagDesc), XREF_SZ);

	// Set up 1st temporary key for each record to be used for sorting.
	sprintf (fmtStr, "%%-%us%%s", XREF_SZ);
	sprintf (sortKey1, fmtStr, famiNo, indiNo);

	// Set up 2nd temporary key for each record to be used for sorting.
	sprintf (fmtStr, "%%-%us%%03u", XREF_SZ);
	sprintf (sortKey2, fmtStr, famiNo, sortNo);

	if (fprintf (DBFiles[ChSoDB]->fp, "%s\t%s\t%s\t%s\n", 
		sortKey1, sortKey2, famiNo, indiNo) < 0)
		FileErrorHandler (ChSoDB);

	return;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		GetRepoRec
//
// Description:	Reads repository data from the GEDCOM file.  Writes the data to the
//						database to creates a repository record.
//
// Parameters:		None.
//						
// Returns:			Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::GetRepoRec (void)
{
	INT	startLevel = atoi (m_szLevelNo); // retain original level number

	// Check if user has decided to copy repositories and sources
	if (pGedWiseFrame->IsExclRepo () || pGedWiseFrame->IsExclSour ()) {
		SkipToStartLevel (false);
		return;
		}

	// Allocate these variables on the heap to avoid stack overflow
	RepoRecType*	repoRec;
	repoRec			= new RepoRecType;
	char*				bldNote = NULL; // holds NOTE text until writen to Note file

	// Initialize variables that may not get used
	*repoRec->repoNo = *repoRec->name = *repoRec->noteNo = *repoRec->addr = '\0';
 
	// Copy repository reference Number: already set in gedcom file.
	strncat (repoRec->repoNo, CleanStr (m_szTagType), XREF_SZ);
	
	m_bHandled = true;

	while (!m_bFileEnd) {

		// Get the next record from the file //
		if (m_bHandled)
			if (!ReadNextRec()) break;  // we are at end of file

		if (atoi (m_szLevelNo) <= startLevel) // then we are at next record.
			break;
			
		if (!strcmp (m_szTagType, "NAME")) {
			// Record Name of Repository //
			GetFieldDesc (repoRec->name, sizeof (repoRec->name), true);
			// do not set m_bHandled = true
		
		} else if (strstr ("ADDR,ADR1,ADR2,CITY,STAE,POST,CTRY,PHON,LOCA", m_szTagType)) {
			// Record Address Information //
			// LOCA is not supported but put here for compatibility with EasyTree
			
			ConsAddrField (repoRec->addr, ADDR_SZ);
			// do not set m_bHandled = true;

		} else if (!strcmp (m_szTagType, "NOTE")) { 
			// Note for a Repository Record //
			if (!bldNote) {
				bldNote = new char[NOTE_SZ+1];
				*bldNote = '\0';
				}
			GetNoteRec (repoRec->noteNo, bldNote, false); // get note data
			// do not set m_bHandled = true;

		} else if (strstr ("CHAN", m_szTagType)) {
			// Ignore these tags //
			
			SkipToStartLevel (false);
			// do not set m_bHandled = true;

		} else if (strstr ("REFN,RIN", m_szTagType)) {
			// Ignore these tags //
			
			SkipToStartLevel (true);
			// do not set m_bHandled = true;

		} else if (m_szTagType[0] == UD_TAG_CHAR) {
			// Genealogy Program Specific Erroneous Tags //
			// User Defined Tags starting with "_" = Family Tree Maker and others
			
			SkipToStartLevel (true);
			// do not set m_bHandled = true;

		} else if (atoi (m_szLevelNo) > startLevel) {// catch all records that don't fit above
			BadDataErrorHandler ("Repository");
			SkipToStartLevel (true);
			// do not set m_bHandled = true;
			}

		// check if error occurred or if user hit cancel button
		if (pGedWiseFrame->GetBadExit ())
			goto DoDeletes;

		}  // end of main while loop.

	if (bldNote && *bldNote)
		GetNoteRec (repoRec->noteNo, bldNote, true);  // now write the note to file
	
 	if (fprintf (DBFiles[RepoDB]->fp, "%s\t%s\t%s\t%s\n",
		repoRec->repoNo,  repoRec->noteNo, repoRec->name,	repoRec->addr) < 0)
		FileErrorHandler (RepoDB);

	/////////
	DoDeletes:
	/////////

	delete [] repoRec;
	repoRec = NULL;

	if (bldNote) {
		delete [] bldNote;
		bldNote = NULL;
		}

	return;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		GetRCitRec
//
// Description:	Reads source-repository data from the GEDCOM file.  Writes the data 
//						to the database to creates a source-repository record.
//
// Parameters:		<- rcitNo - repository citation number to return.
//						
// Returns:			Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::GetRCitRec (char *rcitNo)
{
	int	startLevel = atoi (m_szLevelNo); // // retain original level number

	// Check if user decided to copy Repositories and Sources.
	if (pGedWiseFrame->IsExclRepo () || pGedWiseFrame->IsExclSour ()) {  
		SkipToStartLevel (false);
		return;
		}

	// Allocate these variables on the heap to avoid stack overflow.
	RCitRecType*	rciteRec;
	rciteRec		= new RCitRecType;
	char*			bldNote = NULL; // holds NOTE text until writen to Note file

	// Initialize variables that may not get used.
	*rciteRec->caln = *rciteRec->medi = *rciteRec->noteNo = *rciteRec->repoNo = '\0';

	// Create the rcitNo to return.
	sprintf (rcitNo, "%u", RCitNoCnt); // convert rcitNo_cnt to a string.
	
	RCitNoCnt++; // update RCitation_Rec Reference Counter.

	// Check that m_szTagDesc is a repository XRef tag (eg @R01@). Some genealogy
	// software (eg FTW) erroneously follows an REPO tag by repository text. In
	// such a case we put the text into a NOTE. 
	if (!IsXRef (m_szTagDesc)) {
		
		sprintf (m_szLevelNo,"%i", startLevel+1);
		strcpy (m_szTagType,"NOTE");
		*rciteRec->repoNo = '\0';
		m_bHandled = false;
		}
	else { // repository notation is done properly
		// Copy the Repository reference tag. Already set in gedcom file.

		if (*m_szTagDesc)
			strncat (rciteRec->repoNo, CleanStr (m_szTagDesc), XREF_SZ);  
			m_bHandled = true;
			}

	while (!m_bFileEnd) {

		// Get the next record from the file. //
		if (m_bHandled)
			if (!ReadNextRec()) break;  // we are at end of file

		if (atoi (m_szLevelNo) <= startLevel) // then we are at next record.
			break;

		if (!strcmp (m_szTagType, "CALN")) {
			// Call Number information //
			GetFieldDesc (rciteRec->caln, sizeof(rciteRec->caln), true);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "MEDI")) {
			// Media Type //
			GetFieldDesc (rciteRec->medi, sizeof(rciteRec->medi), true);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "NOTE")) { 
			// Record an Note for a SCite Record //
			
			if (!bldNote) { // create space for the note
				bldNote = new char[NOTE_SZ+1];
				*bldNote = '\0';
				}

			GetNoteRec (rciteRec->noteNo, bldNote, false);
			// do not set m_bHandled = true

		} else if (m_szTagType[0] == UD_TAG_CHAR) {
			// Genealogy Program Specific Erroneous Tags //
			// User Defined Tags starting with "_" = Family Tree Maker and others
			
			SkipToStartLevel (true);
			// do not set m_bHandled = true

		} else if (atoi (m_szLevelNo) > startLevel) { // catch all records that don't fit above
			BadDataErrorHandler ("Repostory Cit");
			SkipToStartLevel (true);
			// do not set m_bHandled = true
			}

		// check if error occurred or if user hit cancel button
		if (pGedWiseFrame->GetBadExit ())
			goto DoDeletes;

		}  // end of main while loop.

	if (bldNote && *bldNote)
		GetNoteRec (rciteRec->noteNo, bldNote, true);

	if (fprintf(DBFiles[RCitDB]->fp, "%s\t%s\t%s\t%s\n",
		rciteRec->repoNo,	rciteRec->noteNo,	rciteRec->caln, rciteRec->medi) < 0)
		FileErrorHandler(RCitDB);

	/////////
	DoDeletes:
	/////////

	delete [] rciteRec;
	rciteRec = NULL;

	if (bldNote) {
		delete [] bldNote;
		bldNote = NULL;
		}

	return;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		GetSourRec
//
// Description:	Reads source data from the GEDCOM file.  Writes the data to the
//						database to creates a source record.
//
//
// Parameters:		None.
//						
// Returns:			Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::GetSourRec (void)
{
	INT	startLevel = atoi (m_szLevelNo); // retain original level number
	
	// Check if user has decided not to copy sources.
	if (pGedWiseFrame->IsExclSour ()) {
		SkipToStartLevel (false);
		return;
		}

	// Allocate these variables on the heap to avoid stack overflow
	SourRecType*	sourRec;
	sourRec			= new SourRecType;
	char*				bldNote; // holds NOTE text until writen to Note file
	bldNote			= new char[NOTE_SZ+1];
	char*				textHolder = NULL; // temporarily holds source from text

	// Initialize variables that may not get used
	*sourRec->sourNo	= *sourRec->titl		= *sourRec->even		= *sourRec->text =
	*sourRec->date		= *sourRec->plac		= *sourRec->agnc		= *sourRec->publ =
	*sourRec->auth    = *sourRec->rcitNo	= *sourRec->noteNo	= *sourRec->numb = '\0';
	*bldNote          = '\0';
	
	// Copy the Source Reference Number: already set in GedCom file.
	strncat (sourRec->sourNo, CleanStr (m_szTagType), XREF_SZ);

	m_bHandled = true;

	while (!m_bFileEnd) {
		
		// Get the next record from the file. //
		if (m_bHandled)
			if (!ReadNextRec()) break;  // we are at end of file

		if (atoi (m_szLevelNo) <= startLevel) // then we are at next record.
			break;
	
		if (!strcmp (m_szTagType,"DATA")) {  // get ready for DATA tags. Just ignore tag.
			m_bHandled = true;

		} else if (!strcmp (m_szTagType, "EVEN")) {
			// Events Recorded. Ignore additional EVEN tags- this program only allows one //
			GetFieldDesc (sourRec->even, sizeof (sourRec->even), true);
			// do not set m_bHandled = true
				
		} else if (!strcmp(m_szTagType, "DATE")) {
			// Date Period. Ignore additional DATE tags as this program only takes one. //
			GetFieldDesc (sourRec->date, sizeof (sourRec->date), true);
			// do not set m_bHandled = true

		} else if (strstr ("PLAC,PHON,LOCA", m_szTagType)) {
			// Place of Event. Ignore additional PLAC tags as this program only takes one //
			GetFieldDesc (sourRec->plac, sizeof (sourRec->plac));
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "AGNC")) {
			// Record Responsible Agency //
			GetFieldDesc (sourRec->agnc, sizeof (sourRec->agnc), true);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "PUBL")) {
			// Record Publication Information //
			GetFieldDesc (sourRec->publ, sizeof (sourRec->publ), true);
			// do not set m_bHandled = true

		} else if (strstr ("TITL,TAXT,PERI,NAME,ABBR", m_szTagType)) {
			//	Record Source Title //
			// TAXT & PERI are not valid tags but put here for EasyTree support.
			// NAME is not valid tag but put here for Reunion 4.0 support
			// Some program only include ABBR tag, instead of TITL tag.  In this case
			// use ABBR tag unless we hit a TITL tag later under the source.
			
			if (!strcmp (m_szTagType, "ABBR")) {
			
				if (!*sourRec->titl) {
					GetFieldDesc (sourRec->titl, sizeof (sourRec->titl), true);
					}
				else {
					SkipToStartLevel (false);
					}
				}
			else {
				*sourRec->titl = '\0'; // erase anything already there
				GetFieldDesc (sourRec->titl, sizeof (sourRec->titl), true);
				}
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "TEXT")) {
			// Record Text from Source //
			
			if (!textHolder) { // create room for text data
				textHolder = new char[NOTE_SZ+1];
				}

			strcpy (textHolder, "\0"); // always init this here

			GetFieldDesc (textHolder, NOTE_SZ+1);
			
			if (strlen (textHolder) >= sizeof (sourRec->text)) { // text too big
				if (*bldNote) // if already have a note, then start new paragraph
					strncat (bldNote, "\r", NOTE_SZ - strlen (bldNote));
				strncat (bldNote, textHolder, NOTE_SZ - strlen (bldNote));
				}
			else if (*textHolder) // just copy the text to sourRec->text
				strncat (sourRec->text, textHolder, sizeof (sourRec->text) 
					- strlen (sourRec->text) - 1);
			// do not set m_bHandled = true

		} else if (!strcmp(m_szTagType, "AUTH")) {	
			//	Record Source Originator.	//
			GetFieldDesc (sourRec->auth, sizeof (sourRec->auth), true);
			// do not set m_bHandled = true
		
		} else if (!strcmp(m_szTagType, "NUMB")) {	
			//	Record Source Originator.	//
			GetFieldDesc (sourRec->numb, sizeof (sourRec->numb));
			// do not set m_bHandled = true

		} else if (!strcmp(m_szTagType, "NOTE")) { 
			// Record an Note for a Source Record //
			GetNoteRec (sourRec->noteNo, bldNote, false);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "REPO")) {
			// Record a Citation for a Source Record //
			
			if (*sourRec->rcitNo) // only 1 rcit allowed
				SkipToStartLevel (false);
			else
				GetRCitRec (sourRec->rcitNo); 
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "CALN")) {
			// Record Call Number for Source.  This is not a standard GEDCOM	//
			// format, but it is done in PAF.											//
			
			if (*bldNote) // if already have a note, then start new paragraph
				strncat (bldNote, "\r", NOTE_SZ - strlen (bldNote));
			strncat (bldNote, "CALN: ", NOTE_SZ - strlen (bldNote));
			strncat (bldNote, m_szTagDesc, NOTE_SZ - strlen (bldNote));
			m_bHandled = true;

		} else if (!strcmp (m_szTagType, "TYPE")) {
			// Record Type for Source.  This is not standard GEDCOM format	//
			// but it is done in PAF.													//
			
			if (*bldNote) // if already have a note, then start new paragraph
				strncat (bldNote, "\r", NOTE_SZ - strlen (bldNote));
			strncat (bldNote, "TYPE: ", NOTE_SZ - strlen (bldNote));
			strncat (bldNote, m_szTagDesc, NOTE_SZ - strlen (bldNote));

			m_bHandled = true;

		} else if (strstr ("OBJE,CHAN,DATE", m_szTagType)) {
			// Ignore these tags.  //
			SkipToStartLevel (false);
			// do not set m_bHandled = true

		} else if (strstr ("REFN,RIN", m_szTagType)) {
			// Ignore these tags.  //
			SkipToStartLevel (true);
			// do not set m_bHandled = true

		} else if (m_szTagType[0] == UD_TAG_CHAR) {
			// Genealogy Program Specific Erroneous Tags //
			// User Defined Tags starting with "_" = Family Tree Maker and others
			SkipToStartLevel (true);
			// do not set m_bHandled = true
		
		} else if (atoi (m_szLevelNo) > startLevel) { // catch all records that don't fit above.
			BadDataErrorHandler ("Source");
			SkipToStartLevel (true);
			// do not set m_bHandled = true
			}

		// check if error occurred or if user hit cancel button
		if (pGedWiseFrame->GetBadExit ())
			goto DoDeletes;

		}  // end of main while loop.

	if (*bldNote)
		GetNoteRec (sourRec->noteNo, bldNote, true);

 	if (fprintf(DBFiles[SourDB]->fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
		sourRec->sourNo,	sourRec->rcitNo,	sourRec->noteNo,	sourRec->titl,	sourRec->even,
		sourRec->date,		sourRec->plac,		sourRec->agnc,		sourRec->auth,	sourRec->text,
		sourRec->publ,		sourRec->numb) < 0)
		FileErrorHandler (SourDB);

	/////////
	DoDeletes:
	/////////

	delete [] sourRec;
	sourRec = NULL;

	delete [] bldNote;
	bldNote = NULL;
	
	if (textHolder) {
		delete [] textHolder;
		textHolder = NULL;
		}

	return;
} 

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		GetSCitRec
//
// Description:	Gets information on the current citation from the GEDCOM file. Writes
//						the data to create a citation record in the database.
//
// Parameters:		<- scitNo	-	the citation number to be included in the record 
//											containing the current source.
//
// Returns:			Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::GetCiteRec (char *scitNo)
{
	INT	startLevel = atoi (m_szLevelNo); // retain original level number
	char	strHld[XREF_SZ+1] = "\0";
	
	if (pGedWiseFrame->IsExclSour ()) { // has user decided to copy Source information.
		SkipToStartLevel (false);
		return;
		}

	if ((strlen (scitNo) + NREF_SZ) > SREF_SZ){ // check if reached maximum scites allowed
		SkipToStartLevel (false);
		return;
		}

	// Allocate these on heap to prevent stack overflow
	CiteRecType*	sciteRec;
	sciteRec			= new CiteRecType;
	char*				bldNote = NULL; // holds NOTE text until written to Note file

	// Initialize variables that may not get used
	*sciteRec->sourNo = *sciteRec->page	= *sciteRec->even	=	*sciteRec->role	=
	*sciteRec->quay   = *sciteRec->date	= *sciteRec->text	=	*sciteRec->noteNo = '\0';
	
	// Create the citeNo to return in format of "S##..."
	sprintf (strHld, "S%u", SCitNoCnt);
	strncat (scitNo, strHld, SREF_SZ - strlen (scitNo));

	SCitNoCnt++;  // update sciteNo counter.

	// Check that m_szTagDesc is a source XRef tag (eg @S01@). Some genealogy
	// software (eg FTW) erroneously follows an SOUR tag by source text. In
	// such a case we put the text into a NOTE. 
	if (!IsXRef (m_szTagDesc)) {
		sprintf (m_szLevelNo,"%i", startLevel+1);
		strcpy (m_szTagType, "NOTE");
		*sciteRec->sourNo = '\0';
		m_bHandled = false;
		}
	else { // source notation is done properly
		// copy the Source reference tag. Already set in gedcom file.
		strncat (sciteRec->sourNo, CleanStr (m_szTagDesc), XREF_SZ);
		m_bHandled = true;
		}

	while (!m_bFileEnd) {

		// Get the next record from the file. //
		if (m_bHandled)
			if (!ReadNextRec()) break;  // we are at end of file

		if (atoi (m_szLevelNo) <= startLevel) // then we are at next record.
			break;

		if (!strcmp (m_szTagType,"PAGE")) {
			// Record where within source //
			GetFieldDesc (sciteRec->page, sizeof(sciteRec->page), true);
			// do not set m_bHandled = true
		
		} else if (!strcmp (m_szTagType, "EVEN")) {
			/* Record event type cited from (an event attribute type)*/
			GetFieldDesc (sciteRec->even, sizeof(sciteRec->even), true);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "ROLE")) {
			// Record roll in event //
			GetFieldDesc (sciteRec->role, sizeof (sciteRec->role), true);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "QUAY")) {
			// Record certainty assessment //
			GetFieldDesc (sciteRec->quay, sizeof (sciteRec->quay), true);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "DATE")) {
			// Record date of recording //
			GetFieldDesc (sciteRec->date, sizeof (sciteRec->date), true);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "DATA")) {
			// Record Data tag //
			// Ignore this line and read next line from file.
			m_bHandled = true;

		} else if (!strcmp (m_szTagType, "TEXT")) {
			// Record text from Text tag. Only record first TEXT item//
			GetFieldDesc (sciteRec->text, sizeof (sciteRec->text), true);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "NOTE") && atoi (m_szLevelNo) > startLevel) { 
			// Record an Note for a SCite Record //
			if (!bldNote) {	
				bldNote = new char[NOTE_SZ+1];
				*bldNote = '\0';
				}

			GetNoteRec (sciteRec->noteNo, bldNote, false);
			// do not set m_bHandled = true

		} else if (strstr ("OBJE" ,m_szTagType)) {
			// Ignore these tags. //
			SkipToStartLevel (false);
			// do not set m_bHandled = true
		
		} else if (m_szTagType[0] == UD_TAG_CHAR) {
			// Genealogy Program Specific Erroneous Tags //
			// User Defined Tags starting with "_" = Family Tree Maker and others
			SkipToStartLevel (true);
			// do not set m_bHandled = true

		} else if (atoi (m_szLevelNo) > startLevel) { // catch all that don't fit above
			BadDataErrorHandler ("Source Cit");
			SkipToStartLevel (true);
			// do not set m_bHandled = true
			}

		// Check if error occurred or if user hit cancel button
		if (pGedWiseFrame->GetBadExit ())
			goto DoDeletes;

		}  // end of main while loop.

	if (bldNote && *bldNote)
		GetNoteRec (sciteRec->noteNo, bldNote, true);

 	// write the record to file
	if (fprintf(DBFiles[SCitDB]->fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
		sciteRec->sourNo,	sciteRec->noteNo,	sciteRec->page,	sciteRec->even,
		sciteRec->role,	sciteRec->quay,	sciteRec->date,	sciteRec->text) < 0)
		FileErrorHandler (SCitDB);

	/////////
	DoDeletes:
	/////////
	
	delete [] sciteRec;
	sciteRec = NULL;

	if (bldNote) {
		delete [] bldNote;
		bldNote = NULL;
		}

	return;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		GetFamRec
//
// Description:	Gets information on the current family from the GEDCOM file. Writes
//						the data to create a family record in the database.
//						Ignores CHIL tags as these are already taken care of in IndiRec.
//
// Parameters:		None.
//
// Returns:			Nothing
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::GetFamiRec (void)
{
	int			startLevel = atoi (m_szLevelNo); // retain original level number
	char		szFamiNoEvt[XREF_SZ+2] = "\0";   // add extra space for starting "F"
	UINT		chilSortNo		 = 0;
	EvenSrtRec	primMarSortRec;

	// Allocate on heap to prevent stack overflow.
	FamiRecType*	famRec;
	famRec			= new FamiRecType;
	EvenSrtRec*		evenSort;
	evenSort		= new EvenSrtRec[EVEN_MAX]; // should not be many of these type of events
	char*			bldNote = NULL; // holds NOTE text until writen to Note file

	// Initialize variables that may not get used.
	*famRec->famiNo	= *famRec->husbNo  = *famRec->wifeNo	= *famRec->nchi	=
	*famRec->evenMNo	= *famRec->evenNo  =	*famRec->noteNo	= *famRec->citeNo = '\0';
	
	m_iERecCnt = 0; // counts no. of family events for each individual.
	
	FamiNoCnt++; // update Family Reference Counter.

	// Copy Family XRef number.
	strncat (famRec->famiNo, CleanStr (m_szTagType), XREF_SZ); 
	sprintf (szFamiNoEvt, "%c%s", CHR_FAM, famRec->famiNo); // get EvenDB OrgNo field

	m_bHandled = true;

	while (!m_bFileEnd) {

		if (m_bHandled)
			if (!ReadNextRec()) break; // we are at end of file

		if (atoi (m_szLevelNo) <= startLevel) // then we are at next record.
			break;

		if (!strcmp (m_szTagType, "HUSB")) {	
			// Record the Husband reference number	//
			if (! *famRec->husbNo) 
				strncat (famRec->husbNo, CleanStr (m_szTagDesc), XREF_SZ);
			m_bHandled = true;
		
		} else if (!strcmp (m_szTagType, "WIFE")) {
			// Record the Wife reference number	//
			if (! *famRec->wifeNo)
				strncat (famRec->wifeNo, CleanStr (m_szTagDesc), XREF_SZ);
			m_bHandled = true;

		} else if (!strcmp (m_szTagType, "NCHI")) {	
			// Record the Child count data for a Family Record	//
			GetFieldDesc (famRec->nchi, sizeof (famRec->nchi), true);
			// do not set m_bHandled = true
			//strncat (famRec->nchi, m_szTagDesc, sizeof (famRec->nchi) - 1);
			//m_bHandled = true;

		} else if (!strcmp (m_szTagType, "MARR")) {	
			// Record Marriage Event //
			wxASSERT (*szFamiNoEvt);
			
			if (! *famRec->evenMNo) { // check for primary marriage
				GetEventRec (false,  &primMarSortRec, szFamiNoEvt, true);
				sprintf (famRec->evenMNo, "%u", primMarSortRec.evenNo);				
				}
			else
				GetEventRec (false, &evenSort[m_iERecCnt], szFamiNoEvt);
			// do not set m_bHandled = true
		
		} else if (strstr ("ANUL,CENS,DIV,DIVF,ENGA,MARB,MARC,MARL,MARS,EVEN", m_szTagType)) {
			// Record a (non-marriage) Family Event //
			wxASSERT (*szFamiNoEvt);
			GetEventRec (false, &evenSort[m_iERecCnt], szFamiNoEvt);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType,"NOTE")) {
			// Record an Note for a Family Record //
			if (!bldNote) {
				bldNote	= new char[NOTE_SZ+1];
				*bldNote = '\0';
				}
			
			GetNoteRec (famRec->noteNo, bldNote, false);
			// do not set m_bHandled = true
	
		} else if (!strcmp (m_szTagType,"SOUR")) {
			// Record a Citation for a Family Record //
			GetCiteRec (famRec->citeNo);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType,"SLGS")) {
			// Record LDS Sealing as a Family Event //
			if (pGedWiseFrame->IsExclOrdn ())
				SkipToStartLevel (false);
			else {
				wxASSERT (*szFamiNoEvt);
				GetEventRec (false, &evenSort[m_iERecCnt],	szFamiNoEvt);
				}
			// do not set m_bHandled = true

		} else if (strstr ("SUBM,OBJE,CHAN", m_szTagType)) {
			// Ignore these tags //
			SkipToStartLevel (false);
			// do not set m_bHandled = true


		} else if (strstr ("_DETS_SEPR,RESI", m_szTagType)) {
			// Record a non-standard event tag used in Reunion and FTM. //

			if (m_szTagType[0] == UD_TAG_CHAR) {
				char strHld[TAGTYPE_SZ + 1];
				strcpy (strHld, &m_szTagType[1]);
				strcpy (m_szTagType, strHld); // remove initial "_" char
				}

			if (strstr ("SEPR", m_szTagType))
				strcpy (m_szTagType, "DIV");

			GetEventRec (false, &evenSort[m_iERecCnt], szFamiNoEvt);
			// do not set m_bHandled = true

		} else if (strstr ("REFN,RIN", m_szTagType)) {
			// Ignore these tags.//
			SkipToStartLevel (true);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType,"CHIL")) {
			// Record Child Record is sorting as per GEDCOM file //
			if (pGedWiseFrame->IsSortChilPerGed ()) {
				GetChilRec (chilSortNo, famRec->famiNo);
				chilSortNo++;
				}
			SkipToStartLevel (false);
			// do not set m_bHandled = true
	
		} else if (m_szTagType[0] == UD_TAG_CHAR) {
			// Genealogy Program Specific Erroneous Tags //
			// User Defined Tags starting with "_" = Family Tree Maker and others
			SkipToStartLevel (true);
			// do not set m_bHandled = true

		} else if (atoi (m_szLevelNo) > startLevel) { // catch all that doesn't fit above.
			BadDataErrorHandler ("Family");
			SkipToStartLevel (true);
			// do not set m_bHandled = true
			}

		// check if error occurred or if user hit cancel button.
		if (pGedWiseFrame->GetBadExit ())
			goto DoDeletes;

		}  // end main while loop.

	if (bldNote && *bldNote)
		GetNoteRec (famRec->noteNo, bldNote, true);

	SortEventArray (evenSort, famRec->evenNo);

	if (fprintf(DBFiles[FamiDB]->fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",famRec->famiNo,
		famRec->husbNo,	famRec->wifeNo,	famRec->nchi,	famRec->evenMNo,	famRec->evenNo,
		famRec->noteNo,	famRec->citeNo) < 0)
		FileErrorHandler (FamiDB);

	/////////
	DoDeletes:
	/////////

	delete [] famRec;
	famRec  = NULL;
	
	delete [] evenSort;
	evenSort = NULL;
	
	if (bldNote) {
		delete [] bldNote;
		bldNote = NULL;
		}

	return;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GetEventRec
//
// Description:	Collects data about the current event from the GEDCOM file. Writes the
//				data to create an event  record in the EvenDB database.
//
//				NOTE: before calling this function, the following variables must be set:
//				recNo = set to an Individual XRef or Family XRef.
//							
//				This routine will be increment EvenNoCnt by 1. It will also increment
//				m_iERecCnt by 1 if it is not a primary event record.
//
//				For primary events (where primRec == true) this routine sets the 
//				m_szBirYrHld and m_szDthYrHld variables.
//
// Parameters	-> indAttr	-	indicates whether event is an individual attribute.
//				-> evenSort	-	pointer to a EvenSrtRec record.
//				-> recNo	-	the XRef of the Individual or Family associate with the
//								event.
//				-> primRec	-	indicates whether the record being written is a primary
//								birth, death or marriage event record.  If so, it will
//								use that	record to get the birth and/or death year.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::GetEventRec (const bool indAttr, EvenSrtRec *evenSort, const char *recNo,
								  const bool primRec)
{
	int		startLevel = atoi (m_szLevelNo);  // retain original level number
	char	year[5] = "\0";
	UINT	evenTypeNo;
	char	evenTypeStr[5];

	// Make sure we don't have too many event tags for this record.
	if (m_iERecCnt >= EVEN_MAX) {
		wxASSERT (false);  // should never reach here
		SkipToStartLevel (false);
		return;
		}

	// Allocate these variables on heap to prevent stack overflow
	EvenRecType*	evenRec;
	evenRec			= new EvenRecType;
	char*				attrHolder;  // temporarily holds Attribute text
	attrHolder		= NULL;
	char*				bldNote;
	bldNote			= new char[NOTE_SZ+1]; // holds NOTE text until writen to Note file

	// Initialize variables that may not get used
	*evenRec->desc	 = *evenRec->type	  = *evenRec->date	= *evenRec->plac	=
	*evenRec->addr	 = *evenRec->age	  = *evenRec->agnc	= *evenRec->caus	=
	*evenRec->hAge	 = *evenRec->wAge	  = *evenRec->reli	= *evenRec->statL	=
	*evenRec->tempL = *evenRec->citeNo = *evenRec->noteNo = '\0';
	*bldNote        = '\0';
	
	// Init default date value, depending if undated events sort to list top or bottom.
	evenSort->dateVal = (pGedWiseFrame->IsSortUndEvenBot () ? 99999999 : 0);

	// Save EvenNoCnt to evenSort array.
	evenSort->evenNo = EvenNoCnt;

	// Convert Event Type tage into a number and copy it into evenRec->type field
	if (strstr ("ADDR,PHON", m_szTagType))	// if ADDR is event tag...
		evenTypeNo = ConvEvenTag ("RESI");	// then covert it to RESI tag
	else
		evenTypeNo = ConvEvenTag (m_szTagType);
	
	sprintf (evenTypeStr, "%u", evenTypeNo);
	
	// Copy the Event Type tag into evenRec->type field.
	strncat (evenRec->type, evenTypeStr, sizeof (evenRec->type) - 1);
	
	// If this is an Attribute type structure then copy description to
	// a NOTE if attribute description text is too big.
	if (indAttr) {

		GetFieldDesc (bldNote, NOTE_SZ+1, true); 
		
		if (strlen (bldNote) <= DESC_SZ) { // check if description too big
			strcpy (evenRec->desc, bldNote); // already size checked above
			*bldNote = '\0'; // erase attribute data from Note
			} // else keep attribute description in the Note field
		
		// note that m_bHandled must still be false at this point
		} 
	else { // not indAttr
		
		if (!strcmp (m_szTagType, "NOTE")) { // if event is NOTE then get note text here
			GetNoteRec (evenRec->noteNo, bldNote, false);
			// do not set m_bHandled = true
			}
		
		else if (strstr ("ADDR,PHON", m_szTagType)) { // handle when ADDR or PHON used
			ConsAddrField (evenRec->addr, ADDR_SZ);    // instead of RESI
			// do not set m_bHandled = true
			}

		else {
			m_bHandled = true; // for all other events tags m_bHandled = true
			}
		}

	wxASSERT (evenTypeNo > 0 && evenTypeNo <= EVEN_ARRAY_SZ);

	while (!m_bFileEnd) { // keep reading events until no more info on this one event.

		// Get the next record from the file.
		if (m_bHandled)
			if (!ReadNextRec()) break; // we are at end of file

		if (atoi (m_szLevelNo) <= startLevel) // then we are at next Indi record.
			break;

		if (!strcmp (m_szTagType, "TYPE")) {	
			//	Copy Event Description (only used for EVEN tags) //
			
			// If already data in m_szTagDesc, add descript at beginning (for IDNO tag)
			if (*evenRec->desc && *m_szTagDesc) {

				if (!attrHolder)
					attrHolder = new char[NOTE_SZ+1];
				
				strcpy (attrHolder, evenRec->desc); // already checked size
				
				*evenRec->desc = '\0'; // re-init
				
				GetFieldDesc (evenRec->desc, sizeof (evenRec->desc), true);
				
				strncat (evenRec->desc, ": ", DESC_SZ - strlen (evenRec->desc));
				strncat (evenRec->desc, attrHolder, DESC_SZ - strlen (evenRec->desc));
				}
			else {
				GetFieldDesc (evenRec->desc, sizeof (evenRec->desc));
				}
			// do not set m_bHandled = true
		
		} else if (!strcmp (m_szTagType, "DATE")) {
			// Record Date of Event //
			
			strncat (evenRec->date, CleanDateStr (m_szTagDesc), DATE_SZ);
			
			evenSort->dateVal = ConvertDateToNumber (evenRec->date, year);
			// don't use GetFieldDesc here due to next few lines!

			if (primRec && *year) {

				if (strstr (EVEN_BIR_STR, evenRec->type)) // BIRT,CHR,BAPM
					strcpy (m_szBirYrHld, year);

				if (strstr (EVEN_DTH_STR, evenRec->type)) // DEAT,BUR,CRE,PROB
					strcpy (m_szDthYrHld, year);
				}

			m_bHandled = true;

		} else if (!strcmp (m_szTagType, "PLAC")) {
			// Record Place of Event //
			ConsPlacField (evenRec->plac, PLAC_SZ);
			// do not set m_bHandled = true;

		} else if (strstr ("ADDR,PHON", m_szTagType)) {
			// Record Address of Event //
			ConsAddrField (evenRec->addr, ADDR_SZ);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "AGE")) {
			// Record Age at Event //
			GetFieldDesc (evenRec->age, sizeof(evenRec->age), true);
			// do not set m_bHandled = true;

		} else if (!strcmp (m_szTagType, "AGNC")) {
			// Record Agency responsible for Event //
			GetFieldDesc (evenRec->agnc, sizeof(evenRec->agnc), true);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "CAUS")) {
			// Record Cause of Event //
			GetFieldDesc (evenRec->caus, sizeof (evenRec->caus), true);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "NOTE")) { 
			// Record an Note for an Event //
			GetNoteRec (evenRec->noteNo, bldNote, false);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "SOUR")) {
			// Record a Citation for an Event //
			GetCiteRec (evenRec->citeNo);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "STAT")) {
			// Record LDS Status of Event  //
			GetFieldDesc (evenRec->statL, sizeof(evenRec->statL), true);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "TEMP")) {
			// Record LDS Temple Code for Event  //
			GetFieldDesc (evenRec->tempL, sizeof(evenRec->tempL), true);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "HUSB")) {
			// Record the Husb Age for Family Event Record	//
			
			if (!ReadNextRec ()) break;  // we are at end of file

			if (!strcmp (m_szTagType, "AGE")) {
				GetFieldDesc (evenRec->hAge, sizeof (evenRec->hAge), true);
				// do not set m_bHandled = true
				}

		} else if (!strcmp (m_szTagType, "WIFE")) {	
			// Record the Wife Age for Family Event Record	//
			
			if (!ReadNextRec ()) break;  // we are at end of file
			
			if (!strcmp (m_szTagType, "AGE")) {
				GetFieldDesc (evenRec->wAge, sizeof (evenRec->wAge), true);
				// do not set m_bHandled = true
				}

		} else if (!strcmp (m_szTagType, "RELI")) {	
			// Record the Religion for Event //
			GetFieldDesc (evenRec->reli, sizeof (evenRec->reli), true);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "RELA")) {	
			// Record the Relationship for Event //
			GetFieldDesc (evenRec->desc, sizeof (evenRec->desc), false);
			// do not set m_bHandled = true

		} else if (strstr ("OBJE,ADOP,FAMC,CHAN,QUAY", m_szTagType)) {
			// Ignore these tags except for FAMC. QUAY is non-standard tag //
			
			if (!strcmp (m_szTagType, "FAMC") && evenTypeNo == EVEN_ADO_TYP) {
				// Set Adoptive Family Number
				*m_szAdoptiveFamiNo = '\0';
				strncat (m_szAdoptiveFamiNo, CleanStr (m_szTagDesc), XREF_SZ);
				m_bHandled = true;
				}
			else
				SkipToStartLevel (false);
				// do not set m_bHandled = true;

		} else if (m_szTagType[0] == UD_TAG_CHAR) {
			// Genealogy Program Specific Erroneous Tags //
			// User Defined Tags starting with "_" = Family Tree Maker and others

			SkipToStartLevel (true);
			// do not set m_bHandled = true;

		} else if (atoi (m_szLevelNo) > startLevel) { // catch all that doesn't fit above
			BadDataErrorHandler ("Event");
			SkipToStartLevel (true);
			// do not set m_bHandled = true;
			}

		// Check if error occurred or if user hit cancel button.
		if (pGedWiseFrame->GetBadExit ())
			goto DoDeletes;

		} // end of main while loop.

	if (*bldNote) // make sure bldNote created, and then that it has data
		GetNoteRec(evenRec->noteNo, bldNote, true); // now write the note to note file

   // Write the record to file.
	if (fprintf(DBFiles[EvenDB]->fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
		evenRec->type,		evenRec->desc,	evenRec->date,	evenRec->plac, evenRec->addr,
		evenRec->age,		evenRec->agnc,	evenRec->caus,	evenRec->hAge,	evenRec->wAge,
		evenRec->reli,		evenRec->statL, evenRec->tempL,	evenRec->noteNo,
		evenRec->citeNo,	recNo) < 0)
      FileErrorHandler (EvenDB);

	/////////
	DoDeletes:
	/////////

	if (!primRec)		// only update m_iERecCnt for non-primary records
		m_iERecCnt++;  // update total events for current Individual.
	
	EvenNoCnt++;	// update total events for Event XRef counter.

	delete [] evenRec;
	evenRec = NULL;

	if (attrHolder) {
		delete [] attrHolder;
		attrHolder = NULL;
		}
	
	delete [] bldNote;
	bldNote = NULL;

	return;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:		GetIndiRec
// 
// Description:	For individuals with more than one name recorded, we assume the first
//						name is the primary name.  The Individual_Rec which has an "M" or "F"
//						indicates the record with the primary name, and this is also the last
//						of the alternate names written for each individual.
//						Because events are not sorted in the GEDCOM file, They must be
//						sorted here in the evenNo field.
//
// Parameters:		None.
// 
// Returns:			Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::GetIndiRec (void)
{
	INT			startLevel = atoi (m_szLevelNo); // retain original level number
	char		szIndiNoEvt[XREF_SZ+2]	= "\0";  // add on extra for starting "I";
	bool		gotTITL			= false;  // flags that record has a TITL tag
	bool		gotNAME			= false;  // flags that record has a NAME tag
	bool		gotBirParents	= false;  // flags that individual has birth parents
	INT			aliaCnt			= 0;	 // counts total alias names in record
	INT			lastAliaCnt		= 0;	 // alias number related to current NOTE
	UINT		primBirType		= 0;	 // holds type of birth record found
	UINT		primDthType		= 0;	 // holds type of death record found
	wxString	holdTitle;				 // holds individuals title
	EvenSrtRec	primBirSortRec = {0, 0}; // primary birth record info
	EvenSrtRec	primDthSortRec = {0, 0}; // primary death record info

	UINT		i;
	
	// Allocate these variables on the heap to prevent stack overflow.
	IndiRecType*	indRec;
	indRec			= new IndiRecType[ALIA_MAX+1];
	char*			bldNote = NULL; // holds NOTE text until writen to Note file
	EvenSrtRec*		evenSort;
	evenSort		= new EvenSrtRec[EVEN_MAX];

	// Initialize variables that may not get used. For alias field, NULL = primary
	// person, A = primary person has alias.
	*indRec[0].surNam	= *indRec[0].givNam	= *indRec[0].life	  = *indRec[0].alias		=
	*indRec[0].sex		= *indRec[0].indiNo	= *indRec[0].title  = *indRec[0].famCNo	=
	*indRec[0].famSNo	= *indRec[0].evenBNo	= *indRec[0].evenNo = *indRec[0].evenDNo	=
	*indRec[0].noteNo	= *indRec[0].citeNo	= *indRec[0].refn	  = *indRec[0].chilFlg	= '\0';

	*m_szAdoptiveFamiNo = *m_szBirYrHld = *m_szDthYrHld = '\0';

	m_iERecCnt = 0; // counts total number of events for each individual

	// Copy Individual XRef number.
	strncat (indRec[0].indiNo, CleanStr (m_szTagType), XREF_SZ);
	sprintf (szIndiNoEvt, "%c%s", CHR_IND, indRec[0].indiNo); // get EvenDB OrgNo field

	m_bHandled = true;

	while (!m_bFileEnd) { // continue this loop until startLevel reached.

		// Get the next record from the file. //
		if (m_bHandled)
			if (!ReadNextRec()) break; // we are at end of file

		if (atoi (m_szLevelNo) <= startLevel) // then we are at next record.
			break;

		if (strstr ("NAME,ALIA", m_szTagType)) {
			// Record an Individual's Name //
			// Some software (eg FTM) still uses ALIA tag for alias names.
			// This is ok if followed by an alias name and not xref tag.
			if (!strcmp (m_szTagType, "ALIA") && IsXRef (m_szTagDesc)) {
				SkipToStartLevel (false);
				// do not set m_bHandled = true;
				continue;
				}

			if (aliaCnt >= ALIA_MAX) {
				m_bHandled = true;
				continue;  // reached max aliases allowed
				}

			ParseName (indRec[aliaCnt]); // split name into first, last and title

			gotNAME = true;

			NameCnt++; // count total names in IndiDB
			aliaCnt++; // count total aliases for this individual

			*indRec[aliaCnt].noteNo = '\0'; // init this here - it may not be used
			*indRec[aliaCnt].citeNo = '\0'; // init this here - it may not be used
			m_bHandled = true;

		} else if (!strcmp (m_szTagType, "SEX")) {
			// Record the Sex of an Individual. //

			if (! *indRec[0].sex)
				strncat (indRec[0].sex, m_szTagDesc, sizeof (indRec[0].sex) - 1);
			m_bHandled = true;

		} else if (!strcmp (m_szTagType, "BIRT")) {
			// Record first birth event as a primary Event Record. //

			if (primBirType != EVEN_BIR_TYP) {

				if (primBirType != 0) { // then already recorded a CHR or BAPM
					evenSort[m_iERecCnt] = primBirSortRec; // so move it to evenSort
					m_iERecCnt++;
					}

				primBirType = EVEN_BIR_TYP;
				GetEventRec (false, &primBirSortRec, szIndiNoEvt, true);
				}
			else 
				GetEventRec (false, &evenSort[m_iERecCnt], szIndiNoEvt);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "DEAT")) {
			// Record first death event as a primary Event Record. //
		
			if (primDthType != EVEN_DTH_TYP) {
				
				if (primDthType != 0) { // then already recorded CREM, BURI or PROB
					evenSort[m_iERecCnt] = primDthSortRec; // so move it to evenSort
					m_iERecCnt++;
					}
				
				primDthType = EVEN_DTH_TYP;
				GetEventRec (false, &primDthSortRec, szIndiNoEvt, true);
				}
			else
				GetEventRec (false, &evenSort[m_iERecCnt], szIndiNoEvt);
			// do not set m_bHandled = true

		} else if (strstr ("CHR,BAPM", m_szTagType)) {
			// Record birth-related event as an Event Record. //

			if (primBirType == 0) { // then no tag entered yet
				primBirType = ConvEvenTag (m_szTagType);
				GetEventRec (false, &primBirSortRec, szIndiNoEvt, true);
				}
			else {
				GetEventRec (false, &evenSort[m_iERecCnt], szIndiNoEvt);
				}
			// do not set m_bHandled = true

		} else if (strstr ("BURI,CREM,PROB", m_szTagType)) {
			// Record death-related events as an Event Record. //
			// We want the DEAT as the primary death event. If there is no DEAT event
			// then use a BURI or CREM event as the primary death event. If
			// these tags don't exist then we will settle for a PROB event.
					
			bool primRec = false;

			if (primDthType != EVEN_DTH_TYP) {
	
				if (!strcmp (m_szTagType, "PROB") && (primDthType == 0)) { // PROB
					primRec = true; // only use PROB if nothing else
					}

				else if (primDthType == 0 || primDthType == EVEN_PRO_TYP) { // BURI or CREM
					
					if (primDthType == EVEN_PRO_TYP) { // always overwrite PROB tag.
						evenSort[m_iERecCnt] = primDthSortRec;
						m_iERecCnt++;
						}
					
					primRec = true;
					}
				}

			if (primRec) {
				primDthType = ConvEvenTag (m_szTagType);
				GetEventRec (false, &primDthSortRec, szIndiNoEvt, true);
				}
			else {
				GetEventRec (false, &evenSort[m_iERecCnt], szIndiNoEvt, primRec);
				}
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "FAMC")) {
			// Record the Family to which Individual was born. //
			
			bool areBirParents			= true;
			char holdFamiNo[XREF_SZ+1] = "\0";

			strncat (holdFamiNo, CleanStr (m_szTagDesc), XREF_SZ);
	
			if (*m_szAdoptiveFamiNo) // set if ADOP tag with FAMC XRef
				if (!strcmp (m_szTagDesc, m_szAdoptiveFamiNo))
					areBirParents = false;

			GetChilFamRec (indRec[0].indiNo, primBirSortRec.dateVal, &areBirParents);
			ChilCnt++;

			if (! *indRec[0].famCNo || !gotBirParents) // catch indiv has parents & adoptive parents
				strcpy (indRec[0].famCNo, holdFamiNo); // already size checked above
	
			if (areBirParents && !gotBirParents)
				gotBirParents = true;
			// do not set m_bHandled = true
			
		} else if (!strcmp (m_szTagType, "FAMS")) {
			// Record the Family to which Individual was a spouse. Can be multiple FAMS //
			// Note that FAMS will be handled with Family Records	
			
			if ((strlen (indRec[0].famSNo) + XREF_SZ) <= FREF_SZ) { // have we hit max families
				strcat (indRec[0].famSNo, "@");
				strcat (indRec[0].famSNo, CleanStr (m_szTagDesc));
				}
			SkipToStartLevel (false);
			// do not set m_bHandled = true
		
		} else if (!strcmp (m_szTagType, "REFN")) {
			// Record Reference Number tag. //
			
			//if (!pGedWiseFrame->m_bExclRefN && !*indRec[0].refn) {
			if (!pGedWiseFrame->IsExclRefN () && !*indRec[0].refn) {
				strncat (indRec[0].refn, m_szTagDesc, sizeof (indRec[0].refn) - 1);
				}
			SkipToStartLevel (false); // ignore any sub tags (eg TYPE)
			// do not set m_bHandled = true

		} else if (strstr ("NATU,EMIG,IMMI,CENS,WILL,GRAD,RETI,ASSO", m_szTagType)) {
			// Record an Event Record. //
			
			GetEventRec (false, &evenSort[m_iERecCnt], szIndiNoEvt);
			// do not set m_bHandled = true
		
		} else if (strstr ("ADOP,BARM,BASM,BLES,CHRA,CONF,FCOM,ORDN", m_szTagType)) {
			// Record an Event Record. //
			
			GetEventRec (false, &evenSort[m_iERecCnt], szIndiNoEvt);
			// do not set m_bHandled = true

		} else if (strstr ("CAST,DSCR,EDUC,IDNO,NATI,NCHI,NMR,OCCU,PROP,RELI,SSN,EVEN,RESI", m_szTagType)) {
			// Record an Individual Attribute as an Event Record. //
			GetEventRec (true, &evenSort[m_iERecCnt], szIndiNoEvt);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "TITL")) {
			// Record Individual's Title. //
			
			if (pGedWiseFrame->IsInslTitl () && !gotTITL && *m_szTagDesc) {
				holdTitle = m_szTagDesc;
				holdTitle.Trim (false);
				holdTitle.Trim (true);
				gotTITL = true;  // only accept one title per individual in title field
				}

			GetEventRec (true, &evenSort[m_iERecCnt], szIndiNoEvt);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType,"NOTE")) { 
			// Record a Note.  A Note can be associated with 1 of 3 items: 1) the primary  //
			// individual name; 2) an alias name or 3) a seperate Note record that will be //
			// treated as an Event Record.  We have to account for all three cases			 //
			
			if (pGedWiseFrame->IsExclNote ()) {
				SkipToStartLevel (false);
				}
			else {  // get the note
			
				if (atoi (m_szLevelNo) == startLevel + 2) {
					// level +2 note is related to respective NAME (primary or alias)

					if (lastAliaCnt < (aliaCnt - 1)) {
						// then we are on new NAME, so now write any current Note record	
						if (bldNote && *bldNote) {
							GetNoteRec (indRec[lastAliaCnt].noteNo, bldNote, true); // write Note to file
							*bldNote = '\0';  // clear the old note
							}
						lastAliaCnt = aliaCnt - 1; // set number of alias name with current note
						}

					if (!bldNote) {
						bldNote = new char[NOTE_SZ+1];
						*bldNote = '\0';
						}
					GetNoteRec (indRec[lastAliaCnt].noteNo, bldNote, false);
					}

				else  { 
					if (atoi(m_szLevelNo) == startLevel + 1)
					// must be event-type note if == startLevel + 1
						GetEventRec (false, &evenSort[m_iERecCnt], szIndiNoEvt);
					else  // must be bad data
						SkipToStartLevel (true);
					}
				}
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType,"SOUR")) {
			// Record a Citation for an Individual //
			
			if (atoi (m_szLevelNo) == startLevel + 2) // level +2 cites go with respective alias name
				GetCiteRec(indRec[(aliaCnt > 0) ? aliaCnt-1 : 0].citeNo);
			else if (atoi (m_szLevelNo) == startLevel + 1) // level +1 cites go with primary name
				GetCiteRec (indRec[0].citeNo);
			else // bad data
				SkipToStartLevel (true);
			// do not set m_bHandled = true

		} else if (strstr ("ADDR,PHON", m_szTagType)) {
			// Record non-standard tags used by Family Tree Maker, Legacy and others. //
			
			GetEventRec (false, &evenSort[m_iERecCnt], szIndiNoEvt);
			// do not set m_bHandled = true

		} else if (strstr ("BAPL,CONL,ENDL,SLGC", m_szTagType)) {
			// Record LDS tag as an Event Record. //
			
			if (pGedWiseFrame->IsExclOrdn ())
				SkipToStartLevel (false);
			else
				GetEventRec (false, &evenSort[m_iERecCnt], szIndiNoEvt);
			// do not set m_bHandled = true;

		} else if (strstr ("_HIST_MILI_MILT_MDCL_MEDC_HEAL_DEG_ELEC", m_szTagType)) {
			// Record a non-standard event tag used in Reunion and FTM and others. //
			
			if (m_szTagType[0] == UD_TAG_CHAR) {
				char strHld[TAGTYPE_SZ + 1];
				strcpy (strHld, &m_szTagType[1]);
				strcpy (m_szTagType, strHld); // remove initial "_" char
				}

			// convert HEAL and MILT tags to MDCL and MILI tags
			if (strstr ("HEAL,MEDC", m_szTagType))
				strcpy (m_szTagType, "MDCL");
			else if (!strcmp (m_szTagType,"MILT"))
				strcpy (m_szTagType, "MILI");
					
			GetEventRec (true, &evenSort[m_iERecCnt], szIndiNoEvt);
			// do not set m_bHandled = true

		} else if (strstr ("CHAN,OBJE,SUBM,RESN,NPFX,GIVN,NICK,SPFX,SURN,NSFX,QUAY", m_szTagType)) {
			// Ignore these tags. //
			
			SkipToStartLevel (false);
			// do not set m_bHandled = true

		//} else if (strstr ("ASSO,ANCI,DESI,RFN,AFN,RIN", m_szTagType)) {
		} else if (strstr ("ANCI,DESI,RFN,AFN,RIN", m_szTagType)) {
			// Ignore these tags. //
			
			SkipToStartLevel (false);
			// do not set m_bHandled = true

		} else if (m_szTagType[0] == UD_TAG_CHAR) {
			// Genealogy Program Specific Erroneous Tags
			// User Defined Tags starting with "_" = Family Tree Maker.
			
			SkipToStartLevel (true);
			// do not set m_bHandled = true	

		} else if (atoi (m_szLevelNo) > startLevel) {
			// Catch everything that doesn't fit above.
			
			BadDataErrorHandler ("Individual");
			SkipToStartLevel (true);
			// do not set m_bHandled = true
			}

		// Check if error occurred or if user hit cancel button.
		if (pGedWiseFrame->GetBadExit ())
			goto DoDeletes;
	
		}  // main while loop
	
	// Save the Note data.
	if (bldNote && *bldNote)
		GetNoteRec (indRec[lastAliaCnt].noteNo, bldNote, true);

	// Save the Primary Birth event record number.
	if (primBirType != 0)
		sprintf (indRec[0].evenBNo, "%u", primBirSortRec.evenNo);

	// Save the Primary Death event record number.
	if (primDthType != 0)
		sprintf (indRec[0].evenDNo, "%u", primDthSortRec.evenNo);

	// Sort the Event records.
	SortEventArray (evenSort, indRec[0].evenNo);

	// Insert title in title field if user has selected this option.
	if (gotTITL && !*indRec[0].title) // skip if already a title
		strncat (indRec[0].title, holdTitle.c_str(), TIT_SZ);

	// Copy birth and death years into record.
	sprintf (indRec[0].life, "%s/%s", m_szBirYrHld, m_szDthYrHld);

	// If indRec has no NAME tag (erroneous situation), make name "Unknown".
	if (!gotNAME) {
		strcpy (indRec[0].surNam, cUnknownStr);
		NameCnt++;
		}

	// Set the following to indicate the primary person has alias name(s)
	if (aliaCnt > 1)
		strcpy (indRec[0].alias, STR_ALI);

	// Write data to file for primary name.
	if (fprintf (DBFiles[IndiDB]->fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
		indRec[0].surNam,	indRec[0].givNam,	  indRec[0].life,		indRec[0].sex,
		indRec[0].alias,	indRec[0].indiNo,	  indRec[0].title,		indRec[0].famSNo,
		indRec[0].famCNo,	indRec[0].evenBNo,	  indRec[0].evenNo,		indRec[0].evenDNo,
		indRec[0].noteNo,	indRec[0].citeNo,	  indRec[0].refn,		indRec[0].chilFlg) < 0) {
		FileErrorHandler (IndiDB);
		goto DoDeletes;
		}

	// Write data to file for alias names.
	for (i = 1; i < (UINT) aliaCnt; i++) { // write the alias records.
		if (fprintf (DBFiles[IndiDB]->fp, "%s\t%s\t%s\t\t%02d\t%s\t\t%s\t\t\t\t\t%s\t%s\t\t\n",
			indRec[i].surNam,	indRec[i].givNam,	indRec[0].life, i, indRec[0].indiNo,
			indRec[i].title,	indRec[i].noteNo, indRec[i].citeNo) < 0) {
			FileErrorHandler (IndiDB);
			goto DoDeletes;
			}
		}

	/////////
	DoDeletes:
	/////////

	delete [] indRec;
	indRec = NULL;
	
	delete [] evenSort;
	evenSort = NULL;

	if (bldNote) {
		delete [] bldNote;
		bldNote = NULL;
		}

	return;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	RecLoop
//
// Description:	Main processing loop for GedRec class.  This routine handles all of
//						the level 0 tags in the GEDCOM file.				
//
// Parameters:	None.
//
// Returns:		Nothing
/////////////////////////////////////////////////////////////////////////////////////////
void ParseGedcom::RecLoop (void)
{
	m_bHandled = true;

	while (!m_bFileEnd) { // continue this loop until end of file.

		if (m_bFileEnd || pGedWiseFrame->Canceled ()) break;

		if (m_bHandled) // if not already at level 0 then get new record.
			if (!ReadNextRec ()) break;

		if (!strcmp (m_szLevelNo,"0")) {
			
			if (!strncmp (m_szTagDesc, "INDI", 4)) {
				IndiCnt++; // only count unique individuals, not names.
				GetIndiRec ();
				// do not set m_bHandled = true;

			} else if (!strncmp (m_szTagDesc, "FAM", 3)) {
				FamiCnt++;
				GetFamiRec ();
				// do not set m_bHandled = true;

			} else if (!strncmp (m_szTagDesc, "SOUR", 4)) {
				SourCnt++;
				GetSourRec ();
				// do not set m_bHandled = true;

			} else if (!strncmp (m_szTagDesc, "REPO", 4)) {
				RepoCnt++;
				GetRepoRec ();
				// do not set m_bHandled = true;

			} else if (!strncmp(m_szTagDesc,"NOTE", 4)) { 
				// Record an Note for an Event  //
				// PAF 3.0 starts the note right after the NOTE tag (eg 0 @T1270@ NOTE Text...)
				GetNoteRecX ();
				// do not set m_bHandled = true;

			} else if (!strncmp (m_szTagType, "SUB", 3)) {
				SkipToStartLevel (false);
				// do not set m_bHandled = true;

			} else if (!strncmp (m_szTagType, "HEAD", 4)) {
				SkipToStartLevel (false);
				// do not set m_bHandled = true;

			} else if (!strncmp (m_szTagType, "TRLR", 4)) {
				m_bFileEnd = true;
				break;
			
			} else if (strstr ("SUBM ,SUBN,OBJE", m_szTagDesc)) {
				SkipToStartLevel (false);
				// do not set m_bHandled = true;

			} else if (m_szTagType[0] == UD_TAG_CHAR) {
				// Genealogy Program Specific Erroneous Tags //
				// User Defined Tags starting with "_" = Family Tree Maker and others
				SkipToStartLevel (true);
				// do not set m_bHandled = true;

			} else {
				BadDataErrorHandler ("Level 0 Tag");
				SkipToStartLevel (true);
				// do not set m_bHandled = true;
				}

		} else {
			BadDataErrorHandler ("Record Loop");
			SkipToStartLevel (true);
			// do not set m_bHandled = true;
			}

		if (pGedWiseFrame->GetBadExit ())  // check if error occurred
			return; 

	}  // end of main while statement.
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GetGedComData
//
// Description:	Collects information from header of Gedcom file and writes it to a file.
//
// Parameters:	<- gedREc	 - structure to hold GEDCOM information retrieved.
//				-> writeData - argument is true if writing GEDCOM data to database.
//
// Returns:		false if error, else true if successful
/////////////////////////////////////////////////////////////////////////////////////////
bool ParseGedcom::GetGedComData (GedcRecType *gedRec, bool writeData)
{
	bool		gotDate = false;
	int			startLevel; // holds original level number.
	double		versNum = 0;
	wxString	msg;

	if (writeData)
		goto WriteData;

	ResetGedFile ();  // reset pointer in GEDCOM file.
	
	// Read first line of GEDCOM file.
	fscanf (GedFileP, "%s %s", m_szLevelNo, m_szTagType);
	
	if (!m_szLevelNo || !m_szTagType)
		goto ErrExit;

	//  Check if valid GEDCOM file...must start with "0 HEAD"  //
	if  (strcmp (m_szLevelNo, "0") || strcmp (m_szTagType, "HEAD")) {
		msg = "Error: The file you selected is not a valid GEDCOM file.    \n";
		wxMessageBox (msg, _THIS_FILE, wxOK | wxICON_ERROR);
		AddLogEntry (msg);
		goto ErrExit;
		}

	// Initialize fields in case they are not used.
	memset (gedRec, '\0' , sizeof(GedcRecType));
	
	strncat (gedRec->fileName, pGedWiseFrame->GetGedFileName (),
		sizeof (gedRec->fileName) - 1);

	ResetGedFile ();  // start at first line in file.
	ReadNextRec ();  // read the first line again to get start level no.

	startLevel = atoi (m_szLevelNo);  // holds original level number.

	m_bHandled = true;

	while (!m_bFileEnd) { // keep reading events until no more info on this one event.
		// Get the next record from the file. 
		if (m_bHandled)
			if (!ReadNextRec ()) break;

		if (atoi (m_szLevelNo) <= startLevel) // then we are at next record.
			break;

		if  (!strcmp (m_szTagType,"GEDC")) {
			// Record Gedcom version
			if (!ReadNextRec ()) break;
			if  (!strcmp (m_szTagType,"VERS"))  {
				GetFieldDesc (gedRec->ver, sizeof (gedRec->ver), true);
			   versNum = atof (gedRec->ver);
		      }

			msg.Printf ("GEDCOM Version:  %s\n", gedRec->ver);
			AddLogEntry (msg);
			// do not set m_bHandled = true;

		} else if (!strcmp (m_szTagType,"DEST")) {
			// Record Gedcom version
			if (strstr (m_szTagDesc,"5.5") || strstr (m_szTagDesc,"55")) {
				strcpy (gedRec->ver, "5.5");
				versNum = 5.5;
				}
			msg.Printf ("GEDCOM Dest: %s \n", m_szTagDesc);
			AddLogEntry (msg);
			m_bHandled = true;

		} else if (!strcmp (m_szTagType, "DATE")) {
			// Record Gedcom creation date.
			if (!gotDate) {
				GetFieldDesc (gedRec->date, sizeof (gedRec->date), true);
				StrToUpr (gedRec->date);
				gotDate = true;
				}
			else m_bHandled = true;
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "SOUR")) {
			// Write GEDCOM source information to log file.
			msg.Printf ("GEDCOM Source: %s ", m_szTagDesc);
			if (!ReadNextRec ()) break;
			if  (!strcmp (m_szTagType, "VERS")) {
				msg+= m_szTagDesc;
				m_bHandled = true;
				}
			msg += "\n";
			AddLogEntry (msg);
			// do not set m_bHandled = true

		} else if (!strcmp (m_szTagType, "CHAR")) {
			// Write GEDCOM character set information to log file.
			if (strstr (m_szTagDesc, "ANSEL")) {
				m_bCharAnsel = true;
				}
			msg.Printf ("GEDCOM Character Set: %s \n", m_szTagDesc);
			AddLogEntry (msg);
			m_bHandled = true;

		} else if (strstr ("VERS,SUBM,SUBN,GEDC,FORM,TIME,NOTE,NAME,CORP", m_szTagType)) {
			// Ignore these tags. //
			m_bHandled = true;

		} else if (strstr ("ADDR,ADR1,ADR2,CITY,STAE,POST,CTRY,PHON,DATA,COPR,LANG,CONT,CONC,FILE",
			m_szTagType)) {
			// Ignore these tags. //
			SkipToStartLevel (false);
			// do not set m_bHandled = true

		} else if (strstr ("_SCHEMA", m_szTagDesc) || m_szTagType[0] == UD_TAG_CHAR) {
			// Genealogy Program Specific Erroneous Tags.
			// User Defined Tags starting with "_" = Family Tree Maker (_SCHEMA) and others.
			SkipToStartLevel (false);
			// do not set m_bHandled = true

		} else if (atoi (m_szLevelNo) > startLevel) { // catch all that doesn't fit above.
			BadDataErrorHandler ("Header");
			SkipToStartLevel (true);
			// do not set m_bHandled = true
		}

		// Check if error occurred.
		if (pGedWiseFrame->GetBadExit ())
			return false; 

		}  // end of main while loop.

	if (!*gedRec->ver) {
	   strcpy (gedRec->ver, cUnknownStr);
	   versNum = 0;
		}
	
	// put log entry if we are decoding ANSEL character set
	if (m_bCharAnsel)
		AddLogEntry ("\n>> Decoding ANSEL"); // add spacer line
	AddLogEntry ("\n"); // add spacer line

	if (versNum < MIN_GED_VERS) {// must check version... do this last.
		wxString msg = IDS_ERR_VERSION;
		int badVerVal = wxMessageBox (msg, IDS_TTL_VERSION,
			wxYES_NO | wxICON_EXCLAMATION);
		AddLogEntry (msg);
		
		if (badVerVal != wxYES) {
			pGedWiseFrame->SetCanceled ();
			return true;
			}
		}
		
	return true; // success

	/////////
	ErrExit: 
	/////////

	pGedWiseFrame->SetBadExit ();
	CleanExit ();

	return false;

	/////////
	WriteData: 
	/////////
	
   // Get date and time that the database was created.
	wxDateTime ltime = wxDateTime::Now ();
	wxString today = ltime.Format ("%d %b %Y");
	today.MakeUpper ();
	gedRec->dateLoaded[0] = '\0';
	strncat (gedRec->dateLoaded, today.c_str(), DATE_SZ);

	if (fprintf (DBFiles[GRecDB]->fp, "%s\t%i\t%s\t%s\t%i\n", gedRec->date, DB_VERSION,
		gedRec->fileName, gedRec->dateLoaded, IndiCnt) < 0) 
		FileErrorHandler (GRecDB);
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	StartParsing
//
// Description:	Initiates the GEDCOM parsing.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
bool ParseGedcom::StartParsing (void)
{
	wxString	filePathName;

	// Open all database files and erase them if they already exist.
	for (int i = IndiDB; i <= GRecDB; i++) {
		
		if ((i == ChSoDB) && (!pGedWiseFrame->IsSortChilPerGed ()))
			continue;
		
		filePathName = pGedWiseFrame->GetWorkDirectory () + DBFiles[i]->name;

		if ((DBFiles[i]->fp = fopen (filePathName.c_str(), "wt")) == NULL) {
			pGedWiseFrame->SetBadExit ();
			CleanExit ();
			wxLogSysError (_("ParseGedcom::StartParsing \n\n" IDS_ERR_FOPEN), filePathName.c_str());
			return false;
			}
		}
	
	// Reset pointer in GEDCOM file.
	ResetGedFile ();

	m_bStartUpdateStats =  true;
	
	// Start processing GEDCOM file.
	RecLoop ();

	m_bStartUpdateStats =  false;

	// Check if error occurred or cancel button hit
	if (pGedWiseFrame->Canceled () || pGedWiseFrame->GetBadExit ()) {
		pGedWiseFrame->CheckStatus ();
		return false;
		}

	// Write the GEDCOM file information for Application Info Block
	GetGedComData (&GedFileRec, true);
	
	if (pGedWiseFrame->GetBadExit ()) // check if error occurred
		return false;

	// Close all files that are open.
	for (int j = IndiDB; j <= GRecDB; j++) {  
		if (DBFiles[j]->fp)
			fclose(DBFiles[j]->fp);
		DBFiles[j]->fp = NULL;
		}

	if (GedFileP)
		fclose (GedFileP);

	return true;
}
