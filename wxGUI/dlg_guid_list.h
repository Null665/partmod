#ifndef DLG_GUID_LIST_H
#define DLG_GUID_LIST_H

//(*Headers(DlgGuidList)
#include <wx/listctrl.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)


#include "../Partmod/disk.h"


class DlgGuidList: public wxDialog
{
	public:

		DlgGuidList(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DlgGuidList();

		//(*Declarations(DlgGuidList)
		wxListCtrl* guidList;
		wxButton* Button1;
		wxStaticText* StaticText1;
		//*)

		int ShowModal(Disk *disk);

	protected:

		//(*Identifiers(DlgGuidList)
		static const long ID_LISTCTRL1;
		static const long ID_STATICTEXT1;
		static const long ID_CANCEL;
		//*)

	private:

		//(*Handlers(DlgGuidList)
		void OnCancel(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
