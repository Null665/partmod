#include "dlg_open_diskimage.h"
#include <string>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <stdio.h>



using namespace std;
//(*InternalHeaders(DlgOpenDiskImage)
#include <wx/intl.h>
#include <wx/string.h>

//*)

//(*IdInit(DlgOpenDiskImage)
const long DlgOpenDiskImage::ID_TEXTCTRL1 = wxNewId();
const long DlgOpenDiskImage::ID_BUTTON1 = wxNewId();
const long DlgOpenDiskImage::ID_STATICTEXT1 = wxNewId();
const long DlgOpenDiskImage::ID_BUTTON2 = wxNewId();
const long DlgOpenDiskImage::ID_STATICBOX1 = wxNewId();
const long DlgOpenDiskImage::ID_TEXTCTRL2 = wxNewId();
const long DlgOpenDiskImage::ID_TEXTCTRL3 = wxNewId();
const long DlgOpenDiskImage::ID_TEXTCTRL4 = wxNewId();
const long DlgOpenDiskImage::ID_TEXTCTRL5 = wxNewId();
const long DlgOpenDiskImage::ID_STATICTEXT2 = wxNewId();
const long DlgOpenDiskImage::ID_STATICTEXT3 = wxNewId();
const long DlgOpenDiskImage::ID_STATICTEXT4 = wxNewId();
const long DlgOpenDiskImage::ID_STATICTEXT5 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DlgOpenDiskImage,wxDialog)
	//(*EventTable(DlgOpenDiskImage)
	//*)
END_EVENT_TABLE()

DlgOpenDiskImage::DlgOpenDiskImage(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DlgOpenDiskImage)
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxSize(345,267));
	Move(wxDefaultPosition);
	TextImagePath = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxPoint(24,48), wxSize(192,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	ButtonBrowse = new wxButton(this, ID_BUTTON1, _("Browse..."), wxPoint(240,48), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Disk image file path:"), wxPoint(24,32), wxSize(144,13), 0, _T("ID_STATICTEXT1"));
	ButtonOK = new wxButton(this, ID_BUTTON2, _("Open"), wxPoint(48,216), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	Button3 = new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(152,216), wxDefaultSize, 0, wxDefaultValidator, _T("wxID_CANCEL"));
	StaticBox1 = new wxStaticBox(this, ID_STATICBOX1, _("Disk image properties"), wxPoint(24,80), wxSize(288,128), 0, _T("ID_STATICBOX1"));
	TextBPS = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxPoint(40,120), wxSize(88,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	TextTPC = new wxTextCtrl(this, ID_TEXTCTRL3, wxEmptyString, wxPoint(168,120), wxSize(88,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	TextSPT = new wxTextCtrl(this, ID_TEXTCTRL4, wxEmptyString, wxPoint(40,176), wxSize(88,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	TextCylinders = new wxTextCtrl(this, ID_TEXTCTRL5, wxEmptyString, wxPoint(168,176), wxSize(88,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL5"));
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Bytes per sector"), wxPoint(40,104), wxSize(88,13), 0, _T("ID_STATICTEXT2"));
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Sectors per track"), wxPoint(40,160), wxSize(96,13), 0, _T("ID_STATICTEXT3"));
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Tracks per cylinder"), wxPoint(168,104), wxSize(96,13), 0, _T("ID_STATICTEXT4"));
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Cylinders"), wxPoint(168,160), wxSize(80,13), 0, _T("ID_STATICTEXT5"));

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgOpenDiskImage::OnButtonBrowseClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgOpenDiskImage::OnButtonOKClick);
	//*)
}

DlgOpenDiskImage::~DlgOpenDiskImage()
{
	//(*Destroy(DlgOpenDiskImage)
	//*)
}


void DlgOpenDiskImage::OnButtonOKClick(wxCommandEvent& event)
{
  string img_path;
  long bps,spt,tpc;
  uint64_t cylinders;

  img_path= TextImagePath->GetLineText(0).c_str();
  TextBPS->GetLineText(0).ToLong(&bps);
  TextSPT->GetLineText(0).ToLong(&spt);
  TextTPC->GetLineText(0).ToLong(&tpc);
  cylinders=StrToU64(TextCylinders->GetLineText(0).c_str());



  GEOMETRY geom;
  geom.bps=bps;
  geom.spt=spt;
  geom.tpc=tpc;
  geom.cylinders=cylinders;

  try
   {
       if(disk->IsOpen())
           disk->Close();

       disk->Open(img_path,geom);
   }
  catch(DiskException &de)
   {
     wxMessageBox( _(de.what()),_("Error"),wxOK | wxICON_ERROR, this );
     return;
   }
  catch(...)
   {
     wxMessageBox( _("An unknown error occured"),_("Error"),wxOK | wxICON_ERROR, this );
     return;
   }

  this->Close();

}

void DlgOpenDiskImage::OnButtonBrowseClick(wxCommandEvent& event)
{
    wxFileDialog
       openFileDialog(this, _("Open disk image"), "", "",
                       ".img files (*.img)|*.img|All files (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    TextImagePath->SetValue(openFileDialog.GetPath());

//
// TODO: Find a portable way to get file size for files larger than 2GB and 4GB
//
#if  defined(_WIN32) || defined(WIN32)
    #include <windows.h>
    HANDLE hFile=CreateFile(openFileDialog.GetPath().c_str(),GENERIC_READ|GENERIC_WRITE,7,0,OPEN_EXISTING,0,0);
    if(hFile==INVALID_HANDLE_VALUE)
      {
          return;

      }
    LARGE_INTEGER li,unused;
    SetFilePointerEx(hFile,unused,&li,FILE_END);
    CloseHandle(hFile);
    uint64_t file_size=li.QuadPart;
#else


    FILE *fp=fopen(openFileDialog.GetPath().c_str(),"rb");
    fseek(fp,0,SEEK_END);
    uint64_t file_size=ftell(fp);
    fclose(fp);
#endif

    int bps=512;
    CHS chs=file_size/bps;


    if(file_size/bps<10485760)
        chs.SetTPC(16);
    else if(file_size/bps<68719476736)
        chs.SetTPC(255);


    TextBPS->SetValue(_(U64ToStr(bps).c_str()));
    TextSPT->SetValue(_(U64ToStr(chs.GetSPT()).c_str()));
    TextTPC->SetValue(_(U64ToStr(chs.GetTPC()).c_str()));
    TextCylinders->SetValue(_(U64ToStr(chs.GetCylinder()).c_str()));
}

int DlgOpenDiskImage::ShowModal(Disk *disk)
{
  this->disk=disk;

  return wxDialog::ShowModal();
}
