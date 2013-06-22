#include "dlg_save_changes.h"

//(*InternalHeaders(DlgSaveChanges)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <exception>
#include <string>
#include <sstream>
using namespace std;


static bool thread_exited=false;


//(*IdInit(DlgSaveChanges)
const long DlgSaveChanges::ID_GAUGE1 = wxNewId();
const long DlgSaveChanges::ID_TEXTCTRL1 = wxNewId();
const long DlgSaveChanges::ID_STATICTEXT1 = wxNewId();
const long DlgSaveChanges::ID_STATICTEXT2 = wxNewId();
const long DlgSaveChanges::ID_OK = wxNewId();
const long DlgSaveChanges::ID_LISTVIEW1 = wxNewId();
const long DlgSaveChanges::ID_STATICTEXT3 = wxNewId();
const long DlgSaveChanges::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DlgSaveChanges,wxDialog)
	//(*EventTable(DlgSaveChanges)
	//*)
END_EVENT_TABLE()

DlgSaveChanges::DlgSaveChanges(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DlgSaveChanges)
	Create(parent, wxID_ANY, _("Writing changes to disk"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(512,373));
	GaugeProgress = new wxGauge(this, ID_GAUGE1, 100, wxPoint(16,272), wxSize(480,28), 0, wxDefaultValidator, _T("ID_GAUGE1"));
	TextLog = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxPoint(104,312), wxSize(392,24), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Progress"), wxPoint(16,256), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Current action: "), wxPoint(16,320), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	ButtonCancel = new wxButton(this, ID_OK, _("Cancel"), wxPoint(200,344), wxDefaultSize, 0, wxDefaultValidator, _T("ID_OK"));
	pendingList = new wxPendingList(this, ID_LISTVIEW1, wxPoint(16,32), wxSize(480,192), wxLC_REPORT, wxDefaultValidator, _T("ID_LISTVIEW1"));
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("The following actions will be applied to disk"), wxPoint(16,16), wxSize(240,13), 0, _T("ID_STATICTEXT3"));
	ButtonConfirm = new wxButton(this, ID_BUTTON1, _("Confirm"), wxPoint(200,232), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));

	Connect(ID_OK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgSaveChanges::OnButtonCancelClick);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgSaveChanges::OnButtonConfirmClick);
	//*)
	thread_progress=0;
}

DlgSaveChanges::~DlgSaveChanges()
{
    //thread_progress->;
	//(*Destroy(DlgSaveChanges)
	//*)
}


int DlgSaveChanges::ShowModal(Disk *disk)
{
  this->disk=disk;
  pendingList->AssignDisk(disk);
  pendingList->Refresh();
  return wxDialog::ShowModal();
}



//
// Thread for calling disk->Save()
//
DiskSaveThread::DiskSaveThread(Disk *disk) : wxThread(wxTHREAD_DETACHED)
{
  this->disk=disk;
}

void *DiskSaveThread::Entry()
{
  try
   {

     disk->Save();
   }
   catch(exception &ex)
   {
      wxMessageBox( ex.what(),_("Error"),wxOK | wxICON_ERROR, 0 );
      disk->UndoAll();
   }
  return 0;
}



//
// Thread for displaying progress
//
ProgressThread::ProgressThread(Disk *disk,DlgSaveChanges *dlg) : wxThread(wxTHREAD_JOINABLE)
{
  this->dlg=dlg;
  this->disk=disk;
}



void *ProgressThread::Entry()
{
  DiskSaveThread *thread_save=new DiskSaveThread(disk);
  thread_save->Create(/*wxTHREAD_DETACHED*/);
  thread_save->Run();
 // dlg->ButtonOk->Disable();


  dlg->GaugeProgress->SetRange(100);
  while(true)
    {
       if(disk->CountPendingActions()==0)
           {
            // dlg->Close();
             break;
           }
       CALLBACK_DATA *cdt=&disk->clbk;

       stringstream ss;
       if(cdt->event==PENDING_WIPE)
           ss<<"Wiping partition ";
       //ss<<cdt->partition<<endl;

       dlg->TextLog->SetValue(ss.str().c_str());

       if(cdt->total==0)
          continue;
       unsigned progress=((long double)cdt->current/cdt->total)*100;
       dlg->GaugeProgress->SetValue(progress);
       Sleep(100);

       if(TestDestroy())
          break;
    }

 dlg->TextLog->SetValue("Done!");
 dlg->GaugeProgress->SetValue(100);

 thread_exited=true;
 return 0;
}

void DlgSaveChanges::OnButtonCancelClick(wxCommandEvent& event)
{
  if(thread_progress!=0)
  {
      if(thread_progress->IsRunning())
        {
           thread_progress->Pause();
           int ret=wxMessageBox(_("Wrting changes to disk in progress."
                                 "Are you sure want to cancel? Data loss is possible is you click yes"),_("Warning!"),wxICON_WARNING | wxYES_NO,this);
           if(ret==wxYES)
             {
               thread_progress->Resume();
               thread_progress->Delete();
               Close();
             }
           else
           {
             thread_progress->Resume();
             return;
           }
        }
  }

  Close();
}

void DlgSaveChanges::OnButtonConfirmClick(wxCommandEvent& event)
{
  int ret=wxMessageBox(_("All changes will be written to disk. "
                         "Are you sure?"),_("Warning!"),wxICON_WARNING | wxYES_NO,this);
  if(ret!=wxYES)
      return;
  ButtonConfirm->Disable();

  thread_progress=new ProgressThread(disk,this);

  thread_progress->Create();
  thread_progress->Run();
}
