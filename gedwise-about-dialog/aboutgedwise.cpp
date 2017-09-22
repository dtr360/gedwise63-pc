/////////////////////////////////////////////////////////////////////////////
// Name:        aboutgedwise.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     09/13/05 16:51:28
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "aboutgedwise.h"
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

#include "aboutgedwise.h"

////@begin XPM images
////@end XPM images

/*!
 * GedWiseAboutDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( GedWiseAboutDialog, wxDialog )

/*!
 * GedWiseAboutDialog event table definition
 */

BEGIN_EVENT_TABLE( GedWiseAboutDialog, wxDialog )

////@begin GedWiseAboutDialog event table entries
////@end GedWiseAboutDialog event table entries

END_EVENT_TABLE()

/*!
 * GedWiseAboutDialog constructors
 */

GedWiseAboutDialog::GedWiseAboutDialog( )
{
}

GedWiseAboutDialog::GedWiseAboutDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * GedWiseAboutDialog creator
 */

bool GedWiseAboutDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin GedWiseAboutDialog member initialisation
    m_bsLogoImage = NULL;
    m_txVersionLb = NULL;
    m_btnOk = NULL;
    m_txCopyrightLb = NULL;
    m_txBpsLb = NULL;
    m_txRightsLb = NULL;
    m_txWebLb = NULL;
    m_txDeveloperLb = NULL;
    m_txProtected1Lb = NULL;
    m_txProtected2Lb = NULL;
////@end GedWiseAboutDialog member initialisation

////@begin GedWiseAboutDialog creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end GedWiseAboutDialog creation
    return TRUE;
}

/*!
 * Control creation for GedWiseAboutDialog
 */

void GedWiseAboutDialog::CreateControls()
{    
////@begin GedWiseAboutDialog content construction
    GedWiseAboutDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_bsLogoImage = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(m_bsLogoImage, 0, wxALIGN_TOP|wxALL, 2);

    m_bsLogoImage->Add(60, 50, 0, wxALIGN_LEFT|wxFIXED_MINSIZE, 0);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(itemBoxSizer6, 0, wxALIGN_TOP|wxALL, 10);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer6->Add(itemBoxSizer7, 0, wxGROW, 0);

    m_txVersionLb = new wxStaticText( itemDialog1, wxID_STATIC, _("GedWise Version 6.3.3.025"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer7->Add(m_txVersionLb, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer7->Add(0, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnOk = new wxButton( itemDialog1, wxID_CANCEL, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnOk->SetDefault();
    itemBoxSizer7->Add(m_btnOk, 0, wxALIGN_CENTER_VERTICAL, 0);

    m_txCopyrightLb = new wxStaticText( itemDialog1, wxID_STATIC, _("Copyright (c)  2001 - 2006"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(m_txCopyrightLb, 0, wxALIGN_LEFT|wxALL, 5);

    m_txBpsLb = new wxStaticText( itemDialog1, wxID_STATIC, _("Battery Park Software Corporation"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(m_txBpsLb, 0, wxALIGN_LEFT|wxALL, 5);

    m_txRightsLb = new wxStaticText( itemDialog1, wxID_STATIC, _("All Rights Reserved"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(m_txRightsLb, 0, wxALIGN_LEFT|wxALL, 5);

    itemBoxSizer6->Add(0, 1, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_txWebLb = new wxStaticText( itemDialog1, wxID_STATIC, _("www.BatteryParkSoftware.com"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(m_txWebLb, 0, wxALIGN_LEFT|wxALL, 5);

    m_txDeveloperLb = new wxStaticText( itemDialog1, wxID_STATIC, _("Senior Software Developer: Daniel Rencricca"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(m_txDeveloperLb, 0, wxALIGN_LEFT|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer17Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Warning"));
    wxStaticBoxSizer* itemStaticBoxSizer17 = new wxStaticBoxSizer(itemStaticBoxSizer17Static, wxVERTICAL);
    itemBoxSizer6->Add(itemStaticBoxSizer17, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_txProtected1Lb = new wxStaticText( itemDialog1, wxID_STATIC, _("This computer program is protected by copyright law and \n international treaties."), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer17->Add(m_txProtected1Lb, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_txProtected2Lb = new wxStaticText( itemDialog1, wxID_STATIC, _("Unauthorized reproduction or distribution of this program,\nor any portion of it, may result in severe civil and criminal\npenalties, and will be prosecuted to the maximum extent\npossible under the law."), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer17->Add(m_txProtected2Lb, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end GedWiseAboutDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool GedWiseAboutDialog::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap GedWiseAboutDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin GedWiseAboutDialog bitmap retrieval
    return wxNullBitmap;
////@end GedWiseAboutDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon GedWiseAboutDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin GedWiseAboutDialog icon retrieval
    return wxNullIcon;
////@end GedWiseAboutDialog icon retrieval
}
