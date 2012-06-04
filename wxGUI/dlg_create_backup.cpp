#include "dlg_create_backup.h"

//(*InternalHeaders(DlgCreateBackup)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/msgdlg.h>

//(*IdInit(DlgCreateBackup)
const long DlgCreateBackup::ID_STATICTEXT1 = wxNewId();
const long DlgCreateBackup::ID_TEXTCTRL1 = wxNewId();
const long DlgCreateBackup::ID_BROWSE = wxNewId();
const long DlgCreateBackup::ID_CREATE_BACKUP = wxNewId();
const long DlgCreateBackup::ID_CANCEL = wxNewId();
//*)

BEGIN_EVENT_TABLE(DlgCreateBackup,wxDialog)
	//(*EventTable(DlgCreateBackup)
	//*)
END_EVENT_TABLE()

DlgCreateBackup::DlgCreateBackup(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(DlgCreateBackup)
	Create(parent, id, _("Create a partition table backup"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxSize(328,156));
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Backup file name:"), wxPoint(24,32), wxSize(152,13), 0, _T("ID_STATICTEXT1"));
	TextBackupFileName = new wxTextCtrl(this, ID_TEXTCTRL1, _("backup.ptb"), wxPoint(24,48), wxSize(168,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	ButtonBrowse = new wxButton(this, ID_BROWSE, _("Browse..."), wxPoint(208,48), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BROWSE"));
	ButtonCreateBackup = new wxButton(this, ID_CREATE_BACKUP, _("Create backup"), wxPoint(56,96), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CREATE_BACKUP"));
	ButtonClose = new wxButton(this, ID_CANCEL, _("Cancel"), wxPoint(168,96), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CANCEL"));
	FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));

	Connect(ID_BROWSE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgCreateBackup::OnButtonBrowseClick);
	Connect(ID_CREATE_BACKUP,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgCreateBackup::OnButtonCreateBackupClick);
	Connect(ID_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgCreateBackup::OnButtonCloseClick);
	//*)
}

DlgCreateBackup::~DlgCreateBackup()
{
	//(*Destroy(DlgCreateBackup)
	//*)
}


void DlgCreateBackup::OnButtonBrowseClick(wxCommandEvent& event)
{
    wxString wstr=_("partition table backups (*.ptb)|*.ptb|All files (*.*)|*.*");

    wxFileDialog
       openFileDialog(this, _("Save partition table backup"), _(""), _(""),
                       wstr, wxFD_SAVE|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    TextBackupFileName->SetValue(openFileDialog.GetPath());
}

int DlgCreateBackup::ShowModal(Disk *disk)
{
  this->disk=disk;
  return wxDialog::ShowModal();
}

void DlgCreateBackup::OnButtonCloseClick(wxCommandEvent& event)
{
  Destroy();
}

void DlgCreateBackup::OnButtonCreateBackupClick(wxCommandEvent& event)
{
  try{
    disk->CreateBackup(TextBackupFileName->GetLineText(0).ToAscii());
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
  wxMessageBox(_("Backup created successfuly"),_("Information"),wxID_OK | wxICON_INFORMATION,this);
  Destroy();
}
