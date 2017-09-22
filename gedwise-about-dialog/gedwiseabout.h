/////////////////////////////////////////////////////////////////////////////
// Name:        gedwiseabout.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     09/13/05 16:49:24
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _GEDWISEABOUT_H_
#define _GEDWISEABOUT_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "gedwiseabout.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
////@end control identifiers

/*!
 * GedWiseAboutApp class declaration
 */

class GedWiseAboutApp: public wxApp
{    
    DECLARE_CLASS( GedWiseAboutApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    GedWiseAboutApp();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin GedWiseAboutApp event handler declarations
////@end GedWiseAboutApp event handler declarations

////@begin GedWiseAboutApp member function declarations
////@end GedWiseAboutApp member function declarations

////@begin GedWiseAboutApp member variables
////@end GedWiseAboutApp member variables
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(GedWiseAboutApp)
////@end declare app

#endif
    // _GEDWISEABOUT_H_
