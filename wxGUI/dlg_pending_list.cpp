#include "dlg_pending_list.h"

//(*InternalHeaders(DlgPendingList)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(DlgPendingList)
const long DlgPendingList::ID_PENDING_LIST = wxNewId();
const long DlgPendingList::ID_BUTTON3 = wxNewId();
const long DlgPendingList::ID_BUTTON1 = wxNewId();
const long DlgPendingList::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DlgPendingList,wxDialog)
	//(*EventTable(DlgPendingList)
	//*)
END_EVENT_TABLE()

DlgPendingList::DlgPendingList(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DlgPendingList)
	wxBoxSizer* BoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, wxID_ANY, _("Pending operations"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(3, 1, 0, 0);
	ListViewPending = new wxPendingList(this, ID_PENDING_LIST, wxDefaultPosition, wxDLG_UNIT(this,wxSize(300,150)), wxLC_REPORT, wxDefaultValidator, _T("ID_PENDING_LIST"));
	FlexGridSizer1->Add(ListViewPending, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, wxDLG_UNIT(this,wxSize(5,0)).GetWidth());
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	ButtonUndoLast = new wxButton(this, ID_BUTTON3, _("Undo last"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	BoxSizer1->Add(ButtonUndoLast, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonUndoAll = new wxButton(this, ID_BUTTON1, _("Undo all"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer1->Add(ButtonUndoAll, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonOk = new wxButton(this, ID_BUTTON2, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	BoxSizer1->Add(ButtonOk, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(BoxSizer1, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgPendingList::OnButtonUndoLastClick);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgPendingList::OnButtonUndoAllClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgPendingList::OnButtonOkClick);
	//*)
}

DlgPendingList::~DlgPendingList()
{
	//(*Destroy(DlgPendingList)
	//*)
}

int DlgPendingList::ShowModal(Disk *disk)
{
  this->disk=disk;
  ListViewPending->AssignDisk(disk);
  ListViewPending->Refresh();
  return wxDialog::ShowModal();
}

void DlgPendingList::OnButtonUndoLastClick(wxCommandEvent& event)
{
  disk->Undo();
  ListViewPending->Refresh();
}

void DlgPendingList::OnButtonUndoAllClick(wxCommandEvent& event)
{
  disk->UndoAll();
  ListViewPending->Refresh();
}

void DlgPendingList::OnButtonOkClick(wxCommandEvent& event)
{
  Close();
}
