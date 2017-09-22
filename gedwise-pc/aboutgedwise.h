/////////////////////////////////////////////////////////////////////////////////////////
// Program:			GedWise 7.0
//
// File:			GedWiseAbout.h
//
// Written by:		Daniel T. Rencricca
//
// Last Revision:	March 9, 2005
//
// Description:		Header file.
/////////////////////////////////////////////////////////////////////////////////////////

#ifndef _ABOUT_GEDWISE_H_
#define _ABOUT_GEDWISE_H_


/////////////////////////////////////////////////////////////////////////////////////////
// Defines
/////////////////////////////////////////////////////////////////////////////////////////
#define GEDWISE_ABOUTDIALOG_STYLE wxDEFAULT_DIALOG_STYLE
#define GEDWISE_ABOUTDIALOG_TITLE wxT("About GedWise")
#define GEDWISE_ABOUTDIALOG_SIZE wxSize(400, 330)

// it is important for the id corresponding to the "About" command to have
// this standard value as otherwise it won't be handled properly under Mac
// (where it is special and put into the "Apple" menu)
#define ID_BTN_ABOUT wxID_ABOUT


/////////////////////////////////////////////////////////////////////////////////////////
// GedWiseAboutDialog Class
/////////////////////////////////////////////////////////////////////////////////////////
class GedWiseAboutDialog : public wxDialog
{
	DECLARE_CLASS (GedWiseAboutDialog)

public:

	// Constructors
	GedWiseAboutDialog ();
    GedWiseAboutDialog (wxWindow* parent, wxWindowID id = ID_BTN_ABOUT, const wxString& caption = GEDWISE_ABOUTDIALOG_TITLE,
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = GEDWISE_ABOUTDIALOG_SIZE, long style = GEDWISE_ABOUTDIALOG_STYLE );
   
	bool Create (wxWindow* parent, wxWindowID id = ID_BTN_ABOUT, const wxString& caption = GEDWISE_ABOUTDIALOG_TITLE, const wxPoint& pos = wxDefaultPosition, const wxSize& size = GEDWISE_ABOUTDIALOG_SIZE, long style = GEDWISE_ABOUTDIALOG_STYLE );

private:

	void OnPaint (wxPaintEvent &WXUNUSED(event));

	DECLARE_EVENT_TABLE()

protected:
	wxBoxSizer*		m_bsLogoImage;
	wxButton*		m_btnOk;
	wxStaticText*	m_txCopyrightLb;
	wxStaticText*	m_txVersionLb;
	wxStaticText*	m_txBpsLb;
	wxStaticText*	m_txRightsLb;
	wxStaticText*	m_txProtected1Lb;
	wxStaticText*	m_txProtected2Lb;
	wxStaticText*	m_txDeveloperLb;
	wxStaticText*	m_txWebLb;
	wxStaticBox*	m_staticBox;

	wxBitmap*	m_Bitmap;
};

#endif // ABOUT_GEDWISE_H_
