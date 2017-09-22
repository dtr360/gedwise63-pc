/////////////////////////////////////////////////////////////////////////////
// Name:        gedwise.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     09/06/05 17:56:54
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _GEDWISE_H_
#define _GEDWISE_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "gedwise.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/statusbr.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
class wxFlexGridSizer;
class wxStatusBar;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10000
#define SYMBOL_GEDWISEFRAME_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMINIMIZE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_GEDWISEFRAME_TITLE _T("GedWise")
#define SYMBOL_GEDWISEFRAME_IDNAME ID_DIALOG
#define SYMBOL_GEDWISEFRAME_SIZE wxSize(730, 540)
#define SYMBOL_GEDWISEFRAME_POSITION wxDefaultPosition
#define ID_TXT_GFILE 10009
#define ID_BTN_FILE 10010
#define ID_TXT_DBNAME 10011
#define ID_BTN_PALM 10012
#define ID_CH_DEFLOC 10014
#define ID_CH_SELLOC 10005
#define ID_TXT_DIR 10015
#define ID_BTN_DIR 10016
#define ID_CB_HOTUSER 10013
#define ID_EX_SOUR 10021
#define ID_EX_REPO 10018
#define ID_EX_REFN 10022
#define ID_EX_NOTE 10023
#define ID_MAX_NOTE 10020
#define ID_BTN_START 10001
#define ID_BTN_CANCEL 10002
#define ID_BTN_EXIT 10003
#define ID_BTN_WWW 10004
#define ID_BTN_HELP 10006
#define ID_BTN_INFO 10007
#define ID_BTN_BUY 10008
////@end control identifiers

/*!
 * GedWiseApp class declaration
 */

class GedWiseApp: public wxApp
{    
    DECLARE_CLASS( GedWiseApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    GedWiseApp();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin GedWiseApp event handler declarations

////@end GedWiseApp event handler declarations

////@begin GedWiseApp member function declarations

////@end GedWiseApp member function declarations

////@begin GedWiseApp member variables
////@end GedWiseApp member variables
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(GedWiseApp)
////@end declare app

/*!
 * GedWiseFrame class declaration
 */

class GedWiseFrame: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( GedWiseFrame )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    GedWiseFrame( );
    GedWiseFrame( wxWindow* parent, wxWindowID id = SYMBOL_GEDWISEFRAME_IDNAME, const wxString& caption = SYMBOL_GEDWISEFRAME_TITLE, const wxPoint& pos = SYMBOL_GEDWISEFRAME_POSITION, const wxSize& size = SYMBOL_GEDWISEFRAME_SIZE, long style = SYMBOL_GEDWISEFRAME_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_GEDWISEFRAME_IDNAME, const wxString& caption = SYMBOL_GEDWISEFRAME_TITLE, const wxPoint& pos = SYMBOL_GEDWISEFRAME_POSITION, const wxSize& size = SYMBOL_GEDWISEFRAME_SIZE, long style = SYMBOL_GEDWISEFRAME_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin GedWiseFrame event handler declarations

////@end GedWiseFrame event handler declarations

////@begin GedWiseFrame member function declarations

    wxString GetBACKCOLOR() const { return DLG_BACK_COLOR ; }
    void SetBACKCOLOR(wxString value) { DLG_BACK_COLOR = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end GedWiseFrame member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin GedWiseFrame member variables
    wxBoxSizer* m_bsMain1;
    wxBoxSizer* m_bsMain2;
    wxBoxSizer* m_bsMain3;
    wxStaticBox* m_bxGedFileName;
    wxTextCtrl* m_txGedFileName;
    wxBitmapButton* m_btnFile;
    wxStaticBox* m_bxDbName;
    wxStaticText* m_txDbNameLb;
    wxTextCtrl* m_txDbName;
    wxBitmapButton* m_btnInst;
    wxStaticBox* m_bxHotUser;
    wxCheckBox* m_chDefLoc;
    wxCheckBox* m_chSelLoc;
    wxTextCtrl* m_txDbDir;
    wxBitmapButton* m_btnDir;
    wxComboBox* m_cbHotUser;
    wxStaticBox* m_bxOpts;
    wxPanel* m_PanelOpts;
    wxFlexGridSizer* m_fgOpts1;
    wxCheckBox* m_chExSour;
    wxCheckBox* m_chExRepo;
    wxCheckBox* m_chExRefN;
    wxCheckBox* m_chExOrd;
    wxCheckBox* m_chExNotes;
    wxFlexGridSizer* m_fgOpts2;
    wxTextCtrl* m_chMaxNote;
    wxStaticText* m_chMaxNoteLb;
    wxCheckBox* m_chInsTitle;
    wxCheckBox* m_chInsSpace;
    wxCheckBox* m_chSortChil;
    wxCheckBox* m_chSortEven;
    wxBoxSizer* m_bsMain4;
    wxStaticBox* m_bxGedDet;
    wxFlexGridSizer* m_gfGedDet;
    wxStaticText* m_txGedVersLb;
    wxStaticText* m_txGedVers;
    wxStaticText* m_txGedDateLb;
    wxStaticText* m_txGedDate;
    wxStaticText* m_txGedSourLb;
    wxStaticText* m_txGedSour;
    wxStaticText* m_txGedLineLb;
    wxStaticText* m_txGedLine;
    wxStaticBox* m_bxPrcDet;
    wxFlexGridSizer* m_gfPrcDet;
    wxStaticText* m_txTotLineLb;
    wxTextCtrl* m_txTotLine;
    wxStaticText* m_txTotFamiLb;
    wxTextCtrl* m_txTotFami;
    wxStaticText* m_txTotPrcLb;
    wxTextCtrl* m_txTotPrct;
    wxStaticText* m_txTotEvenLb;
    wxTextCtrl* m_txTotEven;
    wxStaticText* m_txTotWarnLb;
    wxTextCtrl* m_txTotWarn;
    wxStaticText* m_txTotNoteLb;
    wxTextCtrl* m_txTotNote;
    wxStaticText* m_txTotPeopLb;
    wxTextCtrl* m_txTotPeop;
    wxStaticText* m_txTotSourLb;
    wxTextCtrl* m_txTotSour;
    wxStaticText* m_txTotNameLb;
    wxTextCtrl* m_txTotName;
    wxStaticText* m_txTotRepoLb;
    wxTextCtrl* m_txTotRepo;
    wxStaticText* m_txPrcStat;
    wxStaticText* m_txTime;
    wxBoxSizer* m_bsProg;
    wxGauge* m_ProgressBar;
    wxStaticText* m_txPercent;
    wxBoxSizer* m_bsMain5;
    wxBoxSizer* m_bsButtons1;
    wxButton* m_btnStart;
    wxButton* m_btnCancel;
    wxButton* m_btnExit;
    wxBoxSizer* m_bsButtons2;
    wxBitmapButton* m_btnWWW;
    wxBitmapButton* m_btnHelp;
    wxBitmapButton* m_btnInfo;
    wxBitmapButton* m_btnBuy;
    wxStatusBar* m_StatusBar;
    wxString DLG_BACK_COLOR;
////@end GedWiseFrame member variables
};

#endif
    // _GEDWISE_H_
