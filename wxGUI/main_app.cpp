#include "main_app.hpp"
#include "main_frame.hpp"

#include "../Partmod/disk.h"

bool MyApp::OnInit()
{
	wxInitAllImageHandlers();

    MainFrame *frame = new MainFrame( 0,wxNewId(), wxPoint(50, 50), wxSize(860, 500) );
    frame->SetBackgroundColour( wxColour( 228, 228, 228 ) );
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}



IMPLEMENT_APP(MyApp);
