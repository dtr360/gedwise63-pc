/////////////////////////////////////////////////////////////////////////////////////////
// Program:		GedWise 6.3
//
// Module:		GedWiseAbout.cpp
//
// Written by:	Daniel T. Rencricca
//
// Date:		March 9, 2005
//
// Description:	This module creates the 'About' dialog box for the Palm database "pdb" file from the raw data
//				file created in the DBCreate module.  In creating the database this 
//				module packs up to PACK_REC_MAX records into each Palm record.
/////////////////////////////////////////////////////////////////////////////////////////
#include "stdwx.h"
#include "defines.h"
#include "aboutgedwise.h"

IMPLEMENT_CLASS (GedWiseAboutDialog, wxDialog)

/////////////////////////////////////////////////////////////////////////////////////////
// GedWiseAboutDialog Functions
/////////////////////////////////////////////////////////////////////////////////////////
GedWiseAboutDialog::GedWiseAboutDialog ()
{
}

// Frame Constructor
GedWiseAboutDialog::GedWiseAboutDialog (wxWindow* parent, wxWindowID id, const wxString& caption,
						   const wxPoint& pos, const wxSize& size, long style)
{
    Create ( parent, id, caption, pos, size, style );
}


/////////////////////////////////////////////////////////////////////////////////////////
// Function:	GedWiseAboutDialog::Create
//
// Description:	Creates the 'About' dialog window.  Adds all the data and controls needed.
//
// Parameters:	-> parent	-	parent window.
//				-> id		-	id for the About dialog.
//				-> caption	-	caption for dialog.
//				-> pos		-	position of dialog.
//				-> size		-	size of dialog.
//				-> style	-	style of dialog.
//
// Returns:		True after created successfully.
/////////////////////////////////////////////////////////////////////////////////////////
bool GedWiseAboutDialog::Create (wxWindow* parent, wxWindowID id, const wxString& caption,
						   const wxPoint& pos, const wxSize& size, long style )
{
	m_txVersionLb = NULL;
    m_btnOk = NULL;
    m_btnOk = NULL;
    m_txBpsLb = NULL;
    m_txRightsLb = NULL;
    m_txWebLb = NULL;
    m_txDeveloperLb = NULL;
    m_txProtected1Lb = NULL;
    m_txProtected2Lb = NULL;

    
	// Create the About application window.
	wxDialog::Create (parent, id, caption, pos, size, style );

	//// Paste Dialog Layout Code Below ////

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

    m_txVersionLb = new wxStaticText( itemDialog1, wxID_STATIC, _("GedWise Version 6.3.3.0026"), wxDefaultPosition, wxDefaultSize, 0 );
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

    m_txProtected1Lb = new wxStaticText( itemDialog1, wxID_STATIC, _("This computer program is protected by copyright law and \ninternational treaties."), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer17->Add(m_txProtected1Lb, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_txProtected2Lb = new wxStaticText( itemDialog1, wxID_STATIC, _("Unauthorized reproduction or distribution of this program,\nor any portion of it, may result in severe civil and criminal\npenalties, and will be prosecuted to the maximum extent\npossible under the law."), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer17->Add(m_txProtected2Lb, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);



	// Other Definitions
	m_Bitmap = new wxBitmap (wxXmlResource::Get()->LoadBitmap("LeafLogo"));

    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

	Centre();

    return true;
}


BEGIN_EVENT_TABLE(GedWiseAboutDialog, wxDialog)

	EVT_PAINT		(GedWiseAboutDialog::OnPaint)
	
END_EVENT_TABLE()


void GedWiseAboutDialog::OnPaint (wxPaintEvent &WXUNUSED(event))
{
	wxPaintDC dc( this );
	PrepareDC( dc );

	// Draw the GedWise bitmap logo
	wxPoint pos = m_bsLogoImage->GetPosition ();
	dc.DrawBitmap ( *m_Bitmap, pos.x, pos.y+10, true);
}
