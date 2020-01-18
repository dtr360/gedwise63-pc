/////////////////////////////////////////////////////////////////////////////////////////
// Program:         GedWise 6.3
//
// File:            GedWise.h
//
// Written by:      Daniel T. Rencricca
//
// Last Revision:   March 9, 2005
//
// Description:     Header file for GedWise.cpp.
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef _GEDWISE_H_
#define _GEDWISE_H_

#include "defines.h"
#include "stdwx.h"

/////////////////////////////////////////////////////////////////////////////////////////
// Defines and Enumerated Types
/////////////////////////////////////////////////////////////////////////////////////////

#define INSTAIDE_LIB_NAME       "Instaide.dll"
#define HOTSYNC_LIB_NAME        "HotSync Libraries"
//#define HOTSYNC_LIB_NAME      "\p::Resources:HotSync Libraries" // pascal format
#define PDB_INSTALL_FNAME       "Instapp.exe"
#define PALM_USER_NAME_LEN      256
#define MAC_RESOURCES_DIR       "Resources" // resources directory for MAC

// IDs for the controls and the menu commands
typedef enum
{
    ID_FRAME        = wxID_HIGHEST + 1,
    
    ID_BTN_START,
    ID_BTN_CANCEL,
    ID_BTN_WWW,
    ID_BTN_BUY,
    ID_BTN_FILE,
    ID_BTN_DIR,
    ID_BTN_PALM,

    ID_TXT_GFILE,
    ID_TXT_DBNAME,
    ID_TXT_DIR,

    ID_CB_HOTUSER,

    ID_EX_SOUR,
    ID_EX_REPO,
    ID_EX_REFN,
    ID_EX_NOTE,
    ID_MAX_NOTE,
    
    ID_TIMER,
};

typedef enum {
    eStartProcessing,
    eParseGedcomFile,
    eEndProcessing,
    eCancelledProcessing,
    eErrProcessing
    };

#define GEDWISEFRAME_STYLE      wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMINIMIZE_BOX|wxTAB_TRAVERSAL|wxCLIP_CHILDREN
#define GEDWISEFRAME_TITLE      "GedWise"
#define GEDWISEFRAME_SIZE       wxDefaultSize

// String Definitions
#define IDS_CANCEL              "Cancel GEDCOM file processing"
#define IDS_START               "Start GEDCOM file processing"
#define IDS_EXIT                "Exit GedWise"
#define IDS_HELP                "GedWise help"
#define IDS_INFO                "GedWise information"
#define IDS_BUY                 "Purchase GedWise on the web"
#define IDS_WWW                 "Go to GedWise website"

#define IDS_GFILE               "Enter name of the GEDCOM file to process."
#define IDS_FILE                "Browse for a GEDCOM file to process."
#define IDS_DIR                 "Browse for a folder in which to save your GedWise database."
#define IDS_DBNAME              "Enter a name for your database, or use the default name."
#define IDS_INST                "View databases waiting to be transferred to your handheld device."
#define IDS_HOTUSER             "Select the user to whose handheld you want to transfer the database."
#define IDS_DIR_LOC             "Your GedWise database will be saved to this directory."

#define IDS_SOUR                "Check this box to exclude all source information from the database."
#define IDS_REPO                "Check this box to exclude all repository information from the database."
#define IDS_REFN                "Check this box to exclude all REFN tags from the database."
#define IDS_ORD                 "Check this box to exclude all LDS Ordinance information from the database."
#define IDS_NOTE                "Check this box to exclude all notes from the database."
#define IDS_SORT_EVEN           "Check this box to sort undated events to the bottom of the Event List."
#define IDS_MAX_NOTE            "Enter your desired maximum characters per note up to 35,000."
#define IDS_INS_SPACE           "Check this box to insert a space before each CONC line of text."
#define IDS_SORT_CHIL           "Check this box to sort children according to the order in the GEDCOM file."
#define IDS_INS_TITLE           "Check this box to include the title, if any, as part of individual's name."

#define IDS_STAT_SELECT         "Select the Source GEDCOM File to process."
#define IDS_STAT_VERSION        "GedWise 6.3"
#define IDS_STAT_PROCESS        "Processing GEDCOM file..."
#define IDS_STAT_CANCELED       "Processing canceled by user."
#define IDS_STAT_COMPLETE       "Processing successful... sync now to transfer database(s) to handheld."
#define IDS_STAT_DBNAME         "Choose a name for the Database (or use default) and press Start."
#define IDS_STAT_ERR            "Fatal Error found during processing"

#define IDS_PROG_ERROR          "Error: GEDCOM Processing Terminated."
#define IDS_PROG_PARSING        "Parsing GEDCOM File"
#define IDS_PROG_INDEX          "Creating Index"
#define IDS_PROG_PALMDB         "Creating Palm Database"
#define IDS_PROG_COMPLETE       "GEDCOM Processing Complete"
#define IDS_PROG_DBSIZE         "Est. Database Size: %dK"

#define IDS_ERR_DATA            "Data was found during processing that does not conform \nto the GEDCOM 5.5 standard. This data was ignored and \nexcluded from your database. The GEDCOM file lines that \ncontain the bad data are recorded in the following file:\n\n%s\n\nWould you like to view this file now?"
#define IDS_ERR_VERSION         "The GEDCOM file you have selected does not appear to be \nversion 5.5. If you choose to continue, errors could be \nfound during processing that cause GedWise to operate \nimproperly on your handheld device.\n\nDo you want to continue processing this GEDCOM file?\n"
#define IDS_ERR_FILEEXIST       "File '%s' \ndoes not exist.    \n"
#define IDS_ERR_DIREXIST        "The location you specified to save your database, \n'%s', \ndoes not exist.    \n"
#define IDS_ERR_FOPEN           "Error: unable to open file     \n\n%s\n"
#define IDS_ERR_NO_INDI         "Error: There are no individuals in the GEDCOM file.    \n"
#define IDS_ERR_NO_DB           "There currently are no GedWise databases in your Hotsync install folder.     \n\nOnce you create a GedWise database, you can use this function  \nto view or remove databases waiting to be installed to your \nhandheld device."

// Palm User Manager and FindSymbol (Mac) error messages.
#define IDS_ERR_APP_INVALID_INSTAIDE        "Error: Invalid InstAide.dll!\n"
#define IDS_ERR_APP_INSTAIDE_NOT_FOUND      "Error: InstAide.dll not found! You may be able to fix the problem by\n" "re-installing the GedWise application.\n"
#define IDS_ERR_APP_MOVE_INSTALL_FLD        "Error: unable to move your database into the Palm Install folder\n"
#define IDS_ERR_PILOT_BUFSIZE_TOO_SMALL     "User Manager Error: buffer size too small!\n"
#define    IDS_ERR_PILOT_INVALID_USER       "User Manager Error: invalid user name!\n"
#define IDS_ERR_PILOT_INVALID_USER_INDEX    "User Manager Error: invalid user index!\n"
#define IDS_ERR_PILOT_NO_USERSDAT_FILE      "User Manager Error: no user data file specified\n" "in Palm configuration entries!\n"
#define IDS_ERR_PALM_UNABLE_CREATE_NEW_FILE "User Manager Error: unable to locate user data file! Open the \nPalm Desktop application and create a Hotsync user.\n"
#define IDS_ERR_PILOT_NO_USERS              "User Manager Error: no users are registered with the HotSync \nManager! Open the Palm Desktop application to register a user.\n"
#define IDS_ERR_UM_CORRUPT_USERDAT_FILE     "User Manager Error: the user's data file is corrupt.\n"
#define IDS_ERR_CONNECTION_NOT_FOUND        "FindSymbol Error: Connection ID is not valid"
#define IDS_ERR_UM_SYMBOL_NOT_FOUND         "FindSymbol Error: Symbol was not found in connection."

#define IDS_WRN_OVERWRITE   "Database file %s already exists!   \nWould you like to overwrite it?"
#define IDS_WRN_DBNAME      "You can use only letters and numbers    \nin the database name.   \n"
#define IDS_WRN_DBNAME_LEN  "The database name cannot exceed 6 characters    \nin length.   \n"
#define IDS_WRN_NOTESZ      "You must enter an number between 0 and %d.    "

#define IDS_MSG_CANCEL_TTL  "Processing Canceled"
#define IDS_MSG_CANCEL      "GEDCOM File Processing Canceled by User.    "

#define IDS_TTL_DATAENTRY   "Data Entry Error"
#define IDS_TTL_FILEERROR   "File Error"
#define IDS_TTL_DIRERROR    "Directory Error"
#define IDS_TTL_OVERWRITE   "Confirm File Overwrite"
#define IDS_TTL_GEDCOMERROR "GEDCOM Data Warning"
#define IDS_TTL_VERSION     "GEDCOM Version Warning"

#define IDS_GED_WILDCARD    "GEDCOM Files (*.ged)|*.ged|All Files (*.*)|*.*"
#define IDS_SEL_GEDCOM      "Select GEDCOM file..."
#define IDS_SEL_DIRECTORY   "Select location to save your database..."
#define IDS_SEL_DB          "View GedWise database(s) that you have created..."
#define IDS_DB_WILDCARD     "GedWise Database Files (*-Rend.pdb)|*-Rend.pdb|All Files (*.*)|*.*"

// Palm OS Errors
#define ERR_APP_BASE                    -1200
#define ERR_APP_INVALID_INSTAIDE        (ERR_APP_BASE-1)
#define ERR_APP_INSTAIDE_NOT_FOUND      (ERR_APP_BASE-2)
#define ERR_APP_MOVE_INSTALL_FLD        (ERR_APP_BASE-3)

/*!
 * Compatibility
 */
#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

#define CMD_BROWSE   "firefox"
#define  CMD_EDIT    "gedit"


/////////////////////////////////////////////////////////////////////////////////////////
// GedWiseApp Class
/////////////////////////////////////////////////////////////////////////////////////////
class GedWiseApp : public wxApp
{
    DECLARE_CLASS (GedWiseApp)

public:
    // Constructors
    GedWiseApp::GedWiseApp (void);

    //static void PeekAndPump (bool canceled = false);
    
    // override base class virtuals
    // ----------------------------
    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit (void);

private:

};

DECLARE_APP (GedWiseApp)


/////////////////////////////////////////////////////////////////////////////////////////
// GedWiseFrame Class
/////////////////////////////////////////////////////////////////////////////////////////
class GedWiseFrame : public wxFrame
{
    DECLARE_CLASS (GedWiseFrame)

public:

    /// Constructors
    GedWiseFrame (void);
    GedWiseFrame (wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = GEDWISEFRAME_TITLE, const wxPoint& pos = wxDefaultPosition, const wxSize& size = GEDWISEFRAME_SIZE, long style = GEDWISEFRAME_STYLE);
    ~GedWiseFrame (void);
  
    bool        Canceled (void) {return m_bCanceled;}   // Checks if "Cancel" button hit?
    bool        CheckStatus (bool showMessage = TRUE);
    bool        Create (wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = GEDWISEFRAME_TITLE, const wxPoint& pos = wxDefaultPosition, const wxSize& size = GEDWISEFRAME_SIZE, long style = GEDWISEFRAME_STYLE);
    bool        GetBadExit (void) {return m_bBadExit;}
    wxString&   GetDbName (void) {return m_strDbName;}
    wxString&   GetGedFileName (void) { return m_strFileName;}
    int         GetUserNoteSize (void) { return m_iUserNoteSize;}
    wxString&   GetWorkDirectory (void) { return m_strWorkDir;}
    void        GoToURL (wxString url);
    bool        IndiRefSwap (void);
    void        InstallFile (void);
    bool        IsExclSour (void) {return m_chExSour->GetValue();}
    bool        IsExclRepo (void) {return m_chExRepo->GetValue();}
    bool        IsExclRefN (void) {return m_chExRefN->GetValue();}
    bool        IsExclOrdn (void) {return m_chExOrd->GetValue();}
    bool        IsExclNote (void) {return m_chExNotes->GetValue();}
    bool        IsInslTitl (void) {return m_chInsTitle->GetValue();}
    bool        IsInslSpac (void) {return m_chInsSpace->GetValue();}
    bool        IsSortUndEvenBot (void) {return m_chSortEven->GetValue();}
    bool        IsSortChilPerGed (void) {return m_chSortChil->GetValue();}
    void        ProgressBarReset (int totCalls);
    void        ProgressUpdateData (void);
    void        SetBadExit (void) {m_bBadExit = true;}
    void        SetCanceled (void) {m_bCanceled = true;}
    void        UpdateProgressMessage (const char* msg);

private:
    
    // Button and Text Box Functions
    void        OnAbout (wxCommandEvent& event);
    void        OnBtnBuy (wxCommandEvent& WXUNUSED(event));
    void        OnBtnCancel (wxCommandEvent& WXUNUSED(event));
    void        OnBtnDirOpen (wxCommandEvent& WXUNUSED(event));
    void        OnBtnFileOpen (wxCommandEvent& WXUNUSED(event));
    void        OnBtnInst (wxCommandEvent& WXUNUSED(event));
    void        OnBtnStart (wxCommandEvent& WXUNUSED(event));
    void        OnBtnWWW (wxCommandEvent& WXUNUSED(event));
    void        OnChangeDbName (wxCommandEvent& WXUNUSED(event));
    void        OnChangeDirName (wxCommandEvent& WXUNUSED(event));
    void        OnChangeGedFileName (wxCommandEvent& WXUNUSED(event));
    void        OnChangeNoteSize (wxCommandEvent& WXUNUSED(event));
    void        OnExit (wxCommandEvent& event);
    void        OnHelp (wxCommandEvent& WXUNUSED(event));
    void        OnNoteCb (wxCommandEvent& WXUNUSED(event));
    void        OnPaint (wxPaintEvent &event);
    void        OnSize(wxSizeEvent& event);
    void        OnSourCb (wxCommandEvent& WXUNUSED(event));
    void        OnTimer (wxTimerEvent& event);
    void        OnRepoCb (wxCommandEvent& WXUNUSED(event));
    


    // Windows and Mac OS X Functions for getting Hotsync user data
    #if defined (__WXMSW__) || defined (__WXMAC__)
    void        OnSelHotUser (wxCommandEvent& event);
    bool        GetPalmUserList (int *userCount);
    bool        GetAppPalmDirectory  (wxString& pUserBuffer);
    #endif // __WXMSW__

    // Misc Processing Functions
    void        CleanExit(UINT code);     // called when processing cancelled by user
    bool        CombDbFiles (void);
    bool        CountChil (void);
    wxBitmap    CreateBitmapDisabled (const wxBitmap &bitmap);
    bool        CreateIndxDb (void);
    void        CreateControls (); // creates the controls and sizers
    int         EstimateDBSize (wxString& dbName);
    void        ExecuteFile (wxString& filePathName);
    bool        GetPrefs (void); // gets user preferences.
    bool        LoadLibrary (void);
    bool        LoadPalmUsers (void);
    bool        MakePDbFile (void);
    bool        MergeChilDb (void);
    bool        ParseGedcomFile (void);
    bool        ProcessDBFiles (void);
    void        ProgressInitData (void);
    void        ProgressClearData (void);
    void        ProgressStepIt (void);
    void        ProgressStop (void);
    void        ProgressUpdatePercent (void);
    void        RemoveInstallFile (wxString& dbName);
    bool        SavePrefs (void); // saves user preferences.
    void        SetControlStatus (bool show);
    bool        SortDbFiles (void);
    bool        SortChilDb (void);
    bool        SortIndxDb (void);
    void        StartProcessing (void);
    bool        SubEvenNo (void);
    bool        SubRefNum (void);
    void        WriteDataToLog (int dataNo);
    void        UpdateNoteCtls (void);
    void        UpdateStatusBar (const wxString& msg);
    void        DecodeErrorCode (int errCode);
    

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()

protected:

    // Dialog Setup
    wxBoxSizer*         m_bsMain1;
    wxBoxSizer*         m_bsMain2;
    wxBoxSizer*         m_bsMain3;
    wxBoxSizer*         m_bsMain4;
    wxBoxSizer*         m_bsMain5;
    wxBoxSizer*         m_bsButtons1;
    wxBoxSizer*         m_bsButtons2;
    wxBoxSizer*         m_bsProg;
    wxFlexGridSizer*    m_gfPrcDet;
    wxFlexGridSizer*    m_fgOpts1;
    wxFlexGridSizer*    m_fgOpts2;
    wxFlexGridSizer*    m_gfGedDet;
    wxStatusBar*        m_StatusBar;
    
    wxButton*           m_btnStart;
    wxButton*           m_btnCancel;
    wxButton*           m_btnExit;
    wxBitmapButton*     m_btnInfo;
    wxBitmapButton*     m_btnHelp;
    wxBitmapButton*     m_btnWWW;
    wxBitmapButton*     m_btnBuy;
    wxBitmapButton*     m_btnFile; 
    wxBitmapButton*     m_btnDir;
    wxBitmapButton*     m_btnInst;

    // GEDCOM File Entry Box
    wxStaticBox*        m_bxGedFileName;
    wxTextCtrl*         m_txGedFileName;
    
    // Database Name Entry Box
    wxStaticBox*        m_bxDbName;
    wxStaticText*       m_txDbNameLb;
    wxTextCtrl*         m_txDbName;
    
    /// Hotsync User

    wxStaticBoxSizer*    DbDirBoxSizer;

    wxStaticBox*    m_bxHotUser;
    //#if defined (__WXMSW__) || defined (__WXMAC__)
    wxComboBox*         m_cbHotUser;
    //#else
    wxTextCtrl*        m_txDbDir;
    //#endif // defined (__WXMSW__) || defined (__WXMAC__)

    /// Checkbox Options
    wxStaticBox*        m_bxOpts;
    wxPanel*            m_PanelOpts;
    wxCheckBox*         m_chExSour;
    wxCheckBox*         m_chExRepo;
    wxCheckBox*         m_chExRefN;
    wxCheckBox*         m_chExOrd;
    wxCheckBox*         m_chExNotes;
    wxStaticText*       m_chMaxNoteLb;
    wxTextCtrl*         m_chMaxNote;
    wxCheckBox*         m_chSortEven;
    wxCheckBox*         m_chInsSpace;
    wxCheckBox*         m_chSortChil;
    wxCheckBox*         m_chInsTitle;
    
    /// File Processing Details Box
    wxStaticBox*        m_bxPrcDet;
    wxStaticText*       m_txTotPrcLb;
    wxStaticText*       m_txTotLineLb;
    wxStaticText*       m_txTotWarnLb;
    wxStaticText*       m_txTotPeopLb;
    wxStaticText*       m_txTotNameLb;
    wxStaticText*       m_txTotFamiLb;
    wxStaticText*       m_txTotEvenLb;
    wxStaticText*       m_txTotNoteLb;
    wxStaticText*       m_txTotSourLb;
    wxStaticText*       m_txTotRepoLb;

    wxTextCtrl*         m_txTotPrct;
    wxTextCtrl*         m_txTotLine;
    wxTextCtrl*         m_txTotWarn;
    wxTextCtrl*         m_txTotPeop;
    wxTextCtrl*         m_txTotName;
    wxTextCtrl*         m_txTotFami;
    wxTextCtrl*         m_txTotEven;
    wxTextCtrl*         m_txTotNote;
    wxTextCtrl*         m_txTotSour;
    wxTextCtrl*         m_txTotRepo;
    
    wxGauge*            m_ProgressBar;
    wxStaticText*       m_txPrcStat;
    wxStaticText*       m_txTime;
    wxStaticText*       m_txPercent;
    
    /// GEDCOM File Details Box
    wxStaticBox*        m_bxGedDet;
    wxStaticText*       m_txGedVersLb; 
    wxStaticText*       m_txGedDateLb; 
    wxStaticText*       m_txGedSourLb; 
    wxStaticText*       m_txGedLineLb; 
    wxStaticText*       m_txGedVers;
    wxStaticText*       m_txGedDate;
    wxStaticText*       m_txGedSour;
    wxStaticText*       m_txGedLine;

    /// General Frame Layout
    wxBitmap*        m_bmpLogo; 

    bool            m_bBadExit;             // flag for fatal error
    bool            m_bCanceled;            // signals cancel button hit during processing
    FILE*           m_pPrefFile;            // pointer to file containing user preferences
    wxString        m_strPrefFileName;      // holds name of file for user preferences
    wxString        m_strLogFileName;       // holds name of log file
    wxString        m_strPalmDesktopDir;    // palm directory, eg C:\Program Files\Palm
    wxString        m_strFileName;          // GEDCOM file name
    wxString        m_strFilePathName;      // GEDCOM file path and name 
    wxString        m_strPalmAppDir;        // user's GedWise directory.
    wxString        m_strWorkDir;           // e.g. C:\Program Files\GedWise    
    int             m_iPalmUserNo;          // current palm user number.  Default must be 0!!
    UINT            m_iUserNoteSize;        // user preference for note size
    wxString        m_strWindowsVersion;    // description of windows version.
    wxString        m_strDbName;            // 6 character name for database chosen by user
    bool            m_bHotUserFound;

    // Progress Bar and Elapsed Time 
    int             m_iCurProgBarUpd;
    int             m_iMaxProgBarUpd;
    wxTimer         m_Timer;
    wxDateTime      m_TimeStart;

    wxString        m_aHotUsers[1];         // Name of selected Hotsync user

    #ifndef __WXMSW__
    wxHtmlHelpController* m_help;
    #endif

    #if defined (__WXMSW__)
    
    wxDynamicLibrary    m_InstAideDLL;      // The Palm Install Aide DLL file
    
    #elif defined (__WXMAC__)
    
    CFragConnectionID    m_LibHandle;
    
    #endif

};


#ifdef wxUSE_DRAG_AND_DROP // drag and drop only exists in Windows and Mac
/////////////////////////////////////////////////////////////////////////////////////////
// DnDFile Class
/////////////////////////////////////////////////////////////////////////////////////////
class DnDFile : public wxFileDropTarget
{
public:
    DnDFile (wxTextCtrl *pOwner) {m_pOwner = pOwner;}

    virtual bool OnDropFiles (wxCoord x, wxCoord y, const wxArrayString& filenames) {
        m_pOwner->SetValue (filenames[0]);
        m_pOwner->SetInsertionPointEnd ();
        m_pOwner->SetFocus ();
        return true; 
        }

private:
    wxTextCtrl*    m_pOwner;

};
#endif // wxUSE_DRAG_AND_DROP

#endif // _GEDWISE_H_
