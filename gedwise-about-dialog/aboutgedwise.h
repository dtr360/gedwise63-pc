/////////////////////////////////////////////////////////////////////////////
// Name:        aboutgedwise.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     09/13/05 16:51:28
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _ABOUTGEDWISE_H_
#define _ABOUTGEDWISE_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "aboutgedwise.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10000
#define SYMBOL_GEDWISEABOUTDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_GEDWISEABOUTDIALOG_TITLE _("About GedWise")
#define SYMBOL_GEDWISEABOUTDIALOG_IDNAME ID_DIALOG
#define SYMBOL_GEDWISEABOUTDIALOG_SIZE wxSize(350, 330)
#define SYMBOL_GEDWISEABOUTDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

/*!
 * GedWiseAboutDialog class declaration
 */

class GedWiseAboutDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( GedWiseAboutDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    GedWiseAboutDialog( );
    GedWiseAboutDialog( wxWindow* parent, wxWindowID id = SYMBOL_GEDWISEABOUTDIALOG_IDNAME, const wxString& caption = SYMBOL_GEDWISEABOUTDIALOG_TITLE, const wxPoint& pos = SYMBOL_GEDWISEABOUTDIALOG_POSITION, const wxSize& size = SYMBOL_GEDWISEABOUTDIALOG_SIZE, long style = SYMBOL_GEDWISEABOUTDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_GEDWISEABOUTDIALOG_IDNAME, const wxString& caption = SYMBOL_GEDWISEABOUTDIALOG_TITLE, const wxPoint& pos = SYMBOL_GEDWISEABOUTDIALOG_POSITION, const wxSize& size = SYMBOL_GEDWISEABOUTDIALOG_SIZE, long style = SYMBOL_GEDWISEABOUTDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin GedWiseAboutDialog event handler declarations

////@end GedWiseAboutDialog event handler declarations

////@begin GedWiseAboutDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end GedWiseAboutDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin GedWiseAboutDialog member variables
    wxBoxSizer* m_bsLogoImage;
    wxStaticText* m_txVersionLb;
    wxButton* m_btnOk;
    wxStaticText* m_txCopyrightLb;
    wxStaticText* m_txBpsLb;
    wxStaticText* m_txRightsLb;
    wxStaticText* m_txWebLb;
    wxStaticText* m_txDeveloperLb;
    wxStaticText* m_txProtected1Lb;
    wxStaticText* m_txProtected2Lb;
////@end GedWiseAboutDialog member variables
};

#endif
    // _ABOUTGEDWISE_H_
