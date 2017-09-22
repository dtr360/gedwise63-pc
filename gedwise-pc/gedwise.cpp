/////////////////////////////////////////////////////////////////////////////////////////
// Program:		GedWise 7.0
//				Copyright  2001 - 2005 Battery Park Software Corporation
//
// Module:		GedWise.cpp
//
// Written by:	Daniel T. Rencricca
//
// Date:		February 10, 2005
//
// Description:	This module creates the user interface frame and handles all user
//				user activity.  It also calls the functions to process the GEDCOM file.
//
/////////////////////////////////////////////////////////////////////////////////////////

#include "stdwx.h"
#include "gedwise.h"
#include "defines.h"
#include "miscfunctions.h"
#include "sortroutines.h"
#include "makepdb.h"
#include "dbcreate.h"
#include "parsegedcom.h"
#include "aboutgedwise.h"

#include "resources.h"

#ifdef __WXMSW__
	#include "InstAide.h"
#elif defined (__WXMAC__)
	#include "UserMgr.h"
	#include "CFString.h"
	#include "MachOCFMGlue.h"
#endif

// The application icon. All non-MSW platforms use an xpm. MSW uses an .ico file 
#if !defined (__WXMSW__) && !defined (__WXPM__) && !defined (__WXMAC__)
	#include "gedwiseicon.xpm"
#endif

#ifdef _DEBUG // include after last #include statement in every cpp file where 'new' is used
#define new DEBUG_NEW
#endif 

// Make sure #include "wx/msw/wx.rc" is incuded in the resource.h file.

// The only Resources should be under the Icon and Version folders

// ----------------------------------------------------------------------------
// Definitions
// ----------------------------------------------------------------------------

#define _THIS_FILE	"GedWise"

extern UINT			ErrCnt;
extern UINT			TOTLINES;
extern UINT			LineCnt;
extern UINT			IndiCnt;
extern UINT			NameCnt;
extern UINT			IndxCnt;
extern UINT			FamiCnt;
extern UINT			ChilCnt;
extern UINT			EvenNoCnt;
extern UINT			NoteCnt;
extern UINT			SourCnt;
extern UINT			RepoCnt;
extern UINT			RCitNoCnt;
extern UINT			SCitNoCnt;
extern FILE			*GedFileP;
extern DBFileRec	DBFiles[][DBCount];

UINT	CheckNo; // used to update progress bar
FILE*	LogfileP;


#if defined (__WXMSW__)

// Installation Aide (InstAide.dll) function pointer types
typedef int (WINAPI *PltGetPathPtr)(unsigned short sPathType, TCHAR* pPathBuffer,
									short* psPathBufSize);
typedef int (WINAPI *PltGetUserPtr)(unsigned int iIndex, TCHAR *pUserBuffer,
									short *psUserBufSize);
typedef int (WINAPI *PltGetUserDirectoryPtr)(TCHAR *pUser, TCHAR *pBuffer,
											 int *piBufferSize);
typedef int (WINAPI *PltGetUserCountPtr)(void);
typedef int (WINAPI *PltInstallFilePtr)(TCHAR *pUser, TCHAR *pFileSpec);
typedef int (WINAPI *PlmGetUserIDFromNamePtr)(TCHAR *pUser, DWORD *pdwID);


#elif defined (__WXMAC__)

// HotSync Library function pointer types
typedef OSStatus (*UmGetUserCountPtr) (UInt16* oUserCount);
typedef OSStatus (*UmGetUserNamePtr) (UInt16 iIndex, CFStringRef* oUserName);
typedef OSStatus (*UmGetFilesToInstallFolderSpecPtr) (PalmUserID userID, FSSpec* filesToInstallSpec);
typedef OSStatus (*UmGetIDFromNamePtr)  (CFStringRef iUserName, PalmUserID *oUserID);

#endif

// ----------------------------------------------------------------------------
// Resources
// ----------------------------------------------------------------------------

IMPLEMENT_APP (GedWiseApp)


IMPLEMENT_CLASS (GedWiseFrame, wxFrame)

// ----------------------------------------------------------------------------
// GedWise Frame Event Tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GedWiseFrame, wxFrame)

	EVT_BUTTON   	(wxID_EXIT,		GedWiseFrame::OnExit)
	EVT_BUTTON   	(wxID_ABOUT,	GedWiseFrame::OnAbout)
	EVT_BUTTON  	(ID_BTN_BUY,	GedWiseFrame::OnBtnBuy)
	EVT_BUTTON		(ID_BTN_CANCEL,	GedWiseFrame::OnBtnCancel)
	EVT_BUTTON  	(ID_BTN_DIR,	GedWiseFrame::OnBtnDirOpen)
	EVT_BUTTON  	(ID_BTN_FILE,	GedWiseFrame::OnBtnFileOpen)
	EVT_BUTTON	 	(ID_BTN_START,	GedWiseFrame::OnBtnStart)	
	EVT_BUTTON		(ID_BTN_WWW,	GedWiseFrame::OnBtnWWW)
	EVT_BUTTON		(wxID_HELP,		GedWiseFrame::OnHelp)
	EVT_CHECKBOX 	(ID_EX_NOTE,	GedWiseFrame::OnNoteCb)
	EVT_CHECKBOX 	(ID_EX_SOUR,	GedWiseFrame::OnSourCb)
	EVT_CHECKBOX 	(ID_EX_REPO,	GedWiseFrame::OnRepoCb)
	EVT_TEXT    	(ID_TXT_GFILE,	GedWiseFrame::OnChangeGedFileName)
	EVT_TEXT    	(ID_TXT_DIR,	GedWiseFrame::OnChangeDirName)
	EVT_TEXT    	(ID_TXT_DBNAME,	GedWiseFrame::OnChangeDbName)
	EVT_TEXT     	(ID_MAX_NOTE,	GedWiseFrame::OnChangeNoteSize)
	EVT_TIMER	 	(ID_TIMER,		GedWiseFrame::OnTimer)
	
    EVT_MENU		(wxID_ABOUT,	GedWiseFrame::OnAbout) // only for WXMAC
	EVT_MENU		(wxID_EXIT,		GedWiseFrame::OnExit)  // only for WXMAC
	EVT_MENU		(wxID_HELP,		GedWiseFrame::OnHelp)  // only for WXMAC

	EVT_PAINT		(GedWiseFrame::OnPaint)
	EVT_SIZE		(GedWiseFrame::OnSize)

	#if defined (__WXMSW__) || defined (__WXMAC__)
	EVT_BUTTON   (ID_BTN_PALM,	GedWiseFrame::OnBtnInst)
	EVT_COMBOBOX (ID_CB_HOTUSER,GedWiseFrame::OnSelHotUser)
	#endif
	
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution and also declares the
// accessor function wxGetApp() which will return the reference of the right
// type (i.e. GedWiseFrame and not wxApp)

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_CLASS (GedWiseApp, wxApp)

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GedWiseApp
//
// Description:	Constructor for GedWiseFrame.
//						
// Parameters:	None.
//
// Return:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
GedWiseApp::GedWiseApp (void)
{
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnInit
//
// Description:	The 'Main program' equivalent. Program execution "starts" here.  The 
//				main GedWise frame is created and set as the top window.
//						
// Parameters:	None.
//
// Return:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseApp::OnInit (void)
{   

	wxImage::AddHandler(new wxPNGHandler);

	wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);

	#ifndef __WXMSW__
	wxFileSystem::AddHandler(new wxZipFSHandler); // needed to view help file on Linux 
	#endif

	InitXmlResource(); 

	GedWiseFrame *pFrame = new GedWiseFrame (NULL);

	SetTopWindow (pFrame);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GedWiseFrame
//
// Description:	Constructor for GedWiseFrame.
//						
// Parameters:	None.
//
// Return:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
GedWiseFrame::GedWiseFrame (void)
{
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	~GedWiseFrame
//
// Description:	Destructor for GedWiseFrame.
//						
// Parameters:	None.
//
// Return:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
GedWiseFrame::~GedWiseFrame (void)
{
	SavePrefs ();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GedWiseFrame
//
// Description:	Constructor for GedWiseFrame.
//						
// Parameters:	-> parent	- parent window.
//				-> id		- id for frame.
//				-> caption	- caption for frame.
//				-> position - position of frame on screen.
//				-> size		- size of frame.
//
// Return:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
GedWiseFrame::GedWiseFrame (wxWindow* parent, wxWindowID id, const wxString& caption,
						   const wxPoint& pos, const wxSize& size, long style)
{
	Create ( parent, id, caption, pos, size, style );
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	Create
//
// Description:	All initizlizations for the GedWise Frame are done here.
//						
// Parameters:	None.
//
// Return:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::Create (wxWindow* parent, wxWindowID id, const wxString& caption,
						   const wxPoint& pos, const wxSize& size, long style )
{
	//  -- Initialize variables --
	m_bsMain1 = NULL;
    m_bsMain2 = NULL;
    m_bsMain3 = NULL;
    m_bxGedFileName = NULL;
    m_txGedFileName = NULL;
    m_btnFile = NULL;
    m_bxDbName = NULL;
    m_txDbNameLb = NULL;
    m_txDbName = NULL;
    m_btnInst = NULL;
    m_bxHotUser = NULL;
    m_bxOpts = NULL;
	m_cbHotUser = NULL;
	m_txDbDir = NULL;
	m_btnDir = NULL;
    m_PanelOpts = NULL;
    m_fgOpts1 = NULL;
    m_chExSour = NULL;
    m_chExRepo = NULL;
    m_chExRefN = NULL;
    m_chExOrd = NULL;
    m_chExNotes = NULL;
    m_fgOpts2 = NULL;
    m_chMaxNote = NULL;
    m_chMaxNoteLb = NULL;
    m_chInsTitle = NULL;
    m_chInsSpace = NULL;
    m_chSortChil = NULL;
    m_chSortEven = NULL;
    m_bsMain4 = NULL;
    m_bxGedDet = NULL;
    m_gfGedDet = NULL;
    m_txGedVersLb = NULL;
    m_txGedVers = NULL;
    m_txGedDateLb = NULL;
    m_txGedDate = NULL;
    m_txGedSourLb = NULL;
    m_txGedSour = NULL;
    m_txGedLineLb = NULL;
    m_txGedLine = NULL;
    m_bxPrcDet = NULL;
    m_gfPrcDet = NULL;
    m_txTotLineLb = NULL;
    m_txTotLine = NULL;
    m_txTotFamiLb = NULL;
    m_txTotFami = NULL;
    m_txTotPrcLb = NULL;
    m_txTotPrct = NULL;
    m_txTotEvenLb = NULL;
    m_txTotEven = NULL;
    m_txTotWarnLb = NULL;
    m_txTotWarn = NULL;
    m_txTotNoteLb = NULL;
    m_txTotNote = NULL;
    m_txTotPeopLb = NULL;
    m_txTotPeop = NULL;
    m_txTotSourLb = NULL;
    m_txTotSour = NULL;
    m_txTotNameLb = NULL;
    m_txTotName = NULL;
    m_txTotRepoLb = NULL;
    m_txTotRepo = NULL;
    m_txPrcStat = NULL;
    m_txTime = NULL;
    m_bsProg = NULL;
    m_ProgressBar = NULL; // MUST do this before calling CreateControls()
    m_txPercent = NULL;
    m_bsMain5 = NULL;
    m_bsButtons1 = NULL;
    m_btnStart = NULL;
    m_btnCancel = NULL;
    m_btnExit = NULL;
    m_bsButtons2 = NULL;
    m_btnWWW = NULL;
    m_btnHelp = NULL;
    m_btnInfo = NULL;
    m_btnBuy = NULL;
    m_StatusBar = NULL;

	m_iPalmUserNo		= 0;  // must be initialized to 0!!
	m_bBadExit			= false;
	m_iUserNoteSize		= NOTE_SZ;
	m_bHotUserFound		= false;
	m_strWindowsVersion = wxGetOsDescription();


	// Create the main application window.
	wxFrame::Create (parent, id, caption, pos, size, style );
	
	
	#if defined (__WXMSW__)
	
	this->SetBackgroundColour (wxColour(DLG_BACK_COLOR));
	
	#endif


	#if !defined (__WXMAC__)
	
	SetIcon (wxIcon (wxICON(GedWiseIcon)));
	
	#endif


	// -- Get the working directory.--
	wxFileName	filePathName = wxGetCwd();
	

	#if defined (__WXMSW__)	|| defined (__WXMAC__)
	
	m_strWorkDir = wxFindAppPath (wxTheApp->argv[0], filePathName.GetLongPath (),
		INSTALL_DIR);
	
	#else // Linux

	m_strWorkDir.Printf ("%c%s%c%s%c%s", wxFILE_SEP_PATH, LNX_USER_DIR, 
		wxFILE_SEP_PATH, LNX_SHARE_DIR, wxFILE_SEP_PATH, INSTALL_DIR);
			
	// make the directory if it is not there...should never happen.
	if (! wxDirExists (m_strWorkDir))
		 wxMkdir (m_strWorkDir);

	#endif


	// Add final directory seperator.
	if (m_strWorkDir.Last() != wxFILE_SEP_PATH)
		m_strWorkDir += wxFILE_SEP_PATH;
	
	
	// -- Get Preference File location --
	m_strPrefFileName = m_strWorkDir + PREFFILE;


	// -- Get the Help File location for OSX & Linux --
	#if !defined (__WXMSW__)
	
	// Create a wxWidgets HTML Help controller
	m_help = new wxHtmlHelpController();

	// Initialize the help controller.
	wxFileName helpFilePathName (m_strWorkDir);


	#if defined (__WXMAC__)
	
	// OSX uses a bundle that has the help file under 'Resources' directory
	helpFilePathName.RemoveLastDir ();
	helpFilePathName.AppendDir (MAC_RESOURCES_DIR);

	#endif // defined (__WXMAC__)
	

	helpFilePathName.SetName (MANUAL_FILE_UNI);
		
    if (! m_help->AddBook(helpFilePathName, true))
	  	wxLogSysError( _("Failed adding GedWise help book: '%s'"), 
		helpFilePathName.GetLongPath().c_str());
	
	#endif // !defined (__WXMSW__)


	// -- Get Log File location --
	#if defined (__WXMAC__)

	wxFileName logFilePathName (m_strWorkDir);

	logFilePathName.RemoveLastDir (); // remove 'MacOS'
	logFilePathName.RemoveLastDir (); // remove 'Contents' 
	logFilePathName.RemoveLastDir (); // remove 'GedWise.app'
	logFilePathName.SetName (LOGFILE);
	
	m_strLogFileName = logFilePathName.GetFullPath ();
	
	#else // Windows or Linux
	
	m_strLogFileName = m_strWorkDir + LOGFILE;

	#endif // defined (__WXMAC__)


	// -- Open Log File.  Erase old log file if it is over certain size. --
	char openType[] = "at";
	
	if (wxFileExists (m_strLogFileName)) {

		wxFile fLog (m_strLogFileName, wxFile::read);

		if (!fLog.IsOpened() || fLog.Length () > MAX_LOGFILE_SZ)  // clear log file if too large
			strcpy (openType, "wt");

		fLog.Close ();
		}

	if ((LogfileP = fopen (m_strLogFileName.c_str (), openType)) == NULL)
		wxLogError (_("GedWiseFrame::Create::\n\n" IDS_ERR_FOPEN), m_strLogFileName.c_str ());
	

	// -- Get Temporary Files location on Mac OSX. Use as working directory. --	
	#if defined (__WXMAC__)

	OSErr		errCode = noErr;
	FSRef		foundRef;
	char		dstPathFileName[PATH_MAX]; // 1024 bytes
	
	errCode = FSFindFolder (kOnSystemDisk, kTemporaryFolderType, kCreateFolder, &foundRef);

	if (!errCode) {
		
		FSRefMakePath (&foundRef, (UInt8 *) &dstPathFileName, PATH_MAX);
	
		// set working directory and add the file seperator at end
		m_strWorkDir.Printf ("%s%c", dstPathFileName, wxFILE_SEP_PATH);
		}

	#endif // defined (__WXMAC__)


	// -- Initialize the Main GedWise Frame --
	CreateControls ();

	GetSizer()->Fit(this);
    //GetSizer()->SetSizeHints(this);

	Centre (); // center the frame

	this->Show (true); // show the frame


	// -- Set Min and Max size for window --
	int w, h;
	GetSize (&w, &h);
	SetSizeHints (wxSize (w, h), wxSize (w+300, h+25), wxDefaultSize); 


	// -- Get saved user preferences --
	GetPrefs(); 

	
	// -- Load Hotsync Libraries and Registered Users
	m_bHotUserFound = LoadPalmUsers ();


	// -- Remove HotSync User ComboBox and make a Textbox with directory selector. --
	if (!m_bHotUserFound) {

		//m_btnInst->Enable (false); // hide the palm installer button.
		m_cbHotUser->Destroy(); // remove HotSync User Selector combobox
		m_cbHotUser = NULL;     // re-init

		//// Add Database Location Definitions
		m_bxHotUser->SetLabel (_T("Location to Save Database"));
	
		m_txDbDir = new wxTextCtrl (this, ID_TXT_DIR, _T(""), wxDefaultPosition, wxDefaultSize, 0);
		m_txDbDir->SetToolTip (_T(IDS_DIR_LOC));
		DbDirBoxSizer->Add(m_txDbDir, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

		m_btnDir = new wxBitmapButton (this, ID_BTN_DIR, wxXmlResource::Get()->LoadBitmap("FileNorm"), wxDefaultPosition, wxSize(36, 36), wxNO_BORDER);
		#if defined (__WXMSW__)
		m_btnDir->SetBitmapSelected (wxXmlResource::Get()->LoadBitmap("FileSelected"));
		#endif
		m_btnDir->SetBitmapDisabled (CreateBitmapDisabled (wxXmlResource::Get()->LoadBitmap("FileNorm")));
		m_btnDir->SetToolTip (_T(IDS_DIR));
		DbDirBoxSizer->Add(m_btnDir, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

		GetSizer()->Fit(this);// call this again to fit m_txDbDir or else insertion point problem

		m_txDbDir->SetValue (m_strPalmAppDir);
		m_txDbDir->SetInsertionPointEnd ();

		// Remove the palm installer button. Do this last or else problem with m_txDir field.
		m_btnInst->Destroy ();	   
		m_btnInst  = NULL;
		}

	// -- Misc -- 
	m_txGedFileName->SetInsertionPointEnd ();
	m_txGedFileName->SetFocus ();

	// Success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run.
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	CreateControls
//
// Description:	Creates/Draws all the controls in the GedWise Frame.
//						
// Parameters:	None.
//
// Return:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::CreateControls ()
{   
	GedWiseFrame* itemDialog1 = this;
	
	m_bsMain1 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(m_bsMain1);

    m_bsMain2 = new wxBoxSizer(wxHORIZONTAL);
    m_bsMain1->Add(m_bsMain2, 0, wxGROW|wxALL, 5);

    m_bsMain3 = new wxBoxSizer(wxVERTICAL);
    m_bsMain2->Add(m_bsMain3, 1, wxALIGN_BOTTOM|wxALL, 5);

    m_bxGedFileName = new wxStaticBox(itemDialog1, wxID_ANY, _T("Source GEDCOM File"));
    wxStaticBoxSizer* itemStaticBoxSizer5 = new wxStaticBoxSizer(m_bxGedFileName, wxHORIZONTAL);
    m_bsMain3->Add(itemStaticBoxSizer5, 0, wxGROW|wxALL, 5);

    m_txGedFileName = new wxTextCtrl( itemDialog1, ID_TXT_GFILE, _T(" "), wxDefaultPosition, wxSize(249, -1), 0 );
    m_txGedFileName->SetToolTip(_T(IDS_GFILE));
    itemStaticBoxSizer5->Add(m_txGedFileName, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

 	m_btnFile = new wxBitmapButton (this, ID_BTN_FILE, wxXmlResource::Get()->LoadBitmap("FileNorm"), wxDefaultPosition, wxSize(36, 36), wxNO_BORDER);
    #if defined (__WXMSW__)
	m_btnFile->SetBitmapSelected (wxXmlResource::Get()->LoadBitmap("FileSelected"));
	#endif
	m_btnFile->SetBitmapDisabled (CreateBitmapDisabled (wxXmlResource::Get()->LoadBitmap("FileNorm")));
	m_btnFile->SetToolTip (wxT(IDS_FILE));
    itemStaticBoxSizer5->Add(m_btnFile, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_bxDbName = new wxStaticBox(itemDialog1, wxID_ANY, _T("Database Export File"));
    wxStaticBoxSizer* itemStaticBoxSizer8 = new wxStaticBoxSizer(m_bxDbName, wxHORIZONTAL);
    m_bsMain3->Add(itemStaticBoxSizer8, 0, wxGROW|wxALL, 5);

    m_txDbNameLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("Enter Database Name (up to 6 chars):"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer8->Add(m_txDbNameLb, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txDbName = new wxTextCtrl( itemDialog1, ID_TXT_DBNAME, _T(""), wxDefaultPosition, wxSize(75, -1), 0 );
    m_txDbName->SetToolTip(_T(IDS_DBNAME));
    itemStaticBoxSizer8->Add(m_txDbName, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_btnInst = new wxBitmapButton (this, ID_BTN_PALM, wxXmlResource::Get()->LoadBitmap("PalmNorm"), wxDefaultPosition, wxSize(36, 36), wxNO_BORDER);
    #if defined (__WXMSW__)
	m_btnInst->SetBitmapSelected (wxXmlResource::Get()->LoadBitmap("PalmSelected"));
	#endif
	m_btnInst->SetBitmapDisabled (CreateBitmapDisabled (wxXmlResource::Get()->LoadBitmap("PalmNorm")));
	m_btnInst->SetToolTip (wxT(IDS_INST));
    itemStaticBoxSizer8->Add(m_btnInst, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);
	

	//// Hotsync User Definitions
	m_bxHotUser = new wxStaticBox (itemDialog1, wxID_ANY, _T("Hotsync User"));
    wxStaticBoxSizer* itemStaticBoxSizer12 = new wxStaticBoxSizer(m_bxHotUser, wxHORIZONTAL);
    m_bsMain3->Add(itemStaticBoxSizer12, 0, wxGROW|wxALL, 5);
	
    m_cbHotUser = new wxComboBox ( itemDialog1, ID_CB_HOTUSER, _T(""), wxDefaultPosition, wxSize(280, -1), 0, m_aHotUsers, wxCB_DROPDOWN | wxCB_READONLY | wxCB_SORT);
    m_cbHotUser->SetToolTip (_T(IDS_HOTUSER));
    itemStaticBoxSizer12->Add(m_cbHotUser, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    
	 //// Option Panel Definitions
	m_bxOpts = new wxStaticBox(itemDialog1, wxID_ANY, _T("Processing Options"));
    wxStaticBoxSizer* itemStaticBoxSizer14 = new wxStaticBoxSizer(m_bxOpts, wxVERTICAL);
    m_bsMain3->Add(itemStaticBoxSizer14, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    m_PanelOpts = new wxPanel( itemDialog1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemStaticBoxSizer14->Add(m_PanelOpts, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxVERTICAL);
    m_PanelOpts->SetSizer(itemBoxSizer16);

    m_fgOpts1 = new wxFlexGridSizer(3, 2, 10, 5);
    m_fgOpts1->AddGrowableRow(0);
    m_fgOpts1->AddGrowableCol(1);
    m_fgOpts1->AddGrowableCol(2);
    itemBoxSizer16->Add(m_fgOpts1, 1, wxGROW|wxALL, 5);

    m_chExSour = new wxCheckBox( m_PanelOpts, ID_EX_SOUR, _T("Exclude Source Data."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chExSour->SetValue(FALSE);
    m_chExSour->SetToolTip(_T(IDS_SOUR));
    m_fgOpts1->Add(m_chExSour, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_chExRepo = new wxCheckBox( m_PanelOpts, ID_EX_REPO, _T("Exclude Repository data."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chExRepo->SetValue(FALSE);
    m_chExRepo->SetToolTip(_T(IDS_REPO));
    m_fgOpts1->Add(m_chExRepo, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_chExRefN = new wxCheckBox( m_PanelOpts, ID_EX_REFN, _T("Exclude REFN tags."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chExRefN->SetValue(FALSE);
    m_chExRefN->SetToolTip(_T(IDS_REFN));
    m_fgOpts1->Add(m_chExRefN, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_chExOrd = new wxCheckBox( m_PanelOpts, wxID_ANY, _T("Exclude LDS Ordinance tags."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chExOrd->SetValue(FALSE);
	m_chExOrd->SetToolTip(_T(IDS_ORD));
    m_fgOpts1->Add(m_chExOrd, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_chExNotes = new wxCheckBox( m_PanelOpts, ID_EX_NOTE, _T("Exclude Notes."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chExNotes->SetValue(FALSE);
	m_chExNotes->SetToolTip(_T(IDS_NOTE));
    m_fgOpts1->Add(m_chExNotes, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_fgOpts2 = new wxFlexGridSizer(1, 2, 0, 0);
    m_fgOpts2->AddGrowableRow(0);
    m_fgOpts2->AddGrowableCol(0);
    m_fgOpts1->Add(m_fgOpts2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_chMaxNote = new wxTextCtrl( m_PanelOpts, ID_MAX_NOTE, _T(""), wxDefaultPosition, wxSize(50, 18), 0);
    m_chMaxNote->SetToolTip(_T(IDS_MAX_NOTE));
    m_fgOpts2->Add(m_chMaxNote, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);

    m_chMaxNoteLb = new wxStaticText( m_PanelOpts, wxID_STATIC, _T("Max. characters per Note."), wxDefaultPosition, wxDefaultSize, 0 );
    m_fgOpts2->Add(m_chMaxNoteLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);

    m_chInsTitle = new wxCheckBox( m_PanelOpts, wxID_ANY, _T("Insert TITL (Title) in an individual's name."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chInsTitle->SetValue(FALSE);
    m_chInsTitle->SetToolTip(_T(IDS_INS_TITLE));
    itemBoxSizer16->Add(m_chInsTitle, 0, wxALIGN_LEFT|wxALL, 5);

    m_chInsSpace = new wxCheckBox( m_PanelOpts, wxID_ANY, _T("Insert space character before CONC line."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chInsSpace->SetValue(FALSE);
    m_chInsSpace->SetToolTip(_T(IDS_INS_SPACE));
    itemBoxSizer16->Add(m_chInsSpace, 0, wxALIGN_LEFT|wxALL, 5);

    m_chSortChil = new wxCheckBox( m_PanelOpts, wxID_ANY, _T("List children as per order in GEDCOM file."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chSortChil->SetValue(FALSE);
    m_chSortChil->SetToolTip(_T(IDS_INS_SPACE));
    itemBoxSizer16->Add(m_chSortChil, 0, wxALIGN_LEFT|wxALL, 5);

    m_chSortEven = new wxCheckBox( m_PanelOpts, wxID_ANY, _T("Sort undated Events to bottom of event list."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chSortEven->SetValue(FALSE);
    m_chSortEven->SetToolTip(_T(IDS_SORT_EVEN));
    itemBoxSizer16->Add(m_chSortEven, 0, wxALIGN_LEFT|wxALL, 5);

    
	//// GEDCOM File Details Box and Spacer for GedWise Logo Bitmap
	m_bsMain4 = new wxBoxSizer(wxVERTICAL);
    m_bsMain2->Add(m_bsMain4, 1, wxALIGN_BOTTOM|wxALL, 5);

	m_bsMain4->Add(275, 72, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 8);  // spacer to GedWise Logo


    m_bxGedDet = new wxStaticBox(itemDialog1, wxID_ANY, _T("GEDCOM File Details"));
    wxStaticBoxSizer* itemStaticBoxSizer32 = new wxStaticBoxSizer(m_bxGedDet, wxHORIZONTAL);
    m_bsMain4->Add(itemStaticBoxSizer32, 0, wxGROW|wxALL, 5);

	m_gfGedDet = new wxFlexGridSizer(4, 2, 10, 15);
    m_gfGedDet->AddGrowableCol(1);
    itemStaticBoxSizer32->Add(m_gfGedDet, 0, wxALIGN_TOP|wxALL, 5);

    m_txGedVersLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("GEDCOM Version:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfGedDet->Add(m_txGedVersLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txGedVers = new wxStaticText( itemDialog1, wxID_STATIC, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfGedDet->Add(m_txGedVers, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txGedDateLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("Date Created:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfGedDet->Add(m_txGedDateLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txGedDate = new wxStaticText( itemDialog1, wxID_STATIC, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfGedDet->Add(m_txGedDate, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txGedSourLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("Data Source:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfGedDet->Add(m_txGedSourLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txGedSour = new wxStaticText( itemDialog1, wxID_STATIC, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfGedDet->Add(m_txGedSour, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txGedLineLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("Lines in File:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfGedDet->Add(m_txGedLineLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txGedLine = new wxStaticText( itemDialog1, wxID_STATIC, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfGedDet->Add(m_txGedLine, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    

	//// File Processing Details Definitions
	m_bxPrcDet = new wxStaticBox(itemDialog1, wxID_ANY, _T("File Processing Details"));
    wxStaticBoxSizer* itemStaticBoxSizer42 = new wxStaticBoxSizer(m_bxPrcDet, wxVERTICAL);
    m_bsMain4->Add(itemStaticBoxSizer42, 1, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    m_gfPrcDet = new wxFlexGridSizer(6, 4, 3, 15);
    m_gfPrcDet->AddGrowableRow(0);
    m_gfPrcDet->AddGrowableCol(1);
    m_gfPrcDet->AddGrowableCol(3);
    itemStaticBoxSizer42->Add(m_gfPrcDet, 0, wxGROW|wxALL, 5);

    m_txTotLineLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("Total Lines"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfPrcDet->Add(m_txTotLineLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txTotLine = new wxTextCtrl( itemDialog1, wxID_ANY, _T(""), wxDefaultPosition, wxSize(60, 18), wxTE_READONLY|wxTE_RIGHT );
    m_gfPrcDet->Add(m_txTotLine, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 0);

    m_txTotFamiLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("Families"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfPrcDet->Add(m_txTotFamiLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txTotFami = new wxTextCtrl( itemDialog1, wxID_ANY, _T(""), wxDefaultPosition, wxSize(60, 18), wxTE_READONLY|wxTE_RIGHT );
    m_gfPrcDet->Add(m_txTotFami, 1, wxGROW|wxALIGN_CENTER_VERTICAL, 0);

    m_txTotPrcLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("Processed"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfPrcDet->Add(m_txTotPrcLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txTotPrct = new wxTextCtrl( itemDialog1, wxID_ANY, _T(""), wxDefaultPosition, wxSize(60, 18), wxTE_READONLY|wxTE_RIGHT );
    m_gfPrcDet->Add(m_txTotPrct, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 0);

    m_txTotEvenLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("Events"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfPrcDet->Add(m_txTotEvenLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txTotEven = new wxTextCtrl( itemDialog1, wxID_ANY, _T(""), wxDefaultPosition, wxSize(60, 18), wxTE_READONLY|wxTE_RIGHT );
    m_gfPrcDet->Add(m_txTotEven, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 0);

    m_txTotWarnLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("Warnings"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfPrcDet->Add(m_txTotWarnLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txTotWarn = new wxTextCtrl( itemDialog1, wxID_ANY, _T(""), wxDefaultPosition, wxSize(60, 18), wxTE_READONLY|wxTE_RIGHT );
    m_gfPrcDet->Add(m_txTotWarn, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 0);

    m_txTotNoteLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("Notes"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfPrcDet->Add(m_txTotNoteLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txTotNote = new wxTextCtrl( itemDialog1, wxID_ANY, _T(""), wxDefaultPosition, wxSize(60, 18), wxTE_READONLY|wxTE_RIGHT );
    m_gfPrcDet->Add(m_txTotNote, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 0);

    m_txTotPeopLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("People"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfPrcDet->Add(m_txTotPeopLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txTotPeop = new wxTextCtrl( itemDialog1, wxID_ANY, _T(""), wxDefaultPosition, wxSize(60, 18), wxTE_READONLY|wxTE_RIGHT );
    m_gfPrcDet->Add(m_txTotPeop, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 0);

    m_txTotSourLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("Sources"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfPrcDet->Add(m_txTotSourLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txTotSour = new wxTextCtrl( itemDialog1, wxID_ANY, _T(""), wxDefaultPosition, wxSize(60, 18), wxTE_READONLY|wxTE_RIGHT );
    m_gfPrcDet->Add(m_txTotSour, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 0);

    m_txTotNameLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("Names"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfPrcDet->Add(m_txTotNameLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txTotName = new wxTextCtrl( itemDialog1, wxID_ANY, _T(""), wxDefaultPosition, wxSize(60, 18), wxTE_READONLY|wxTE_RIGHT );
    m_gfPrcDet->Add(m_txTotName, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 0);

    m_txTotRepoLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("Repositories"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gfPrcDet->Add(m_txTotRepoLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_txTotRepo = new wxTextCtrl( itemDialog1, wxID_ANY, _T(""), wxDefaultPosition, wxSize(60, 18), wxTE_READONLY|wxTE_RIGHT );
    m_gfPrcDet->Add(m_txTotRepo, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 0);


	//// Gauge and Percent Completed Data
	wxBoxSizer* itemBoxSizer64 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer42->Add(itemBoxSizer64, 0, wxGROW, 0);

    m_txPrcStat = new wxStaticText( itemDialog1, wxID_STATIC, _T(""), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    itemBoxSizer64->Add(m_txPrcStat, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txTime = new wxStaticText( itemDialog1, wxID_STATIC, _T("00:00"), wxDefaultPosition, wxSize(40, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
    itemBoxSizer64->Add(m_txTime, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxFIXED_MINSIZE, 5);

    m_bsProg = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer42->Add(m_bsProg, 0, wxGROW, 0);

    wxBoxSizer* itemBoxSizer68 = new wxBoxSizer(wxHORIZONTAL);
    m_bsProg->Add(itemBoxSizer68, 1, wxALIGN_CENTER_VERTICAL, 0);

    m_ProgressBar = new wxGauge( itemDialog1, wxID_ANY, 100, wxDefaultPosition, wxSize(-1, 18), wxGA_HORIZONTAL );
    //m_ProgressBar->SetValue(1);
    itemBoxSizer68->Add(m_ProgressBar, 1, wxALIGN_CENTER_VERTICAL, 5);

	itemBoxSizer68->Add(1, 18, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_txPercent = new wxStaticText( itemDialog1, wxID_STATIC, _T(""), wxDefaultPosition, wxSize(40, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
    m_bsProg->Add(m_txPercent, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxFIXED_MINSIZE, 5);


	//// Bottom Button Definitions
	m_bsMain5 = new wxBoxSizer(wxHORIZONTAL);
    m_bsMain1->Add(m_bsMain5, 1, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_bsButtons1 = new wxBoxSizer(wxHORIZONTAL);
    m_bsMain5->Add(m_bsButtons1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 10);

    m_btnStart = new wxButton( itemDialog1, ID_BTN_START, _T("Start"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnStart->SetToolTip(_T(IDS_START));
    m_bsButtons1->Add(m_btnStart, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 10);

    m_btnCancel = new wxButton( itemDialog1, ID_BTN_CANCEL, _T("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnCancel->SetToolTip(_T(IDS_CANCEL));
    m_bsButtons1->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 10);

    m_btnExit = new wxButton( itemDialog1, wxID_EXIT, _T("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnExit->SetToolTip(_T(IDS_EXIT));
    m_bsButtons1->Add(m_btnExit, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 10);

    m_bsMain5->Add(0, 39, 1, wxALIGN_BOTTOM, 0);

	m_bsButtons2 = new wxBoxSizer(wxHORIZONTAL);
    m_bsMain5->Add(m_bsButtons2, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 10);

	m_btnWWW = new wxBitmapButton (itemDialog1, ID_BTN_WWW, wxXmlResource::Get()->LoadBitmap("WWWNorm"), wxDefaultPosition, wxSize(36, 36), wxNO_BORDER);
	#if defined (__WXMSW__)
	m_btnWWW->SetBitmapSelected (wxXmlResource::Get()->LoadBitmap("WWWSelected"));
	#endif
    m_btnWWW->SetBitmapDisabled (CreateBitmapDisabled (wxXmlResource::Get()->LoadBitmap("WWWNorm")));
	m_btnWWW->SetToolTip(wxT (IDS_WWW));
	m_bsButtons2->Add (m_btnWWW, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 12);
	
	m_btnHelp = new wxBitmapButton (itemDialog1, wxID_HELP, wxXmlResource::Get()->LoadBitmap("HelpNorm"),	wxDefaultPosition, wxSize(36, 36), wxNO_BORDER);
	#if defined (__WXMSW__)
	m_btnHelp->SetBitmapSelected (wxXmlResource::Get()->LoadBitmap("HelpSelected"));
	#endif
	m_btnHelp->SetBitmapDisabled (CreateBitmapDisabled (wxXmlResource::Get()->LoadBitmap("HelpNorm")));
	m_btnHelp->SetToolTip (wxT(IDS_HELP));
    m_bsButtons2->Add(m_btnHelp, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 12);

	m_btnInfo = new wxBitmapButton (itemDialog1, wxID_ABOUT, wxXmlResource::Get()->LoadBitmap("InfoNorm"), wxDefaultPosition, wxSize(36, 36), wxNO_BORDER);
	#if defined (__WXMSW__)
	m_btnInfo->SetBitmapSelected (wxXmlResource::Get()->LoadBitmap("InfoSelected"));
	#endif
	m_btnInfo->SetBitmapDisabled (CreateBitmapDisabled (wxXmlResource::Get()->LoadBitmap("InfoNorm")));
	m_btnInfo->SetToolTip (wxT(IDS_INFO));
    m_bsButtons2->Add(m_btnInfo, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 12);

	#if defined (__WXMAC__)
	m_btnBuy = new wxBitmapButton (itemDialog1, ID_BTN_BUY, wxXmlResource::Get()->LoadBitmap("BuyNormMAC"), wxDefaultPosition, wxSize(84, 36), wxNO_BORDER);
	#else
	m_btnBuy = new wxBitmapButton (itemDialog1, ID_BTN_BUY, wxXmlResource::Get()->LoadBitmap("BuyNorm"), wxDefaultPosition, wxSize(-1, -1), wxNO_BORDER);
	#endif

	#if defined (__WXMSW__)
	m_btnBuy->SetBitmapSelected (wxXmlResource::Get()->LoadBitmap("BuySelected"));
	#endif
    m_btnBuy->SetBitmapDisabled (CreateBitmapDisabled (wxXmlResource::Get()->LoadBitmap("BuyNorm")));
	m_btnBuy->SetToolTip (wxT(IDS_BUY));
    m_bsButtons2->Add(m_btnBuy, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 12);

    //// Status Bar Definitions
	m_StatusBar = new wxStatusBar ( itemDialog1, wxID_ANY, wxST_SIZEGRIP|wxCLIP_CHILDREN);
    int	widths[3] = {-2, -1, 100};
	m_StatusBar->SetFieldsCount (3, widths);
	m_StatusBar->SetStatusText (_T(""), 0);
	m_StatusBar->SetStatusText (_T(""), 1);
	m_StatusBar->SetStatusText (_T(IDS_STAT_VERSION), 2);
  	m_bsMain1->Add(m_StatusBar, 0, wxGROW|wxTOP, 5);


	#if defined (__WXMAC__)

	// Add Help and About items to Mac Menu.
	wxMenu *appMenu = new wxMenu;
	appMenu->Append(wxID_ABOUT, _T("About..."), _T("Show about dialog"));
	appMenu->Append(wxID_HELP, _T("GedWise Help\tCtrl+?"), _T("GedWise Help"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(appMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar (menuBar);

	#endif


	//// Other Settings
	DbDirBoxSizer = itemStaticBoxSizer12;  // save this in case problem loading HotSync libs

	m_btnCancel->Enable (false); // disable button until we start processing

	wxFont font = m_txPrcStat->GetFont();
	font.SetWeight (wxBOLD);
	m_txPrcStat->SetFont (font);
	
	#if defined (wxUSE_DRAG_AND_DROP)
	SetDropTarget (new DnDFile (m_txGedFileName)); // associate drop targets with the controls
	#endif // wxUSE_DRAG_AND_DROP

	m_bmpLogo = new wxBitmap (wxXmlResource::Get()->LoadBitmap("LeafPCDialog"));
}

/////////////////////////////////////////////////////////////////////////////////////////
//	Function:		OnSize
//
//	Description:	Updates the GedWise logo when the frame is resized.
//
//	Parameters:		event -		the size event.
//
//	Returns:		Nothing.
//
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnSize (wxSizeEvent& event)
{
	wxSize  sz  = m_bsMain4->GetSize ();
	wxPoint pos = m_bsMain4->GetPosition ();
	
	RefreshRect (wxRect (pos.x, pos.y, sz.x, m_bmpLogo->GetHeight ()), false);	

	event.Skip(); // process the rest of the event
}

/////////////////////////////////////////////////////////////////////////////////////////
//	Function:		OnPaint
//
//	Description:	Creates a bitmap for the disables state of a given bitmap image.
//
//	Parameters:		event -		paint event.
//
//	Returns:		Nothing.
//
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
	wxPaintDC  dc( this );
	PrepareDC( dc );

	// Draw the GedWise bitmap logo
	wxSize  sz  = m_bsMain4->GetSize ();
	wxPoint pos = m_bsMain4->GetPosition ();
	dc.DrawBitmap ( *m_bmpLogo, pos.x + sz.x/2 - m_bmpLogo->GetWidth ()/2, pos.y, true);
}

/////////////////////////////////////////////////////////////////////////////////////////
//	Function:		CreateBitmapDisabled
//
//	Description:	Creates a bitmap for the disables state of a given bitmap image.
//
//	Parameters:		bitmap -	the bitmap image to convert.
//
//	Returns:		the disabled bitmap
//
/////////////////////////////////////////////////////////////////////////////////////////
wxBitmap GedWiseFrame::CreateBitmapDisabled (const wxBitmap &bitmap)
{    
    wxCHECK_MSG(bitmap.Ok(), wxNullBitmap, wxT("invalid bitmap"));
    
    unsigned char br = GetBackgroundColour().Red();
    unsigned char bg = GetBackgroundColour().Green();
    unsigned char bb = GetBackgroundColour().Blue();
        
    wxImage image = bitmap.ConvertToImage();
    int pos, width = image.GetWidth(), height = image.GetHeight();
    unsigned char *img_data = image.GetData();

    for (int j=0; j<height; j++)
    {
        for (int i=j%2; i<width; i+=2)
        {   
            pos = (j*width+i)*3;
            img_data[pos  ] = br; 
            img_data[pos+1] = bg; 
            img_data[pos+2] = bb; 
        }
    }

	return wxBitmap (image);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	LoadPalmUsers
//
// Description:	For Windows and Mac users, this function loads the Palm InstAide DLL and
//				the	list of registered Hotsync users. It also fills the Hotync user 
//				drop down selector box.  For all other users, this function simple
//				displays the folder where the GedWise database will be saved.
//						
// Parameters:	None.
//
// Return:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::LoadPalmUsers (void)
{
	#if defined (__WXMSW__) || defined (__WXMAC__)

	int			userCount = 0;
	wxString	pUserBuffer;

	// -- Load Palm Install Aide Library. -- 
	if (!LoadLibrary ())
		return false; // library could not be loaded
	
	// -- Get a list of Hotsync users registered on computer. --
	if (!GetPalmUserList (&userCount))
		return false; // no users found

	// -- Set the selected Palm username from the pick list. --
	if (m_iPalmUserNo >= userCount)
		m_iPalmUserNo = 0;
	
	m_cbHotUser->SetSelection (m_iPalmUserNo);
	pUserBuffer = m_cbHotUser->GetStringSelection ();

	// -- Get the Palm folder's GedWise directory. --
	if (!GetAppPalmDirectory (pUserBuffer)) {
		return false;
		}

	return true; // if we got this far a HotSync user was found

	#else // Linux
	
	return false;

	#endif // defined (__WXMSW__) || defined (__WXMAC__)
}


/////////////////////////////////////////////////////////////////////////////////////////
//	Function:		LoadLibrary
//
//	Description:	Loads the Install Aide DLL. Uses the following algorithm:
//					1. Load the temporary Install Aide Dll shipped with the installer.
//					2. Find the Palm Desktop installation directory.
//					3. Load the Palm Desktop Install Aide Dll.
//					4. Return a handle to the Install Aide Dll.
//
//	Parameters:		None.
//
//	Returns:		0			- no error.
//					non-zero	- error code.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::LoadLibrary (void)
{
	#if defined (__WXMSW__) 

	int					errCode;
	wxFileName			palmDesktopPathFile;
	wxString			instAidePathName;
	wxString			installerInstAidePathName;
	wxDynamicLibrary	installerInstAideDll; // InstAide included with GedWise installer
	char				szPalmDesktopDir[MAX_PATH];
	short				size = MAX_PATH;

	// First, load the temporary Install Aide Dll shipped with the installer
	installerInstAidePathName = m_strWorkDir + INSTAIDE_LIB_NAME;

	// Load Install Aide dll included with GedWise Installer package
	if (!installerInstAideDll.Load (installerInstAidePathName)) {
		DecodeErrorCode (ERR_APP_INSTAIDE_NOT_FOUND); // error loading library
		return false;
		}

	wxDYNLIB_FUNCTION (PltGetPathPtr, PltGetPath, installerInstAideDll);

	if (!pfnPltGetPath) { // invalid library - missing functions
		DecodeErrorCode (ERR_APP_INVALID_INSTAIDE);
		return false;
		}

	// Get the Palm Desktop Installation directory and hInstAideDll
	if ((errCode = pfnPltGetPath (PILOT_PATH_HOTSYNC, szPalmDesktopDir, &size)) < 0) {
		DecodeErrorCode (errCode);
		return false; //error determining Palm Desktop install directory
		}

	// Remove the "HotSync.exe" portion and hold on to the Palm Desktop Directory
	palmDesktopPathFile = szPalmDesktopDir;
	m_strPalmDesktopDir = palmDesktopPathFile.GetPath () + palmDesktopPathFile.GetPathSeparator ();

	// Construct the path of the Palm Desktop Install Aide Dll
	instAidePathName = m_strPalmDesktopDir + INSTAIDE_LIB_NAME;

	if (!m_InstAideDLL.Load (instAidePathName)) {
		// Error loading Palm Desktop Install Aide dll so use one shipped with installer.
		m_InstAideDLL.Load (installerInstAidePathName);
		}

	installerInstAideDll.Unload ();

	return true;


	#elif defined (__WXMAC__)

	wxString	msg;
	FSSpec		myFSSpec;
    Ptr			myMainAddr;
    Str255		myErrName;
    CFURLRef    hotsyncManagerURL;
	FSRef		myFSRef;
	OSErr		errCode = noErr;

	m_LibHandle = NULL; // must init!!

	//errCode = FSMakeFSSpec (0, 0L, HOTSYNC_LIB_NAME, &myFSSpec);
	 
    // Look for a resource in the main bundle by name.
    hotsyncManagerURL = CFBundleCopyResourceURL (CFBundleGetMainBundle(), CFSTR(HOTSYNC_LIB_NAME), NULL, NULL);
		
    if (hotsyncManagerURL && CFURLGetFSRef (hotsyncManagerURL, &myFSRef))
		errCode = FSGetCatalogInfo (&myFSRef, kFSCatInfoNone, NULL, NULL, &myFSSpec, NULL);

	if (errCode != noErr)  {

		if (errCode == fnfErr) {
			msg.Printf  (_("\nGedWiseFrame::LoadLibrary, Cannot locate '%s' : File or Directory does not exist. \n"), HOTSYNC_LIB_NAME);
			AddLogEntry (msg);
			}

		else if (errCode == nsvErr) {
			msg.Printf  ( _("\nGedWiseFrame::LoadLibrary, Cannot locate '%s' : Volume does not exist. \n"), HOTSYNC_LIB_NAME);
			AddLogEntry (msg);
			}

		return false;
		}

    errCode = GetDiskFragment (&myFSSpec, 0, kCFragGoesToEOF, "\p", kPrivateCFragCopy,
		&m_LibHandle, &myMainAddr, myErrName);
						 					 
	if (errCode != noErr)  {
       
		msg.Printf (_("\nGedWiseFrame::LoadLibrary, Failed to load shared library '%s' '%s' : GetDiskFragment Error %i \n"),
			HOTSYNC_LIB_NAME, p2cstr (myErrName), errCode);
		AddLogEntry (msg);

		m_LibHandle = NULL;
		return false;
		}

	return true;

	#else // not WXMSW nor WXMAC

	return false;

	#endif
}


#if defined (__WXMSW__) || defined (__WXMAC__)

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GetPalmUserList
//
// Description:	Loads the list of palm users registered on this computer.	
//						
// Parameters:	<- userCount -	number of HotSync users registered on this PC
//
// Returns:		true if successful, else false
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::GetPalmUserList (int *userCount)
{
	char	szUserBuffer[PALM_USER_NAME_LEN];
	int		iIndex;

	*userCount = 0; // init
	

	#if defined (__WXMSW__)
	
	short int	pUserBufSize;
	int			errCode = 0;
	
	wxDYNLIB_FUNCTION (PltGetUserCountPtr, PltGetUserCount, m_InstAideDLL);
	wxDYNLIB_FUNCTION (PltGetUserPtr, PltGetUser, m_InstAideDLL);

	// Get number of palm users that are registered on this computer
	*userCount = pfnPltGetUserCount ();

	if (*userCount < 0) {
		errCode = *userCount; // err code is negative userCount
		DecodeErrorCode (errCode);
		return false;	
		}

	if (*userCount == 0) { // test for no registered users
		DecodeErrorCode (ERR_PILOT_NO_USERS); // error no Hotsync users found
		return false;
		}

	// Add Users to combo box.
	for (iIndex = 0; iIndex < *userCount; iIndex++) {
		
		pUserBufSize = PALM_USER_NAME_LEN; // init

		errCode = pfnPltGetUser (iIndex, szUserBuffer, &pUserBufSize);

		if (errCode < 0) { // error found
			DecodeErrorCode (errCode);
			return false;
			}

		m_cbHotUser->Append (szUserBuffer); // add string to list box
		} // for loop


	#elif defined (__WXMAC__)

	OSErr		errCode			= noErr;
	UInt16		iUserCount		= 0;
	Ptr			ptrUmGetUserCount;
	Ptr			ptrUmGetUserName;

	if (!m_LibHandle) return false;
	
	errCode = FindSymbol (m_LibHandle, "\pUmGetUserCount", &ptrUmGetUserCount, 0);
	
	UmGetUserCountPtr pfnUmGetUserCount = (UmGetUserCountPtr) MachOFunctionPtrForCFMFunctionPtr (ptrUmGetUserCount);

	if (errCode) {
		AddLogEntry (_("\nGedWiseFrame::GetPalmUserList, Error loading 'UmGetUserCount' from HotSync Library \n"));
		return false;
		}

	errCode = FindSymbol (m_LibHandle, "\pUmGetUserName", &ptrUmGetUserName, 0);

	UmGetUserNamePtr pfnUmGetUserName    = (UmGetUserNamePtr) MachOFunctionPtrForCFMFunctionPtr (ptrUmGetUserName);

	if (errCode) {
		AddLogEntry (_("\nGedWiseFrame::GetPalmUserList, Error loading 'UmGetUserName' from HotSync Library \n"));
		return false;
		}

	errCode = pfnUmGetUserCount ((UInt16*) &iUserCount);

	DisposeMachOFunctionPtr (&pfnUmGetUserCount);
	
	*userCount = iUserCount;

	if (errCode) {
		DecodeErrorCode (errCode);
		return false;
		}
	
	if (*userCount == 0) { // test for no registered users
		DecodeErrorCode (kUserMgrUserNotFoundErr); // error no Hotsync users found
		return false;
		}

	// Load user names into combo box
	for (iIndex = 0; iIndex < *userCount; iIndex++) {

		CFStringRef cfsUserBuffer;
		
		errCode = pfnUmGetUserName ((UInt16) iIndex, &cfsUserBuffer);

		if (errCode) {
			DisposeMachOFunctionPtr (&pfnUmGetUserName);
			DecodeErrorCode (errCode);
			return false;
			}
		
		CFStringGetCString (cfsUserBuffer, szUserBuffer, PALM_USER_NAME_LEN,
			kCFStringEncodingMacRoman);

		m_cbHotUser->Append (szUserBuffer); // add string to list box
		} // for loop

	DisposeMachOFunctionPtr (&pfnUmGetUserName);
	
	#endif


	return true; // no errors
}


/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GetAppPalmDirectory
//
// Description:	Finds the 'GedWise' directory within a user's palm directory.
//				
//				The following variables are updated in this function:
//				
//				m_strPalmAppDir (e.g. "c:\Program Files\Palm\Username")
//						
// Parameters:	<-> pUserBuffer -	holds the username when calling this function.
//									returns the folder for the HotSync user.
//
// Returns:		Returns true if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::GetAppPalmDirectory (wxString& pUserBuffer)
{
	m_strPalmAppDir.Empty();


	#if defined (__WXMSW__)

	char	pPathBuffer[MAX_PATH+1];
	char	szUserBuffer[PALM_USER_NAME_LEN+1] = "";
	int		pPathBufSize = MAX_PATH;
	int		errCode;

	// Get the directory for the selected user (e.g. C:\Palm\UserName)
	wxDYNLIB_FUNCTION (PltGetUserDirectoryPtr, PltGetUserDirectory, m_InstAideDLL);

	// Get the directory for the selected user (e.g. C:\Palm\UserName)	
	strncat (szUserBuffer, pUserBuffer.c_str (), PALM_USER_NAME_LEN);
	
	errCode = pfnPltGetUserDirectory (szUserBuffer, pPathBuffer, &pPathBufSize);

	// Check for error getting directory
	if (errCode < 0) {
		DecodeErrorCode (errCode);
		return false;
		}

	// Get the Install directory.
	m_strPalmAppDir.Printf ("%s\\Install\\", pPathBuffer);


	#elif defined (__WXMAC__) 

	OSErr			errCode = noErr;
	FSSpec			fileSpec;
	FSRef			dstFSRef;
	Ptr				ptrUmGetIDFromName;
	Ptr				ptrUmGetFilesToInstallFolderSpec;
	PalmUserID		oUserID;
	char			dstPathFileName[PATH_MAX]; // 1024 bytes
	
	// get the user ID
	errCode = FindSymbol (m_LibHandle, "\pUmGetIDFromName", &ptrUmGetIDFromName, 0);

	UmGetIDFromNamePtr pfnUmGetIDFromName = 
		(UmGetIDFromNamePtr) MachOFunctionPtrForCFMFunctionPtr (ptrUmGetIDFromName);
	
	if (errCode) {
		AddLogEntry (_("\nGedWiseFrame::GetAppPalmDirectory, Error loading 'UmGetIDFromName' from HotSync Library \n"));
		return false;
		}

	CFStringRef userName;

	userName = CFStringCreateWithCString (NULL, pUserBuffer.c_str(),
		kCFStringEncodingMacRoman);

	errCode = pfnUmGetIDFromName (userName, &oUserID);

	DisposeMachOFunctionPtr (&pfnUmGetIDFromName);

	// get the user directory where we want to move the pdb database file
	if (!errCode) {

		errCode = FindSymbol (m_LibHandle, "\pUmGetFilesToInstallFolderSpec",
			&ptrUmGetFilesToInstallFolderSpec, 0);
		}

	UmGetFilesToInstallFolderSpecPtr pfnUmGetFilesToInstallFolderSpec =
		(UmGetFilesToInstallFolderSpecPtr) MachOFunctionPtrForCFMFunctionPtr
		(ptrUmGetFilesToInstallFolderSpec);

	if (errCode) {
		AddLogEntry (_("\nGedWiseFrame::GetAppPalmDirectory, Error loading 'UmGetFilesToInstallFolderSpec' from HotSync Library \n"));
		return false;
		}

	errCode = pfnUmGetFilesToInstallFolderSpec (oUserID, &fileSpec);

	DisposeMachOFunctionPtr (&pfnUmGetFilesToInstallFolderSpec);

	if (errCode) {
		DecodeErrorCode (errCode);
		return false;
		}
		
	// Get the path information for m_strPalmAppDir
	errCode = FSpMakeFSRef (&fileSpec, &dstFSRef);

	if (errCode) {
		AddLogEntry (_T("\nGedWiseFrame::GetAppPalmDirectory, Error making FileRef. \n"));
		return false;
		}
			
	FSRefMakePath (&dstFSRef, (UInt8 *) &dstPathFileName, PATH_MAX);
	
	// add the '/' character at end
	m_strPalmAppDir.Printf ("%s%c", dstPathFileName, wxFILE_SEP_PATH);

	#endif //defined (__WXMSW__) elif defined (__WXMAC__)


	return true;
}

#endif // defined (__WXMSW__) || defined (_WXMAC)


/////////////////////////////////////////////////////////////////////////////////////////
// Function:	InstallFile
//
// Description:	Copies the current database file to the user's Palm install directory. It 
//				It also removes the database file from the working directory after the
//				file is copied successfully.
//
// Parameters:	None.
//
// Returns:		true if successful.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::InstallFile (void)
{
	wxString srcPathFileName;

	srcPathFileName = m_strWorkDir + m_strDbName + PDB_DB_FILE;

	#if defined (__WXMSW__) || defined (_WXMAC) 
		
	if (m_bHotUserFound) {

		int		errCode;
		char	pUserBuffer[MAX_PATH+1] = ""; // install folder..eg C:\Program Files\Palm\User\Install
		char	szInstappdPath[MAX_PATH+1];   // full path and name of file to be installed

		wxDYNLIB_FUNCTION (PltInstallFilePtr, PltInstallFile, m_InstAideDLL);

		strcpy (szInstappdPath, srcPathFileName.c_str());

		strncat (pUserBuffer, m_cbHotUser->GetStringSelection ().c_str(), MAX_PATH);

		errCode = pfnPltInstallFile (pUserBuffer, szInstappdPath);

		if (errCode) {
			DecodeErrorCode (ERR_APP_MOVE_INSTALL_FLD);
			}
		else { // file moved ok so erase old copy
			remove (szInstappdPath);
			}
		}

	else {
	
		wxString dstPathFileName;

		dstPathFileName = m_strPalmAppDir + m_strDbName + PDB_DB_FILE;

		if (!wxRenameFile (srcPathFileName, dstPathFileName)) {
			DecodeErrorCode (ERR_APP_MOVE_INSTALL_FLD);
			}
		}

	#else

	wxString dstPathFileName;

	dstPathFileName = m_strPalmAppDir + m_strDbName + PDB_DB_FILE;

	if (!wxRenameFile (srcPathFileName, dstPathFileName)) {
		DecodeErrorCode (ERR_APP_MOVE_INSTALL_FLD);
		}

	#endif
}


/////////////////////////////////////////////////////////////////////////////////////////
// Function:	RemoveInstallFile
//
// Description:	Removes a file from the Palm install list.
//
// Parameters:	-> dbName	- name of file to remove from install list.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::RemoveInstallFile (wxString& dbName)
{
	wxString filePathName;

	filePathName = m_strPalmAppDir + dbName + PDB_DB_FILE;;

	wxRemoveFile (filePathName);
}


/////////////////////////////////////////////////////////////////////////////////////////
// Function:	SavePrefs
//
// Description:	Saves the user preferences to the preferences file. 
//
// Parameters:	None.
//						
// Returns:		True if successfull, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::SavePrefs (void) 
{
	int	result = 0;

	// Set database name
	if (m_strDbName.IsEmpty())
		m_strDbName = DB_NAME_DFLT;

	if (m_strFilePathName.IsEmpty())
		m_strFilePathName = " "; // set it to a space so strtok works in GetPrefs

	if (m_strPalmAppDir.IsEmpty())
		m_strPalmAppDir = " "; // set it to a space so strtok works in GetPrefs

	// Open the preferences file located in same directory as help menu for this dialog.
	// NOTE: all variables must have values, except for m_strFilePathName or else the strtok
	// function will screw up.
	if ((m_pPrefFile = fopen (m_strPrefFileName.c_str(), "wt")) != NULL) {
		result = fprintf (m_pPrefFile, "%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%s\t%s\t%s\t<END>",
		IsExclSour (),				IsExclRepo (),			IsExclNote (),
		m_iUserNoteSize,			IsInslTitl (),			IsExclRefN (),
		IsExclOrdn (),				IsInslSpac (),			IsSortUndEvenBot (),
		IsSortChilPerGed (),		m_iPalmUserNo,			m_strDbName.c_str(),
		m_strFilePathName.c_str(),	m_strPalmAppDir.c_str());

		fclose (m_pPrefFile);
		}

	if (result > 0)
		return true;
	else 
		return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GetPrefs
//
// Description:	Gets the user preferences from the preferences file. 
//				Sets the screen checkboxes depending on the saved user preferences.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::GetPrefs (void) 
{
	char		buffer[BUFFER_SZ];
	wxString	totNoteChars;
	char		tabStr[] = {CHR_TAB, CHR_NUL};

	if ((m_pPrefFile = fopen (m_strPrefFileName.c_str(), "rt")) != NULL) {
		
		if (fgets (buffer, sizeof (buffer), m_pPrefFile) != NULL) {
			char*	token;
			int		i = 0;
			char*	tokStr[TOT_PREFS];
			memset (&tokStr, ' ', TOT_PREFS);

			token = strtok (buffer, tabStr);
			tokStr[i] = token;
			i++;

			while (i < TOT_PREFS) {
				token = strtok (NULL, tabStr);
				tokStr[i] = token;
				i++;	
				}

			m_chExSour->SetValue   (tokStr[0]  ? (BOOL) atoi (tokStr[0]) : false);
			m_chExRepo->SetValue   (tokStr[1]  ? (BOOL) atoi (tokStr[1]) : false);
			m_chExNotes->SetValue  (tokStr[2]  ? (BOOL) atoi (tokStr[2]) : false);
			m_iUserNoteSize		=  (tokStr[3]  ? (int)  atoi (tokStr[3]) : NOTE_SZ);
			m_chInsTitle->SetValue (tokStr[4]  ? (BOOL) atoi (tokStr[4]) : false);
			m_chExRefN->SetValue   (tokStr[5]  ? (BOOL) atoi (tokStr[5]) : false);
			m_chExOrd->SetValue    (tokStr[6]  ? (BOOL) atoi (tokStr[6]) : false);
			m_chInsSpace->SetValue (tokStr[7]  ? (BOOL) atoi (tokStr[7]) : false);
			m_chSortEven->SetValue (tokStr[8]  ? (BOOL) atoi (tokStr[8]) : false);
			m_chSortChil->SetValue (tokStr[9]  ? (BOOL) atoi (tokStr[9]) : false);
			m_iPalmUserNo		=  (tokStr[10] ? (int)  atoi (tokStr[10]): 0);
			m_strDbName			=   tokStr[11];
			m_strFilePathName	=   tokStr[12];
			m_strPalmAppDir		=   tokStr[13];
			}
		
		fclose (m_pPrefFile);
		}
	
	// set total characters for note size
	totNoteChars.Printf ("%u", m_iUserNoteSize);
	
	m_chMaxNote->SetValue (totNoteChars);

	// if note size is zero then check exclude notes box
	if (!IsExclNote () && atoi (m_chMaxNote->GetValue ()) == 0)
		m_chExNotes->SetValue (true);

	UpdateNoteCtls ();

	// check if last GEDCOM file converted exists
	if (!wxFileExists (m_strFilePathName))
		m_strFilePathName.Empty ();

	m_txGedFileName->SetValue (m_strFilePathName);

	// check if Palm App User directory exists
	if (!wxDirExists (m_strPalmAppDir))
		m_strPalmAppDir = m_strWorkDir; // use work directory as default
	
	// set database name
	if (m_strDbName.IsEmpty())
		m_strDbName = DB_NAME_DFLT;

	m_txDbName->SetValue (m_strDbName);
	
	return true; 
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	UpdateProgressMessage
//
// Description:	Updates the message in the Progress Box (UpdateStatDlg).
//
// Parameters:	-> message - message to display in progress box
//
// Returns:		Nothing.						
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::UpdateProgressMessage (const char* msg)
{
	m_txPrcStat->SetLabel (msg);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	UpdateStatusBar
//
// Description:	Updates the Status Bar status message first box and clears any
//				message in second box.
//
// Parameters:	-> messageID - string ID number for string to display.
//
// Returns:		Nothing.					
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::UpdateStatusBar (const wxString& msg) //(int messageID)
{

	m_StatusBar->SetStatusText (msg, 0);
	m_StatusBar->SetStatusText ("",  1);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	CheckStatus
//
// Description:	Checks if user cancelled processing and updates the progress bar.
//						
// Parameters:	->	showMessage	-	set to true if we want to display cancelled message.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::CheckStatus (bool showMessage /*= TRUE*/)
{
	wxGetApp().Yield (true);

	if (Canceled ()) {
		
		if (showMessage) {
			wxMessageBox (IDS_MSG_CANCEL, IDS_MSG_CANCEL_TTL, wxOK | wxICON_EXCLAMATION);
			}
		return false;
		}
	
	ProgressStepIt ();

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	CleanExit
//
// Description:	Called if fatal error or cancel occurs during processing.
//				It closes all open files and erases all temporary files
//				that have been created.
//						
// Parameters:	-> code	- 0 if closing files due to cancellation.
//						- 1 if closing files due to error.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::CleanExit (UINT code)
{
	wxString filePathName;

	// close and remove all DB files
	for (int i = IndiDB; i <= PdbDB; i++) {  
		if (DBFiles[i]->fp) {
			fclose (DBFiles[i]->fp);
			DBFiles[i]->fp = NULL;
			}

		filePathName = m_strWorkDir + DBFiles[i]->name;
		remove (filePathName.c_str());
		}

	if (code == 1) {
		if (GedFileP) {
			fclose (GedFileP);
			GedFileP = NULL;
			}
		}
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ParseGedcomFile
//
// Description:	Main processing loop.
//
// Parameters:	None.
//
// Returns:		True if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::ParseGedcomFile (void)
{
	ParseGedcom ParseRecs (this); // initialize ParseGedcom class

	// check that file is valid GEDCOM file.
	if (!ParseRecs.GetGedComData (&ParseRecs.GedFileRec, false)) {
		return false;
		}

	if (m_bCanceled || m_bBadExit)
		return false;

	// update the status message in UpdateStatDlg.
	UpdateProgressMessage (IDS_PROG_PARSING);

	// display GEDCOM file information
	m_txGedVers->SetLabel (ParseRecs.GedFileRec.ver);
	m_txGedDate->SetLabel (ParseRecs.GedFileRec.date);
	m_txGedSour->SetLabel (ParseRecs.GedFileRec.fileName);
	
	wxString sTotLines;
	m_txGedLine->SetLabel (sTotLines.Format ("%u", TOTLINES));

	// -- start the processing of the GEDCOM file. --
	ParseRecs.StartParsing ();

	if (m_bCanceled || m_bBadExit)
		return false;

	UpdateProgressMessage (""); // clear status message
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	SortDBFiles
//
// Description:	Sorts the database files.
//
// Parameters:	None.
//
// Returns:		True if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::SortDbFiles (void)
{
	SortRoutines	SortRoutines (this);

	if (!SortRoutines.SortMultFiles ())
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	CreateIndex
//
// Description:	Creates an index for the IndiDB database.
//
// Parameters:	None.
//						
// Returns:		True if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::CreateIndxDb (void)
{
	DBCreateIndex	CreateIndex (this);

	if (!CreateIndex.CreateIndex ())
		return false;
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	SortIndex
//
// Description:	Sorts the index file.
//
// Parameters:	None.
//						
// Returns:		True if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::SortIndxDb (void)
{
	SortRoutines	SortRoutines (this);
	
	if (!SortRoutines.SortOneFile (IndxDB, false))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	MergeChilDb
//
// Description:	Merges the child database files. Only called if user is ordering 
//					children as per the GEDCOM file.
//
// Parameters:	None.
//						
// Returns:		True if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::MergeChilDb (void)
{
	DBMergeChilDb	MergeChilDb (this);

	if (!MergeChilDb.ProcessFiles ())
		return false;
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	SortChilDb
//
// Description: Sorts the child database.
//
// Parameters:	None.
//					
// Returns:		True if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::SortChilDb (void)
{
	SortRoutines	SortChilDb (this);

	if (!SortChilDb.SortOneFile (ChilDB, false))
		return false;
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	IndiRefSwap
//
// Description:	Substitutes the individual and family xrefno's in the IndiDB with
//				actual record numbers in the FamiDB.  Also builds list
//				of alias records in AliaDB for individuals with alias names.
//
// Parameters:	None.
//						
// Returns:		True if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::IndiRefSwap (void)
{
	DBIndiRefSwap	IndiRefSwap (this);

	if (!IndiRefSwap.ProcessDB ())
		return false;
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	CountChil
//
// Description:	Counts the children in the FamCDB and inserts the total into the 
//				FamiDB.  Also sets a field in IndiDB record to indicate whether person
//				has children.
//						
// Parameters:	None.	
//
// Returns:		True if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::CountChil (void)
{
	DBFamiRefSwap	CountChil (this);

	if (!CountChil.ProcessFiles ())
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	SubRefNum
//
// Description:	Substitutes cross-references numbers in several databases: FamiDB,
//				SourDB and RepoDB.
//
// Parameters:	None.
//
// Returns:		True if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::SubRefNum (void)
{
	DBSubRefNo	SubRefNo (this);

	if (!SubRefNo.ProcessFiles ())
		return false;
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	SubEvenNo
//
// Description:	Substitute the Event record numbers in IndiDB and FamiDB
//
// Parameters:	None.
//
// Returns:		True if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::SubEvenNo (void)
{
	DBSubEvenNo	SubEvenNo (this);

	if (!SubEvenNo.ProcessFiles ())
		return false;
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	CombDBFiles
//
// Description:	Combines all of the separate database into one large database file.
//
// Parameters:	None.
//						
// Returns:		True if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::CombDbFiles (void)
{
	DBCombFiles	CombFiles (this);

	if (!CombFiles.CombineFiles ())
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	MakePDBFile
//
// Description:	Creates the Palm OS database.
//
// Parameters:	None.
//						
// Returns:		True if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::MakePDbFile (void)
{
	MakePDB	MakePDBFile (this);

	if (!MakePDBFile.CreatePDB ())
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ProcessDBFiles
//
// Description:	Calls a bunch of miscellaneous file preparation routines to get the 
//				the databases ready to be combined.						
//						
// Parameters:	None.
//
// Returns:		True if successful, else false.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseFrame::ProcessDBFiles (void)
{
	UINT progCnt;

	// Sort the database files created during processing
	if (!SortDbFiles ())
		return false;

	// Update progress message.
	UpdateProgressMessage (IDS_PROG_INDEX);

	// Reset the progress bar
	ProgressBarReset (((NameCnt-1)/(DB_PROG_DIV+1)) + 1);

	// Create the IndxDb file.
	if (!CreateIndxDb ())
		return false;

	// Sort the IndxDb file.
	if (!SortIndxDb ())
		return false;

	AddLogEntry ("\nCreateIndxDb: Successful\n");

	// If sorting children as per GEDCOM file, then merge and sort the child files.
	if (IsSortChilPerGed ()) {
		
		if (!MergeChilDb ())
			return false;

		if (!SortChilDb ())
			return false;
		}

	wxASSERT (m_iCurProgBarUpd == m_iMaxProgBarUpd);

	// Update progress message.
	UpdateProgressMessage (IDS_PROG_PALMDB);

	// Reset the progress bar. Count all the times we will call fgets in
	// the DBCreate and MakePDB functions.
	progCnt =	// following for checks done in DBCreate module
	
				// IndiRefSwap
				ChilCnt + FamiCnt + EvenNoCnt + IndxCnt +
					
				// CountChil
				IndxCnt + FamiCnt +

				// SubEvenNo
				IndxCnt + FamiCnt +

				// MakePDB module
				NameCnt	  + ChilCnt   + FamiCnt + EvenNoCnt +
				SCitNoCnt + RCitNoCnt + NoteCnt +
				(!IsExclRepo () ? RepoCnt : 0)  +
				(!IsExclSour () ? SourCnt : 0)  +
				9 - 1; // add 9 for each of EODMarkers

	progCnt = (progCnt/(DB_PROG_DIV+1)) + 1; // add 1 as 0 % DB_PROG_DIV = 0

	// Reset the progress bar
	ProgressBarReset (progCnt);

	CheckNo = 0;
	
	if (!IndiRefSwap ())
		return false;

	AddLogEntry ("IndiRefSwap: Successful\n");

	if (!CountChil ())
		return false;

	AddLogEntry ("CountChil: Successful\n");

	if (!SubRefNum ())
		return false;

	AddLogEntry ("SubRefNum: Successful\n");

	if (!SubEvenNo ())
		return false;

	AddLogEntry ("SubEvenNo: Successful\n");

	if (!CombDbFiles ())
		return false;

	AddLogEntry ("CombDBFiles: Successful\n");

	if (!MakePDbFile ())
		return false;

	AddLogEntry ("MakePDBFile: Successful\n");

	wxASSERT (m_iCurProgBarUpd == m_iMaxProgBarUpd);
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	UpdateNoteCtls
//
// Description:	Hides or shows the Maximum Note Size edit field depending on whether
//				the user choses to exclude notes.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::UpdateNoteCtls (void)
{
	m_chMaxNoteLb->Enable (!m_chExNotes->IsChecked());
	m_chMaxNote->Enable (!m_chExNotes->IsChecked());
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	StartProcessing
//
// Description:	Displays the UdpateStats dialog and starts the data processing.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::StartProcessing (void) 
{
	bool		okToContinue;
	wxString	msg;

	UpdateStatusBar (IDS_STAT_PROCESS);

	WriteDataToLog (eStartProcessing);

	// initialize progress data
	ProgressInitData ();
	ProgressBarReset (((TOTLINES-1)/(DB_PROG_DIV+1))+1);

	// show the Cancel Button
	m_btnCancel->Enable (true);
	
	SetControlStatus (false); // hide all controls

	// -- begin processing the GEDCOM file --
	okToContinue = ParseGedcomFile ();

	if (okToContinue && IndiCnt == 0) {
		okToContinue = false;
		SetBadExit ();
		msg = IDS_ERR_NO_INDI;
		wxLogError (msg);
		AddLogEntry (msg);
		}

	#ifdef DEBUG
	if (okToContinue)
		wxASSERT (okm_iCurProgBarUpd == m_iMaxProgBarUpd);
	#endif

	// update data in dialog
	ProgressUpdateData ();
	Update ();
	
	// -- sort data files, create index and process data files --
	if (okToContinue)
		okToContinue = ProcessDBFiles ();

	// -- install file to user's hotsync folder --
	#if defined (__WXMAC__)
	if (okToContinue)
		InstallFile ();

	#else

	//if (m_bHotUserFound && okToContinue)
	if (okToContinue)
		InstallFile ();

	#endif

	// hide the Cancel Button after we finish processing GEDCOM file.
	m_btnCancel->Enable (false);

	if (okToContinue) {
		
		// make sure progress bar shows 100% and correct time elapsed
		ProgressBarReset (1);
		CheckStatus ();

		// update progress message in UpdateStatDlg.
		UpdateProgressMessage (IDS_PROG_COMPLETE);
		
		// update Status Bar Messages and get estimated file size
		UpdateStatusBar (IDS_STAT_COMPLETE);
	
		// estimate file size for Status Bar entry
		msg.Printf (IDS_PROG_DBSIZE, EstimateDBSize (m_strDbName));
		m_StatusBar->SetStatusText (msg,  1);

		// place entries into log file
		AddLogEntry ("\n"); // add spacer line
		AddLogEntry (msg);
		WriteDataToLog (eEndProcessing);
		}

	else { // !okToContinue
		
		// place entry into log file
		if (m_bCanceled) {
			ProgressClearData ();
			UpdateStatusBar (IDS_STAT_CANCELED);
			WriteDataToLog (eCancelledProcessing);
			}
		else if (m_bBadExit) { 
			UpdateStatusBar (IDS_STAT_ERR); 
			WriteDataToLog (eErrProcessing);
			m_txPrcStat->SetForegroundColour(*wxRED); // set text color red 
			UpdateProgressMessage (IDS_PROG_ERROR);
			m_txPrcStat->SetForegroundColour (wxNullColour);
			}
		}

	wxASSERT(!(m_bCanceled && m_bBadExit));
	
	// If cancel button hit during processing then close/erase all files created.
	if (m_bCanceled)
		CleanExit(0);

	// If error during processing then close/erase all files created.
	if (m_bBadExit)
		CleanExit(1);

	ProgressStop ();
	SetControlStatus (true);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Event Handler Functions
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnAbout
//
// Description:	Displays the About dialog box.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnAbout (wxCommandEvent& WXUNUSED(event))
{
	GedWiseAboutDialog *pDialog = new GedWiseAboutDialog (NULL);
      
	pDialog->ShowModal();	
	pDialog->Destroy();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnBtnBuy
//
// Description:	Handles user clicking on Purchase button.  Opens web browser and
//				goes to www.BatteryParkSoftware.com URL.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnBtnBuy (wxCommandEvent& WXUNUSED(event))
{
	GoToURL (PURCHASE_URL);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnBtnCancel
//
// Description:	Called when user presses the Cancel button during GEDCOM processing.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnBtnCancel (wxCommandEvent& WXUNUSED(event)) 
{
	m_bCanceled = true;
	ProgressStop ();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnChangeDbName
//
// Description:	Called when user changes the database name.
//						
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnChangeDbName (wxCommandEvent& WXUNUSED(event))
{
	bool err = false;

	m_strDbName = m_txDbName->GetValue ();

	// check max size has not been exceeded
	if (m_strDbName.Length() > DB_NAME_SZ) {

		err = true;

		wxMessageBox (IDS_WRN_DBNAME_LEN, IDS_TTL_DATAENTRY, wxOK | wxICON_EXCLAMATION);
		}

	// check database name for correct characters (it can bee numbers and letters only).
	if (!err && !m_strDbName.IsEmpty ()) {

		int lastPos = m_strDbName.Length() - 1;

		if (!((m_strDbName[lastPos] >= 'a' && m_strDbName[lastPos] <= 'z') ||
			  (m_strDbName[lastPos] >= 'A' && m_strDbName[lastPos] <= 'Z') ||
			  (m_strDbName[lastPos] >= '0' && m_strDbName[lastPos] <= '9'))) {
			
			err = true;

			wxMessageBox (IDS_WRN_DBNAME, IDS_TTL_DATAENTRY, wxOK | wxICON_EXCLAMATION);
			}
		}

	if (err) {

		m_strDbName.RemoveLast ();

		m_txDbName->SetValue (m_strDbName);

		// set cursor at end of text
		m_txDbName->SetInsertionPointEnd();
		}
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnChangeGedFileName
//
// Description:	Handles certain processing if a change to the File Edit field.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnChangeGedFileName (wxCommandEvent& WXUNUSED(event)) 
{
	//if (!m_ProgressBar)
	//	return;

	ProgressClearData ();

	if (!m_txGedVers || !m_txGedDate || !m_txGedSour || !m_txGedLine || !m_txGedFileName)
		return;

	// erase file information data
	m_txGedVers->SetLabel ("");
	m_txGedDate->SetLabel ("");
	m_txGedSour->SetLabel ("");
	m_txGedLine->SetLabel ("");

	m_strFilePathName = m_txGedFileName->GetValue ();

	// if no file selected then hide start button and update Status Bar
	if (!m_strFilePathName.IsEmpty())	{
		m_btnStart->Enable (true);
		UpdateStatusBar (IDS_STAT_DBNAME);
		}
	else {
		m_btnStart->Enable (false);
		UpdateStatusBar (IDS_STAT_SELECT);	
		}
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnChangeDirName
//
// Description:	Handles certain processing if a change to the Directory Edit field.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnChangeDirName (wxCommandEvent& WXUNUSED(event)) 
{
	if (!m_txGedVers || !m_txGedDate || !m_txGedSour || !m_txGedLine || !m_txDbDir)
		return;
	
	ProgressClearData ();

	// erase file information data
	m_txGedVers->SetLabel ("");
	m_txGedDate->SetLabel ("");
	m_txGedSour->SetLabel ("");
	m_txGedLine->SetLabel ("");

	m_strPalmAppDir = m_txDbDir->GetValue ();
}


/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnChangeNoteSize
//
// Description:	Called when there is a change to the edit field for maximum note size.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnChangeNoteSize (wxCommandEvent& WXUNUSED(event)) 
{
	wxString strNoteChars = m_chMaxNote->GetValue ();

	// check database for correct characters and display error message if incorrect.
	int lastPos = strNoteChars.Length() - 1;
	int	noteLen = atoi (m_chMaxNote->GetValue());

	// check note size and display error message if not in correct range.
	if (lastPos >= 0 && 
		((!(strNoteChars[lastPos] >= '0' && strNoteChars[lastPos] <= '9') ||
		(noteLen > NOTE_SZ || noteLen < 0)))) {

		wxString msg;
		msg.Printf (IDS_WRN_NOTESZ, NOTE_SZ);
		wxMessageBox (msg, IDS_TTL_DATAENTRY, wxOK | wxICON_EXCLAMATION);
		
		strNoteChars.erase (strNoteChars.Length()-1, 1);
		
		m_chMaxNote->SetValue (strNoteChars);
		
		// set cursor at end of text
		m_chMaxNote->SetInsertionPointEnd ();
		}
	
	m_iUserNoteSize = atoi (m_chMaxNote->GetValue());
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnExitBtn
//
// Description:	Called when user user clicks on the Exit button.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnExit (wxCommandEvent& WXUNUSED(event))
{
    Close (true); // true is to force the frame to close
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnBtnFileOpen
//
// Description:	Displays the File Selector dialog box and displays the name of the
//				selected file in the Gedcom File text box.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnBtnFileOpen (wxCommandEvent& WXUNUSED(event))
{
	wxFileName	filePathName (m_strFilePathName);

	wxFileDialog *dlg = new wxFileDialog (this, _(IDS_SEL_GEDCOM), filePathName.GetPath(),
		filePathName.GetName (), _(IDS_GED_WILDCARD), wxOPEN);

    if (dlg->ShowModal() == wxID_OK) {
        filePathName = dlg->GetPath();
		}
	dlg->Destroy();


	if (filePathName.IsOk ()) {
		
		if (filePathName.FileExists ()) { // don't copy file name if no file selected

			m_strFilePathName = filePathName.GetFullPath();

			m_txGedFileName->SetValue (m_strFilePathName);
			}
				
		else { // if file does not exist, signal an error
			wxLogError (_("GedWiseFrame::OnBtnFileOpen\n\n" IDS_ERR_FILEEXIST), filePathName.GetFullPath ().c_str());
			}
		}

	// set cursor at end of file name in text control
	m_txGedFileName->SetInsertionPointEnd ();
	m_txGedFileName->SetFocus ();
}
/*
void GedWiseFrame::OnDbFileSelBtn (wxCommandEvent& WXUNUSED(event))
{
	wxString	wildcard = "*-Rend.pdb";

	wxFileName	filePathName (m_strPalmAppDir);
	
	wxFileDialog *dlg = new wxFileDialog (this, _(IDS_SEL_DB), filePathName.GetPath(),
		filePathName.GetName (), _(IDS_DB_WILDCARD), wxOPEN);

    if (dlg->ShowModal() == wxID_OK) {
		}

	dlg->Destroy();

	// set cursor at end of file name in text control
	//m_txGedFileName->SetInsertionPointEnd ();
	//m_txGedFileName->SetFocus ();
}
*/

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnBtnDirOpen
//
// Description:	Displays the File Selector dialog box and displays the name of the
//				selected file in the Gedcom File text box.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnBtnDirOpen (wxCommandEvent& WXUNUSED(event))
{
	wxFileName	filePathName (m_strPalmAppDir); // init with m_strPalmAppDir

	wxDirDialog *dlg = new wxDirDialog (this, _(IDS_SEL_DIRECTORY), m_strPalmAppDir);

	if (dlg->ShowModal() == wxID_OK) {
		filePathName = dlg->GetPath();
		}
			
	dlg->Destroy();
	
	if (filePathName.IsOk())	
		m_strPalmAppDir = filePathName.GetFullPath();
	else
		m_strPalmAppDir = m_strWorkDir;


	// Add final directory seperator.
	if (m_strPalmAppDir.Last() != wxFILE_SEP_PATH)
		m_strPalmAppDir += wxFILE_SEP_PATH;

	m_txDbDir->SetValue (m_strPalmAppDir);
	m_txDbDir->SetInsertionPointEnd ();
	m_txDbDir->SetFocus ();
	}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnHelpBtn
//
// Description:	Called when user presses the Help button.
//						
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnHelp (wxCommandEvent& WXUNUSED(event)) 
{
	
	#if defined (__WXMSW__)

	wxString	filePathName = m_strWorkDir + MANUAL_FILE_MSW;
	
	ExecuteFile (filePathName);
	
	#else

	m_help->DisplayContents(); // display help contents
	
	#endif // defined (__WXMSW__)
}


/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnBtnInst
//
// Description:	Called when user presses the View Database List button.
//						
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnBtnInst (wxCommandEvent& event)
{
	wxString	pUserBuffer;
	wxString	filePathName;


	#if defined (__WXMSW__)

	pUserBuffer = m_cbHotUser->GetStringSelection ();
	
	filePathName = m_strPalmDesktopDir + PDB_INSTALL_FNAME;	

	if (ShellExecute (NULL, "open", filePathName, NULL,NULL, SW_SHOW) <= (HINSTANCE)32)
		wxLogError ( "GedWiseFrame::OnBtnInst\n\n" IDS_ERR_FOPEN, filePathName.c_str());
	
	#endif // defined (__WXMSW__)


	#if defined (__WXMAC__)

	// check to see if any GedWise databases are in the Install Folder
	filePathName = m_strPalmAppDir + "*" + PDB_DB_FILE;

	wxString f = wxFindFirstFile (filePathName.c_str());

	if (f.empty()) { // then no databases are in the install folder
		wxMessageBox (IDS_ERR_NO_DB, _THIS_FILE, wxOK | wxICON_EXCLAMATION);
		return;
		}

	ExecuteFile (f);

	#endif

}


/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnNoteCb
//
// Description:	Hides or shows the Maximum Note Size edit field depending on whether
//				the user choses to exclude notes.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnNoteCb (wxCommandEvent& WXUNUSED(event))
{
	UpdateNoteCtls ();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnRepoCb
//
// Description:	Hides or shows the "Exclude Sources" Checkbox depending on whether
//				the user chooses to exclude Repositories.  If user choses to include					
//				Repositories, then Sources must be included too.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnRepoCb (wxCommandEvent& WXUNUSED(event))
{
	if (!m_chExRepo->IsChecked())
		m_chExSour->SetValue (false);
}

#if defined (__WXMSW__) || defined (__WXMAC__)
/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnSelchangePalmuser
//
// Description:	Called when user changes the hotsync user.  It sets the new user and
//				calls a function to get the new user's Palm directory.
//						
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnSelHotUser (wxCommandEvent& event)
{
	wxString	pUserBuffer;
	int			iPalmUserNo_old;

	iPalmUserNo_old = m_iPalmUserNo; // save the old user number
	
	m_iPalmUserNo = m_cbHotUser->GetSelection (); // set the currently hotsync user

	pUserBuffer = m_cbHotUser->GetStringSelection ();

	if ((iPalmUserNo_old != m_iPalmUserNo) && !GetAppPalmDirectory (pUserBuffer)) {

		// Get back to originally selected hotsync user
		m_iPalmUserNo = iPalmUserNo_old;
		
		m_cbHotUser->SetSelection (m_iPalmUserNo);
	
		wxString msg;
		msg.Printf (_T("The user '%s' you selected is invalid."), pUserBuffer.c_str());
		wxMessageBox (msg, _T("User Selection Error"), wxOK | wxICON_EXCLAMATION);

		pUserBuffer = m_cbHotUser->GetStringSelection ();
		GetAppPalmDirectory (pUserBuffer);
		//m_bHotUserFound = false; // no users found
		}
}
#endif // defined (__WXMSW__) || defined (__WXMAC__)

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnSourCb
//
// Description:	Checks or unchecks the Exclude Repository Checkbox depending on whether
//				the user chooses to exclude Sources.  If user chooses to exclude Sources,
//				then Repositories must be excluded too.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnSourCb (wxCommandEvent& WXUNUSED(event))
{
	if (m_chExSour->IsChecked())
		m_chExRepo->SetValue (true);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnStart
//
// Description:	Called when user presses the Start button.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnBtnStart (wxCommandEvent& WXUNUSED(event)) 
{
	char		buffer[BUFFER_SZ]; // used to count lines in GEDCOM file.
	wxString	filePathName;
	wxString	errLogFilePathName;
	int			response;
	wxString	fileName;
	wxString	fileExt;
	
	// Initialize variables.
	m_bBadExit	   = false;
	m_bCanceled    = false;

	// Load m_strGedFileName (just the name of GEDCOM file, not the path)
	wxSplitPath (m_strFilePathName, NULL, &fileName, &fileExt);
	m_strFileName.Printf ("%s.%s", fileName.c_str(), fileExt.c_str());

	// If User enters a notesize of zero then don't copy notes.
	if (!IsExclNote () && atoi (m_chMaxNote->GetValue ()) == 0) {
		m_chExNotes->SetValue (true);
		UpdateNoteCtls ();
		}

	// Use default database name if name is empty
	if (m_strDbName.IsEmpty()) {
		m_strDbName = DB_NAME_DFLT;
		m_txDbName->SetValue (DB_NAME_DFLT);
		}

	// Strip any spaces at start or end of file name.
	m_strFilePathName.Trim (false);
	m_strFilePathName.Trim (true);

	// Redraw file name without spaces and set cursor to end of file name.
	m_txGedFileName->SetValue (m_strFilePathName);
	m_txGedFileName->SetInsertionPointEnd ();
	m_txGedFileName->SetFocus ();

	// If file entered does not exist, signal an error.
	if (!wxFileExists (m_strFilePathName)) {
		wxString msg;
		msg.Printf (IDS_ERR_FILEEXIST, m_strFilePathName.c_str());
		wxMessageBox (msg, IDS_TTL_FILEERROR, wxOK | wxICON_EXCLAMATION);
		return;
		}

	// If database already in export folder, make sure user wants to overwrite it.
	filePathName = m_strPalmAppDir + m_strDbName +	PDB_DB_FILE;
	
	if (wxFileExists (filePathName)) {
		wxString msg;
		int		 response;

		msg.Printf (IDS_WRN_OVERWRITE, m_strDbName.c_str());
		response = wxMessageBox (msg, IDS_TTL_OVERWRITE, wxYES_NO | wxICON_EXCLAMATION);
		
		if (response == wxNO)
			return;
		else
			RemoveInstallFile (m_strDbName);
		}

	// If directory in which to save database does not exist, signal an error.
	if (!m_bHotUserFound && !wxDirExists (m_strPalmAppDir)) {
		wxString msg;
		msg.Printf (IDS_ERR_DIREXIST, m_strPalmAppDir.c_str());
		wxMessageBox (msg, IDS_TTL_DIRERROR, wxOK | wxICON_EXCLAMATION);
		return;
		}

	// Open GEDCOM file selected.
	if ((GedFileP = fopen (m_strFilePathName.c_str(), "rt")) == NULL) {
		wxString msg;
		msg.Printf (_("GedWiseFrame::OnBtnStart \n\nError #GW1 opening: \n%s. \n"), m_strFilePathName.c_str());
		wxLogSysError (msg, m_strFilePathName.c_str());
		AddLogEntry (msg);
		return;
		}

	// count the total lines in the selected GEDCOM file
	TOTLINES = 0;
	while (GetFileRecAny (buffer, sizeof(buffer), GedFileP) != NULL)
   	TOTLINES++;


	// open the error log file
	errLogFilePathName = m_strWorkDir + DBFiles[ErrLog]->name;

	remove (errLogFilePathName.c_str()); // remove old error log, if there

	if ((DBFiles[ErrLog]->fp = fopen (errLogFilePathName.c_str(), "wt")) == NULL) {
		wxString msg;
		msg.Printf ("GedWiseFrame::OnBtnStart \n\nError #GW2 opening: \n%s. \n",
			errLogFilePathName.c_str());
		wxLogSysError (msg);
		AddLogEntry (msg);
		return;
     	}

	// start processing the selected GEDCOM file
	StartProcessing();

	// close error log
	fclose (DBFiles[ErrLog]->fp);
	DBFiles[ErrLog]->fp = NULL;

	// show warning message if errors found after file processing.
	if (ErrCnt > 0 && !m_bCanceled && !m_bBadExit) {
		
		wxString msg;
		
		msg.Printf (IDS_ERR_DATA, errLogFilePathName.c_str());
		
		response = wxMessageBox (msg, IDS_TTL_GEDCOMERROR, 
			wxYES_NO | wxICON_EXCLAMATION);
		
		if (response == wxYES) {

			ExecuteFile (errLogFilePathName);
			} 
		}

	// set cursor at back end of text in file name edit box.
	m_txGedFileName->SetInsertionPointEnd();
	m_txGedFileName->SetFocus();
	
	// close and re-open the logfile to save results to this point.
	if (LogfileP) {
		fclose (LogfileP);
		LogfileP = NULL;
		}

	if ((LogfileP = fopen (m_strLogFileName.c_str (), "at")) == NULL) {
		wxString msg;
		msg.Printf ("GedWiseFrame::OnBtnStart \n\nError #GW3 opening: \n%s. \n",
			m_strLogFileName.c_str());
		wxLogSysError (msg);
		AddLogEntry (msg);
		SetBadExit ();
  		return;
    	} 

}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ExecuteFile
//
// Description:	Executes a file by checking the MIME type, given the extension of the
//				file name.
//
// Parameters:	-> filePathName - full path and name of file to execute.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::ExecuteFile (wxString& filePathName)

{
	#if defined (__WXMSW__)
	
	ShellExecute (NULL, "open", filePathName, NULL,NULL, SW_SHOWNORMAL);
	

	#else

	bool		retValue = false; 
	wxFileName	fname = filePathName;
	wxString	ext   = fname.GetExt();                                                         
	wxFileType* ft	  = wxTheMimeTypesManager->GetFileTypeFromExtension (ext);
	wxString	cmd;

	if (ft) {
		
		retValue = ft->GetOpenCommand (&cmd, wxFileType::MessageParameters	(filePathName, ""));
		
		delete ft;                                                                    

		if (retValue) { 
       

			#if defined (__WXMAC__)
			
			// we need to format the command line for osx as follows:
			// "open '/Applications/Palm/HotSync Manager' '/Users/danielrencricca/Documents/Palm/Users/Daniel Rencricca/Files to Install/GenDB-Rend.pdb'"

			cmd.Replace (" <", "' ", true);
			cmd.Prepend ("open '");
			
			#endif


			retValue = (wxExecute (cmd) != 0);

			if (!retValue)
				wxLogSysError ("GedWiseFrame::ExecuteFile\n\n" IDS_ERR_FOPEN, filePathName.c_str());
			}

		}

	if (!retValue) {
		cmd.Printf ("%s %s", CMD_EDIT, filePathName.c_str());
		wxExecute (cmd);
		}

	#endif // defined (__WXMSW__)
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnBtnWWW
//
// Description:	Handles user clicking on Internet button.  Opens web browser and
//				goes to www.BatteryParkSoftware.com URL.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnBtnWWW (wxCommandEvent& WXUNUSED(event))
{
	GoToURL (WEBSITE_URL);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GoToURL
//
// Description:	Opens the default browser and goes to the given url.
//
// Parameters:	url -	the URL to go to.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::GoToURL (wxString url)                                       
{     
 	#if defined (__WXMSW__)

	ShellExecute (NULL, "open", url, NULL,NULL, SW_SHOWNORMAL);
	
	#else
	
    bool		retValue = false;
	wxString	cmd;

	wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension ("html");

    if (ft) {

        retValue = ft->GetOpenCommand (&cmd, wxFileType::MessageParameters(url, ""));
		
        delete ft;                                                                    
                                                                                      
        if (retValue) { 
			
            retValue = (wxExecute (cmd) != 0);

			}                                                                             
		}

	if (!retValue) {  // as last resort, try openning 'firefox'
		cmd.Printf ("%s %s", CMD_BROWSE, url.c_str());
		wxExecute (cmd);
		}

	#endif // defined (__WXMSW__)
}           

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	SetControlStatus
//
// Description:	Hides or shows the dialogs checkboxes and buttons and edit boxes during
//				the processing phase (after the Start Button is pressed).
//
// Parameters:	show - true to show the items and false to hide them.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::SetControlStatus (bool show)
{
	m_btnStart->Enable (show);
	m_btnExit->Enable (show);
	m_btnFile->Enable (show);

	m_txGedFileName->Enable (show);
	m_txDbNameLb->Enable (show);
	m_txDbName->Enable (show);

	if (m_btnDir)
		m_btnDir->Enable (show);

	if (m_txDbDir)
		m_txDbDir->Enable (show);

	#if defined (__WXMSW__) || defined (__WXMAC__)

	if (m_cbHotUser)
		m_cbHotUser->Enable (show);

	if (m_bHotUserFound && m_btnInst)
		m_btnInst->Enable (show);

	#endif //defined (__WXMSW__) || defined (__WXMAC__)
		

	m_PanelOpts->Enable (show);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	EstimateDBSize
//
// Description:	Estimates the size of a pdb file on the handheld.
//
// Parameters:	-> dbName - user name of a database.
//
// Returns:		Nothing.
//
// CHECK IF THIS WORKS ON NVFS DEVICES!!!
//
/////////////////////////////////////////////////////////////////////////////////////////
int GedWiseFrame::EstimateDBSize (wxString& dbName) 
{
	wxString	filePathName;
	FILE*		fi;
	DWORD		totSzB = 0;	// size of file in bytes
	int			totSzK = 0; // size of file in kilobytes
	WORD		recCnt;
	WORD		recCntF;
	
	filePathName = m_strPalmAppDir + dbName + PDB_DB_FILE;
	if ((fi = fopen (filePathName.c_str(), "rb")) == NULL)
		return 0;

	fseek (fi, PBD_REC_CNT_POS, SEEK_SET);
	fread ((char*) &recCntF, sizeof (WORD), 1, fi);
	
	#if defined (__WXMAC__)
	
	recCnt = recCntF; 

	#else

	recCnt = SwapShort (recCntF);  // number stored in little-endian for HH
	
	#endif
	
	fseek (fi, 0, SEEK_END);
	totSzB = ftell (fi);
	totSzB+= recCnt*8; // add room for one long per mega-record
	totSzB+= recCnt*4; // add 4 more bytes per records...not sure why but it works

	fclose (fi);

	totSzK = totSzB / 1024;  // convert to kilobytes

	return totSzK;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ProgressBarReset
//
// Description:	Resets the progress bar range based on total times progress bar will
//				be updated.
//
// Parameters:	-> totUpd - total times progress bar will be updated during current
//							operation.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::ProgressBarReset (int totUpd)
{
	m_iMaxProgBarUpd = totUpd;
	m_iCurProgBarUpd = 0;
	m_ProgressBar->SetRange (totUpd);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ProgressClearData
//
// Description:	Resets the progress data to zeros and hides the progress bar.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::ProgressClearData (void)
{
	if (!m_ProgressBar)
		return;

	ProgressBarReset (0);

	m_txTotPrct->Clear ();
	m_txTotLine->Clear ();
	m_txTotWarn->Clear ();
	m_txTotPeop->Clear ();
	m_txTotName->Clear ();
	m_txTotFami->Clear ();
	m_txTotEven->Clear ();
	m_txTotNote->Clear ();
	m_txTotSour->Clear ();
	m_txTotRepo->Clear ();
	m_txTime->SetLabel ("");
	m_txPercent->SetLabel ("");
	m_ProgressBar->Hide ();
	UpdateProgressMessage ("");

}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ProgressInitData
//
// Description:	Initializes the progress data and starts the timer.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::ProgressInitData (void)
{
	ProgressClearData ();

	m_ProgressBar->Show ();

	m_Timer.SetOwner (this, ID_TIMER);
	m_Timer.Start (200);
	m_TimeStart = wxDateTime::Now ();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ProgressStepIt
//
// Description:	Increments the progress bar and percent completed.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::ProgressStepIt (void)
{
	m_iCurProgBarUpd++;
	
	m_ProgressBar->SetValue (m_iCurProgBarUpd);

    ProgressUpdatePercent ();

    wxASSERT (m_iCurProgBarUpd <= m_iCurProgBarUpd);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ProgressStop
//
// Description:	Stopes the timer for the progress box updates.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::ProgressStop (void)
{
	m_Timer.Stop ();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ProgressUpdateData
//
// Description:	This function is called by OnTimer to update all the counts in the
//				progress box and to update the elapsed time.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::ProgressUpdateData (void)
{
	wxString strHld;

	m_txTotPrct->SetValue (strHld.Format ("%u", LineCnt));
	m_txTotLine->SetValue (strHld.Format ("%u", TOTLINES));
	m_txTotWarn->SetValue (strHld.Format ("%u", ErrCnt));
	m_txTotPeop->SetValue (strHld.Format ("%u", IndiCnt));
	m_txTotName->SetValue (strHld.Format ("%u", NameCnt));
	m_txTotFami->SetValue (strHld.Format ("%u", FamiCnt));
	m_txTotEven->SetValue (strHld.Format ("%u", EvenNoCnt));
	m_txTotNote->SetValue (strHld.Format ("%u", NoteCnt));
	m_txTotSour->SetValue (strHld.Format ("%u", SourCnt));
	m_txTotRepo->SetValue (strHld.Format ("%u", RepoCnt));

	// set elapsed time
	wxTimeSpan timeSpan = wxDateTime::Now () - m_TimeStart;
	m_txTime->SetLabel (timeSpan.Format ("%M:%S"));

	ProgressUpdatePercent ();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	ProgressUpdatePercent
//
// Description:	Calculates the percent completed and updates the percent field in the
//				progress box.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::ProgressUpdatePercent (void)
{
    int nPercent;
    
	int nDivisor = m_iMaxProgBarUpd;

    wxASSERT (nDivisor>0);

	int nDividend = m_iCurProgBarUpd;
    
	wxASSERT (nDividend>=0);   // Current position should be greater than m_nLower

    nPercent = (nDividend * 100) / nDivisor;

	wxASSERT (nPercent <= 100);

     // Since the Progress Control wraps, we will wrap the percentage
     // along with it. However, don't reset 100% back to 0%
    if (nPercent!=100)
		nPercent %= 100;

    // Display the percentage
    wxString strBuf;
	
	m_txPercent->SetLabel (strBuf.Format ("%d%c",nPercent,'%'));
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	OnTimer
//
// Description:	Called by system according to the intervals set in ProgressInitData.
//
// Parameters:	None.
//
// Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::OnTimer (wxTimerEvent& event)
{
	ProgressUpdateData();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function:	WriteDataToLog
//
// Description:	Writes data to log file.
//						
// Parameters:	-> dataNo - specifies which data to write to log file.
//
// Returns:		true if written successfully else false.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::WriteDataToLog (int dataNo)
{
	wxString	msg;
	wxString	data;
	wxDateTime	t;

	switch (dataNo)
		{
		case eStartProcessing:
			{
			msg.Printf ("\n%s\n**** Started processing GEDCOM file: %s\n",
				LOG_START_STR, m_strFileName.c_str());
			AddLogEntry (msg);
			wxDateTime t = wxDateTime::Now ();
			msg = t.Format ("Processing started: %B %d, %Y at %H:%M:%S\n\n");
			AddLogEntry (msg);
			
			//#if defined (__WXMSW__) || defined (__WXMAC__)
			
			if (m_cbHotUser)
				msg.Printf ("HotSync User Name: %s\n", m_cbHotUser->GetStringSelection ().c_str());
			else
				msg.Printf ("No HotSync Users Found\n");

			AddLogEntry (msg);
			
			//#endif // defined (__WXMSW__) || defined (__WXMAC__)

			msg.Printf ("User Hotsync Install Directory: %s\n", m_strPalmAppDir.c_str());
			AddLogEntry (msg);
			msg.Printf ("User GedWise Working Directory: %s\n", m_strWorkDir.c_str());
			AddLogEntry (msg);
			msg.Printf ("User OS: %s\n", m_strWindowsVersion.c_str());
			AddLogEntry (msg);
			break;
			}

		case eEndProcessing:
			msg.Printf ("\nProcessed %d lines of %d total lines.\nProcessed %d individuals, "
				"%d families, %d events, %d notes,\n%d citations, %d sources, "
				"%d sr-citations and %d repositories.\n", TOTLINES, LineCnt, IndiCnt, FamiCnt,
				EvenNoCnt, NoteCnt, SCitNoCnt, SourCnt, RCitNoCnt, RepoCnt);
			AddLogEntry (msg);
			msg.Printf ("There were %d data warnings found.\n", ErrCnt);
			AddLogEntry (msg);
			msg.Printf ("\n**** Finished processing GEDCOM file %s\n", m_strFileName.c_str());
			AddLogEntry (msg);
			t = wxDateTime::Now();
			msg = t.Format ("Processing ended: %B %d, %Y at %H:%M:%S\n\n");
			AddLogEntry (msg);
			break;

		case eCancelledProcessing:
			msg.Printf ("**** Processing of %s file canceled by user.  \n\n", m_strFileName.c_str());
			AddLogEntry(msg);
			break;

		case eErrProcessing:
			msg.Printf ("Fatal Error processing GEDCOM file %s.\n",	m_strFileName.c_str());
			AddLogEntry (msg);
			break;

		default:
			wxASSERT (false);
			break;
		}
}

/////////////////////////////////////////////////////////////////////////////////////////
//	Function:		DecodeErrorCode
//
//	Description:	Displays a Message Box detailing the given error code.
//
//	Parameters		-> errCode - the result of the Application Dialog routine.
//
//	Returns:		Nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void GedWiseFrame::DecodeErrorCode (int errCode)
{
	wxString	msg;

	switch (errCode)
	{
	case	0:
		// Success... No Error!
		return;

	case ERR_APP_MOVE_INSTALL_FLD:
		msg = _(IDS_ERR_APP_MOVE_INSTALL_FLD);
		break;

	#ifdef __WXMSW__  // the following errors are returne by InstAide.dll

	case ERR_APP_INVALID_INSTAIDE:
		msg = _(IDS_ERR_APP_INVALID_INSTAIDE);
		break;
	
	case ERR_APP_INSTAIDE_NOT_FOUND:
		msg = _(IDS_ERR_APP_INSTAIDE_NOT_FOUND);
		break;

	case ERR_PILOT_BUFSIZE_TOO_SMALL:
		msg = _(IDS_ERR_PILOT_BUFSIZE_TOO_SMALL);
		break;

	case ERR_PILOT_INVALID_USER:
		msg = _(IDS_ERR_PILOT_INVALID_USER);
		break;

	case ERR_PILOT_INVALID_USER_INDEX:
		msg = _(IDS_ERR_PILOT_INVALID_USER_INDEX);
		break;

	case ERR_PILOT_NO_USERSDAT_FILE:
		msg = _(IDS_ERR_PILOT_NO_USERSDAT_FILE);
		break;

	case ERR_PALM_UNABLE_TO_CREATE_NEW_FILE:
		msg = _(IDS_ERR_PALM_UNABLE_CREATE_NEW_FILE);
		break;

	case ERR_PILOT_NO_USERS:
		msg = _(IDS_ERR_PILOT_NO_USERS);
		break;

		
	#elif defined (__WXMAC__)

	case kUserMgrCorruptUsersFileErr: 
		msg = _(IDS_ERR_UM_CORRUPT_USERDAT_FILE);
		break;

	case kUserMgrUserNotFoundErr:
		msg = _(IDS_ERR_PILOT_NO_USERS);
		break;
	
	case kUserMgrInvalidUserNameErr:
		msg = _(IDS_ERR_PILOT_INVALID_USER);
		break;

	case kUserMgrInvalidUserIndexErr:
		msg = _(IDS_ERR_PILOT_INVALID_USER_INDEX);
		break;

	#endif // defined (__WXMSW__) || defined (__WXMAC__)


	default:
		msg = "An unknown error occurred!\n";
		break;
	}

	msg+=	_("\nYour GedWise database will be saved in the following folder:\n\n"
			+ m_strPalmAppDir +
			"\n\nYou can manually transfer your database to your Handheld \ndevice "
			"after processing is completed.\n");

	wxMessageBox (msg, _THIS_FILE, wxOK | wxICON_EXCLAMATION);
	AddLogEntry (msg);
}


/*

	//typedef int (WINAPI *PltInstallFilePtr)(TCHAR *pUser, TCHAR *pFileSpec);
	//typedef int (WINAPI *PltRemoveInstallFilePtr)(TCHAR *pUser, TCHAR *pFileSpec);

	typedef OSStatus (*UmCopyUserDirectoryPtr) (PalmUserID userID, CFURLRef* oUserDirectory);


	#if !defined (__WXMSW__)
 	// Initialize the help controller.
	wxString	helpFilePathName;
	helpFilePathName = m_strWorkDir + MANUAL_FILE_UNI;
    if (! m_help->AddBook(wxFileName (helpFilePathName, wxPATH_UNIX)))
	  	wxMessageBox("Failed adding GedWise help book");
	#endif // !defined (__WXMSW__)


  
	OSStatus	errCode = noErr;
	FSSpec		fileSpec;

	// get the User ID
	Ptr			ptrUmGetUserID;
	PalmUserID	oUserID;

	errCode = FindSymbol (m_LibHandle, "\pUmGetUserID", &ptrUmGetUserID, 0);

	UmGetUserIDPtr pfnUmGetUserID = (UmGetUserIDPtr) MachOFunctionPtrForCFMFunctionPtr(ptrUmGetUserID);

	if (errCode) return true;

	errCode = pfnUmGetUserID (m_iPalmUserNo, &oUserID);

	if (errCode) return true;

	// get the user directory where we want to move the pdb database file
	Ptr ptrUmGetFilesToInstallFolderSpec;
	
	errCode = FindSymbol (m_LibHandle, "\pUmGetFilesToInstallFolderSpec", &ptrUmGetFilesToInstallFolderSpec, 0);

	if (errCode) return true;

	UmGetFilesToInstallFolderSpecPtr pfnUmGetFilesToInstallFolderSpec = (UmGetFilesToInstallFolderSpecPtr) MachOFunctionPtrForCFMFunctionPtr( ptrUmGetFilesToInstallFolderSpec );
	
	errCode = pfnUmGetFilesToInstallFolderSpec (oUserID, &fileSpec);

	if (errCode) return true;

	// get the user directory where we want to move the pdb database file
	Ptr ptrUmCopyUserDirectory;

	errCode = FindSymbol (m_LibHandle, "\pUmCopyUserDirectory", &ptrUmCopyUserDirectory, 0);

	if (errCode) return true;

	UmCopyUserDirectoryPtr pfnUmCopyUserDirectory = (UmCopyUserDirectoryPtr) MachOFunctionPtrForCFMFunctionPtr(ptrUmCopyUserDirectory);

	CFURLRef userDirectory;
	
	errCode = pfnUmCopyUserDirectory (oUserID, &userDirectory);

	if (errCode) return true;

// DO I NEED TO DUMP CFURLREF????
	
	CFStringRef filePathName;

	filePathName = CFURLGetString (userDirectory);

	//CFStringGetCStringPtr

	// NOW COPY FILE TO FILES TO INSTALL DIRECTORY


  //errCode = FSMoveObject (&srcFSRef, &desFSRef, NULL);
//CFStringRef filePathName1;
//
//filePathName1 = CFURLGetString (userDirectory);
//
//wxString dstPathFileName = buffer;
//dstPathFileName += "/" LOGFILE; 
//
//FSCopyObject 
//
//	FSRef srcRef;
//
//if (!CFURLGetFSRef (userDirectory, &srcRef))
//     return bdNamErr;
//
//char buffer[256];
//
//   Boolean resolveAgainstBase,
//   UInt8 *buffer,
//   CFIndex maxBufLen
//);
//CFURLGetFileSystemRepresentation (userDirectory, true, (UInt8*) &buffer, 256);
//
//char strSrc[300];
//
//strcpy (strSrc, srcPathFileName.c_str());
//Boolean CFURLGetFSRef (userDirectory, &fsRef)
//   CFURLRef url,
//   FSRef *fsRef
//);

*/

/*	#if defined (__WXMSW__)

	int			errCode;
	char		pUserBuffer[MAX_PATH+1] = ""; // install folder..eg C:\Program Files\Palm\User\Install
	char		szInstappdPath[MAX_PATH+1];   // full path and name of file to be installed
	wxString	filePathName;

	wxDYNLIB_FUNCTION (PltInstallFilePtr, PltInstallFile, m_InstAideDLL);

	filePathName = GetWorkDirectory () + m_strDbName + PDB_DB_FILE;
	
	strcpy (szInstappdPath, filePathName.c_str());

	strncat (pUserBuffer, m_cbHotUser->GetStringSelection ().c_str(), MAX_PATH);

	errCode = pfnPltInstallFile (pUserBuffer, szInstappdPath);

	if (errCode) {
		DecodeErrorCode (ERR_APP_MOVE_INSTALL_FLD);
		}
	else {
		remove (szInstappdPath);
		}

	return (errCode == 0);
*/

/*

	// get the User ID
	
	OSStatus	errCode = noErr;
	FSSpec		fileSpec;
	Ptr			ptrUmGetUserID;

	errCode = FindSymbol (m_LibHandle, "\pUmGetUserID", &ptrUmGetUserID, 0);

	UmGetUserIDPtr pfnUmGetUserID = (UmGetUserIDPtr) MachOFunctionPtrForCFMFunctionPtr(ptrUmGetUserID);

	if (errCode) return;

	PalmUserID oUserID;
	errCode = pfnUmGetUserID (m_iPalmUserNo, &oUserID);

	if (errCode) return;
	
	// get the user directory where we want to remove the pdb database file
	
	Ptr ptrUmGetFilesToInstallFolderSpec;
	
	errCode = FindSymbol (m_LibHandle, "\pUmGetFilesToInstallFolderSpec", &ptrUmGetFilesToInstallFolderSpec, 0);

	if (errCode) return;

	UmGetFilesToInstallFolderSpecPtr pfnUmGetFilesToInstallFolderSpec = (UmGetFilesToInstallFolderSpecPtr) MachOFunctionPtrForCFMFunctionPtr( ptrUmGetFilesToInstallFolderSpec );
	
	errCode = pfnUmGetFilesToInstallFolderSpec (oUserID, &fileSpec);

	if (errCode) return;
*/

/*
	#if defined (__WXMSW__)

	char	pUserBuffer[MAX_PATH+1] = "";
	char	szFileName[PC_FNAME_SZ+1];

	wxDYNLIB_FUNCTION (PltRemoveInstallFilePtr, PltRemoveInstallFile, m_InstAideDLL);
	
	strcpy (szFileName, dbName.c_str());

	strncat (pUserBuffer, m_cbHotUser->GetStringSelection ().c_str(), MAX_PATH);

	pfnPltRemoveInstallFile (pUserBuffer, szFileName);

	#elif defined (__WXMAC__)

  
	#else // not WXMSW or WXMAC (must be Linux)
	wxString	filePathName;
	filePathName = m_strWorkDir + dbName;
	remove (filePathName.c_str());
	#endif

void GedWiseFrame::LoadPalmUsers (void)
{
	#if defined (__WXMSW__) || defined (__WXMAC__)

	int			userCount = 0;
	bool		loadedInstAidDll = false;
	wxString	pUserBuffer;

	m_bHotUserFound = false; // init

	// -- Load Palm Install Aide Library. -- 
	if (!LoadLibrary ())
		return false;
	
	if (!GetPalmUserList (&userCount))
		return false;

	//if (LoadLibrary ()) {
	//	loadedInstAidDll = true; // library successfully loaded
	//	}

	// -- Get a list of Hotsync users registered on this computer. --
	//if (loadedInstAidDll && (GetPalmUserList (&userCount))) {
	//	m_bHotUserFound = true; // users found
	//	}
	//else {
		//userCount = 0;
		//m_cbHotUser->Clear ();
		//m_cbHotUser->Append ("No Hotsync Users Found"); // add string to list box
		//m_btnInst->Enable (false); // hide the palm installer button.
		//}

	// -- Set the selected Palm username from the pick list. --
	if (m_iPalmUserNo >= userCount)
		m_iPalmUserNo = 0;
	
	m_cbHotUser->SetSelection (m_iPalmUserNo);

	pUserBuffer = m_cbHotUser->GetStringSelection ();

	// -- Get the Palm GedWise directory. --
	if (!GetAppPalmDirectory (pUserBuffer)) {
		wxASSERT (false);
		return false;
		}

	// -- Get the Palm GedWise directory. --
	//if (m_bHotUserFound && (!GetAppPalmDirectory (pUserBuffer))) {
	//	m_bHotUserFound = false; // no users found
	//	wxASSERT (false);
	//	}
	//m_bHotUserFound = true; // if we got this far, a Palm user was found.

	#else // must be Linux
	
	//m_txDbDir->SetValue (m_strWorkDir);
	//m_txDbDir->SetInsertionPointEnd ();

	#endif // defined (__WXMSW__) || defined (__WXMAC__)


	// If error getting Hotsync user, then save database the work directory.
	if (!m_bHotUserFound) {

		m_btnInst->Enable (false); // hide the palm installer button.

		m_cbHotUser->Destroy(); // remove HotSync User Selector
		m_cbHotUser = NULL;

		//// Add Database Location Definitions
		m_bxHotUser->SetLabel (_T("Location to Save Database"));
		
		m_txDbDir = new wxTextCtrl (this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize,  wxTE_READONLY|wxTE_LEFT);
		m_txDbDir->SetToolTip (_T(IDS_DIR_LOC));
		DbDirBoxSizer->Add(m_txDbDir, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

		m_btnDir = new wxBitmapButton (this, ID_BTN_DIR, wxXmlResource::Get()->LoadBitmap("FileNorm"), wxDefaultPosition, wxSize(36, 36), wxNO_BORDER);
		#if defined (__WXMSW__)
		m_btnDir->SetBitmapSelected (wxXmlResource::Get()->LoadBitmap("FileSelected"));
		#endif
		m_btnDir->SetBitmapDisabled (CreateBitmapDisabled (wxXmlResource::Get()->LoadBitmap("FileNorm")));
		m_btnDir->SetToolTip (_T(IDS_DIR));
		DbDirBoxSizer->Add(m_btnDir, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

		m_txDbDir->SetValue (m_strPalmAppDir);
		}
}

*/	

