#include "dlg_restore_backup.h"
#include "../Partmod/numstr.h"

//(*InternalHeaders(DlgRestoreBackup)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/msgdlg.h>
#include <fstream>
using std::fstream;


//(*IdInit(DlgRestoreBackup)
const long DlgRestoreBackup::ID_TEXTCTRL1 = wxNewId();
const long DlgRestoreBackup::ID_BUTTON1 = wxNewId();
const long DlgRestoreBackup::ID_STATICTEXT1 = wxNewId();
const long DlgRestoreBackup::ID_TEXTCTRL2 = wxNewId();
const long DlgRestoreBackup::ID_STATICTEXT2 = wxNewId();
const long DlgRestoreBackup::ID_TEXTCTRL3 = wxNewId();
const long DlgRestoreBackup::ID_STATICTEXT3 = wxNewId();
const long DlgRestoreBackup::ID_TEXTCTRL4 = wxNewId();
const long DlgRestoreBackup::ID_STATICTEXT4 = wxNewId();
const long DlgRestoreBackup::ID_TEXTCTRL5 = wxNewId();
const long DlgRestoreBackup::ID_BUTTON2 = wxNewId();
const long DlgRestoreBackup::ID_BUTTON3 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DlgRestoreBackup,wxDialog)
	//(*EventTable(DlgRestoreBackup)
	//*)
END_EVENT_TABLE()

DlgRestoreBackup::DlgRestoreBackup(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DlgRestoreBackup)
	wxStaticBoxSizer* StaticBoxSizer2;
	wxFlexGridSizer* FlexGridSizer2;
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, wxID_ANY, _("Restore partition table"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(4, 1, 0, 0);
	FlexGridSizer1->AddGrowableRow(2);
	FlexGridSizer1->AddGrowableRow(3);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Backup file"));
	TextBackupFileName = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	StaticBoxSizer1->Add(TextBackupFileName, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonBrowse = new wxButton(this, ID_BUTTON1, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	StaticBoxSizer1->Add(ButtonBrowse, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Backup file properties"));
	FlexGridSizer2 = new wxFlexGridSizer(4, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Number of sectors"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer2->Add(StaticText1, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	TextNumberOfSectors = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	FlexGridSizer2->Add(TextNumberOfSectors, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Number of partitions"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer2->Add(StaticText2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	TextNumPart = new wxTextCtrl(this, ID_TEXTCTRL3, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	FlexGridSizer2->Add(TextNumPart, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Checksum"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer2->Add(StaticText3, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	TextChecksum = new wxTextCtrl(this, ID_TEXTCTRL4, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	TextChecksum->SetToolTip(_("CRC32 checksum of the backup file"));
	FlexGridSizer2->Add(TextChecksum, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Description"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer2->Add(StaticText4, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	TextDescription = new wxTextCtrl(this, ID_TEXTCTRL5, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(this,wxSize(66,30)), wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL5"));
	TextDescription->SetToolTip(_("Description entered by the creator of the backup"));
	FlexGridSizer2->Add(TextDescription, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(FlexGridSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer2, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	Button1 = new wxButton(this, ID_BUTTON2, _("Restore"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	BoxSizer1->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button2 = new wxButton(this, ID_BUTTON3, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	BoxSizer1->Add(Button2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(BoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FileDialog1 = new wxFileDialog(this, _("Select a file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&DlgRestoreBackup::OnTextBackupFileNameTextEnter);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgRestoreBackup::OnButtonBrowseClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgRestoreBackup::OnButtonRestoreClick);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgRestoreBackup::OnButtonCancelClick);
	//*)

}

DlgRestoreBackup::~DlgRestoreBackup()
{
	//(*Destroy(DlgRestoreBackup)
	//*)
}

void DlgRestoreBackup::OnButtonBrowseClick(wxCommandEvent& event)
{
    wxFileDialog
       openFileDialog(this, _("Open partition table backup"), "", "",
                       "Partition table backups (*.ptb)|*.ptb|All files (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    file_path=openFileDialog.GetPath();
    TextBackupFileName->SetValue(file_path);
    update_fields();
}


int DlgRestoreBackup::ShowModal(Disk *disk)
{
  this->disk=disk;
  return wxDialog::ShowModal();
}

void DlgRestoreBackup::OnButtonCancelClick(wxCommandEvent& event)
{
    Destroy();
}

void DlgRestoreBackup::OnButtonRestoreClick(wxCommandEvent& event)
{
  try{
    disk->LoadBackup(TextBackupFileName->GetLineText(0).ToStdString());
  }
  catch(DiskException &de)
  {
     switch(de.error_code)
       {
          case ERR_NOT_THAT_BACKUP:
              {
                int ans=wxMessageBox(_("Disk size registered in backup does not match current disk size.\n"
                               "This means that the backup was created for a different disk and will not work."
                               "Click OK only if you are *sure* what you are doing" ),_("Warning"),wxOK | wxICON_WARNING | wxCANCEL,this);
                if(ans==wxOK)
                {
                    try{
                       disk->LoadBackup(TextBackupFileName->GetLineText(0).ToStdString(),true);
                    }
                    catch(DiskException &de)
                    {
                        wxMessageBox(de.what(),_("Error"),wxICON_ERROR,this);
                        return;
                    }
                }
              }
              break;
          default:
            wxMessageBox(de.what(),_("Error"),wxOK|wxICON_ERROR,this);
            return;
       }
  }
  catch(...)
  {
      wxMessageBox(_("Unknown error"),_("Error"),wxOK | wxICON_ERROR,this);
      return;
  }
  Close();
}


void DlgRestoreBackup::update_fields()
{
  BACKUP_FILE_HEADER bfh;
  fstream file(file_path.ToAscii(),std::ios::in | std::ios::binary);

  if(!file.is_open())
    {
      wxMessageBox(_("Could not open the backup file. Make sure that file exists and is not open by another program."),_("Error"),wxOK | wxICON_ERROR,this);
      return;
    }

  file.read((char*)&bfh,sizeof(BACKUP_FILE_HEADER));
 // file.seekg(0);
 // file.read((char*)&bfh,bfh.header_size);

  TextNumberOfSectors->SetValue(to_string(bfh.n_sect));
  TextNumPart->SetValue(to_string(bfh.n_partitions));
  TextChecksum->SetValue(to_string(bfh.checksum,STR_HEX));
  TextDescription->SetValue(bfh.description);

  file.close();
}

void DlgRestoreBackup::OnTextBackupFileNameTextEnter(wxCommandEvent& event)
{
  file_path=TextBackupFileName->GetLineText(0).ToAscii();
  update_fields();
}
