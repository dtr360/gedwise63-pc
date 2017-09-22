/////////////////////////////////////////////////////////////////////////////
// Name:        gedwise.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     09/06/05 17:56:54
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "gedwise.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "gedwise.h"

////@begin XPM images
////@end XPM images

/*!
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( GedWiseApp )
////@end implement app

/*!
 * GedWiseApp type definition
 */

IMPLEMENT_CLASS( GedWiseApp, wxApp )

/*!
 * GedWiseApp event table definition
 */

BEGIN_EVENT_TABLE( GedWiseApp, wxApp )

////@begin GedWiseApp event table entries
////@end GedWiseApp event table entries

END_EVENT_TABLE()

/*!
 * Constructor for GedWiseApp
 */

GedWiseApp::GedWiseApp()
{
////@begin GedWiseApp member initialisation
////@end GedWiseApp member initialisation
}

/*!
 * Initialisation for GedWiseApp
 */

bool GedWiseApp::OnInit()
{    
////@begin GedWiseApp initialisation
    // Remove the comment markers above and below this block
    // to make permanent changes to the code.

#if wxUSE_XPM
    wxImage::AddHandler( new wxXPMHandler );
#endif
#if wxUSE_LIBPNG
    wxImage::AddHandler( new wxPNGHandler );
#endif
#if wxUSE_LIBJPEG
    wxImage::AddHandler( new wxJPEGHandler );
#endif
#if wxUSE_GIF
    wxImage::AddHandler( new wxGIFHandler );
#endif
    GedWiseFrame* mainWindow = new GedWiseFrame(NULL, ID_DIALOG, _T("GedWise"));
    mainWindow->Show(true);
////@end GedWiseApp initialisation

    return true;
}

/*!
 * Cleanup for GedWiseApp
 */
int GedWiseApp::OnExit()
{    
////@begin GedWiseApp cleanup
    return wxApp::OnExit();
////@end GedWiseApp cleanup
}


/*!
 * GedWiseFrame type definition
 */

IMPLEMENT_DYNAMIC_CLASS( GedWiseFrame, wxDialog )

/*!
 * GedWiseFrame event table definition
 */

BEGIN_EVENT_TABLE( GedWiseFrame, wxDialog )

////@begin GedWiseFrame event table entries
////@end GedWiseFrame event table entries

END_EVENT_TABLE()

/*!
 * GedWiseFrame constructors
 */

GedWiseFrame::GedWiseFrame( )
{
}

GedWiseFrame::GedWiseFrame( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * GedWiseFrame creator
 */

bool GedWiseFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin GedWiseFrame member initialisation
    DLG_BACK_COLOR = 224, 223, 227;
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
    m_chDefLoc = NULL;
    m_chSelLoc = NULL;
    m_txDbDir = NULL;
    m_btnDir = NULL;
    m_cbHotUser = NULL;
    m_bxOpts = NULL;
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
    m_ProgressBar = NULL;
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
////@end GedWiseFrame member initialisation

////@begin GedWiseFrame creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end GedWiseFrame creation
    return TRUE;
}

/*!
 * Control creation for GedWiseFrame
 */

void GedWiseFrame::CreateControls()
{    
////@begin GedWiseFrame content construction
    GedWiseFrame* itemDialog1 = this;

    this->SetBackgroundColour(wxColour(0, 0, 13));
    m_bsMain1 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(m_bsMain1);

    m_bsMain2 = new wxBoxSizer(wxHORIZONTAL);
    m_bsMain1->Add(m_bsMain2, 0, wxGROW|wxALL, 5);

    m_bsMain3 = new wxBoxSizer(wxVERTICAL);
    m_bsMain2->Add(m_bsMain3, 1, wxALIGN_BOTTOM|wxALL, 5);

    m_bxGedFileName = new wxStaticBox(itemDialog1, wxID_ANY, _T("Source GEDCOM File"));
    wxStaticBoxSizer* itemStaticBoxSizer5 = new wxStaticBoxSizer(m_bxGedFileName, wxHORIZONTAL);
    m_bsMain3->Add(itemStaticBoxSizer5, 0, wxGROW|wxALL, 5);

    m_txGedFileName = new wxTextCtrl( itemDialog1, ID_TXT_GFILE, _T(""), wxDefaultPosition, wxSize(249, -1), 0 );
    if (ShowToolTips())
        m_txGedFileName->SetToolTip(_T("IDS_GFILE"));
    itemStaticBoxSizer5->Add(m_txGedFileName, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBitmap m_btnFileBitmap(itemDialog1->GetBitmapResource(wxT("../GedWisePC/res/FileNorm.png")));
    m_btnFile = new wxBitmapButton( itemDialog1, ID_BTN_FILE, m_btnFileBitmap, wxDefaultPosition, wxDefaultSize, 0 );
    if (ShowToolTips())
        m_btnFile->SetToolTip(_T("IDS_FILE"));
    m_btnFile->SetBackgroundColour(wxColour(224, 223, 227));
    itemStaticBoxSizer5->Add(m_btnFile, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_bxDbName = new wxStaticBox(itemDialog1, wxID_ANY, _T("Database Export File"));
    wxStaticBoxSizer* itemStaticBoxSizer8 = new wxStaticBoxSizer(m_bxDbName, wxHORIZONTAL);
    m_bsMain3->Add(itemStaticBoxSizer8, 0, wxGROW|wxALL, 5);

    m_txDbNameLb = new wxStaticText( itemDialog1, wxID_STATIC, _T("Enter Database Name (up to 6 chars):"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer8->Add(m_txDbNameLb, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txDbName = new wxTextCtrl( itemDialog1, ID_TXT_DBNAME, _T(""), wxDefaultPosition, wxSize(75, -1), 0 );
    if (ShowToolTips())
        m_txDbName->SetToolTip(_T("IDS_DBNAME"));
    itemStaticBoxSizer8->Add(m_txDbName, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBitmap m_btnInstBitmap(itemDialog1->GetBitmapResource(wxT("../GedWisePC/res/PalmNorm.png")));
    m_btnInst = new wxBitmapButton( itemDialog1, ID_BTN_PALM, m_btnInstBitmap, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    if (ShowToolTips())
        m_btnInst->SetToolTip(_T("IDS_INST"));
    m_btnInst->SetBackgroundColour(wxColour(224, 223, 227));
    itemStaticBoxSizer8->Add(m_btnInst, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_bxHotUser = new wxStaticBox(itemDialog1, wxID_ANY, _T("Hotsync User"));
    wxStaticBoxSizer* itemStaticBoxSizer12 = new wxStaticBoxSizer(m_bxHotUser, wxHORIZONTAL);
    m_bsMain3->Add(itemStaticBoxSizer12, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxVERTICAL);
    itemStaticBoxSizer12->Add(itemBoxSizer13, 1, wxGROW|wxALL, 0);

    m_chDefLoc = new wxCheckBox( itemDialog1, ID_CH_DEFLOC, _T("Default handheld folder: \"'\\My Documents\\GedWise\\\""), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chDefLoc->SetValue(TRUE);
    if (ShowToolTips())
        m_chDefLoc->SetToolTip(_T("IDS_DEF_LOC"));
    itemBoxSizer13->Add(m_chDefLoc, 0, wxALIGN_LEFT|wxALL, 5);

    m_chSelLoc = new wxCheckBox( itemDialog1, ID_CH_SELLOC, _T("Select location:"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chSelLoc->SetValue(FALSE);
    if (ShowToolTips())
        m_chSelLoc->SetToolTip(_T("IDS_SEL_LOC"));
    itemBoxSizer13->Add(m_chSelLoc, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer13->Add(itemBoxSizer16, 1, wxGROW|wxALL, 0);

    m_txDbDir = new wxTextCtrl( itemDialog1, ID_TXT_DIR, _T(""), wxDefaultPosition, wxSize(249, -1), 0 );
    if (ShowToolTips())
        m_txDbDir->SetToolTip(_T("IDS_DIR_LOC"));
    itemBoxSizer16->Add(m_txDbDir, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBitmap m_btnDirBitmap(itemDialog1->GetBitmapResource(wxT("../GedWisePC/res/FileNorm.png")));
    m_btnDir = new wxBitmapButton( itemDialog1, ID_BTN_DIR, m_btnDirBitmap, wxDefaultPosition, wxDefaultSize, 0 );
    if (ShowToolTips())
        m_btnDir->SetToolTip(_T("IDS_DIR"));
    m_btnDir->SetBackgroundColour(wxColour(224, 223, 227));
    itemBoxSizer16->Add(m_btnDir, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxString* m_cbHotUserStrings = NULL;
    m_cbHotUser = new wxComboBox( itemDialog1, ID_CB_HOTUSER, _T(""), wxDefaultPosition, wxSize(280, -1), 0, m_cbHotUserStrings, wxCB_READONLY );
    if (ShowToolTips())
        m_cbHotUser->SetToolTip(_T("IDS_HOTUSER"));
    itemBoxSizer13->Add(m_cbHotUser, 1, wxGROW|wxALL, 5);

    m_bxOpts = new wxStaticBox(itemDialog1, wxID_ANY, _T("Processing Options"));
    wxStaticBoxSizer* itemStaticBoxSizer20 = new wxStaticBoxSizer(m_bxOpts, wxVERTICAL);
    m_bsMain3->Add(itemStaticBoxSizer20, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    m_PanelOpts = new wxPanel( itemDialog1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemStaticBoxSizer20->Add(m_PanelOpts, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer22 = new wxBoxSizer(wxVERTICAL);
    m_PanelOpts->SetSizer(itemBoxSizer22);

    m_fgOpts1 = new wxFlexGridSizer(3, 2, 10, 5);
    m_fgOpts1->AddGrowableRow(0);
    m_fgOpts1->AddGrowableCol(1);
    m_fgOpts1->AddGrowableCol(2);
    itemBoxSizer22->Add(m_fgOpts1, 1, wxGROW|wxALL, 5);

    m_chExSour = new wxCheckBox( m_PanelOpts, ID_EX_SOUR, _T("Exclude Source Data."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chExSour->SetValue(FALSE);
    if (ShowToolTips())
        m_chExSour->SetToolTip(_T("IDS_SOUR"));
    m_fgOpts1->Add(m_chExSour, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_chExRepo = new wxCheckBox( m_PanelOpts, ID_EX_REPO, _T("Exclude Repository data."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chExRepo->SetValue(FALSE);
    if (ShowToolTips())
        m_chExRepo->SetToolTip(_T("IDS_REPO"));
    m_fgOpts1->Add(m_chExRepo, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_chExRefN = new wxCheckBox( m_PanelOpts, ID_EX_REFN, _T("Exclude REFN tags."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chExRefN->SetValue(FALSE);
    if (ShowToolTips())
        m_chExRefN->SetToolTip(_T("IDS_REFN"));
    m_fgOpts1->Add(m_chExRefN, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_chExOrd = new wxCheckBox( m_PanelOpts, wxID_ANY, _T("Exclude LDS Ordinance tags."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chExOrd->SetValue(FALSE);
    if (ShowToolTips())
        m_chExOrd->SetToolTip(_T("IDS_ORD"));
    m_fgOpts1->Add(m_chExOrd, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_chExNotes = new wxCheckBox( m_PanelOpts, ID_EX_NOTE, _T("Exclude Notes."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chExNotes->SetValue(FALSE);
    if (ShowToolTips())
        m_chExNotes->SetToolTip(_T("IDS_NOTE"));
    m_fgOpts1->Add(m_chExNotes, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_fgOpts2 = new wxFlexGridSizer(1, 2, 0, 0);
    m_fgOpts2->AddGrowableRow(0);
    m_fgOpts2->AddGrowableCol(0);
    m_fgOpts1->Add(m_fgOpts2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0);

    m_chMaxNote = new wxTextCtrl( m_PanelOpts, ID_MAX_NOTE, _T(""), wxDefaultPosition, wxSize(50, 18), 0 );
    if (ShowToolTips())
        m_chMaxNote->SetToolTip(_T("IDS_MAX_NOTE"));
    m_fgOpts2->Add(m_chMaxNote, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);

    m_chMaxNoteLb = new wxStaticText( m_PanelOpts, wxID_STATIC, _T("Max. characters per Note."), wxDefaultPosition, wxDefaultSize, 0 );
    m_fgOpts2->Add(m_chMaxNoteLb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);

    m_chInsTitle = new wxCheckBox( m_PanelOpts, wxID_ANY, _T("Insert TITL (Title) in an individual's name."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chInsTitle->SetValue(FALSE);
    if (ShowToolTips())
        m_chInsTitle->SetToolTip(_T("IDS_INS_TITLE"));
    itemBoxSizer22->Add(m_chInsTitle, 0, wxALIGN_LEFT|wxALL, 5);

    m_chInsSpace = new wxCheckBox( m_PanelOpts, wxID_ANY, _T("Insert space character before CONC line."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chInsSpace->SetValue(FALSE);
    if (ShowToolTips())
        m_chInsSpace->SetToolTip(_T("IDS_INS_SPACE"));
    itemBoxSizer22->Add(m_chInsSpace, 0, wxALIGN_LEFT|wxALL, 5);

    m_chSortChil = new wxCheckBox( m_PanelOpts, wxID_ANY, _T("List children as per order in GEDCOM file."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chSortChil->SetValue(FALSE);
    if (ShowToolTips())
        m_chSortChil->SetToolTip(_T("IDS_INS_SPACE"));
    itemBoxSizer22->Add(m_chSortChil, 0, wxALIGN_LEFT|wxALL, 5);

    m_chSortEven = new wxCheckBox( m_PanelOpts, wxID_ANY, _T("Sort undated Events to bottom of event list."), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_chSortEven->SetValue(FALSE);
    if (ShowToolTips())
        m_chSortEven->SetToolTip(_T("IDS_SORT_EVEN"));
    itemBoxSizer22->Add(m_chSortEven, 0, wxALIGN_LEFT|wxALL, 5);

    m_bsMain4 = new wxBoxSizer(wxVERTICAL);
    m_bsMain2->Add(m_bsMain4, 1, wxALIGN_BOTTOM|wxALL, 5);

    m_bsMain4->Add(275, 72, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 8);

    m_bxGedDet = new wxStaticBox(itemDialog1, wxID_ANY, _T("GEDCOM File Details"));
    wxStaticBoxSizer* itemStaticBoxSizer38 = new wxStaticBoxSizer(m_bxGedDet, wxHORIZONTAL);
    m_bsMain4->Add(itemStaticBoxSizer38, 0, wxGROW|wxALL, 5);

    m_gfGedDet = new wxFlexGridSizer(4, 2, 10, 15);
    m_gfGedDet->AddGrowableCol(1);
    itemStaticBoxSizer38->Add(m_gfGedDet, 0, wxALIGN_TOP|wxALL, 5);

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

    m_bxPrcDet = new wxStaticBox(itemDialog1, wxID_ANY, _T("File Processing Details"));
    wxStaticBoxSizer* itemStaticBoxSizer48 = new wxStaticBoxSizer(m_bxPrcDet, wxVERTICAL);
    m_bsMain4->Add(itemStaticBoxSizer48, 1, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    m_gfPrcDet = new wxFlexGridSizer(6, 4, 3, 15);
    m_gfPrcDet->AddGrowableRow(0);
    m_gfPrcDet->AddGrowableCol(1);
    m_gfPrcDet->AddGrowableCol(3);
    itemStaticBoxSizer48->Add(m_gfPrcDet, 0, wxGROW|wxALL, 5);

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

    wxBoxSizer* itemBoxSizer70 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer48->Add(itemBoxSizer70, 0, wxGROW, 0);

    m_txPrcStat = new wxStaticText( itemDialog1, wxID_STATIC, _T(""), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    itemBoxSizer70->Add(m_txPrcStat, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txTime = new wxStaticText( itemDialog1, wxID_STATIC, _T("00:00"), wxDefaultPosition, wxSize(40, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
    itemBoxSizer70->Add(m_txTime, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxFIXED_MINSIZE, 5);

    m_bsProg = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer48->Add(m_bsProg, 0, wxGROW, 0);

    wxBoxSizer* itemBoxSizer74 = new wxBoxSizer(wxHORIZONTAL);
    m_bsProg->Add(itemBoxSizer74, 1, wxALIGN_CENTER_VERTICAL, 0);

    m_ProgressBar = new wxGauge( itemDialog1, wxID_ANY, 100, wxDefaultPosition, wxSize(-1, 18), wxGA_HORIZONTAL );
    m_ProgressBar->SetValue(0);
    itemBoxSizer74->Add(m_ProgressBar, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer74->Add(1, 18, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_txPercent = new wxStaticText( itemDialog1, wxID_STATIC, _T("100%"), wxDefaultPosition, wxSize(40, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
    m_bsProg->Add(m_txPercent, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxFIXED_MINSIZE, 5);

    m_bsMain5 = new wxBoxSizer(wxHORIZONTAL);
    m_bsMain1->Add(m_bsMain5, 1, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_bsButtons1 = new wxBoxSizer(wxHORIZONTAL);
    m_bsMain5->Add(m_bsButtons1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 10);

    m_btnStart = new wxButton( itemDialog1, ID_BTN_START, _T("Start"), wxDefaultPosition, wxDefaultSize, 0 );
    if (ShowToolTips())
        m_btnStart->SetToolTip(_T("IDS_START"));
    m_btnStart->SetBackgroundColour(wxColour(224, 223, 227));
    m_bsButtons1->Add(m_btnStart, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 10);

    m_btnCancel = new wxButton( itemDialog1, ID_BTN_CANCEL, _T("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    if (ShowToolTips())
        m_btnCancel->SetToolTip(_T("IDS_CANCEL"));
    m_btnCancel->SetBackgroundColour(wxColour(224, 223, 227));
    m_bsButtons1->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 10);

    m_btnExit = new wxButton( itemDialog1, ID_BTN_EXIT, _T("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
    if (ShowToolTips())
        m_btnExit->SetToolTip(_T("IDS_EXIT"));
    m_btnExit->SetBackgroundColour(wxColour(224, 223, 227));
    m_bsButtons1->Add(m_btnExit, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 10);

    m_bsMain5->Add(0, 39, 1, wxALIGN_BOTTOM, 0);

    m_bsButtons2 = new wxBoxSizer(wxHORIZONTAL);
    m_bsMain5->Add(m_bsButtons2, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 10);

    wxBitmap m_btnWWWBitmap(itemDialog1->GetBitmapResource(wxT("../GedWisePC/res/WWWNorm.png")));
    m_btnWWW = new wxBitmapButton( itemDialog1, ID_BTN_WWW, m_btnWWWBitmap, wxDefaultPosition, wxSize(32, 32), 0 );
    if (ShowToolTips())
        m_btnWWW->SetToolTip(_T("IDS_WEBSITE"));
    m_btnWWW->SetBackgroundColour(wxColour(224, 223, 227));
    m_bsButtons2->Add(m_btnWWW, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 12);

    wxBitmap m_btnHelpBitmap(itemDialog1->GetBitmapResource(wxT("../GedWisePC/res/HelpNorm.png")));
    m_btnHelp = new wxBitmapButton( itemDialog1, ID_BTN_HELP, m_btnHelpBitmap, wxDefaultPosition, wxSize(32, 32), 0 );
    if (ShowToolTips())
        m_btnHelp->SetToolTip(_T("IDS_HELP\n"));
    m_btnHelp->SetBackgroundColour(wxColour(224, 223, 227));
    m_bsButtons2->Add(m_btnHelp, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 12);

    wxBitmap m_btnInfoBitmap(itemDialog1->GetBitmapResource(wxT("../GedWisePC/res/InfoNorm.png")));
    m_btnInfo = new wxBitmapButton( itemDialog1, ID_BTN_INFO, m_btnInfoBitmap, wxDefaultPosition, wxSize(32, 32), 0 );
    if (ShowToolTips())
        m_btnInfo->SetToolTip(_T("IDS_INFO"));
    m_btnInfo->SetBackgroundColour(wxColour(224, 223, 227));
    m_bsButtons2->Add(m_btnInfo, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 12);

    wxBitmap m_btnBuyBitmap(itemDialog1->GetBitmapResource(wxT("../GedWisePC/res/BuyNorm.png")));
    m_btnBuy = new wxBitmapButton( itemDialog1, ID_BTN_BUY, m_btnBuyBitmap, wxDefaultPosition, wxSize(80, 32), 0 );
    if (ShowToolTips())
        m_btnBuy->SetToolTip(_T("IDS_BUY"));
    m_btnBuy->SetBackgroundColour(wxColour(224, 223, 227));
    m_bsButtons2->Add(m_btnBuy, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 12);

    m_StatusBar = new wxStatusBar( itemDialog1, wxID_ANY, wxST_SIZEGRIP|wxCLIP_CHILDREN  );
    m_StatusBar->SetFieldsCount(3);
    m_bsMain1->Add(m_StatusBar, 0, wxGROW|wxTOP, 5);

////@end GedWiseFrame content construction
}

/*!
 * Should we show tooltips?
 */

bool GedWiseFrame::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap GedWiseFrame::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin GedWiseFrame bitmap retrieval
    if (name == wxT("../GedWisePC/res/FileNorm.png"))
    {
        wxBitmap bitmap(_T("../GedWisePC/res/FileNorm.png"), wxBITMAP_TYPE_PNG);
        return bitmap;
    }
    else if (name == wxT("../GedWisePC/res/PalmNorm.png"))
    {
        wxBitmap bitmap(_T("../GedWisePC/res/PalmNorm.png"), wxBITMAP_TYPE_PNG);
        return bitmap;
    }
    else if (name == wxT("../GedWisePC/res/WWWNorm.png"))
    {
        wxBitmap bitmap(_T("../GedWisePC/res/WWWNorm.png"), wxBITMAP_TYPE_PNG);
        return bitmap;
    }
    else if (name == wxT("../GedWisePC/res/HelpNorm.png"))
    {
        wxBitmap bitmap(_T("../GedWisePC/res/HelpNorm.png"), wxBITMAP_TYPE_PNG);
        return bitmap;
    }
    else if (name == wxT("../GedWisePC/res/InfoNorm.png"))
    {
        wxBitmap bitmap(_T("../GedWisePC/res/InfoNorm.png"), wxBITMAP_TYPE_PNG);
        return bitmap;
    }
    else if (name == wxT("../GedWisePC/res/BuyNorm.png"))
    {
        wxBitmap bitmap(_T("../GedWisePC/res/BuyNorm.png"), wxBITMAP_TYPE_PNG);
        return bitmap;
    }
    return wxNullBitmap;
////@end GedWiseFrame bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon GedWiseFrame::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin GedWiseFrame icon retrieval
    return wxNullIcon;
////@end GedWiseFrame icon retrieval
}
