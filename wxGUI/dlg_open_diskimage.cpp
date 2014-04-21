#include "dlg_open_diskimage.h"
#include <string>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>



using namespace std;
//(*InternalHeaders(DlgOpenDiskImage)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(DlgOpenDiskImage)
const long DlgOpenDiskImage::ID_TEXTCTRL1 = wxNewId();
const long DlgOpenDiskImage::ID_BUTTON1 = wxNewId();
const long DlgOpenDiskImage::ID_STATICTEXT1 = wxNewId();
const long DlgOpenDiskImage::ID_TEXTCTRL2 = wxNewId();
const long DlgOpenDiskImage::ID_STATICTEXT2 = wxNewId();
const long DlgOpenDiskImage::ID_TEXTCTRL3 = wxNewId();
const long DlgOpenDiskImage::ID_STATICTEXT3 = wxNewId();
const long DlgOpenDiskImage::ID_TEXTCTRL4 = wxNewId();
const long DlgOpenDiskImage::ID_STATICTEXT4 = wxNewId();
const long DlgOpenDiskImage::ID_TEXTCTRL5 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DlgOpenDiskImage,wxDialog)
    //(*EventTable(DlgOpenDiskImage)
    //*)
    EVT_BUTTON(wxID_OK,DlgOpenDiskImage::OnButtonOKClick)
END_EVENT_TABLE()

DlgOpenDiskImage::DlgOpenDiskImage(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(DlgOpenDiskImage)
    wxStaticBoxSizer* StaticBoxSizer2;
    wxGridSizer* GridSizer1;
    wxStaticBoxSizer* StaticBoxSizer1;
    wxFlexGridSizer* FlexGridSizer1;
    wxStdDialogButtonSizer* StdDialogButtonSizer1;

    Create(parent, id, _("Open a disk image"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
    SetClientSize(wxDefaultSize);
    Move(wxDefaultPosition);
    FlexGridSizer1 = new wxFlexGridSizer(3, 1, 0, 0);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Disk image file"));
    TextImagePath = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    StaticBoxSizer1->Add(TextImagePath, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonBrowse = new wxButton(this, ID_BUTTON1, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    StaticBoxSizer1->Add(ButtonBrowse, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Geometry"));
    GridSizer1 = new wxGridSizer(4, 2, 0, 0);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Bytes per sector"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    GridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    TextBPS = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    GridSizer1->Add(TextBPS, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Sectors per track"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    GridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    TextSPT = new wxTextCtrl(this, ID_TEXTCTRL3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    GridSizer1->Add(TextSPT, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Tracks per cylinder"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    GridSizer1->Add(StaticText3, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    TextTPC = new wxTextCtrl(this, ID_TEXTCTRL4, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    GridSizer1->Add(TextTPC, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Cylinders"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    GridSizer1->Add(StaticText4, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    TextCylinders = new wxTextCtrl(this, ID_TEXTCTRL5, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL5"));
    GridSizer1->Add(TextCylinders, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(GridSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(StaticBoxSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
    StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
    StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
    StdDialogButtonSizer1->Realize();
    FlexGridSizer1->Add(StdDialogButtonSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);

    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&DlgOpenDiskImage::OnTextImagePathTextEnter);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgOpenDiskImage::OnButtonBrowseClick);
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
    cylinders=StrToU64(string(TextCylinders->GetLineText(0).c_str()));






    GEOMETRY geom;
    geom.bps=bps;
    geom.spt=spt;
    geom.tpc=tpc;
    geom.cylinders=cylinders;

    try
    {
        if(disk->IsOpen())
            disk->Close();

        disk->OpenDiskImage(img_path,geom);
    }
    catch(DiskException &de)
    {
        wxMessageBox( de.what(),_("Error"),wxOK | wxICON_ERROR, this );
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
    openFileDialog(this, _("Open disk image"), _(""), _(""),
                   _(".img files (*.img)|*.img|All files (*.*)|*.*"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    TextImagePath->SetValue(openFileDialog.GetPath());

    set_values();
}

void DlgOpenDiskImage::set_values()
{
    uint64_t file_size=GetFileSize(TextImagePath->GetLineText(0).ToStdString());
    if(file_size==0)
      {
          wxMessageBox("File is empty or you don\'t have rights to access it","Error",wxOK | wxICON_ERROR,this);
          return;
      }

    int bps=512;
    CHS chs=file_size/bps;

    if(file_size/bps<10485760)
        chs.SetTPC(16);
    else if(file_size/bps<(uint64_t)68719476736)
        chs.SetTPC(255);


    TextBPS->SetValue(U64ToStr(bps).c_str());
    TextSPT->SetValue(U64ToStr(chs.GetSPT()).c_str());
    TextTPC->SetValue(U64ToStr(chs.GetTPC()).c_str());
    TextCylinders->SetValue(U64ToStr(chs.GetCylinder()).c_str());
}


int DlgOpenDiskImage::ShowModal(Disk *disk)
{
    this->disk=disk;

    return wxDialog::ShowModal();
}

void DlgOpenDiskImage::OnTextImagePathTextEnter(wxCommandEvent& event)
{
  set_values();
}
