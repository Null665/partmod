#include "dlg_restore_backup.h"

//(*InternalHeaders(DlgRestoreBackup)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/msgdlg.h>


//(*IdInit(DlgRestoreBackup)
const long DlgRestoreBackup::ID_STATICTEXT1 = wxNewId();
const long DlgRestoreBackup::ID_TEXTCTRL1 = wxNewId();
const long DlgRestoreBackup::ID_BUTTON1 = wxNewId();
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
	Create(parent, id, _("Restore partition table"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxSize(328,156));
	Move(wxDefaultPosition);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Backup file name"), wxPoint(24,32), wxSize(128,13), 0, _T("ID_STATICTEXT1"));
	TextBackupFileName = new wxTextCtrl(this, ID_TEXTCTRL1, _("backup.ptb"), wxPoint(24,48), wxSize(168,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	ButtonBrowse = new wxButton(this, ID_BUTTON1, _("Browse..."), wxPoint(208,48), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	ButtonRestore = new wxButton(this, ID_BUTTON2, _("Restore"), wxPoint(64,96), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	ButtonCancel = new wxButton(this, ID_BUTTON3, _("Cancel"), wxPoint(160,96), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));

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
       openFileDialog(this, _("Save partition table backup"), "", "",
                       "partition table backups (*.ptb)|*.ptb|All files (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    TextBackupFileName->SetValue(openFileDialog.GetPath());
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
    disk->LoadBackup(TextBackupFileName->GetLineText(0).ToAscii());
  }
  catch(DiskException &de)
  {
      wxMessageBox(_(de.what()),_("Error"),wxID_OK | wxICON_ERROR,this);
      return;
  }
  catch(...)
  {
      wxMessageBox(_("Unknown error"),_("Error"),wxID_OK | wxICON_ERROR,this);
      return;
  }
 // wxMessageBox(_("Partition tables successfuly"),_("Information"),wxID_OK | wxICON_INFORMATION,this);
  Destroy();
}
