#include "dlg_guid_list.h"

//(*InternalHeaders(DlgGuidList)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(DlgGuidList)
const long DlgGuidList::ID_LISTCTRL1 = wxNewId();
const long DlgGuidList::ID_STATICTEXT1 = wxNewId();
const long DlgGuidList::ID_CANCEL = wxNewId();
//*)

BEGIN_EVENT_TABLE(DlgGuidList,wxDialog)
	//(*EventTable(DlgGuidList)
	//*)
END_EVENT_TABLE()

DlgGuidList::DlgGuidList(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DlgGuidList)
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxSize(555,403));
	Move(wxDefaultPosition);
	guidList = new wxListCtrl(this, ID_LISTCTRL1, wxPoint(16,32), wxSize(512,312), wxLC_REPORT, wxDefaultValidator, _T("ID_LISTCTRL1"));
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Known GUIDs:"), wxPoint(16,16), wxSize(80,13), 0, _T("ID_STATICTEXT1"));
	Button1 = new wxButton(this, ID_CANCEL, _("Cancel"), wxPoint(216,360), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CANCEL"));

	Connect(ID_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DlgGuidList::OnCancel);
	//*)
}

DlgGuidList::~DlgGuidList()
{
	//(*Destroy(DlgGuidList)
	//*)
}


int DlgGuidList::ShowModal(Disk *disk)
{
    guidList->InsertColumn(0,_("GUID"),0,200);
    guidList->InsertColumn(1,_("Description"),0,264);

    for(unsigned i=0;i<disk->guid_man->Count();i++)
      {
          guidList->InsertItem(i,disk->guid_man->GetGuidAsString(i));
          guidList->SetItem(i,1,disk->guid_man->GetDescription(i));
      }


    return wxDialog::ShowModal();
}

void DlgGuidList::OnCancel(wxCommandEvent& event)
{
   this->Destroy();
}
