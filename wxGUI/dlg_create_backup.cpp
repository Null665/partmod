#include <wx/msgdlg.h>
#include "dlg_create_backup.h"


//(*InternalHeaders(DlgCreateBackup)
#include <wx/intl.h>
#include <wx/string.h>
//*)


//(*IdInit(DlgCreateBackup)
const long DlgCreateBackup::ID_TEXTCTRL1 = wxNewId();
const long DlgCreateBackup::ID_BUTTON1 = wxNewId();
const long DlgCreateBackup::ID_STATICTEXT1 = wxNewId();
const long DlgCreateBackup::ID_TEXTCTRL2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DlgCreateBackup,wxDialog)
	//(*EventTable(DlgCreateBackup)
	//*)
	EVT_BUTTON(wxID_OK,DlgCreateBackup::OnButtonCreateBackupClick)
	EVT_BUTTON(wxID_CANCEL,DlgCreateBackup::OnButtonCloseClick)
END_EVENT_TABLE()

DlgCreateBackup::DlgCreateBackup(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(DlgCreateBackup)
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, id, _("Create a partition table backup"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Save backup to"));
	TextBackupFileName = new wxTextCtrl(this, ID_TEXTCTRL1, _("backup.ptb"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	StaticBoxSizer1->Add(TextBackupFileName, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonBrowse = new wxButton(this, ID_BUTTON1, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	StaticBoxSizer1->Add(ButtonBrowse, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Description (optional)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer2->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextDescription = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	TextDescription->SetMaxLength(128);
	BoxSizer2->Add(TextDescription, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(StdDialogButtonSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgCreateBackup::OnButtonBrowseClick);
	//*)
}

DlgCreateBackup::~DlgCreateBackup()
{
	//(*Destroy(DlgCreateBackup)
	//*)
}


void DlgCreateBackup::OnButtonBrowseClick(wxCommandEvent& event)
{
    wxString wstr=_("Partition table backup (*.ptb)|*.ptb|All files (*.*)|*.*");

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
  wxDialog::Destroy();
}

void DlgCreateBackup::OnButtonCreateBackupClick(wxCommandEvent& event)
{
  try{
    disk->CreateBackup(TextBackupFileName->GetLineText(0).ToStdString(),TextDescription->GetLabelText().ToStdString());
  }
  catch(DiskException &de)
  {
      wxMessageBox(de.what(),_("Error"),wxOK | wxICON_ERROR,this);
      return;
  }
  catch(...)
  {
      wxMessageBox(_("Unknown error"),_("Error"),wxOK | wxICON_ERROR,this);
      return;
  }
  wxMessageBox(_("Backup created successfuly"),_("Information"),wxOK | wxICON_INFORMATION,this);
  wxDialog::Destroy();
}
