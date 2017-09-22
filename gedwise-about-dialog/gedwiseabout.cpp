/////////////////////////////////////////////////////////////////////////////
// Name:        gedwiseabout.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     09/13/05 16:49:24
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "gedwiseabout.h"
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

#include "gedwiseabout.h"

////@begin XPM images

////@end XPM images

/*!
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( GedWiseAboutApp )
////@end implement app

/*!
 * GedWiseAboutApp type definition
 */

IMPLEMENT_CLASS( GedWiseAboutApp, wxApp )

/*!
 * GedWiseAboutApp event table definition
 */

BEGIN_EVENT_TABLE( GedWiseAboutApp, wxApp )

////@begin GedWiseAboutApp event table entries
////@end GedWiseAboutApp event table entries

END_EVENT_TABLE()

/*!
 * Constructor for GedWiseAboutApp
 */

GedWiseAboutApp::GedWiseAboutApp()
{
////@begin GedWiseAboutApp member initialisation
////@end GedWiseAboutApp member initialisation
}

/*!
 * Initialisation for GedWiseAboutApp
 */

bool GedWiseAboutApp::OnInit()
{    
////@begin GedWiseAboutApp initialisation
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
////@end GedWiseAboutApp initialisation

    return true;
}

/*!
 * Cleanup for GedWiseAboutApp
 */
int GedWiseAboutApp::OnExit()
{    
////@begin GedWiseAboutApp cleanup
    return wxApp::OnExit();
////@end GedWiseAboutApp cleanup
}

