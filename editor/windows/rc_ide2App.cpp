/***************************************************************
 * Name:      rc_ide2App.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2015-08-18
 * Copyright:  ()
 * License:
 **************************************************************/

//#include "wx_pch.h"
#include "rc_ide2App.h"

//(*AppHeaders
#include "rc_ide2Main.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(rc_ide2App);

bool rc_ide2App::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	rc_ide2Frame* Frame = new rc_ide2Frame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
