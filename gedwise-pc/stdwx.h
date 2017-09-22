#ifndef _STDWX_H_
#define _STDWX_H_

#include <wx/wxprec.h>
#include <wx/filedlg.h>		// file selection dialog
#include <wx/filename.h>	// wxFileType functions
#include <wx/mimetype.h>	// Mime type funtions
#include <wx/textfile.h>	// wxFile functions
#include <wx/dnd.h>			// drag and drop functions
#include <wx/dynlib.h>		// dynamic link library functions
#include <wx/xrc/xmlres.h>  // to get xrc resources
#include <wx/xrc/xh_all.h>  // to add handlers for wxXmlResource
#include <wx/fs_zip.h> 		// unzip library to open help files
#include <wx/help.h>  		// for wxHtmlHelpController (only for non-Windows computers)
#include "wx/image.h"		// for images

// For compilers without precomilation support, include the necessary headers
// (this file is usually all you need because it includes almost all "standard"
// wxWindows headers)
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#ifdef _DEBUG
	#include <crtdbg.h>
	#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
	#define DEBUG_NEW new
#endif 


#endif // _STDWX_H_
