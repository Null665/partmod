#ifndef DLG_PENDING_LIST_H
#define DLG_PENDING_LIST_H

//(*Headers(DlgPendingList)
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

#include "ctrl_pending_list.h"
#include "../Partmod/disk.h"

class DlgPendingList: public wxDialog
{
	public:

		DlgPendingList(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DlgPendingList();

		//(*Declarations(DlgPendingList)
		wxButton* ButtonUndoLast;
		wxButton* ButtonUndoAll;
		wxPendingList* ListViewPending;
		wxButton* ButtonOk;
		//*)
		int ShowModal(Disk *disk);

	protected:

		//(*Identifiers(DlgPendingList)
		static const long ID_PENDING_LIST;
		static const long ID_BUTTON3;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		//*)

	private:
         Disk *disk;
		//(*Handlers(DlgPendingList)
		void OnButtonUndoLastClick(wxCommandEvent& event);
		void OnButtonUndoAllClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
